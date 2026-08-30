#include "lua-bindings/runtime/axlua_runtime.h"

template <>
std::function<void()> axlua::make_lua_callback<void()>(lua_State* state, int index)
{
    return axlua::detail::LuaCallbackFactory<void()>::create(state, index);
}
#include "axmol/base/Object.h"
#include "axmol/base/WeakPtr.h"
#include "axmol/base/Logging.h"
#include "axmol/base/Director.h"
#include "axmol/base/EventDispatcher.h"

#include <utility>
#include <unordered_map>
#include <vector>
#include <string.h>
#include <mutex>
#include <thread>

namespace
{
struct DynamicObjectPushers
{
    // Fast path: RTTI descriptors have process lifetime and normally have one
    // canonical address.  The name map is the cross-DSO fallback; a successful
    // fallback is cached into this pointer map.
    std::unordered_map<const std::type_info*, axlua::AdapterObjectPusher> byAddress;
    std::unordered_map<std::string, axlua::AdapterObjectPusher> byName;
};

class AxluaRuntimeState
{
public:
    static AxluaRuntimeState& instance()
    {
        if (_instance == nullptr)
            _instance = new AxluaRuntimeState();
        return *_instance;
    }

    static AxluaRuntimeState* existing() { return _instance; }

    static void dispose()
    {
        auto* state = _instance;
        if (state == nullptr)
            return;
        _instance = nullptr;
        delete state;
    }

    std::unordered_map<ax::Object*, ax::WeakPtr<ax::Object>> nativeObjects;
    std::mutex nativeObjectsMutex;
    std::unordered_map<ax::Object*, std::thread::id> nativeObjectThreads;
    std::unordered_map<lua_State*, std::vector<std::weak_ptr<axlua::detail::LuaCallbackState>>> luaCallbacks;
    std::unordered_map<lua_State*, std::unordered_map<std::string, axlua::AdapterObjectPusher>> objectPushers;
    std::unordered_map<lua_State*, DynamicObjectPushers> dynamicObjectPushers;
    std::unordered_map<const std::type_info*, std::string> nativeTypeNamesByAddress;
    std::unordered_map<std::string, std::string> nativeTypeNamesByName;

private:
    AxluaRuntimeState()
    {
        _dispatcher        = ax::Director::getInstance()->getEventDispatcher();
        _disposingListener = _dispatcher->addCustomEventListener(
            ax::Director::EVENT_DISPOSING, [](ax::CustomEvent*) { AxluaRuntimeState::dispose(); }, 2);
    }

    ~AxluaRuntimeState()
    {
        if (_dispatcher != nullptr && _disposingListener != nullptr)
            _dispatcher->removeEventListener(_disposingListener);
        _disposingListener = nullptr;
        for (auto& entry : luaCallbacks)
        {
            for (auto& weakCallback : entry.second)
            {
                if (auto callback = weakCallback.lock())
                    callback->invalidate();
            }
        }
        luaCallbacks.clear();
        objectPushers.clear();
        dynamicObjectPushers.clear();
        nativeTypeNamesByAddress.clear();
        nativeTypeNamesByName.clear();
        nativeObjects.clear();
        nativeObjectThreads.clear();
    }

