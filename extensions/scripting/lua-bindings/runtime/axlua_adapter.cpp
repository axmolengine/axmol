/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "lua-bindings/runtime/axlua_adapter.h"
#include "lua-bindings/runtime/axlua_runtime.h"
#include "axmol/base/Object.h"
#include "lua-bindings/runtime/axlua_conversions.h"
#include <algorithm>
#include <string.h>
#include <string>
#include <unordered_map>

using namespace ax;

namespace
{
constexpr auto OBJECT_BOX_REGISTRY           = axlua::adapter::kObjectBoxRegistry;
constexpr auto OWNED_OBJECT_REGISTRY         = axlua::adapter::kOwnedObjectRegistry;
constexpr auto ADAPTER_CLASS_TABLES_REGISTRY = "axlua.adapter.class_tables";
constexpr auto MANUAL_FUNCTIONS_REGISTRY     = "axlua.adapter.manual_functions";

int absolute_index(lua_State* state, int index)
{
    return index > 0 || index <= LUA_REGISTRYINDEX ? index : lua_gettop(state) + index + 1;
}

void set_error(axlua::adapter::Error* error, int index, const char* type)
{
    if (error == nullptr)
        return;
    error->index = index;
    error->array = 0;
    error->type  = type;
}

void ensure_registry_table(lua_State* state, const char* name)
{
    lua_getfield(state, LUA_REGISTRYINDEX, name);
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        lua_newtable(state);
        lua_pushvalue(state, -1);
        lua_setfield(state, LUA_REGISTRYINDEX, name);
    }
    lua_pop(state, 1);
}

bool mark_manual_function(lua_State* state, const char* name)
{
    const int tableIndex = absolute_index(state, -1);
    lua_getfield(state, LUA_REGISTRYINDEX, MANUAL_FUNCTIONS_REGISTRY);
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        lua_newtable(state);
        lua_pushvalue(state, -1);
        lua_setfield(state, LUA_REGISTRYINDEX, MANUAL_FUNCTIONS_REGISTRY);
    }
    const int functions = absolute_index(state, -1);
    lua_pushlightuserdata(state, const_cast<void*>(lua_topointer(state, tableIndex)));
    lua_rawget(state, functions);
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        lua_newtable(state);
        lua_pushlightuserdata(state, const_cast<void*>(lua_topointer(state, tableIndex)));
        lua_pushvalue(state, -1);
        lua_rawset(state, functions);
    }
    const int names = absolute_index(state, -1);
    lua_getfield(state, names, name);
    const bool duplicate = !lua_isnil(state, -1);
    lua_pop(state, 1);
    if (!duplicate)
    {
        lua_pushboolean(state, 1);
        lua_setfield(state, names, name);
    }
    lua_pop(state, 2);  // names and registry table
    return !duplicate;
}

void register_metatable(lua_State* state, const char* name)
{
    if (name == nullptr || *name == '\0')
        return;

    if (luaL_newmetatable(state, name) != 0)
    {
        lua_pushstring(state, name);
        lua_setfield(state, -2, ".classname");
        lua_pushvalue(state, -1);
        lua_pushstring(state, name);
        lua_rawset(state, LUA_REGISTRYINDEX);
    }
    lua_pop(state, 1);
}

bool is_registered_class_table(lua_State* state, int index, const char* type)
{
    index = absolute_index(state, index);
    if (!lua_istable(state, index))
        return false;

    lua_pushvalue(state, index);
    for (int depth = 0; depth < 64 && lua_istable(state, -1); ++depth)
    {
        lua_getfield(state, -1, "__axlua_name");
        const char* className = lua_tostring(state, -1);
        const bool matched    = className != nullptr && type != nullptr && std::strcmp(className, type) == 0;
        lua_pop(state, 1);
        if (matched)
        {
            lua_pop(state, 1);
            return true;
        }

        lua_getfield(state, LUA_REGISTRYINDEX, "axlua.class.bases");
        if (!lua_istable(state, -1))
        {
            lua_pop(state, 2);
            return false;
        }
        lua_pushlightuserdata(state, const_cast<void*>(lua_topointer(state, -2)));
        lua_rawget(state, -2);
        lua_remove(state, -2);  // base registry
        lua_remove(state, -2);  // previous class table
    }
    lua_pop(state, 1);
    return false;
}

