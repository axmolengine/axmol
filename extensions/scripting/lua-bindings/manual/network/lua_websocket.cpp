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

#include "lua-bindings/manual/network/lua_websocket.h"
#include <map>
#include <string>
#include "lua-bindings/manual/tolua_fix.h"

#include "lua-bindings/manual/LuaStack.h"
#include "lua-bindings/manual/LuaValue.h"
#include "lua-bindings/manual/LuaEngine.h"
#include "lua-bindings/manual/LuaBasicConversions.h"
#include "lua-bindings/manual/base/LuaScriptHandlerMgr.h"

using namespace ax;

static int SendBinaryMessageToLua(int nHandler,const unsigned char* pTable,int nLength)
{
    if (NULL == pTable || nHandler <= 0) {
        return 0;
    }
    
    if (NULL == ScriptEngineManager::getInstance()->getScriptEngine()) {
        return 0;
    }
    
    LuaStack *pStack = LuaEngine::getInstance()->getLuaStack();
    if (NULL == pStack) {
        return 0;
    }

    lua_State *tolua_s = pStack->getLuaState();
    if (NULL == tolua_s) {
        return 0;
    }
    
    int nRet = 0;
    LuaValueArray array;
    for (int i = 0 ; i < nLength; i++) {
        LuaValue value = LuaValue::intValue(pTable[i]);
        array.push_back(value);
    }
    
    pStack->pushLuaValueArray(array);
    nRet = pStack->executeFunctionByHandler(nHandler, 1);
    pStack->clean();
    return nRet;
}



LuaWebSocket::~LuaWebSocket()
{
    ScriptHandlerMgr::getInstance()->removeObjectAllHandlers((void*)this);
}

void LuaWebSocket::onOpen(WebSocket* ws)
{
    LuaWebSocket* luaWs = dynamic_cast<LuaWebSocket*>(ws);
    if (NULL != luaWs) {
        int nHandler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)this,ScriptHandlerMgr::HandlerType::WEBSOCKET_OPEN);
        if (0 != nHandler) {
            CommonScriptData data(nHandler,"");
            ScriptEvent event(kCommonEvent,(void*)&data);
            ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(event);
        }
    }
}
    
void LuaWebSocket::onMessage(WebSocket* ws, const WebSocket::Data& data)
{
    LuaWebSocket* luaWs = dynamic_cast<LuaWebSocket*>(ws);
    if (NULL != luaWs) {
        if (data.isBinary) {
            int handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)this,ScriptHandlerMgr::HandlerType::WEBSOCKET_MESSAGE);
            if (0 != handler) {
                SendBinaryMessageToLua(handler, (const unsigned char*)data.bytes, (int)data.len);
            }
        }
        else{
                
            int handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)this,ScriptHandlerMgr::HandlerType::WEBSOCKET_MESSAGE);
            if (0 != handler)
            {
                LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
                if (nullptr != stack)
                {
                    stack->pushString(data.bytes,(int)data.len);
                    stack->executeFunctionByHandler(handler,  1);
                }
            }
        }
    }
}
    
void LuaWebSocket::onClose(WebSocket* ws)
{
    LuaWebSocket* luaWs = dynamic_cast<LuaWebSocket*>(ws);
    if (NULL != luaWs) {
        int nHandler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)this,ScriptHandlerMgr::HandlerType::WEBSOCKET_CLOSE);
        if (0 != nHandler)
        {
            CommonScriptData data(nHandler,"");
            ScriptEvent event(kCommonEvent,(void*)&data);
            ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(event);
        }
    }
}
    
void LuaWebSocket::onError(WebSocket* ws, const WebSocket::ErrorCode& error)
{
    LuaWebSocket* luaWs = dynamic_cast<LuaWebSocket*>(ws);
    if (NULL != luaWs) {
        int nHandler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)this,ScriptHandlerMgr::HandlerType::WEBSOCKET_ERROR);
        if (0 != nHandler)
        {
            CommonScriptData data(nHandler,"");
            ScriptEvent event(kCommonEvent,(void*)&data);
            ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(event);
        }
    }
}



#ifdef __cplusplus
static int tolua_collect_WebSocket (lua_State* tolua_S)
{
    LuaWebSocket* self = (LuaWebSocket*) tolua_tousertype(tolua_S,1,0);
    Mtolua_delete(self);
    return 0;
}
#endif
/* function to release collected object via destructor */
static void tolua_reg_Web_Socket_type(lua_State* tolua_S)
{
    tolua_usertype(tolua_S, "ax.WebSocket");
}

