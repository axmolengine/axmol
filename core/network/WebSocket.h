/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmolengine.github.io/

 Permission is
 * hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files
 * (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to
 * whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this
 * permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS
 * PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

 * ****************************************************************************/

#pragma once

#if !defined(__EMSCRIPTEN__)

#    include <string>
#    include <vector>
#    include <mutex>
#    include <memory>  // for std::shared_ptr
#    include <atomic>
#    include <condition_variable>
#    include <future>

#    include "platform/PlatformMacros.h"
#    include "platform/StdC.h"
#    include "base/Scheduler.h"
#    include "base/ConcurrentDeque.h"
#    include "yasio/yasio_fwd.hpp"
#    include "yasio/byte_buffer.hpp"
#    include "network/Uri.h"
#    include "llhttp.h"
#    include "websocket_parser.h"

NS_AX_BEGIN

class EventListenerCustom;

namespace network
{

struct WebSocketProtocol;

/**
 * WebSocket implementation using yasio.
 * Please note that all public methods of WebSocket have to be invoked on Axmol Thread.
 */
class AX_DLL WebSocket
{
    friend struct WebSocketProtocol;

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

    class Event : public ax::Object
    {
    public:
        enum class Type
        {
            ON_OPEN,
            ON_ERROR,
            ON_CLOSE,
            ON_MESSAGE,
        };
        Type getType() const { return _type; }

    protected:
        Type _type;
    };

    class WsOpenEvent : public Event
    {
    public:
        WsOpenEvent() { this->_type = Type::ON_OPEN; }
    };

    class CloseEvent : public Event
    {
    public:
        CloseEvent() { this->_type = Type::ON_CLOSE; }
    };

    class ErrorEvent : public Event
    {
    public:
        ErrorEvent(ErrorCode errorCode) : _errorCode(errorCode) { this->_type = Type::ON_ERROR; }

        ErrorCode getErrorCode() const { return _errorCode; }

    private:
        ErrorCode _errorCode;
    };

    class MessageEvent : public Event
    {
    public:
        MessageEvent(yasio::sbyte_buffer&& message, bool isBinary) : _message(std::move(message)), _isBinary(isBinary)
        {
            this->_type = Type::ON_MESSAGE;
        }

        yasio::sbyte_buffer& getMessage() { return _message; }
        const yasio::sbyte_buffer& getMessage() const { return _message; }
        bool isBinary() const { return _isBinary; }

    private:
        yasio::sbyte_buffer _message;
        bool _isBinary;
        /* TODO:
        issued;
        bool isBinary;
        void* ext;
        */
    };

    /**
     * Data structure for message view
     */
    struct Data
    {
        Data() : bytes(nullptr), len(0), issued(0), isBinary(false), ext(nullptr) {}
        Data(MessageEvent* event)
            : bytes(event->getMessage().data())
            , len(event->getMessage().size())
            , isBinary(event->isBinary())
            , issued(0)
            , ext(nullptr)
        {}
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
     *  @param caFilePath The ca file path for wss connection
     *  @param protocols Comma-separated list of sub-protocols that agree with websocket server
     *  @return true: Success, false: Failure.
     *  @lua NA
     */
    bool open(Delegate* delegate,
              std::string_view url,
              std::string_view caFilePath = "",
              std::string_view protocols = "");

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
     *  @param data binary data.
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
    State getReadyState() const { return _state; };

    /**
     *  @brief Gets the URL of websocket connection.
     */
    std::string_view getUrl() const { return _url; }

    /**
     *  @brief Gets the protocol selected by websocket server.
     */
    std::string_view getProtocol() const { return _selectedProtocol; }

    /**
     * Set custom-defined headers.
     *
     * @param headers The string vector of custom-defined headers.
     */
    void setHeaders(const std::vector<std::string>& headers) { _headers = headers; }

    /**
     * Get custom headers.
     *
     * @return std::vector<std::string> the string vector of custom-defined headers.
     */
    const std::vector<std::string>& getHeaders() const { return _headers; }

protected:
    void purgePendingEvents();
    void dispatchEvents();

    void setupParsers();
    void generateHandshakeSecKey();
    void handleNetworkEvent(yasio::io_event* event);

    void do_handshake(const char* d, size_t n)
    {
        enum llhttp_errno err = llhttp_execute(&_context, d, n);
        if (err != HPE_OK)
        {
            _handshakeFinished = true;
        }
    }

    static int on_header_field(llhttp_t* context, const char* at, size_t length)
    {
        auto thiz = (WebSocket*)context->data;
        thiz->_currentHeader.insert(thiz->_currentHeader.end(), at, at + length);
        return 0;
    }
    static int on_header_field_complete(llhttp_t* context)
    {
        auto thiz = (WebSocket*)context->data;
        std::transform(thiz->_currentHeader.begin(), thiz->_currentHeader.end(), thiz->_currentHeader.begin(),
                       ::tolower);
        return 0;
    }
    static int on_header_value(llhttp_t* context, const char* at, size_t length)
    {
        auto thiz = (WebSocket*)context->data;
        thiz->_currentHeaderValue.insert(thiz->_currentHeaderValue.end(), at, at + length);
        return 0;
    }
    static int on_header_value_complete(llhttp_t* context)
    {
        auto thiz = (WebSocket*)context->data;
        thiz->_responseHeaders.emplace(std::move(thiz->_currentHeader), std::move(thiz->_currentHeaderValue));
        return 0;
    }
    static int on_body(llhttp_t* context, const char* at, size_t length)
    {
        auto thiz = (WebSocket*)context->data;
        thiz->_responseData.insert(thiz->_responseData.end(), at, at + length);
        return 0;
    }
    static int on_complete(llhttp_t* context)
    {
        auto thiz                = (WebSocket*)context->data;
        thiz->_responseCode      = context->status_code;
        thiz->_handshakeFinished = true;
        return 0;
    }

    static int on_frame_header(websocket_parser*);
    static int on_frame_body(websocket_parser*, const char* at, size_t length);
    static int on_frame_end(websocket_parser*);

protected:
    using ResponseHeaderMap = std::multimap<std::string, std::string>;

    Scheduler* _scheduler;

    std::shared_ptr<std::atomic<bool>> _isDestroyed;

    // for close sync
    std::shared_ptr<std::promise<int>> _syncCloseState;

    std::string _url;
    Uri _requestUri;
    std::string _selectedProtocol;
    std::string _caFilePath;
    std::string _handshakeSecKey;
    std::string _verifySecKey;
    Delegate* _delegate{};
    yasio::io_service* _service;
    yasio::transport_handle_t _transport{};
    yasio::highp_timer* _heartbeatTimer;
    State _state            = State::CLOSED;
    bool _handshakeFinished = false;

    // for parse handshake response
    llhttp_t _context;
    llhttp_settings_t _contextSettings;

    // for parse websocket frame
    websocket_parser _wsParser;
    websocket_parser_settings _wsParserSettings;

    enum class FrameState
    {
        BEGIN,
        HEADER,
        BODY,
        END,
        FIN,
    };
    FrameState _frameState = FrameState::BEGIN;
    int _opcode            = 0;

    std::string _currentHeader;
    std::string _currentHeaderValue;
    ResponseHeaderMap _responseHeaders;  /// the returned raw header data. You can also dump it as a string
    std::string _responseData;           /// the handshake repsonse message when fail

    /// the http status code returned from server, e.g. 200, 404, refer to
    /// https://developer.mozilla.org/en-US/docs/Web/HTTP/Status
    int _responseCode = -1;

    // for receiveData
    yasio::sbyte_buffer _receivedData;
    std::recursive_mutex _receivedDataMtx;

    EventListenerCustom* _resetDirectorListener;

    ConcurrentDeque<Event*> _eventQueue;
    std::vector<std::string> _headers;  /// custom headers
    std::string _protocols;
};
}  // namespace network

NS_AX_END
#else
#    include "network/WebSocket-wasm.h"
#endif