bool is_registered_class(lua_State* state, int index, const char* type)
{
    index = absolute_index(state, index);
    if (lua_istable(state, index))
    {
        lua_getfield(state, index, ".c_instance");
        const bool result = lua_isuserdata(state, -1) && is_registered_class(state, -1, type);
        lua_pop(state, 1);
        return result || is_registered_class_table(state, index, type);
    }
    if (!lua_isuserdata(state, index) || !lua_getmetatable(state, index))
        return false;

    const int metatable = absolute_index(state, -1);
    lua_getfield(state, metatable, ".classname");
    const char* name = lua_tostring(state, -1);
    const bool exact = name != nullptr && type != nullptr && std::strcmp(name, type) == 0;
    lua_pop(state, 1);
    if (exact)
    {
        lua_pop(state, 1);
        return true;
    }

    // Generated classes keep their Lua inheritance graph in axlua.class.bases.
    lua_getfield(state, LUA_REGISTRYINDEX, "axlua.class.tables");
    if (lua_istable(state, -1))
    {
        lua_pushlightuserdata(state, const_cast<void*>(lua_topointer(state, metatable)));
        lua_rawget(state, -2);
        if (lua_istable(state, -1))
        {
            const bool matched = is_registered_class_table(state, -1, type);
            lua_pop(state, 1);  // generated class table
            lua_pop(state, 1);  // class table registry
            lua_pop(state, 1);  // userdata metatable
            return matched;
        }
        lua_pop(state, 1);
    }
    lua_pop(state, 2);
    return false;
}

void ensure_ubox(lua_State* state)
{
    lua_getfield(state, LUA_REGISTRYINDEX, OBJECT_BOX_REGISTRY);
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        lua_newtable(state);
        axlua::make_weak_value_table(state, -1);
        lua_pushvalue(state, -1);
        lua_setfield(state, LUA_REGISTRYINDEX, OBJECT_BOX_REGISTRY);
    }
    else
    {
        axlua::make_weak_value_table(state, -1);
    }
    lua_pop(state, 1);
}

void ensure_identity_table(lua_State* state)
{
    lua_getfield(state, LUA_REGISTRYINDEX, "axlua.object.identity");
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        lua_newtable(state);
        lua_newtable(state);
        axlua::adapter::push_literal(state, "v");
        lua_setfield(state, -2, "__mode");
        lua_setmetatable(state, -2);
        lua_pushvalue(state, -1);
        lua_setfield(state, LUA_REGISTRYINDEX, "axlua.object.identity");
    }
}

void remove_pointer_registry_entry(lua_State* state, const char* registryName, void* value)
{
    lua_getfield(state, LUA_REGISTRYINDEX, registryName);
    if (lua_istable(state, -1))
    {
        lua_pushlightuserdata(state, value);
        lua_pushnil(state);
        lua_rawset(state, -3);
    }
    lua_pop(state, 1);
}

void clear_user_environment(lua_State* state, int index)
{
    index = absolute_index(state, index);
#if LUA_VERSION_NUM == 501 || defined(LUAJIT_VERSION)
    lua_pushvalue(state, LUA_REGISTRYINDEX);
    lua_setfenv(state, index);
#elif LUA_VERSION_NUM >= 504
    lua_pushnil(state);
    lua_setiuservalue(state, index, 1);
#else
    lua_pushnil(state);
    lua_setuservalue(state, index);
#endif
}

void mark_userdata_invalid(lua_State* state, int index)
{
    index = absolute_index(state, index);
    lua_newtable(state);
    lua_pushboolean(state, 1);
    lua_setfield(state, -2, "__axlua_invalid");
#if LUA_VERSION_NUM == 501 || defined(LUAJIT_VERSION)
    lua_setfenv(state, index);
#elif LUA_VERSION_NUM >= 504
    lua_setiuservalue(state, index, 1);
#else
    lua_setuservalue(state, index);
#endif
    if (auto** storage = static_cast<void**>(lua_touserdata(state, index)))
        *storage = nullptr;
}

