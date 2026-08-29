/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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
#include "lua-bindings/adapters/sceneio/axlua_sceneio_adapter.hpp"
#include "lua-bindings/generated/axlua_sceneio_gen.h"
#include "lua-bindings/runtime/axlua_adapter.h"
#include "lua-bindings/runtime/axlua_conversions.h"
#include "lua-bindings/runtime/LuaValue.h"
#include "lua-bindings/runtime/LuaEngine.h"
#include "sceneext/ActionTimeline/ActionTimeline.h"
#include "sceneext/ActionTimeline/ActionTimelineNode.h"
#include "sceneio/ActionTimeline/CSLoader.h"

int axlua_sceneio_CSLoader_createTimeline(lua_State* luaState)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertable(luaState, 1, "axext.CSLoader", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;

    if (argc == 1)
    {
        std::string arg0;
        ok &= luaval_to_std_string(luaState, 2, &arg0, "axext.CSLoader:createTimeline");
        if (!ok)
            return 0;
        auto ret = ax::CSLoader::createTimeline(arg0);
        object_to_luaval<ax::ext::timeline::ActionTimeline>(luaState, "axext.ActionTimeline",
                                                            (ax::ext::timeline::ActionTimeline*)ret);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n ", "axext.CSLoader:createTimeline",
               argc, 1);
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_CSLoader_createTimeline'.", &conversionError);
#endif
    return 0;
}

int axlua_sceneio_CSLoader_createNode(lua_State* luaState)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertable(luaState, 1, "axext.CSLoader", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(luaState) - 1;

    do
    {
        if (argc == 2)
        {
            std::string filename;
            ok &= luaval_to_std_string(luaState, 2, &filename, "axext.CSLoader:createNode");
            if (!ok)
            {
                break;
            }

#if _AX_DEBUG >= 1
            if (!axlua::adapter::is_function(luaState, 3, "LUA_FUNCTION", 0, &conversionError))
            {
                goto argumentError;
            }
#endif

            LUA_FUNCTION handler = (axlua::adapter::ref_function(luaState, 3, 0));
            auto callback        = [handler](ax::Object* ref) {
                if (nullptr == ref)
                    return;
                auto stack = LuaEngine::getInstance()->getLuaStack();
                axlua::adapter::push_object(stack->getLuaState(), (void*)ref,
                                                    "axext.Object");
                stack->executeFunctionByHandler(handler, 1);
            };

            ax::Node* ret = ax::CSLoader::createNode(filename, callback);
            AxluaCallbackRegistry::getInstance()->addCustomHandler((void*)ret, handler);
            object_to_luaval<ax::Node>(luaState, "axext.Node", (ax::Node*)ret);
            return 1;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 1)
        {
            std::string filename;
            ok &= luaval_to_std_string(luaState, 2, &filename, "axext.CSLoader:createNode");
            if (!ok)
            {
                break;
            }
            ax::Node* ret = ax::CSLoader::createNode(filename);
            object_to_luaval<ax::Node>(luaState, "axext.Node", (ax::Node*)ret);
            return 1;
        }
    } while (0);
    ok = true;
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d", "axext.CSLoader:createNode", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_sceneio_CSLoader_createNode'.", &conversionError);
#endif
    return 0;
}

int register_all_ax_sceneio_adapter(lua_State* L)
{
    lua_pushstring(L, "axext.CSLoader");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "createTimeline", axlua_sceneio_CSLoader_createTimeline);
        axlua::adapter::set_function(L, "createNode", axlua_sceneio_CSLoader_createNode);
    }
    lua_pop(L, 1);

    return 0;
}

int register_sceneio_module(lua_State* L)
{
    lua_getglobal(L, "_G");
    if (lua_istable(L, -1))  // stack:...,_G,
    {
        register_all_ax_sceneio(L);
        register_all_ax_sceneio_adapter(L);
    }
    lua_pop(L, 1);
    return 1;
}
