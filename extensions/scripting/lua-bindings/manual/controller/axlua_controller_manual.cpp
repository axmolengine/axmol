/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
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
#include "lua-bindings/manual/controller/axlua_controller_manual.hpp"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS)

#    include "lua-bindings/manual/tolua_fix.h"
#    include "lua-bindings/manual/LuaBasicConversions.h"
#    include "lua-bindings/manual/base/LuaScriptHandlerMgr.h"
#    include "lua-bindings/manual/LuaValue.h"
#    include "lua-bindings/manual/LuaEngine.h"
#    include "base/GameController.h"

static void cloneEventListenerControllerHandler(const ax::EventListenerController* src,
                                                ax::EventListenerController* dst,
                                                ScriptHandlerMgr::HandlerType type)
{
    if (nullptr == src || nullptr == dst)
        return;

    LUA_FUNCTION handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)src, type);
    if (0 != handler)
    {
        int newscriptHandler = ScriptEngineManager::getInstance()->getScriptEngine()->reallocateScriptHandler(handler);

        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)dst, newscriptHandler, type);
        switch (type)
        {
        case ScriptHandlerMgr::HandlerType::EVENT_CONTROLLER_CONNECTED:
        {
            dst->onConnected = [=](ax::Controller* controller, Event* event) {

            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_CONTROLLER_DISCONNECTED:
        {
            dst->onDisconnected = [=](ax::Controller* controller, Event* event) {

            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_CONTROLLER_KEYDOWN:
        {
            dst->onKeyDown = [=](ax::Controller* controller, int keyCode, Event* event) {

            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_CONTROLLER_KEYUP:
        {
            dst->onKeyUp = [=](ax::Controller* controller, int keyCode, Event* event) {

            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_CONTROLLER_KEYREPEAT:
        {
            dst->onKeyRepeat = [=](ax::Controller* controller, int keyCode, Event* event) {};
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_CONTROLLER_AXIS:
        {
            dst->onAxisEvent = [=](ax::Controller* controller, int keyCode, Event* event) {};
        }
        break;
        default:
            break;
        }
    }
}

static int toaxlua_EventListenerController_clone(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc                               = 0;
    ax::EventListenerController* self = nullptr;
#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.EventListenerController", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    self = static_cast<ax::EventListenerController*>(tolua_tousertype(tolua_S, 1, 0));
#    if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'toaxlua_EventListenerController_clone'\n", nullptr);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        ax::EventListenerController* tolua_ret = ax::EventListenerController::create();
        if (nullptr == tolua_ret)
            return 0;

        cloneEventListenerControllerHandler(self, tolua_ret, ScriptHandlerMgr::HandlerType::EVENT_CONTROLLER_CONNECTED);
        cloneEventListenerControllerHandler(self, tolua_ret,
                                            ScriptHandlerMgr::HandlerType::EVENT_CONTROLLER_DISCONNECTED);
        cloneEventListenerControllerHandler(self, tolua_ret, ScriptHandlerMgr::HandlerType::EVENT_CONTROLLER_KEYDOWN);
        cloneEventListenerControllerHandler(self, tolua_ret, ScriptHandlerMgr::HandlerType::EVENT_CONTROLLER_KEYUP);
        cloneEventListenerControllerHandler(self, tolua_ret, ScriptHandlerMgr::HandlerType::EVENT_CONTROLLER_KEYREPEAT);
        cloneEventListenerControllerHandler(self, tolua_ret, ScriptHandlerMgr::HandlerType::EVENT_CONTROLLER_AXIS);

        int ID     = (int)tolua_ret->_ID;
        int* luaID = &tolua_ret->_luaID;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)tolua_ret, "ax.EventListenerController");

        return 1;
    }

    AXLOG("'clone' has wrong number of arguments: %d, was expecting %d\n", argc, 0);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'clone'.", &tolua_err);
    return 0;
#    endif
}

static int toaxlua_EventListenerController_registerScriptHandler(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc                               = 0;
    ax::EventListenerController* self = nullptr;
#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.EventListenerController", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    self = static_cast<ax::EventListenerController*>(tolua_tousertype(tolua_S, 1, 0));
#    if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S,
                    "invalid 'self' in function 'toaxlua_EventListenerController_registerScriptHandler'\n",
                    nullptr);
        return 0;
    }
#    endif
    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
#    if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err) ||
            !tolua_isnumber(tolua_S, 3, 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#    endif
        LUA_FUNCTION handler = toluafix_ref_function(tolua_S, 2, 0);
        ScriptHandlerMgr::HandlerType type =
            static_cast<ScriptHandlerMgr::HandlerType>((int)tolua_tonumber(tolua_S, 3, 0));

        auto stack = LuaEngine::getInstance()->getLuaStack();
        auto Ls    = stack->getLuaState();

        switch (type)
        {
        case ScriptHandlerMgr::HandlerType::EVENT_CONTROLLER_CONNECTED:
        {
            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);

            self->onConnected = [=](ax::Controller* controller, Event* event) {
                tolua_pushusertype(Ls, (void*)controller, "ax.Controller");
                int ID     = (event) ? (int)event->_ID : -1;
                int* luaID = (event) ? &event->_luaID : NULL;
                toluafix_pushusertype_object(Ls, ID, luaID, (void*)event, "ax.event");
                stack->executeFunctionByHandler(handler, 2);
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_CONTROLLER_DISCONNECTED:
        {
            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);

            self->onDisconnected = [=](ax::Controller* controller, Event* event) {
                tolua_pushusertype(Ls, (void*)controller, "ax.Controller");
                int ID     = (event) ? (int)event->_ID : -1;
                int* luaID = (event) ? &event->_luaID : NULL;
                toluafix_pushusertype_object(Ls, ID, luaID, (void*)event, "ax.event");
                stack->executeFunctionByHandler(handler, 2);
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_CONTROLLER_KEYDOWN:
        {

            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);

            self->onKeyDown = [=](ax::Controller* controller, int keyCode, Event* event) {
                tolua_pushusertype(Ls, (void*)controller, "ax.Controller");
                tolua_pushnumber(Ls, (lua_Number)keyCode);
                int ID     = (event) ? (int)event->_ID : -1;
                int* luaID = (event) ? &event->_luaID : NULL;
                toluafix_pushusertype_object(Ls, ID, luaID, (void*)event, "ax.event");
                stack->executeFunctionByHandler(handler, 3);
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_CONTROLLER_KEYUP:
        {
            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);

            self->onKeyUp = [=](ax::Controller* controller, int keyCode, Event* event) {
                tolua_pushusertype(Ls, (void*)controller, "ax.Controller");
                tolua_pushnumber(Ls, (lua_Number)keyCode);
                int ID     = (event) ? (int)event->_ID : -1;
                int* luaID = (event) ? &event->_luaID : NULL;
                toluafix_pushusertype_object(Ls, ID, luaID, (void*)event, "ax.event");
                stack->executeFunctionByHandler(handler, 3);
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_CONTROLLER_KEYREPEAT:
        {
            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);

            self->onKeyRepeat = [=](ax::Controller* controller, int keyCode, Event* event) {
                tolua_pushusertype(Ls, (void*)controller, "ax.Controller");
                tolua_pushnumber(Ls, (lua_Number)keyCode);
                int ID     = (event) ? (int)event->_ID : -1;
                int* luaID = (event) ? &event->_luaID : NULL;
                toluafix_pushusertype_object(Ls, ID, luaID, (void*)event, "ax.event");
                stack->executeFunctionByHandler(handler, 3);
            };
        }
        break;
        case ScriptHandlerMgr::HandlerType::EVENT_CONTROLLER_AXIS:
        {
            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, type);

            self->onAxisEvent = [=](ax::Controller* controller, int keyCode, Event* event) {
                tolua_pushusertype(Ls, (void*)controller, "ax.Controller");
                tolua_pushnumber(Ls, (lua_Number)keyCode);
                int ID     = (event) ? (int)event->_ID : -1;
                int* luaID = (event) ? &event->_luaID : NULL;
                toluafix_pushusertype_object(Ls, ID, luaID, (void*)event, "ax.event");
                stack->executeFunctionByHandler(handler, 3);
            };
        }
        break;
        default:
            break;
        }
        return 0;
    }

    AXLOG("'registerScriptHandler' has wrong number of arguments: %d, was expecting %d\n", argc, 2);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'registerScriptHandler'.", &tolua_err);
    return 0;
#    endif
}

static void extendEventListenerController(lua_State* L)
{
    lua_pushstring(L, "ax.EventListenerController");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "clone", toaxlua_EventListenerController_clone);
        tolua_function(L, "registerScriptHandler", toaxlua_EventListenerController_registerScriptHandler);
    }
    lua_pop(L, 1);
}

static int toaxlua_Controller_getKeyStatus(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;

    int argc                  = 0;
    ax::Controller* self = nullptr;
#    if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S, 1, "ax.Controller", 0, &tolua_err))
        goto tolua_lerror;
#    endif

    self = static_cast<ax::Controller*>(tolua_tousertype(tolua_S, 1, 0));
#    if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S, "invalid 'self' in function 'toaxlua_Controller_getKeyStatus'\n", nullptr);
        return 0;
    }
#    endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
#    if _AX_DEBUG >= 1
        if (!tolua_isnumber(tolua_S, 2, 0, &tolua_err))
            goto tolua_lerror;
#    endif
        int keyCode                              = (int)tolua_tonumber(tolua_S, 2, 0);
        ax::Controller::KeyStatus keyStatus = self->getKeyStatus(keyCode);

        lua_newtable(tolua_S);                                /* L: table */
        lua_pushstring(tolua_S, "isPressed");                 /* L: table key */
        lua_pushboolean(tolua_S, (int)keyStatus.isPressed);   /* L: table key value*/
        lua_rawset(tolua_S, -3);                              /* table[key] = value, L: table */
        lua_pushstring(tolua_S, "value");                     /* L: table key */
        lua_pushnumber(tolua_S, (lua_Number)keyStatus.value); /* L: table key value*/
        lua_rawset(tolua_S, -3);                              /* table[key] = value, L: table */
        lua_pushstring(tolua_S, "isAnalog");                  /* L: table key */
        lua_pushnumber(tolua_S, (int)keyStatus.isAnalog);     /* L: table key value*/
        lua_rawset(tolua_S, -3);                              /* table[key] = value, L: table */

        return 1;
    }

    AXLOG("'clone' has wrong number of arguments: %d, was expecting %d\n", argc, 0);
    return 0;

#    if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'getKeyStatus'.", &tolua_err);
    return 0;
#    endif
}

static void extendController(lua_State* L)
{
    lua_pushstring(L, "ax.Controller");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "getKeyStatus", toaxlua_Controller_getKeyStatus);
    }
    lua_pop(L, 1);
}

int register_all_ax_controller_manual(lua_State* L)
{
    if (nullptr == L)
        return 0;

    extendEventListenerController(L);
    extendController(L);

    return 0;
}

#endif  //#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