bool push_named_class_table(lua_State* state, const char* name)
{
    if (name == nullptr || *name == '\0')
        return false;

    lua_getfield(state, LUA_REGISTRYINDEX, name);
    if (lua_istable(state, -1))
    {
        lua_getfield(state, -1, "__axlua_name");
        const bool isClass = lua_isstring(state, -1) && std::strcmp(lua_tostring(state, -1), name) == 0;
        lua_pop(state, 1);
        if (isClass)
            return true;
    }
    lua_pop(state, 1);

    lua_getfield(state, LUA_REGISTRYINDEX, ADAPTER_CLASS_TABLES_REGISTRY);
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        return false;
    }
    lua_getfield(state, -1, name);
    lua_remove(state, -2);
    if (lua_istable(state, -1))
        return true;
    lua_pop(state, 1);
    return false;
}

bool is_generated_class_table(lua_State* state, int index)
{
    index                  = absolute_index(state, index);
    const auto storageName = sol::to_string(sol::meta_function::storage);
    lua_getfield(state, index, storageName.c_str());
    const bool generated = lua_islightuserdata(state, -1);
    lua_pop(state, 1);
    return generated;
}

void register_adapter_class_table(lua_State* state, const char* name, int classTableIndex)
{
    classTableIndex = absolute_index(state, classTableIndex);
    ensure_registry_table(state, ADAPTER_CLASS_TABLES_REGISTRY);
    lua_getfield(state, LUA_REGISTRYINDEX, ADAPTER_CLASS_TABLES_REGISTRY);
    lua_pushvalue(state, classTableIndex);
    lua_setfield(state, -2, name);
    lua_pop(state, 1);
}

void register_class_metatable(lua_State* state, int metatableIndex, int classTableIndex)
{
    metatableIndex  = absolute_index(state, metatableIndex);
    classTableIndex = absolute_index(state, classTableIndex);
    ensure_registry_table(state, "axlua.class.tables");
    lua_getfield(state, LUA_REGISTRYINDEX, "axlua.class.tables");
    lua_pushlightuserdata(state, const_cast<void*>(lua_topointer(state, metatableIndex)));
    lua_pushvalue(state, classTableIndex);
    lua_rawset(state, -3);
    lua_pop(state, 1);
}

void register_class_base(lua_State* state, int classTableIndex, const char* base)
{
    if (base == nullptr || *base == '\0' || !push_named_class_table(state, base))
        return;

    classTableIndex     = absolute_index(state, classTableIndex);
    const int baseTable = absolute_index(state, -1);
    ensure_registry_table(state, "axlua.class.bases");
    lua_getfield(state, LUA_REGISTRYINDEX, "axlua.class.bases");
    lua_pushlightuserdata(state, const_cast<void*>(lua_topointer(state, classTableIndex)));
    lua_pushvalue(state, baseTable);
    lua_rawset(state, -3);
    lua_pop(state, 2);
}

bool push_collector_for_metatable(lua_State* state, int metatableIndex)
{
    metatableIndex = absolute_index(state, metatableIndex);
    lua_getfield(state, metatableIndex, ".collector");
    if (lua_isfunction(state, -1))
        return true;
    lua_pop(state, 1);

    lua_getfield(state, LUA_REGISTRYINDEX, "axlua.class.tables");
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        return false;
    }
    lua_pushlightuserdata(state, const_cast<void*>(lua_topointer(state, metatableIndex)));
    lua_rawget(state, -2);
    lua_remove(state, -2);
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        return false;
    }
    lua_getfield(state, -1, ".collector");
    lua_remove(state, -2);
    if (lua_isfunction(state, -1))
        return true;
    lua_pop(state, 1);
    return false;
}