/* method: create of class WebSocket */
#ifndef TOLUA_DISABLE_axlua_WebSocket_create00
static int axlua_WebSocket_create00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
#endif

    int argumentCount = lua_gettop(tolua_S);
    if (argumentCount >= 2)
    {
        std::string_view url;
        const char* protocols{nullptr};
        std::string_view caCertPath;

#ifndef TOLUA_RELEASE
        if (!tolua_isusertable(tolua_S,1,"ax.WebSocket",0,&tolua_err) ||
            !tolua_isstring(tolua_S,2,0,&tolua_err)
            )
            goto tolua_lerror;
#endif
        if (argumentCount == 3)
        {
            if (lua_isstring(tolua_S, 3))
            {
                luaval_to_std_string_view(tolua_S, 3, &caCertPath);
            }
            else
            {
                luaL_error(tolua_S, "The second parameter should be a table contains protocols of string type or a string indicates ca certificate path!");
                return 0;
            }
        }
        else if (argumentCount == 4)
        {
            luaval_to_std_string_view(tolua_S, 3, &caCertPath);
            protocols = lua_tostring(tolua_S, 4);
        }

        luaval_to_std_string_view(tolua_S, 2, &url);

        LuaWebSocket *wSocket = new (std::nothrow) LuaWebSocket();
        wSocket->open(wSocket, url, caCertPath, protocols);
        tolua_pushusertype(tolua_S,(void*)wSocket,"ax.WebSocket");
        tolua_register_gc(tolua_S,lua_gettop(tolua_S));
        return 1;
    }
#ifndef TOLUA_RELEASE
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'create'.",&tolua_err);
#endif
    return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: getReadyState of class WebSocket */
#ifndef TOLUA_DISABLE_axlua_WebSocket_getReadyState00
static int axlua_WebSocket_getReadyState00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (
        !tolua_isusertype(tolua_S,1,"ax.WebSocket",0,&tolua_err) ||
        !tolua_isnoobj(tolua_S,2,&tolua_err)
        )
        goto tolua_lerror;
    else
#endif
    {
        LuaWebSocket *self = (LuaWebSocket*)tolua_tousertype(tolua_S,1,0);
        int tolua_ret = -1;
        if (NULL != self) {
            tolua_ret = (int)self->getReadyState();
        }
        tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
    }
    return 1;
#ifndef TOLUA_RELEASE
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'getReadyState'.",&tolua_err);
    return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE


/* method: close of class WebSocket */
#ifndef TOLUA_DISABLE_axlua_WebSocket_close00
static int axlua_WebSocket_close00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (
        !tolua_isusertype(tolua_S,1,"ax.WebSocket",0,&tolua_err) ||
        !tolua_isnoobj(tolua_S,2,&tolua_err)
        )
        goto tolua_lerror;
    else
#endif
    {
        LuaWebSocket* self    = (LuaWebSocket*)  tolua_tousertype(tolua_S,1,0);
        if (NULL != self ) {
            self->closeAsync();
        }
    }
    return 0;
#ifndef TOLUA_RELEASE
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'getReadyState'.",&tolua_err);
    return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: sendString of class WebSocket */
#ifndef TOLUA_DISABLE_axlua_WebSocket_sendString00
static int axlua_WebSocket_sendString00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (
        !tolua_isusertype(tolua_S, 1, "ax.WebSocket", 0, &tolua_err) ||
        !tolua_isstring(tolua_S, 2, 0, &tolua_err)                ||
        !tolua_isnoobj(tolua_S, 3, &tolua_err)
        )
        goto tolua_lerror;
    else
#endif
    {
        LuaWebSocket* self    = (LuaWebSocket*)  tolua_tousertype(tolua_S,1,0);
        size_t size = 0;
        const char* data = (const char*) lua_tolstring(tolua_S, 2, &size);
        if ( NULL == data)
            return 0;

        if (strlen(data) != size)
        {
            self->send((const unsigned char*)data, (unsigned int)size);
        }
        else
        {
            self->send(data);
        }
    }
    return 0;
#ifndef TOLUA_RELEASE
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'sendString'.",&tolua_err);
    return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

static int websocket_url_getter(lua_State* L)
{
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.WebSocket", 0, &tolua_err))
        goto tolua_lerror;
    else
#endif
    {
        LuaWebSocket* self = (LuaWebSocket*)tolua_tousertype(L, 1, 0);
        tolua_pushsv(L, self->getUrl());
        return 1;
    }
#ifndef TOLUA_RELEASE
tolua_lerror:
    tolua_error(L,"#ferror in function 'sendString'.",&tolua_err);
    return 0;
#endif
}

