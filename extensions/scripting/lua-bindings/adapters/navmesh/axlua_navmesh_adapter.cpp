/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
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
#include "axmol/platform/PlatformConfig.h"
#include "axmol/base/Config.h"
#if defined(AX_ENABLE_NAVMESH)
#    include "axlua_navmesh_adapter.h"
#    include "lua-bindings/generated/axlua_navmesh_gen.h"
#    include "lua-bindings/runtime/axlua_adapter.h"
#    include "lua-bindings/runtime/axlua_conversions.h"
#    include "lua-bindings/runtime/LuaEngine.h"
#    include "axmol/navmesh/NavMesh.h"

int axlua_navmesh_NavMeshAgent_move(lua_State* luaState)
{
    int argc              = 0;
    ax::NavMeshAgent* obj = nullptr;
    bool ok               = true;

#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif

#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.NavMeshAgent", 0, &conversionError))
        goto argumentError;
#    endif

    obj = (ax::NavMeshAgent*)axlua::adapter::to_usertype(luaState, 1, 0);

#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_navmesh_NavMeshAgent_move'", nullptr);
        return 0;
    }
#    endif

    argc = lua_gettop(luaState) - 1;
    if (argc == 1)
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(luaState, 2, &arg0, "ax.NavMeshAgent:move");
        if (!ok)
        {
            axlua::adapter::raise_error(luaState, "invalid arguments in function 'axlua_navmesh_NavMeshAgent_move'", nullptr);
            return 0;
        }
        obj->move(arg0);
        lua_settop(luaState, 1);
        return 1;
    }
    if (argc == 2)
    {
        ax::Vec3 arg0;
        LUA_FUNCTION handler;

        ok &= luaval_to_vec3(luaState, 2, &arg0, "ax.NavMeshAgent:move");

#    if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(luaState, 3, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#    endif
        handler = axlua::adapter::ref_function(luaState, 3, 0);

        if (!ok)
        {
            axlua::adapter::raise_error(luaState, "invalid arguments in function 'axlua_navmesh_NavMeshAgent_move'", nullptr);
            return 0;
        }
        obj->move(arg0, [=](ax::NavMeshAgent* agent, float totalTimeAfterMove) {
            auto stack = LuaEngine::getInstance()->getLuaStack();
            auto Ls    = stack->getLuaState();
            object_to_luaval<ax::NavMeshAgent>(Ls, "ax.NavMeshAgent", (ax::NavMeshAgent*)agent);
            axlua::adapter::push_number(Ls, (lua_Number)totalTimeAfterMove);
            stack->executeFunctionByHandler(handler, 2);
        });
        AxluaCallbackRegistry::getInstance()->addCustomHandler((void*)obj, handler);
        lua_settop(luaState, 1);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.NavMeshAgent:move", argc, 1);
    return 0;

#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_navmesh_NavMeshAgent_move'.", &conversionError);
#    endif

    return 0;
}

static void extendNavMeshAgent(lua_State* L)
{
    lua_pushstring(L, "ax.NavMeshAgent");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "move", axlua_navmesh_NavMeshAgent_move);
    }
    lua_pop(L, 1);
}

static int register_all_navmesh_adapter(lua_State* L)
{
    if (nullptr == L)
        return 0;

    extendNavMeshAgent(L);

    return 1;
}

int register_navmesh_module(lua_State* L)
{
    lua_getglobal(L, "_G");
    if (lua_istable(L, -1))  // stack:...,_G,
    {
        register_all_ax_navmesh(L);
        register_all_navmesh_adapter(L);
    }
    lua_pop(L, 1);
    return 1;
}

#endif