int adapter_gc_event(lua_State* state)
{
    auto** storage = static_cast<void**>(lua_touserdata(state, 1));
    if (storage == nullptr || *storage == nullptr)
        return 0;
    void* value = *storage;

    lua_getfield(state, LUA_REGISTRYINDEX, OWNED_OBJECT_REGISTRY);
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        return 0;
    }
    const int ownership = absolute_index(state, -1);
    lua_pushlightuserdata(state, value);
    lua_rawget(state, ownership);
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 2);
        return 0;
    }
    const int ownedMetatable = absolute_index(state, -1);

    if (!lua_getmetatable(state, 1))
    {
        lua_pop(state, 2);
        return 0;
    }
    const bool sameMetatable = lua_rawequal(state, ownedMetatable, -1) != 0;
    lua_pop(state, 1);
    if (!sameMetatable || !push_collector_for_metatable(state, ownedMetatable))
    {
        lua_pop(state, 2);
        return 0;
    }

    // Remove ownership before invoking user code so re-entrant GC or an Object
    // destructor cannot collect the same native allocation twice.
    lua_pushlightuserdata(state, value);
    lua_pushnil(state);
    lua_rawset(state, ownership);

    lua_pushvalue(state, 1);
    if (lua_pcall(state, 1, 0, 0) != LUA_OK)
    {
        const char* message = lua_tostring(state, -1);
        AXLOGE("[LUA GC ERROR] {}", message != nullptr ? message : "manual binding collector failed");
        lua_pop(state, 1);
    }

    *storage = nullptr;
    clear_user_environment(state, 1);
    remove_pointer_registry_entry(state, "axlua.object.identity", value);
    remove_pointer_registry_entry(state, OBJECT_BOX_REGISTRY, value);
    remove_pointer_registry_entry(state, axlua::adapter::kValueRootRegistry, value);
    lua_pop(state, 2);  // owned metatable and ownership table
    return 0;
}

void install_gc_event(lua_State* state, int metatableIndex)
{
    metatableIndex = absolute_index(state, metatableIndex);
    lua_pushcfunction(state, &adapter_gc_event);
    lua_setfield(state, metatableIndex, "__gc");
}

void root_userdata(lua_State* state, void* value, int userdataIndex)
{
    userdataIndex = absolute_index(state, userdataIndex);
    ensure_registry_table(state, axlua::adapter::kValueRootRegistry);
    lua_getfield(state, LUA_REGISTRYINDEX, axlua::adapter::kValueRootRegistry);
    lua_pushlightuserdata(state, value);
    lua_pushvalue(state, userdataIndex);
    lua_rawset(state, -3);
    lua_pop(state, 1);
}

void push_user_type(lua_State* state, void* value, const char* type, bool root)
{
    if (value == nullptr)
    {
        lua_pushnil(state);
        return;
    }

    ensure_identity_table(state);
    const int identity = absolute_index(state, -1);
    auto* nativeObject = static_cast<ax::Object*>(value);
    lua_pushlightuserdata(state, value);
    lua_rawget(state, identity);
    if (lua_isuserdata(state, -1))
    {
        if (axlua::is_invalid_userdata(state, -1))
        {
            lua_pop(state, 1);
            lua_pushlightuserdata(state, value);
            lua_pushnil(state);
            lua_rawset(state, identity);
        }
        else
        {
            const int userdata = absolute_index(state, -1);
            ensure_ubox(state);
            lua_getfield(state, LUA_REGISTRYINDEX, OBJECT_BOX_REGISTRY);
            const int ubox = absolute_index(state, -1);
            lua_pushlightuserdata(state, value);
            lua_pushvalue(state, userdata);
            lua_rawset(state, ubox);
            lua_pop(state, 1);
            lua_remove(state, identity);
            if (root)
                root_userdata(state, value, -1);
            return;
        }
    }
    else
        lua_pop(state, 1);

    ensure_ubox(state);
    lua_getfield(state, LUA_REGISTRYINDEX, OBJECT_BOX_REGISTRY);
    const int ubox = absolute_index(state, -1);
    lua_pushlightuserdata(state, value);
    lua_rawget(state, ubox);
    if (lua_isuserdata(state, -1))
    {
        if (axlua::is_invalid_userdata(state, -1))
        {
            lua_pop(state, 1);
            lua_pushlightuserdata(state, value);
            lua_pushnil(state);
            lua_rawset(state, ubox);
        }
        else
        {
            const int userdata = absolute_index(state, -1);
            lua_pushlightuserdata(state, value);
            lua_pushvalue(state, userdata);
            lua_rawset(state, identity);
            lua_remove(state, ubox);
            lua_remove(state, identity);
            if (root)
                root_userdata(state, value, -1);
            return;
        }
    }
    else
        lua_pop(state, 1);

    const bool generated = type != nullptr && axlua::push_registered_object(state, value, type);
    if (!generated)
    {
        void** userdata = static_cast<void**>(lua_newuserdata(state, sizeof(void*)));
        *userdata       = value;
        if (type != nullptr)
        {
            luaL_getmetatable(state, type);
            if (lua_istable(state, -1))
                lua_setmetatable(state, -2);
            else
                lua_pop(state, 1);
        }
    }
    lua_pushlightuserdata(state, value);
    lua_pushvalue(state, -2);
    lua_rawset(state, ubox);
    lua_pushlightuserdata(state, value);
    lua_pushvalue(state, -2);
    lua_rawset(state, identity);
    axlua::remember_userdata_object(state, -1, nativeObject);
    lua_remove(state, ubox);
    lua_remove(state, identity);

    if (root)
        root_userdata(state, value, -1);
}
}  // namespace