    static AxluaRuntimeState* _instance;
    ax::EventDispatcher* _dispatcher            = nullptr;
    ax::CustomEventListener* _disposingListener = nullptr;
};

AxluaRuntimeState* AxluaRuntimeState::_instance = nullptr;

AxluaRuntimeState& runtime_state()
{
    return AxluaRuntimeState::instance();
}

int absolute_index(lua_State* state, int index)
{
    return index > 0 || index <= LUA_REGISTRYINDEX ? index : lua_gettop(state) + index + 1;
}

void push_user_environment(lua_State* state, int index)
{
    index = absolute_index(state, index);
#if LUA_VERSION_NUM == 501 || defined(LUAJIT_VERSION)
    lua_getfenv(state, index);
    // Lua 5.1/LuaJIT may return the shared global environment for userdata.
    // Never use that table as Axmol's peer storage.
    bool sharedEnvironment = false;
    if (lua_istable(state, -1))
    {
        lua_pushvalue(state, LUA_GLOBALSINDEX);
        sharedEnvironment = lua_rawequal(state, -1, -2) != 0;
        lua_pop(state, 1);
    }
#elif LUA_VERSION_NUM >= 504
    lua_getiuservalue(state, index, 1);
#else
    lua_getuservalue(state, index);
#endif
    if (!lua_istable(state, -1)
#if LUA_VERSION_NUM == 501 || defined(LUAJIT_VERSION)
        || sharedEnvironment
#endif
    )
    {
        lua_pop(state, 1);
        lua_newtable(state);
#if LUA_VERSION_NUM == 501 || defined(LUAJIT_VERSION)
        lua_pushvalue(state, -1);
        lua_setfenv(state, index);
#elif LUA_VERSION_NUM >= 504
        lua_pushvalue(state, -1);
        lua_setiuservalue(state, index, 1);
#else
        lua_pushvalue(state, -1);
        lua_setuservalue(state, index);
#endif
    }
}

void mark_userdata_invalid(lua_State* state, int index)
{
    index = absolute_index(state, index);
    if (!lua_isuserdata(state, index))
        return;

    // Install a private marker table. In Lua 5.1/LuaJIT the existing fenv may
    // be the shared global environment, which would otherwise invalidate every
    // userdata sharing that table.
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

    // Both sol2 pointer userdata and the remaining compatibility userdata keep
    // the non-owning native pointer in their first machine word.
    if (auto** storage = static_cast<void**>(lua_touserdata(state, index)))
        *storage = nullptr;
}

void invalidate_pointer_registry_entry(lua_State* state, const char* registryName, void* object)
{
    lua_getfield(state, LUA_REGISTRYINDEX, registryName);
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        return;
    }

    const int table = absolute_index(state, -1);
    lua_pushlightuserdata(state, object);
    lua_rawget(state, table);
    mark_userdata_invalid(state, -1);
    lua_pop(state, 1);
    lua_pushlightuserdata(state, object);
    lua_pushnil(state);
    lua_rawset(state, table);
    lua_pop(state, 1);
}

void remove_pointer_registry_entry(lua_State* state, const char* registryName, void* object)
{
    lua_getfield(state, LUA_REGISTRYINDEX, registryName);
    if (lua_istable(state, -1))
    {
        lua_pushlightuserdata(state, object);
        lua_pushnil(state);
        lua_rawset(state, -3);
    }
    lua_pop(state, 1);
}

int lua_getpeer(lua_State* state)
{
    if (!lua_isuserdata(state, 1))
    {
        lua_pushnil(state);
        return 1;
    }
    push_user_environment(state, 1);
    return 1;
}

int lua_setpeer(lua_State* state)
{
    if (!lua_isuserdata(state, 1))
        return luaL_error(state, "axlua.setpeer expects userdata");
    if (lua_isnoneornil(state, 2))
    {
#if LUA_VERSION_NUM == 501 || defined(LUAJIT_VERSION)
        lua_pushnil(state);
        lua_setfenv(state, 1);
#elif LUA_VERSION_NUM >= 504
        lua_pushnil(state);
        lua_setiuservalue(state, 1, 1);
#else
        lua_pushnil(state);
        lua_setuservalue(state, 1);
#endif
        return 0;
    }
    if (!lua_istable(state, 2))
        return luaL_error(state, "axlua.setpeer expects a table or nil");
#if LUA_VERSION_NUM == 501 || defined(LUAJIT_VERSION)
    lua_pushvalue(state, 2);
    lua_setfenv(state, 1);
#elif LUA_VERSION_NUM >= 504
    lua_pushvalue(state, 2);
    lua_setiuservalue(state, 1, 1);
#else
    lua_pushvalue(state, 2);
    lua_setuservalue(state, 1);
#endif
    return 0;
}

bool push_class_table(lua_State* state, int userdataIndex)
{
    if (!lua_isuserdata(state, userdataIndex) || !lua_getmetatable(state, userdataIndex))
        return false;

    const void* metatableKey = lua_topointer(state, -1);
    lua_getfield(state, LUA_REGISTRYINDEX, "axlua.class.tables");
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 2);
        return false;
    }
    lua_pushlightuserdata(state, const_cast<void*>(metatableKey));
    lua_rawget(state, -2);
    lua_remove(state, -2);  // class table registry
    lua_remove(state, -2);  // userdata metatable
    return lua_istable(state, -1);
}

bool push_registered_base(lua_State* state, int classTableIndex)
{
    classTableIndex = absolute_index(state, classTableIndex);
    lua_getfield(state, LUA_REGISTRYINDEX, "axlua.class.bases");
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        return false;
    }
    lua_pushlightuserdata(state, const_cast<void*>(lua_topointer(state, classTableIndex)));
    lua_rawget(state, -2);
    lua_remove(state, -2);
    return lua_istable(state, -1);
}

