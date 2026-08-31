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

#include "lua-bindings/adapters/network/lua_websocket.h"
#include <map>
#include <string>
#include "lua-bindings/runtime/axlua_adapter.h"
#include "lua-bindings/runtime/LuaValue.h"

#include "lua-bindings/runtime/axlua_conversions.h"

using namespace ax;

namespace
{
}  // namespace

LuaWebSocket::~LuaWebSocket() = default;

void LuaWebSocket::onOpen(WebSocket* ws)
{
    LuaWebSocket* luaWs = dynamic_cast<LuaWebSocket*>(ws);
    if (luaWs)
    {
        onOpenCallback();
    }
}

void LuaWebSocket::onMessage(WebSocket* ws, const WebSocket::Data& data)
{
    LuaWebSocket* luaWs = dynamic_cast<LuaWebSocket*>(ws);
    if (luaWs)
    {
        if (onMessageCallback)
            onMessageCallback(std::string_view(data.bytes, data.len), data.isBinary);
    }
}

void LuaWebSocket::onClose(WebSocket* ws, uint16_t code, std::string_view reason)
{
    LuaWebSocket* luaWs = dynamic_cast<LuaWebSocket*>(ws);
    if (luaWs)
    {
        onCloseCallback();
    }
}

void LuaWebSocket::onError(WebSocket* ws, const WebSocket::ErrorCode& error)
{
    LuaWebSocket* luaWs = dynamic_cast<LuaWebSocket*>(ws);
    if (luaWs)
    {
        onErrorCallback();
    }
}

static int collect_web_socket(lua_State* luaState)
{
    LuaWebSocket* self = (LuaWebSocket*)axlua::adapter::to_usertype(luaState, 1, 0);
    delete self;
    return 0;
}
/* function to release collected object via destructor */
static void register_web_socket_type(lua_State* luaState)
{
    axlua::adapter::register_usertype(luaState, "ax.WebSocket");
}

/* method: create of class WebSocket */
static int axlua_WebSocket_create00(lua_State* luaState)
{
    axlua::adapter::Error conversionError;

    int argumentCount = lua_gettop(luaState);
    if (argumentCount >= 2)
    {
        std::string_view url;
        std::string_view protocols = ""sv;
        std::string_view caCertPath;

        if (!axlua::adapter::is_usertable(luaState, 1, "ax.WebSocket", 0, &conversionError) ||
            !axlua::adapter::is_string(luaState, 2, 0, &conversionError))
            goto argumentError;
        if (argumentCount == 3)
        {
            if (lua_isstring(luaState, 3))
            {
                luaval_to_std_string_view(luaState, 3, &caCertPath);
            }
            else
            {
                luaL_error(luaState,
                           "The second parameter should be a table contains protocols of string type or a string "
                           "indicates ca certificate path!");
                return 0;
            }
        }
        else if (argumentCount == 4)
        {
            luaval_to_std_string_view(luaState, 3, &caCertPath);
            protocols = axlua_tosv(luaState, 4);
        }

        luaval_to_std_string_view(luaState, 2, &url);

        LuaWebSocket* wSocket = new (std::nothrow) LuaWebSocket();
        wSocket->open(wSocket, url, caCertPath, protocols);
        axlua::adapter::push_usertype(luaState, (void*)wSocket, "ax.WebSocket");
        axlua::adapter::register_gc(luaState, lua_gettop(luaState));
        return 1;
    }
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'create'.", &conversionError);
    return 0;
}

/* method: getReadyState of class WebSocket */
static int axlua_WebSocket_getReadyState00(lua_State* luaState)
{
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.WebSocket", 0, &conversionError) ||
        !axlua::adapter::is_no_object(luaState, 2, &conversionError))
        goto argumentError;
    else
    {
        LuaWebSocket* self = (LuaWebSocket*)axlua::adapter::to_usertype(luaState, 1, 0);
        int returnValue    = -1;
        if (self)
        {
            returnValue = (int)self->getReadyState();
        }
        lua_pushnumber(luaState, (lua_Number)returnValue);
    }
    return 1;
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'getReadyState'.", &conversionError);
    return 0;
}

/* method: close of class WebSocket */
static int axlua_WebSocket_close00(lua_State* luaState)
{
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.WebSocket", 0, &conversionError) ||
        !axlua::adapter::is_no_object(luaState, 2, &conversionError))
        goto argumentError;
    else
    {
        LuaWebSocket* self = (LuaWebSocket*)axlua::adapter::to_usertype(luaState, 1, 0);
        if (self)
        {
            self->closeAsync();
        }
    }
    return 0;
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'getReadyState'.", &conversionError);
    return 0;
}

/* method: sendString of class WebSocket */
static int axlua_WebSocket_sendString00(lua_State* luaState)
{
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.WebSocket", 0, &conversionError) ||
        !axlua::adapter::is_string(luaState, 2, 0, &conversionError) ||
        !axlua::adapter::is_no_object(luaState, 3, &conversionError))
        goto argumentError;
    else
    {
        LuaWebSocket* self = (LuaWebSocket*)axlua::adapter::to_usertype(luaState, 1, 0);
        size_t size        = 0;
        const char* data   = (const char*)lua_tolstring(luaState, 2, &size);
        if (!data)
            return 0;

        self->send((const unsigned char*)data, (unsigned int)size);
    }
    return 0;
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'sendString'.", &conversionError);
    return 0;
}

