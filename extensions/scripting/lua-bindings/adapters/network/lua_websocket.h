/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "lua-bindings/runtime/axlua_adapter.h"
#include "lua-bindings/runtime/axlua_runtime.h"

#include "axmol/network/WebSocket.h"

///@cond
class LuaWebSocket : public ax::network::WebSocket, public ax::network::WebSocket::Delegate
{
public:
    ~LuaWebSocket() override;
    ;
    void onOpen(WebSocket* ws) override;
    void onMessage(WebSocket* ws, const WebSocket::Data& data) override;
    void onClose(WebSocket* ws, uint16_t code, std::string_view reason) override;
    void onError(WebSocket* ws, const WebSocket::ErrorCode& error) override;

    axlua::Callback<void()> onOpenCallback;
    axlua::Callback<void(std::string_view, bool)> onMessageCallback;
    axlua::Callback<void()> onCloseCallback;
    axlua::Callback<void()> onErrorCallback;

    enum WebSocketScriptHandlerType
    {
        kWebSocketScriptHandlerOpen,
        kWebSocketScriptHandlerMessage,
        kWebSocketScriptHandlerClose,
        kWebSocketScriptHandlerError,
    };
};

int open_web_socket_binding(lua_State* luaState);
int register_web_socket_adapter(lua_State* luaState);
///@endcond