void axlua::adapter::open(lua_State* state)
{
    ensure_registry_table(state, axlua::adapter::kValueRootRegistry);
    ensure_ubox(state);
    ensure_registry_table(state, "axlua.class.super");
    ensure_registry_table(state, OWNED_OBJECT_REGISTRY);
    ensure_registry_table(state, ADAPTER_CLASS_TABLES_REGISTRY);
    ensure_registry_table(state, axlua::adapter::kFunctionRegistry);
    register_metatable(state, "axlua.common_class");
}

void axlua::adapter::register_usertype(lua_State* state, const char* type)
{
    register_metatable(state, type);
    if (type != nullptr)
    {
        const std::string constType = std::string("const ") + type;
        register_metatable(state, constType.c_str());
    }
}

void axlua::adapter::module(lua_State* state, const char* name, int)
{
    if (name == nullptr)
        return;
    const bool pushedGlobal = lua_gettop(state) == 0 || !lua_istable(state, -1);
    if (pushedGlobal)
        lua_pushglobaltable(state);
    const int parent = absolute_index(state, -1);
    lua_getfield(state, parent, name);
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        lua_newtable(state);
        lua_pushvalue(state, -1);
        lua_setfield(state, parent, name);
    }
    lua_pop(state, 1);
    if (pushedGlobal)
        lua_pop(state, 1);
}

void axlua::adapter::begin_module(lua_State* state, const char* name)
{
    if (name == nullptr)
    {
        lua_pushglobaltable(state);
        return;
    }
    const bool pushedGlobal = lua_gettop(state) == 0 || !lua_istable(state, -1);
    if (pushedGlobal)
        lua_pushglobaltable(state);
    const int parent = absolute_index(state, -1);
    lua_getfield(state, parent, name);
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        lua_newtable(state);
        lua_pushvalue(state, -1);
        lua_setfield(state, parent, name);
    }
    lua_remove(state, parent);
}

void axlua::adapter::end_module(lua_State* state)
{
    if (lua_gettop(state) > 0)
        lua_pop(state, 1);
}

