/****************************************************************************
 Copyright (c) 2015-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
 
 http://www.cocos2d-x.org

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
 
#pragma once

#include "platform/PlatformMacros.h"
#include <string>
#include "base/Object.h"
#include "yasio/byte_buffer.hpp"

#include <emscripten/websocket.h>

NS_AX_BEGIN

namespace network
{
/**
 * WebSocket implementation using yasio.
 * Please note that all public methods of WebSocket have to be invoked on Axmol Thread.
 */
class AX_DLL WebSocket
{
public:
    /**
     * Constructor of WebSocket.
     *
     * @js ctor
     */
    WebSocket();
    /**
     * Destructor of WebSocket.
     *
     * @js NA
     * @lua NA
     */
    virtual ~WebSocket();

    /**
     * ErrorCode enum used to represent the error in the websocket.
     */
    enum class ErrorCode
    {
        OK,
        TIME_OUT,           /** &lt; value 0 */
        CONNECTION_FAILURE, /** &lt; value 1 */
        NO_SEC_ACCEPT,
        VERIFY_SEC_ACCEPT_FAILURE,
        UPGRADE_FAILURE
    };

    /**
     *  State enum used to represent the Websocket state.
     */
    enum class State
    {
        CONNECTING, /** &lt; value 0 */
        OPEN,       /** &lt; value 1 */
        CLOSING,    /** &lt; value 2 */
        CLOSED,     /** &lt; value 3 */
    };

    /**
     * Data structure for message view
     */
    struct Data
    {
        Data() : bytes(nullptr), len(0), issued(0), isBinary(false), ext(nullptr) {}
        const char* bytes;
        size_t len, issued;
        bool isBinary;
        void* ext;
    };

    /**
     * The delegate class is used to process websocket events.
     *
     * The most member function are pure virtual functions,they should be implemented the in subclass.
     * @lua NA
     */
    class Delegate
    {
    public:
        /** Destructor of Delegate. */
        virtual ~Delegate() {}
        /**
         * This function to be called after the client connection complete a handshake with the remote server.
         * This means that the WebSocket connection is ready to send and receive data.
         *
         * @param ws The WebSocket object connected
         */
        virtual void onOpen(WebSocket* ws) = 0;
        /**
         * This function to be called when data has appeared from the server for the client connection.
         *
         * @param ws The WebSocket object connected.
         * @param data Data object for message.
         */
        virtual void onMessage(WebSocket* ws, const Data& data) = 0;
        /**
         * When the WebSocket object connected wants to close or the protocol won't get used at all and current
         * _readyState is State::CLOSING,this function is to be called.
         *
         * @param ws The WebSocket object connected.
         */
        virtual void onClose(WebSocket* ws) = 0;
        /**
         * This function is to be called in the following cases:
         * 1. client connection is failed.
         * 2. the request client connection has been unable to complete a handshake with the remote server.
         * 3. the protocol won't get used at all after this callback and current _readyState is State::CONNECTING.
         * 4. when a socket descriptor needs to be removed from an external polling array. in is again the struct
         * libwebsocket_pollargs containing the fd member to be removed. If you are using the internal polling loop, you
         * can just ignore it and current _readyState is State::CONNECTING.
         *
         * @param ws The WebSocket object connected.
         * @param error WebSocket::ErrorCode enum,would be ErrorCode::TIME_OUT or ErrorCode::CONNECTION_FAILURE.
         */
        virtual void onError(WebSocket* ws, const ErrorCode& error) = 0;
    };

    /**
     *  @brief The initialized method for websocket.
     *         It needs to be invoked right after websocket instance is allocated.
     *  @param delegate The delegate which want to receive event from websocket.
     *  @param url The URL of websocket server.
     *  @param protocols The websocket protocols that agree with websocket server
     *  @param caFilePath The ca file path for wss connection
     *  @param protocols Comma-separated list of sub-protocols that agree with websocket server
     *  @return true: Success, false: Failure.
     *  @lua NA
     */
    bool open(Delegate* delegate,
              std::string_view url,
              std::string_view caFilePath = "",
              std::string_view protocols  = "");

    /**
     *  @brief Sends string data to websocket server.
     *
     *  @param message string data.
     *  @lua sendstring
     */
    void send(std::string_view message);

    /**
     *  @brief Sends binary data to websocket server.
     *
     *  @param data binary string data.
     *  @param len the size of binary string data.
     *  @lua sendstring
     */
    void send(const void* data, unsigned int len);

    /**
     *  @brief Closes the connection to server synchronously.
     *  @note It's a synchronous method, it will not return until websocket thread exits.
     */
    void close();

    /**
     *  @brief Closes the connection to server asynchronously.
     *  @note It's an asynchronous method, it just notifies websocket thread to exit and returns directly,
     *        If using 'closeAsync' to close websocket connection,
     *        be careful of not using destructed variables in the callback of 'onClose'.
     */
    void closeAsync();

    /**
     *  @brief Gets current state of connection.
     *  @return State the state value could be State::CONNECTING, State::OPEN, State::CLOSING or State::CLOSED
     */
    State getReadyState() const {  return _state; }

    /**
     *  @brief Gets the URL of websocket connection.
     */
    std::string_view getUrl() const { return _url; }

    /**
     *  @brief Gets the protocol selected by websocket server.
     */
    std::string_view getProtocol() const { return _selectedProtocol; }

protected:
    static EM_BOOL em_ws_onopen(int eventType, const EmscriptenWebSocketOpenEvent *websocketEvent, void *userData);
    static EM_BOOL em_ws_onerror(int eventType, const EmscriptenWebSocketErrorEvent *websocketEvent, void *userData);
    static EM_BOOL em_ws_onclose(int eventType, const EmscriptenWebSocketCloseEvent *websocketEvent, void *userData);
    static EM_BOOL em_ws_onmessage(int eventType, const EmscriptenWebSocketMessageEvent *websocketEvent, void *userData);

protected:
    int _wsfd = -1;
    State _state = State::CLOSED;
    std::string _url;
    std::string _subProtocols;
    std::string _selectedProtocol;

    Delegate* _delegate{};
};
}  // namespace network

NS_AX_END