static int websocket_url_getter(lua_State* L)
{
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "ax.WebSocket", 0, &conversionError))
        goto argumentError;
    else
    {
        LuaWebSocket* self = (LuaWebSocket*)axlua::adapter::to_usertype(L, 1, 0);
        axlua::adapter::push_string_view(L, self->getUrl());
        return 1;
    }
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'sendString'.", &conversionError);
    return 0;
}

static int websocket_protocol_getter(lua_State* L)
{
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "ax.WebSocket", 0, &conversionError))
        goto argumentError;
    else
    {
        LuaWebSocket* self = (LuaWebSocket*)axlua::adapter::to_usertype(L, 1, 0);
        axlua::adapter::push_string_view(L, self->getProtocol());
        return 1;
    }
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'sendString'.", &conversionError);
    return 0;
}

int open_web_socket_binding(lua_State* luaState)
{
    axlua::adapter::open(luaState);
    register_web_socket_type(luaState);
    axlua::adapter::module(luaState, "ax", 0);
    axlua::adapter::begin_module(luaState, "ax");
    axlua::adapter::register_class(luaState, "WebSocket", "ax.WebSocket", "", collect_web_socket);
    axlua::adapter::begin_module(luaState, "WebSocket");
    axlua::adapter::set_function(luaState, "create", axlua_WebSocket_create00);
    axlua::adapter::set_function(luaState, "getReadyState", axlua_WebSocket_getReadyState00);
    axlua::adapter::set_function(luaState, "close", axlua_WebSocket_close00);
    axlua::adapter::set_function(luaState, "sendString", axlua_WebSocket_sendString00);
    axlua::adapter::set_variable(luaState, "url", websocket_url_getter, nullptr);
    axlua::adapter::set_variable(luaState, "protocol", websocket_protocol_getter, nullptr);
    axlua::adapter::end_module(luaState);
    axlua::adapter::end_module(luaState);
    return 1;
}

int axlua_WebSocket_registerScriptHandler00(lua_State* luaState)
{
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.WebSocket", 0, &conversionError) ||
        !axlua::adapter::is_function(luaState, 2, "LUA_FUNCTION", 0, &conversionError) ||
        !axlua::adapter::is_number(luaState, 3, 0, &conversionError) ||
        !axlua::adapter::is_no_object(luaState, 4, &conversionError))
        goto argumentError;
    else
    {
        LuaWebSocket* self = (LuaWebSocket*)axlua::adapter::to_usertype(luaState, 1, 0);
        if (self)
        {
            const int handlerType = static_cast<int>(axlua::adapter::to_number(luaState, 3, 0));
            switch (handlerType)
            {
            case LuaWebSocket::kWebSocketScriptHandlerOpen:
                self->onOpenCallback = axlua::Callback<void()>(luaState, 2);
                break;
            case LuaWebSocket::kWebSocketScriptHandlerMessage:
                self->onMessageCallback = axlua::Callback<void(std::string_view, bool)>(luaState, 2);
                break;
            case LuaWebSocket::kWebSocketScriptHandlerClose:
                self->onCloseCallback = axlua::Callback<void()>(luaState, 2);
                break;
            case LuaWebSocket::kWebSocketScriptHandlerError:
                self->onErrorCallback = axlua::Callback<void()>(luaState, 2);
                break;
            default:
                return luaL_error(luaState, "WebSocket handler type out of range: %d", handlerType);
            }
        }
    }
    return 0;
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'registerScriptHandler'.", &conversionError);
    return 0;
}

int axlua_WebSocket_unregisterScriptHandler00(lua_State* luaState)
{
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.WebSocket", 0, &conversionError) ||
        !axlua::adapter::is_number(luaState, 2, 0, &conversionError) ||
        !axlua::adapter::is_no_object(luaState, 3, &conversionError))
        goto argumentError;
    else
    {
        LuaWebSocket* self = (LuaWebSocket*)axlua::adapter::to_usertype(luaState, 1, 0);
        if (self)
        {
            switch (static_cast<int>(axlua::adapter::to_number(luaState, 2, 0)))
            {
            case LuaWebSocket::kWebSocketScriptHandlerOpen: self->onOpenCallback.reset(); break;
            case LuaWebSocket::kWebSocketScriptHandlerMessage: self->onMessageCallback.reset(); break;
            case LuaWebSocket::kWebSocketScriptHandlerClose: self->onCloseCallback.reset(); break;
            case LuaWebSocket::kWebSocketScriptHandlerError: self->onErrorCallback.reset(); break;
            default: return luaL_error(luaState, "WebSocket handler type out of range");
            }
        }
    }
    return 0;
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'unregisterScriptHandler'.", &conversionError);
    return 0;
}

int register_web_socket_adapter(lua_State* luaState)
{
    if (nullptr == luaState)
        return 0;

    axlua::adapter::push_literal(luaState, "ax.WebSocket");
    lua_rawget(luaState, LUA_REGISTRYINDEX);
    if (lua_istable(luaState, -1))
    {
        axlua::adapter::push_literal(luaState, "registerScriptHandler");
        lua_pushcfunction(luaState, axlua_WebSocket_registerScriptHandler00);
        lua_rawset(luaState, -3);
        axlua::adapter::push_literal(luaState, "unregisterScriptHandler");
        lua_pushcfunction(luaState, axlua_WebSocket_unregisterScriptHandler00);
        lua_rawset(luaState, -3);
    }
    lua_pop(luaState, 1);

    return 1;
}