void axlua::adapter::register_class(lua_State* state,
                                    const char* luaName,
                                    const char* nativeName,
                                    const char* base,
                                    lua_CFunction collector)
{
    axlua::adapter::register_usertype(state, nativeName);
    if (base != nullptr && *base != '\0')
        axlua::adapter::register_usertype(state, base);

    const int parent = lua_gettop(state);
    if (parent == 0 || !lua_istable(state, parent))
        return;
    lua_getfield(state, parent, luaName);
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        lua_newtable(state);
        lua_pushvalue(state, -1);
        lua_setfield(state, parent, luaName);
    }
    lua_pushboolean(state, 1);
    lua_setfield(state, -2, ".isclass");
    lua_pushstring(state, nativeName);
    lua_setfield(state, -2, "__axlua_name");
    const int classTable = absolute_index(state, -1);
    register_adapter_class_table(state, nativeName, classTable);

    // A manual adapter may extend a class that was already registered by the
    // generator (PointerEventListener is one example).  In that case the
    // registry name intentionally points at sol2's class table, not a legacy
    // userdata metatable.  Keep the existing sol2 metatables intact and only
    // add the requested methods to the shared class table.
    if (is_generated_class_table(state, classTable))
    {
        register_class_base(state, classTable, base);
        lua_pop(state, 1);
        return;
    }

    luaL_getmetatable(state, nativeName);
    if (lua_istable(state, -1))
    {
        const int metatable = absolute_index(state, -1);
        lua_pushcfunction(state, &axlua::class_index);
        lua_setfield(state, metatable, "__index");
        lua_pushcfunction(state, &axlua::class_new_index);
        lua_setfield(state, metatable, "__newindex");
        if (collector != nullptr)
        {
            lua_pushcfunction(state, collector);
            lua_setfield(state, metatable, ".collector");
            install_gc_event(state, metatable);
        }
        register_class_metatable(state, metatable, classTable);
        lua_pushvalue(state, metatable);
        lua_setmetatable(state, classTable);
    }
    lua_pop(state, 1);

    const std::string constNativeName = std::string("const ") + nativeName;
    luaL_getmetatable(state, constNativeName.c_str());
    if (lua_istable(state, -1))
    {
        const int constMetatable = absolute_index(state, -1);
        lua_pushcfunction(state, &axlua::class_index);
        lua_setfield(state, constMetatable, "__index");
        lua_pushcfunction(state, &axlua::class_new_index);
        lua_setfield(state, constMetatable, "__newindex");
        if (collector != nullptr)
        {
            lua_pushcfunction(state, collector);
            lua_setfield(state, constMetatable, ".collector");
            install_gc_event(state, constMetatable);
        }
        register_class_metatable(state, constMetatable, classTable);
    }
    lua_pop(state, 1);

    register_class_base(state, classTable, base);
    lua_pop(state, 1);
}

void axlua::adapter::set_function(lua_State* state, const char* name, lua_CFunction function)
{
    if (state == nullptr || name == nullptr || function == nullptr)
        return;
    if (!mark_manual_function(state, name))
    {
        luaL_error(state, "duplicate manual Lua binding registration: %s", name);
        return;
    }
    lua_pushcfunction(state, function);
    lua_setfield(state, -2, name);
}

void axlua::adapter::set_constant(lua_State* state, const char* name, lua_Number value)
{
    lua_pushnumber(state, value);
    lua_setfield(state, -2, name);
}

void axlua::adapter::set_variable(lua_State* state, const char* name, lua_CFunction getter, lua_CFunction setter)
{
    lua_getfield(state, -1, ".get");
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        lua_newtable(state);
        lua_pushvalue(state, -1);
        lua_setfield(state, -3, ".get");
    }
    lua_pushcfunction(state, getter);
    lua_setfield(state, -2, name);
    lua_pop(state, 1);

    if (setter != nullptr)
    {
        lua_getfield(state, -1, ".set");
        if (!lua_istable(state, -1))
        {
            lua_pop(state, 1);
            lua_newtable(state);
            lua_pushvalue(state, -1);
            lua_setfield(state, -3, ".set");
        }
        lua_pushcfunction(state, setter);
        lua_setfield(state, -2, name);
        lua_pop(state, 1);
    }
}