enum class ClassMemberKind
{
    none,
    value,
    getter,
};

bool lookup_class_accessor(lua_State* state, int classTableIndex, int keyIndex, const char* accessorTable)
{
    classTableIndex = absolute_index(state, classTableIndex);
    keyIndex        = absolute_index(state, keyIndex);
    lua_getfield(state, classTableIndex, accessorTable);
    if (!lua_istable(state, -1))
    {
        lua_pop(state, 1);
        return false;
    }
    lua_pushvalue(state, keyIndex);
    lua_rawget(state, -2);
    lua_remove(state, -2);
    if (lua_isfunction(state, -1))
        return true;
    lua_pop(state, 1);
    return false;
}

ClassMemberKind lookup_class_member(lua_State* state, int classTableIndex, int keyIndex)
{
    classTableIndex = absolute_index(state, classTableIndex);
    keyIndex        = absolute_index(state, keyIndex);
    for (int depth = 0; depth < 64; ++depth)
    {
        lua_pushvalue(state, keyIndex);
        lua_gettable(state, classTableIndex);
        if (!lua_isnil(state, -1))
            return ClassMemberKind::value;
        lua_pop(state, 1);

        if (lookup_class_accessor(state, classTableIndex, keyIndex, ".get"))
            return ClassMemberKind::getter;

        if (!push_registered_base(state, classTableIndex))
            return ClassMemberKind::none;
        lua_remove(state, classTableIndex);
        classTableIndex = absolute_index(state, -1);
    }
    return ClassMemberKind::none;
}

bool lookup_class_setter(lua_State* state, int classTableIndex, int keyIndex)
{
    classTableIndex = absolute_index(state, classTableIndex);
    keyIndex        = absolute_index(state, keyIndex);
    for (int depth = 0; depth < 64; ++depth)
    {
        if (lookup_class_accessor(state, classTableIndex, keyIndex, ".set"))
            return true;
        if (!push_registered_base(state, classTableIndex))
            return false;
        lua_remove(state, classTableIndex);
        classTableIndex = absolute_index(state, -1);
    }
    return false;
}

bool lookup_registered_table_member(lua_State* state, int tableIndex, int keyIndex)
{
    tableIndex = absolute_index(state, tableIndex);
    keyIndex   = absolute_index(state, keyIndex);
    lua_pushvalue(state, keyIndex);
    lua_gettable(state, tableIndex);
    return !lua_isnil(state, -1);
}

bool lookup_base_member(lua_State* state, int classTableIndex, int keyIndex)
{
    classTableIndex = absolute_index(state, classTableIndex);
    keyIndex        = absolute_index(state, keyIndex);
    for (int depth = 0; depth < 64; ++depth)
    {
        if (!push_registered_base(state, classTableIndex))
            return false;
        classTableIndex = absolute_index(state, -1);
        if (lookup_registered_table_member(state, classTableIndex, keyIndex))
            return true;
    }
    return false;
}

void canonicalize_registered_userdata(lua_State* state, int firstResult)
{
    firstResult = absolute_index(state, firstResult);
    for (int index = firstResult; index <= lua_gettop(state); ++index)
    {
        if (!lua_isuserdata(state, index) || !lua_getmetatable(state, index))
            continue;

        const void* metatableKey = lua_topointer(state, -1);
        lua_getfield(state, LUA_REGISTRYINDEX, "axlua.class.pointer_tables");
        if (!lua_istable(state, -1))
        {
            lua_pop(state, 2);  // pointer registry and userdata metatable
            continue;
        }
        lua_pushlightuserdata(state, const_cast<void*>(metatableKey));
        lua_rawget(state, -2);
        const bool registered = lua_istable(state, -1);
        lua_pop(state, 2);  // class table and registry metatable
        if (!registered)
        {
            lua_pop(state, 1);  // userdata metatable
            continue;
        }

        // Pointer metatables may be materialized lazily by sol2 after class
        // registration. Install the Axmol dispatcher on the concrete
        // metatable as soon as the first factory result is observed.
        lua_pushcfunction(state, &axlua::class_index);
        lua_setfield(state, -2, "__index");
        lua_pushcfunction(state, &axlua::class_new_index);
        lua_setfield(state, -2, "__newindex");

        void* nativeObject = *static_cast<void**>(lua_touserdata(state, index));
        lua_pop(state, 1);  // userdata metatable
        if (nativeObject == nullptr)
            continue;

        lua_getfield(state, LUA_REGISTRYINDEX, "axlua.object.identity");
        if (!lua_istable(state, -1))
        {
            lua_pop(state, 1);
            lua_newtable(state);
            axlua::make_weak_value_table(state, -1);
            lua_pushvalue(state, -1);
            lua_setfield(state, LUA_REGISTRYINDEX, "axlua.object.identity");
        }
        const int identityTable = absolute_index(state, -1);
        lua_pushlightuserdata(state, nativeObject);
        lua_rawget(state, identityTable);
        if (lua_isuserdata(state, -1) && !lua_rawequal(state, -1, index))
        {
            lua_replace(state, index);
        }
        else
        {
            lua_pop(state, 1);
            lua_pushlightuserdata(state, nativeObject);
            lua_pushvalue(state, index);
            lua_rawset(state, identityTable);
        }
        lua_pop(state, 1);  // identity table
    }
}