static int websocket_protocol_getter(lua_State* L)
{
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.WebSocket", 0, &tolua_err))
        goto tolua_lerror;
    else
#endif
    {
        LuaWebSocket* self = (LuaWebSocket*)tolua_tousertype(L, 1, 0);
        tolua_pushsv(L, self->getProtocol());
        return 1;
    }
#ifndef TOLUA_RELEASE
tolua_lerror:
    tolua_error(L,"#ferror in function 'sendString'.",&tolua_err);
    return 0;
#endif
}

TOLUA_API int tolua_web_socket_open(lua_State* tolua_S){
    tolua_open(tolua_S);
    tolua_reg_Web_Socket_type(tolua_S);
    tolua_module(tolua_S,"ax",0);
    tolua_beginmodule(tolua_S,"ax");
      #ifdef __cplusplus
      tolua_cclass(tolua_S,"WebSocket","ax.WebSocket","",tolua_collect_WebSocket);
      #else
      tolua_cclass(tolua_S,"WebSocket","ax.WebSocket","",NULL);
      #endif
      tolua_beginmodule(tolua_S,"WebSocket");
        tolua_function(tolua_S, "create", axlua_WebSocket_create00);
        tolua_function(tolua_S, "getReadyState", axlua_WebSocket_getReadyState00);
        tolua_function(tolua_S, "close", axlua_WebSocket_close00);
        tolua_function(tolua_S, "sendString", axlua_WebSocket_sendString00);
        tolua_variable(tolua_S, "url", websocket_url_getter, nullptr);
        tolua_variable(tolua_S, "protocol", websocket_protocol_getter, nullptr);
      tolua_endmodule(tolua_S);
    tolua_endmodule(tolua_S);
	return 1;
}

int axlua_WebSocket_registerScriptHandler00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (
        !tolua_isusertype(tolua_S,1,"ax.WebSocket",0,&tolua_err) ||
        !toluafix_isfunction(tolua_S,2,"LUA_FUNCTION",0,&tolua_err) ||
        !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
        !tolua_isnoobj(tolua_S,4,&tolua_err)
        )
        goto tolua_lerror;
    else
#endif
    {
        LuaWebSocket* self    = (LuaWebSocket*)  tolua_tousertype(tolua_S,1,0);
        if (NULL != self ) {
            int handler = (  toluafix_ref_function(tolua_S,2,0));
            ScriptHandlerMgr::HandlerType handlerType = (ScriptHandlerMgr::HandlerType)((int)tolua_tonumber(tolua_S,3,0) + (int)ScriptHandlerMgr::HandlerType::WEBSOCKET_OPEN);
            ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, handlerType);
        }
    }
    return 0;
#ifndef TOLUA_RELEASE
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'registerScriptHandler'.",&tolua_err);
    return 0;
#endif
}

int axlua_WebSocket_unregisterScriptHandler00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
    tolua_Error tolua_err;
    if (
        !tolua_isusertype(tolua_S,1,"ax.WebSocket",0,&tolua_err) ||
        !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
        !tolua_isnoobj(tolua_S,3,&tolua_err)
        )
        goto tolua_lerror;
    else
#endif
    {
        LuaWebSocket* self    = (LuaWebSocket*)  tolua_tousertype(tolua_S,1,0);
        if (NULL != self ) {
            ScriptHandlerMgr::HandlerType handlerType = (ScriptHandlerMgr::HandlerType)((int)tolua_tonumber(tolua_S,2,0) + (int)ScriptHandlerMgr::HandlerType::WEBSOCKET_OPEN);
            
            ScriptHandlerMgr::getInstance()->removeObjectHandler((void*)self, handlerType);
        }
    }
    return 0;
#ifndef TOLUA_RELEASE
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'unregisterScriptHandler'.",&tolua_err);
    return 0;
#endif
}

TOLUA_API int register_web_socket_manual(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0 ;
    
    lua_pushstring(tolua_S,"ax.WebSocket");
    lua_rawget(tolua_S,LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S,-1))
    {
        lua_pushstring(tolua_S,"registerScriptHandler");
        lua_pushcfunction(tolua_S,axlua_WebSocket_registerScriptHandler00);
        lua_rawset(tolua_S,-3);
        lua_pushstring(tolua_S,"unregisterScriptHandler");
        lua_pushcfunction(tolua_S,axlua_WebSocket_unregisterScriptHandler00);
        lua_rawset(tolua_S,-3);
    }
    lua_pop(tolua_S, 1);
    
    return 1;
}