int axlua::adapter::register_gc(lua_State* state, int index)
{
    index = absolute_index(state, index);
    if (!lua_isuserdata(state, index))
        return 0;
    auto** storage = static_cast<void**>(lua_touserdata(state, index));
    if (storage == nullptr || *storage == nullptr || !lua_getmetatable(state, index))
        return 0;

    const int metatable = absolute_index(state, -1);
    if (!push_collector_for_metatable(state, metatable))
    {
        lua_pop(state, 1);
        return 0;
    }
    lua_pop(state, 1);  // collector
    install_gc_event(state, metatable);

    ensure_registry_table(state, OWNED_OBJECT_REGISTRY);
    lua_getfield(state, LUA_REGISTRYINDEX, OWNED_OBJECT_REGISTRY);
    const int ownership = absolute_index(state, -1);
    lua_pushlightuserdata(state, *storage);
    lua_rawget(state, ownership);
    if (!lua_isnil(state, -1))
    {
        lua_pop(state, 3);
        return 0;
    }
    lua_pop(state, 1);
    lua_pushlightuserdata(state, *storage);
    lua_pushvalue(state, metatable);
    lua_rawset(state, ownership);
    lua_pop(state, 2);
    return 1;
}

const char* axlua::adapter::type_name(lua_State* state, int index)
{
    if (lua_isnone(state, index))
    {
        axlua::adapter::push_literal(state, "[no object]");
        return lua_tostring(state, -1);
    }
    if (!lua_getmetatable(state, index))
    {
        lua_pushstring(state, luaL_typename(state, index));
        return lua_tostring(state, -1);
    }
    lua_getfield(state, -1, ".classname");
    lua_remove(state, -2);
    if (!lua_isstring(state, -1))
    {
        lua_pop(state, 1);
        lua_pushstring(state, luaL_typename(state, index));
    }
    return lua_tostring(state, -1);
}

void axlua::adapter::raise_error(lua_State* state, const char* message, axlua::adapter::Error* error)
{
    if (message != nullptr && message[0] == '#' && error != nullptr)
    {
        const char* providedName   = axlua::adapter::type_name(state, error->index);
        const std::string provided = providedName != nullptr ? providedName : "value";
        const char* expected       = error->type != nullptr ? error->type : "value";
        lua_remove(state, -1);
        luaL_error(state, "%s\n     argument #%d is '%s'; '%s' expected.", message + 2, error->index, provided.c_str(),
                   expected);
    }
    luaL_error(state, "%s", message != nullptr ? message : "Lua binding error");
}

#define AXLUA_ADAPTER_CHECK(name, predicate, expected)                                      \
    int name(lua_State* state, int index, int hasDefault, axlua::adapter::Error* error)     \
    {                                                                                       \
        if ((hasDefault && lua_gettop(state) < std::abs(index)) || predicate(state, index)) \
            return 1;                                                                       \
        set_error(error, index, expected);                                                  \
        return 0;                                                                           \
    }

AXLUA_ADAPTER_CHECK(axlua::adapter::is_boolean, lua_isboolean, "boolean")
AXLUA_ADAPTER_CHECK(axlua::adapter::is_number, lua_isnumber, "number")
AXLUA_ADAPTER_CHECK(axlua::adapter::is_string, lua_isstring, "string")
AXLUA_ADAPTER_CHECK(axlua::adapter::is_table, lua_istable, "table")

#undef AXLUA_ADAPTER_CHECK

int axlua::adapter::is_no_object(lua_State* state, int index, axlua::adapter::Error* error)
{
    if (lua_gettop(state) < std::abs(index))
        return 1;
    set_error(error, index, "[no object]");
    return 0;
}

int axlua::adapter::is_usertype(lua_State* state,
                                int index,
                                const char* type,
                                int hasDefault,
                                axlua::adapter::Error* error)
{
    const bool missing = hasDefault && lua_gettop(state) < std::abs(index);
    const bool valid = is_registered_class(state, index, type) && !axlua::is_invalid_userdata(state, index) &&
                       axlua::adapter::to_usertype(state, index, nullptr) != nullptr;
    if (missing || valid)
        return 1;
    set_error(error, index, type);
    return 0;
}

int axlua::adapter::is_usertable(lua_State* state,
                                 int index,
                                 const char* type,
                                 int hasDefault,
                                 axlua::adapter::Error* error)
{
    if (lua_istable(state, index) && is_registered_class(state, index, type))
        return 1;
    if (hasDefault && lua_gettop(state) < std::abs(index))
        return 1;
    set_error(error, index, type);
    return 0;
}