int lua_isnull(lua_State* state)
{
    const bool invalid = axlua::is_invalid_userdata(state, 1);
    lua_pushboolean(state, lua_isnoneornil(state, 1) || invalid);
    return 1;
}

int lua_type_name(lua_State* state)
{
    if (!push_class_table(state, 1))
    {
        lua_pushstring(state, luaL_typename(state, 1));
        return 1;
    }
    lua_getfield(state, -1, "__axlua_name");
    lua_remove(state, -2);
    return 1;
}

int lua_iskindof(lua_State* state)
{
    const char* expected = luaL_checkstring(state, 2);
    if (!push_class_table(state, 1))
    {
        lua_pushboolean(state, 0);
        return 1;
    }

    for (int depth = 0; depth < 64 && lua_istable(state, -1); ++depth)
    {
        lua_getfield(state, -1, "__axlua_name");
        const bool matches = lua_isstring(state, -1) && std::strcmp(lua_tostring(state, -1), expected) == 0;
        lua_pop(state, 1);
        if (matches)
        {
            lua_settop(state, lua_gettop(state) - 1);
            lua_pushboolean(state, 1);
            return 1;
        }

        if (!push_registered_base(state, -1))
            break;
        lua_remove(state, -2);
    }
    lua_settop(state, lua_gettop(state) - 1);
    lua_pushboolean(state, 0);
    return 1;
}

int lua_cast(lua_State* state)
{
    if (lua_isnoneornil(state, 1))
    {
        lua_pushnil(state);
        return 1;
    }
    luaL_checktype(state, 1, LUA_TUSERDATA);
    const char* expected = luaL_checkstring(state, 2);
    if (!push_class_table(state, 1))
    {
        lua_pushnil(state);
        return 1;
    }

    bool matches = false;
    for (int depth = 0; depth < 64 && lua_istable(state, -1); ++depth)
    {
        lua_getfield(state, -1, "__axlua_name");
        matches = lua_isstring(state, -1) && std::strcmp(lua_tostring(state, -1), expected) == 0;
        lua_pop(state, 1);
        if (matches)
            break;
        if (!push_registered_base(state, -1))
            break;
        lua_remove(state, -2);
    }
    lua_settop(state, lua_gettop(state) - 1);
    if (!matches)
    {
        lua_pushnil(state);
        return 1;
    }
    lua_pushvalue(state, 1);
    return 1;
}

}  // namespace