lua_Number axlua::adapter::to_number(lua_State* state, int index, lua_Number defaultValue)
{
    return lua_gettop(state) < std::abs(index) ? defaultValue : lua_tonumber(state, index);
}

lua_Integer axlua::adapter::to_integer(lua_State* state, int index, lua_Integer defaultValue)
{
    return lua_gettop(state) < std::abs(index) ? defaultValue : lua_tointeger(state, index);
}

const char* axlua::adapter::to_string(lua_State* state, int index, const char* defaultValue)
{
    return lua_gettop(state) < std::abs(index) ? defaultValue : lua_tostring(state, index);
}

void* axlua::adapter::to_usertype(lua_State* state, int index, void* defaultValue)
{
    if (lua_gettop(state) < std::abs(index))
        return defaultValue;
    if (lua_islightuserdata(state, index))
        return lua_touserdata(state, index);
    if (!lua_isuserdata(state, index))
        return defaultValue;
    if (axlua::is_invalid_userdata(state, index))
    {
        luaL_error(state, "attempt to access an expired Axmol object");
        return nullptr;
    }
    void** value = static_cast<void**>(lua_touserdata(state, index));
    return value != nullptr ? *value : nullptr;
}

int axlua::adapter::to_boolean(lua_State* state, int index, int defaultValue)
{
    return lua_gettop(state) < std::abs(index) ? defaultValue : lua_toboolean(state, index);
}

lua_Number axlua::adapter::to_field_number(lua_State* state, int tableIndex, int field, lua_Number defaultValue)
{
    lua_pushinteger(state, field);
    lua_gettable(state, tableIndex);
    const lua_Number value = lua_isnil(state, -1) ? defaultValue : lua_tonumber(state, -1);
    lua_pop(state, 1);
    return value;
}

void axlua::adapter::push_usertype(lua_State* state, void* value, const char* type)
{
    push_user_type(state, value, type, false);
}
void axlua::adapter::push_usertype_rooted(lua_State* state, void* value, const char* type)
{
    push_user_type(state, value, type, true);
}

void axlua::adapter::open_bindings(lua_State* L)
{
    axlua::adapter::open(L);
    ensure_identity_table(L);
    lua_pop(L, 1);
    lua_pushstring(L, axlua::adapter::kFunctionRegistry);
    lua_newtable(L);
    lua_rawset(L, LUA_REGISTRYINDEX);
}

int axlua::adapter::push_object(lua_State* L, void* ptr, const char* type)
{
    if (ptr == NULL)
    {
        lua_pushnil(L);
        return -1;
    }

    Object* vPtr = static_cast<Object*>(ptr);
    axlua::remember_object(L, vPtr);
    const char* vType = getLuaTypeName(vPtr, type);

    axlua::adapter::push_usertype_rooted(L, vPtr, vType);

    return 0;
}

int axlua::adapter::ref_function(lua_State* L, int lo, int def)
{
    if (!lua_isfunction(L, lo))
        return 0;

    lo = absolute_index(L, lo);
    lua_pushstring(L, axlua::adapter::kFunctionRegistry);
    lua_rawget(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, lo);
    const int refid = luaL_ref(L, -2);
    lua_pop(L, 1);
    return refid;
}

void axlua::adapter::push_function(lua_State* L, int refid)
{
    lua_pushstring(L, axlua::adapter::kFunctionRegistry);
    lua_rawget(L, LUA_REGISTRYINDEX);
    lua_rawgeti(L, -1, refid);
    lua_remove(L, -2);
}

void axlua::adapter::remove_function(lua_State* L, int refid)
{
    lua_pushstring(L, axlua::adapter::kFunctionRegistry);
    lua_rawget(L, LUA_REGISTRYINDEX);
    luaL_unref(L, -1, refid);
    lua_pop(L, 1);
}

// check lua value is function
int axlua::adapter::is_function(lua_State* L, int lo, const char* type, int def, axlua::adapter::Error* err)
{
    if (lua_gettop(L) >= abs(lo) && lua_isfunction(L, lo))
    {
        return 1;
    }
    err->index = lo;
    err->array = 0;
    err->type  = "[not function]";
    return 0;
}