namespace axlua
{

void register_object_pusher(lua_State* state,
                            std::string_view typeName,
                            const std::type_info& nativeType,
                            AdapterObjectPusher pusher)
{
    if (state == nullptr || typeName.empty() || pusher == nullptr)
        return;
    runtime_state().objectPushers[state][std::string(typeName)] = pusher;
    auto& dynamicPushers                                        = runtime_state().dynamicObjectPushers[state];
    dynamicPushers.byAddress[&nativeType]                       = pusher;
    dynamicPushers.byName[nativeType.name()]                    = pusher;
    register_native_type_name(nativeType, typeName);
}

bool push_registered_object(lua_State* state, void* object, std::string_view typeName)
{
    if (state == nullptr || object == nullptr || typeName.empty())
        return false;
    const auto stateEntry = runtime_state().objectPushers.find(state);
    if (stateEntry == runtime_state().objectPushers.end())
        return false;
    const auto pusherEntry = stateEntry->second.find(std::string(typeName));
    if (pusherEntry == stateEntry->second.end())
        return false;
    pusherEntry->second(state, object);
    return true;
}

void register_native_type_name(const std::type_info& nativeType, std::string_view luaTypeName)
{
    if (luaTypeName.empty())
        return;
    const std::string name(luaTypeName);
    runtime_state().nativeTypeNamesByAddress[&nativeType]    = name;
    runtime_state().nativeTypeNamesByName[nativeType.name()] = name;
}

const char* find_native_type_name(const std::type_info& nativeType)
{
    if (const auto found = runtime_state().nativeTypeNamesByAddress.find(&nativeType);
        found != runtime_state().nativeTypeNamesByAddress.end())
        return found->second.c_str();

    const auto fallback = runtime_state().nativeTypeNamesByName.find(nativeType.name());
    if (fallback == runtime_state().nativeTypeNamesByName.end())
        return nullptr;
    return runtime_state().nativeTypeNamesByAddress.emplace(&nativeType, fallback->second).first->second.c_str();
}

bool push_registered_dynamic_object(lua_State* state, ax::Object* object, const std::type_info& staticType)
{
    if (state == nullptr || object == nullptr)
        return false;

    const std::type_info& dynamicType = typeid(*object);
    if (dynamicType == staticType)
        return false;

    const auto stateEntry = runtime_state().dynamicObjectPushers.find(state);
    if (stateEntry == runtime_state().dynamicObjectPushers.end())
        return false;

    auto pusherEntry = stateEntry->second.byAddress.find(&dynamicType);
    if (pusherEntry == stateEntry->second.byAddress.end())
    {
        const auto fallback = stateEntry->second.byName.find(dynamicType.name());
        if (fallback == stateEntry->second.byName.end())
            return false;
        pusherEntry = stateEntry->second.byAddress.emplace(&dynamicType, fallback->second).first;
    }

    // dynamic_cast<void*> yields the complete object address even when the
    // statically returned pointer denotes a non-primary base subobject.
    pusherEntry->second(state, dynamic_cast<void*>(object));
    return true;
}

namespace detail
{
void BorrowedObjectScope::track(int index)
{
    if (_state == nullptr || !lua_isuserdata(_state, index))
        return;

    lua_pushvalue(_state, index);
    _references.push_back(luaL_ref(_state, LUA_REGISTRYINDEX));
}

void BorrowedObjectScope::invalidate() noexcept
{
    if (_state == nullptr)
        return;

    for (const int reference : _references)
    {
        lua_rawgeti(_state, LUA_REGISTRYINDEX, reference);
        invalidate_borrowed_userdata(_state, -1);
        lua_pop(_state, 1);
        luaL_unref(_state, LUA_REGISTRYINDEX, reference);
    }
    _references.clear();
}

void invalidate_borrowed_userdata(lua_State* state, int index)
{
    mark_userdata_invalid(state, index);
}

std::shared_ptr<LuaCallbackState> LuaCallbackState::create(lua_State* state, int index)
{
    if (state == nullptr || !lua_isfunction(state, index))
        return {};

    auto callback = std::shared_ptr<LuaCallbackState>(new LuaCallbackState(state));
    lua_pushvalue(state, index);
    callback->_ref    = luaL_ref(state, LUA_REGISTRYINDEX);
    callback->_active = callback->_ref != LUA_NOREF && callback->_ref != LUA_REFNIL;
    if (callback->_active)
        runtime_state().luaCallbacks[state].emplace_back(callback);
    return callback;
}

LuaCallbackState::~LuaCallbackState()
{
    invalidate();
}

void LuaCallbackState::invalidate() noexcept
{
    if (_active && _state != nullptr && _ref != LUA_NOREF && _ref != LUA_REFNIL)
        luaL_unref(_state, LUA_REGISTRYINDEX, _ref);
    _state  = nullptr;
    _ref    = LUA_NOREF;
    _active = false;
}

void LuaCallbackState::report_callback_error(const char* message)
{
    AXLOGE("[LUA CALLBACK ERROR] {}", message != nullptr ? message : "unknown Lua error");
}
}  // namespace detail

void shutdown_callbacks(lua_State* state)
{
    if (state == nullptr || AxluaRuntimeState::existing() == nullptr)
        return;

    auto found = runtime_state().luaCallbacks.find(state);
    if (found == runtime_state().luaCallbacks.end())
        return;

    for (auto& weakCallback : found->second)
    {
        if (auto callback = weakCallback.lock())
            callback->invalidate();
    }
    runtime_state().luaCallbacks.erase(found);
}

int class_static_index(lua_State* state)
{
    if (lua_isfunction(state, lua_upvalueindex(1)))
    {
        lua_pushvalue(state, lua_upvalueindex(1));
        lua_pushvalue(state, 1);
        lua_pushvalue(state, 2);
        if (lua_pcall(state, 2, 1, 0) != LUA_OK)
            return lua_error(state);
        if (!lua_isnil(state, -1))
            return 1;
        lua_pop(state, 1);
    }
    else if (lua_istable(state, lua_upvalueindex(1)))
    {
        lua_pushvalue(state, 2);
        lua_rawget(state, lua_upvalueindex(1));
        if (!lua_isnil(state, -1))
            return 1;
        lua_pop(state, 1);
    }

    if (lookup_base_member(state, 1, 2))
        return 1;
    lua_pushnil(state);
    return 1;
}

void install_class_static_index(lua_State* state, int classTableIndex)
{
    classTableIndex = absolute_index(state, classTableIndex);
    if (!lua_getmetatable(state, classTableIndex))
        return;
    const int metatableIndex = absolute_index(state, -1);
    lua_getfield(state, metatableIndex, "__axlua_original_index");
    if (!lua_isnil(state, -1))
    {
        lua_pop(state, 2);
        return;
    }
    lua_pop(state, 1);
    lua_getfield(state, metatableIndex, "__index");
    lua_pushvalue(state, -1);
    lua_setfield(state, metatableIndex, "__axlua_original_index");
    lua_pushcclosure(state, &class_static_index, 1);
    lua_setfield(state, metatableIndex, "__index");
    lua_pop(state, 1);
}

bool is_invalid_userdata(lua_State* state, int index)
{
    if (!lua_isuserdata(state, index))
        return false;
    push_user_environment(state, index);
    lua_getfield(state, -1, "__axlua_invalid");
    const bool invalid = lua_toboolean(state, -1) != 0;
    lua_pop(state, 2);
    return invalid;
}

void remember_object(ax::Object* object)
{
    if (object == nullptr)
        return;

    object->markScriptBindingExposed();

    auto* state = AxluaRuntimeState::existing();
    if (state == nullptr)
        return;

    std::lock_guard lock(state->nativeObjectsMutex);

    auto found = state->nativeObjects.find(object);
    if (found == state->nativeObjects.end() || found->second.expired())
    {
        state->nativeObjects[object]       = ax::WeakPtr<ax::Object>(object);
        state->nativeObjectThreads[object] = std::this_thread::get_id();
    }
}

bool object_expired(ax::Object* object)
{
    auto* state = AxluaRuntimeState::existing();
    if (state == nullptr)
        return true;

    std::lock_guard lock(state->nativeObjectsMutex);
    auto found         = state->nativeObjects.find(object);
    const bool expired = found != state->nativeObjects.end() && found->second.expired();
    if (expired)
    {
        state->nativeObjects.erase(object);
        state->nativeObjectThreads.erase(object);
    }
    return expired;
}

int class_index(lua_State* state)
{
    if (lua_istable(state, 1))
    {
        if (lookup_base_member(state, 1, 2))
            return 1;
        lua_pushnil(state);
        return 1;
    }
    if (!lua_isuserdata(state, 1))
    {
        lua_pushnil(state);
        return 1;
    }

    if (is_invalid_userdata(state, 1))
        return luaL_error(state, "attempt to access an expired Axmol object");

    push_user_environment(state, 1);
    lua_pushvalue(state, 2);
    lua_rawget(state, -2);
    if (!lua_isnil(state, -1))
    {
        lua_remove(state, -2);
        return 1;
    }
    lua_pop(state, 1);

    // Legacy Lua classes attach their methods to the peer table's metatable
    // (`__index = cls`).  Sol2 userdata reaches this dispatcher first, so a
    // raw lookup alone would hide ctor and every other Lua-side class method.
    // Preserve raw peer fields as the first priority, then honor that
    // metatable lookup before consulting the native generated class table.
    lua_pushvalue(state, 2);
    lua_gettable(state, -2);
    if (!lua_isnil(state, -1))
    {
        lua_remove(state, -2);
        return 1;
    }
    lua_pop(state, 2);

    if (push_class_table(state, 1))
    {
        switch (lookup_class_member(state, -1, 2))
        {
        case ClassMemberKind::value:
            return 1;
        case ClassMemberKind::getter:
            lua_pushvalue(state, 1);
            if (lua_pcall(state, 1, 1, 0) != LUA_OK)
                return lua_error(state);
            return 1;
        case ClassMemberKind::none:
            lua_pop(state, 1);
            break;
        }
    }
    lua_pushnil(state);
    return 1;
}

int class_new_index(lua_State* state)
{
    if (lua_istable(state, 1))
    {
        lua_pushvalue(state, 2);
        lua_pushvalue(state, 3);
        lua_rawset(state, 1);
        return 0;
    }
    if (!lua_isuserdata(state, 1))
        return luaL_error(state, "axlua class expects userdata");
    if (is_invalid_userdata(state, 1))
        return luaL_error(state, "attempt to modify an expired Axmol object");

    if (push_class_table(state, 1))
    {
        if (lookup_class_setter(state, -1, 2))
        {
            lua_pushvalue(state, 1);
            lua_pushvalue(state, 3);
            if (lua_pcall(state, 2, 0, 0) != LUA_OK)
                return lua_error(state);
            return 0;
        }
        lua_pop(state, 1);
    }

    push_user_environment(state, 1);
    lua_pushvalue(state, 2);
    lua_pushvalue(state, 3);
    lua_rawset(state, -3);
    lua_pop(state, 1);
    return 0;
}

int dispatch_callable(lua_State* state, int argumentCount, int contextUpvalue)
{
    lua_pushvalue(state, lua_upvalueindex(1));
    lua_insert(state, 1);
    if (lua_pcall(state, argumentCount, LUA_MULTRET, 0) != LUA_OK)
    {
        const char* context = lua_tostring(state, lua_upvalueindex(contextUpvalue));
        const char* message = lua_tostring(state, -1);
        return luaL_error(state, "%s: %s", context != nullptr ? context : "generated Lua binding",
                          message != nullptr ? message : "unknown error");
    }
    canonicalize_registered_userdata(state, 1);
    // lua_pcall leaves exactly the values returned by the native callable.
    // Preserve zero results for void methods instead of manufacturing nil.
    return lua_gettop(state);
}

int method_dispatch(lua_State* state)
{
    return dispatch_callable(state, lua_gettop(state), 2);
}

int static_method_dispatch(lua_State* state)
{
    int argumentCount = lua_gettop(state);
    if (argumentCount > 0 && lua_rawequal(state, 1, lua_upvalueindex(2)))
    {
        lua_remove(state, 1);
        --argumentCount;
    }
    return dispatch_callable(state, argumentCount, 3);
}

int dispatch_overload_callable(lua_State* state, int argumentStart, int contextUpvalue)
{
    const int callArgumentCount      = lua_gettop(state);
    const int signatureArgumentCount = callArgumentCount - argumentStart + 1;
    int selected                     = 0;

    for (int candidate = 1;; ++candidate)
    {
        lua_rawgeti(state, lua_upvalueindex(1), candidate);
        if (lua_isnil(state, -1))
        {
            lua_pop(state, 1);
            break;
        }

        const int candidateTable = absolute_index(state, -1);
        lua_rawgeti(state, candidateTable, 2);
        for (int argument = argumentStart; argument <= callArgumentCount; ++argument)
            lua_pushvalue(state, argument);
        if (lua_pcall(state, signatureArgumentCount, 1, 0) != LUA_OK)
        {
            const char* message = lua_tostring(state, -1);
            return luaL_error(state, "generated Lua overload matcher failed: %s",
                              message != nullptr ? message : "unknown error");
        }
        const bool matches = lua_toboolean(state, -1) != 0;
        lua_pop(state, 2);
        if (matches)
        {
            selected = candidate;
            break;
        }
    }

    if (selected == 0)
    {
        const char* context = lua_tostring(state, lua_upvalueindex(contextUpvalue));
        return luaL_error(state, "%s: no overload accepts the supplied Lua arguments",
                          context != nullptr ? context : "generated Lua binding");
    }

    lua_rawgeti(state, lua_upvalueindex(1), selected);
    lua_rawgeti(state, -1, 1);
    lua_remove(state, -2);
    lua_insert(state, 1);
    if (lua_pcall(state, callArgumentCount, LUA_MULTRET, 0) != LUA_OK)
    {
        const char* context = lua_tostring(state, lua_upvalueindex(contextUpvalue));
        const char* message = lua_tostring(state, -1);
        return luaL_error(state, "%s: %s", context != nullptr ? context : "generated Lua binding",
                          message != nullptr ? message : "unknown error");
    }
    canonicalize_registered_userdata(state, 1);
    return lua_gettop(state);
}

int overload_method_dispatch(lua_State* state)
{
    return dispatch_overload_callable(state, 2, 2);
}

int overload_static_method_dispatch(lua_State* state)
{
    if (lua_gettop(state) > 0 && lua_rawequal(state, 1, lua_upvalueindex(2)))
        lua_remove(state, 1);
    return dispatch_overload_callable(state, 1, 3);
}

Module Module::from(lua_State* state, std::string_view namespaceName, std::string_view typeNamespace)
{
    sol::state_view view(state);
    const std::string name(namespaceName);
    sol::object existing = view[name];
    sol::table module =
        existing.valid() && existing.is<sol::table>() ? existing.as<sol::table>() : view.create_named_table(name);
    const std::string effectiveTypeNamespace =
        typeNamespace.empty() ? std::string(namespaceName) : std::string(typeNamespace);
    return Module(view, module, std::string(namespaceName), effectiveTypeNamespace);
}

void install(lua_State* state)
{
    sol::state_view view(state);
    sol::object existing = view["axlua"];
    sol::table module =
        existing.valid() && existing.is<sol::table>() ? existing.as<sol::table>() : view.create_named_table("axlua");

    module.set_function("getpeer", &lua_getpeer);
    module.set_function("setpeer", &lua_setpeer);
    module.set_function("isnull", &lua_isnull);
    module.set_function("type", &lua_type_name);
    module.set_function("iskindof", &lua_iskindof);
    module.set_function("cast", &lua_cast);
}

void shutdown(lua_State* state)
{
    if (AxluaRuntimeState::existing() == nullptr)
        return;
    if (state != nullptr)
    {
        shutdown_callbacks(state);
        lua_pushnil(state);
        lua_setfield(state, LUA_REGISTRYINDEX, "axlua.object.identity");
        lua_pushnil(state);
        lua_setfield(state, LUA_REGISTRYINDEX, "axlua.class.tables");
        lua_pushnil(state);
        lua_setfield(state, LUA_REGISTRYINDEX, "axlua.class.pointer_tables");
        runtime_state().objectPushers.erase(state);
        runtime_state().dynamicObjectPushers.erase(state);
    }
    {
        std::lock_guard lock(runtime_state().nativeObjectsMutex);
        runtime_state().nativeObjects.clear();
        runtime_state().nativeObjectThreads.clear();
    }
}

void push_peer(lua_State* state, int userdataIndex)
{
    if (!lua_isuserdata(state, userdataIndex))
    {
        lua_pushnil(state);
        return;
    }
    push_user_environment(state, userdataIndex);
}

void set_peer(lua_State* state, int userdataIndex, int peerIndex)
{
#if LUA_VERSION_NUM == 501 || defined(LUAJIT_VERSION)
    lua_pushvalue(state, peerIndex);
    lua_setfenv(state, userdataIndex);
#elif LUA_VERSION_NUM >= 504
    lua_pushvalue(state, peerIndex);
    lua_setiuservalue(state, userdataIndex, 1);
#else
    lua_pushvalue(state, peerIndex);
    lua_setuservalue(state, userdataIndex);
#endif
}

void invalidate_object(lua_State* state, void* object)
{
    auto* nativeObject = static_cast<ax::Object*>(object);
    if (nativeObject == nullptr)
        return;

    // Director dispatches EVENT_DISPOSING before destroying the engine.  The
    // runtime state is already disposed at that point; never recreate it from
    // an Object destructor running during Director teardown.
    auto* runtime = AxluaRuntimeState::existing();
    if (runtime == nullptr || !nativeObject->hasScriptBindingExposure())
        return;

    // Avoid touching Lua for objects that were never exposed to this binding,
    // including objects destroyed from worker threads.
    {
        std::lock_guard lock(runtime->nativeObjectsMutex);
        if (runtime->nativeObjects.find(nativeObject) == runtime->nativeObjects.end())
            return;
        if (runtime->nativeObjectThreads[nativeObject] != std::this_thread::get_id())
        {
            // Lua is thread-confined.  Keep the weak entry so subsequent Lua
            // reads reject the expired userdata without touching the VM.
            return;
        }
    }
    if (state != nullptr)
    {
        invalidate_pointer_registry_entry(state, "axlua.object.identity", nativeObject);
        invalidate_pointer_registry_entry(state, axlua::adapter::kObjectBoxRegistry, nativeObject);
        invalidate_pointer_registry_entry(state, axlua::adapter::kValueRootRegistry, nativeObject);
        remove_pointer_registry_entry(state, axlua::adapter::kOwnedObjectRegistry, nativeObject);
    }
    {
        std::lock_guard lock(runtime->nativeObjectsMutex);
        runtime->nativeObjects.erase(nativeObject);
        runtime->nativeObjectThreads.erase(nativeObject);
    }
}

}  // namespace axlua
