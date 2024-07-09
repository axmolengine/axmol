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

#include "yasio/yasio.hpp"

#include "network/WebSocket.h"

#include "fmt/format.h"

using namespace yasio;

#define WS_MAX_PAYLOAD_LENGTH (1 << 24)  // 16M

NS_AX_BEGIN

namespace network
{
namespace ws
{
namespace detail
{

/** Close status codes.

These codes accompany close frames.

@see <a href="https://tools.ietf.org/html/rfc6455#section-7.4.1">RFC 6455 7.4.1 Defined Status Codes</a>
*/
enum close_code : std::uint16_t
{
    /// Normal closure; the connection successfully completed whatever purpose for which it was created.
    normal = 1000,

    /// The endpoint is going away, either because of a server failure or because the browser is navigating away from
    /// the page that opened the connection.
    going_away = 1001,

    /// The endpoint is terminating the connection due to a protocol error.
    protocol_error = 1002,

    /// The connection is being terminated because the endpoint received data of a type it cannot accept (for example, a
    /// text-only endpoint received binary data).
    unknown_data = 1003,

    /// The endpoint is terminating the connection because a message was received that contained inconsistent data
    /// (e.g., non-UTF-8 data within a text message).
    bad_payload = 1007,

    /// The endpoint is terminating the connection because it received a message that violates its policy. This is a
    /// generic status code, used when codes 1003 and 1009 are not suitable.
    policy_error = 1008,

    /// The endpoint is terminating the connection because a data frame was received that is too large.
    too_big = 1009,

    /// The client is terminating the connection because it expected the server to negotiate one or more extension, but
    /// the server didn't.
    needs_extension = 1010,

    /// The server is terminating the connection because it encountered an unexpected condition that prevented it from
    /// fulfilling the request.
    internal_error = 1011,

    /// The server is terminating the connection because it is restarting.
    service_restart = 1012,

    /// The server is terminating the connection due to a temporary condition, e.g. it is overloaded and is casting off
    /// some of its clients.
    try_again_later = 1013,

    //----
    //
    // The following are illegal on the wire
    //

    /** Used internally to mean "no error"

        This code is reserved and may not be sent.
    */
    none = 0,

    /** Reserved for future use by the WebSocket standard.

        This code is reserved and may not be sent.
    */
    reserved1 = 1004,

    /** No status code was provided even though one was expected.

        This code is reserved and may not be sent.
    */
    no_status = 1005,

    /** Connection was closed without receiving a close frame

        This code is reserved and may not be sent.
    */
    abnormal = 1006,

    /** Reserved for future use by the WebSocket standard.

        This code is reserved and may not be sent.
    */
    reserved2 = 1014,

    /** Reserved for future use by the WebSocket standard.

        This code is reserved and may not be sent.
    */
    reserved3 = 1015

    //
    //----

    // last = 5000 // satisfy warnings
};

// frame header opcodes
enum class opcode : std::uint8_t
{
    cont   = 0,
    text   = 1,
    binary = 2,
    rsv3   = 3,
    rsv4   = 4,
    rsv5   = 5,
    rsv6   = 6,
    rsv7   = 7,
    close  = 8,
    ping   = 9,
    pong   = 10,
    crsvb  = 11,
    crsvc  = 12,
    crsvd  = 13,
    crsve  = 14,
    crsvf  = 15
};

}  // namespace detail
}  // namespace ws

struct WebSocketProtocol
{
    static int sendFrame(WebSocket& ws,
                         const char* buf,
                         size_t len,
                         ws::detail::opcode opcode /* = WS_OPCODE_BINARY */,
                         bool fin = true)
    {
        int flags = (int)opcode;

        // role == WS_CLIENT
        uint32_t key = ax::random();
        char mask[4];
        mask[0] = (key >> 0) & 0xff;
        mask[1] = (key >> 8) & 0xff;
        mask[2] = (key >> 16) & 0xff;
        mask[3] = (key >> 24) & 0xff;
        flags |= WS_HAS_MASK;

        if (fin)
            flags |= WS_FIN;
        auto frame_size = websocket_calc_frame_size((websocket_flags)flags, len);
        yasio::sbyte_buffer sb;
        sb.resize(frame_size);
        websocket_build_frame(sb.data(), (websocket_flags)flags, mask, buf, len);
        return ws._service->write(ws._transport, std::move(sb));  // write(sendbuf_.base, frame_size);
    }
};

WebSocket::WebSocket() : _isDestroyed(std::make_shared<std::atomic<bool>>(false)), _delegate(nullptr)
{
    _service        = new yasio::io_service();
    _heartbeatTimer = new yasio::highp_timer(*_service);
    _scheduler      = Director::getInstance()->getScheduler();
    _service->set_option(yasio::YOPT_S_FORWARD_PACKET, 1);  // forward packet immediately when got data from OS kernel
    _service->set_option(yasio::YOPT_S_DNS_QUERIES_TIMEOUT, 3);
    _service->set_option(yasio::YOPT_S_DNS_QUERIES_TRIES, 1);
    _service->start([this](yasio::event_ptr&& e) { handleNetworkEvent(e.get()); });

    std::shared_ptr<std::atomic<bool>> isDestroyed = _isDestroyed;
    _resetDirectorListener = Director::getInstance()->getEventDispatcher()->addCustomEventListener(
        Director::EVENT_RESET, [this, isDestroyed](EventCustom*) {
            if (*isDestroyed)
                return;
            close();
        });

    _scheduler->schedule([this](float) { dispatchEvents(); }, this, 0, false, "#");
}
WebSocket::~WebSocket()
{
    Director::getInstance()->getEventDispatcher()->removeEventListener(_resetDirectorListener);
    *_isDestroyed = true;

    delete _service;
    delete _heartbeatTimer;

    _scheduler->unscheduleAllForTarget(this);

    // Design A: dispatch pending events at destructor. user can't delete websocket instance in delegate
    // callbacks
    // Design B(Preferred): purge pending events without dispatch.
    purgePendingEvents();
}

bool WebSocket::open(Delegate* delegate,
                     std::string_view url,
                     std::string_view caFilePath,
                     std::string_view protocols)
{
    _delegate   = delegate;
    _url        = url;
    _caFilePath = FileUtils::getInstance()->fullPathForFilename(caFilePath);
    _requestUri = Uri::parse(url);
    _protocols  = protocols;

    setupParsers();
    generateHandshakeSecKey();

    if (!_caFilePath.empty())
        _service->set_option(YOPT_S_SSL_CACERT, _caFilePath.c_str());

    _service->set_option(YOPT_C_REMOTE_ENDPOINT, 0, _requestUri.getHost().data(), (int)_requestUri.getPort());

    _state = State::CONNECTING;
    if (_requestUri.isSecure())
        _service->open(0, YCK_SSL_CLIENT);
    else
        _service->open(0, YCK_TCP_CLIENT);

    return true;
}

void WebSocket::purgePendingEvents()
{
    auto lck = _eventQueue.get_lock();
    for (auto it = _eventQueue.unsafe_begin(); it != _eventQueue.unsafe_end(); ++it)
    {
        (*it)->release();
    }
    _eventQueue.unsafe_clear();
}

void WebSocket::dispatchEvents()
{
    if (_eventQueue.unsafe_empty())
        return;
    if (_delegate)
    {
        auto lck = _eventQueue.get_lock();
        while (!_eventQueue.empty())
        {
            auto event = _eventQueue.front();
            _eventQueue.unsafe_pop_front();

            switch (event->getType())
            {
            case Event::Type::ON_OPEN:
                _delegate->onOpen(this);
                break;
            case Event::Type::ON_CLOSE:
                _delegate->onClose(this);
                break;
            case Event::Type::ON_ERROR:
                _delegate->onError(this, static_cast<ErrorEvent*>(event)->getErrorCode());
                break;
            case Event::Type::ON_MESSAGE:
                _delegate->onMessage(this, WebSocket::Data{static_cast<MessageEvent*>(event)});
                break;
            }

            event->release();
        }
    }
}

void WebSocket::setupParsers()
{
    /// http parser for handshake

    /* Initialize user callbacks and settings */
    llhttp_settings_init(&_contextSettings);

    /* Initialize the parser in HTTP_BOTH mode, meaning that it will select between
     * HTTP_REQUEST and HTTP_RESPONSE parsing automatically while reading the first
     * input.
     */
    llhttp_init(&_context, HTTP_RESPONSE, &_contextSettings);

    _context.data = this;

    /* Set user callbacks */
    _contextSettings.on_header_field          = on_header_field;
    _contextSettings.on_header_field_complete = on_header_field_complete;
    _contextSettings.on_header_value          = on_header_value;
    _contextSettings.on_header_value_complete = on_header_value_complete;
    _contextSettings.on_body                  = on_body;
    _contextSettings.on_message_complete      = on_complete;

    /// websocket parser frame from server
    websocket_parser_init(&_wsParser);
    _wsParser.data = this;

    websocket_parser_settings_init(&_wsParserSettings);
    _wsParserSettings.on_frame_header = on_frame_header;
    _wsParserSettings.on_frame_body   = on_frame_body;
    _wsParserSettings.on_frame_end    = on_frame_end;
}

int WebSocket::on_frame_header(websocket_parser* parser)
{
    WebSocket* ws = static_cast<WebSocket*>(parser->data);
    int opcode    = parser->flags & WS_OP_MASK;

    std::unique_lock<std::recursive_mutex> lck(ws->_receivedDataMtx);
    auto& message = ws->_receivedData;

    if (opcode != WS_OP_CONTINUE)
        ws->_opcode = opcode;
    auto length         = parser->length;
    auto reserve_length = (std::min)(length + 1, static_cast<size_t>(WS_MAX_PAYLOAD_LENGTH));
    if (reserve_length > ws->_receivedData.capacity())
    {
        message.reserve(reserve_length);
    }
    switch (ws->_frameState)
    {
    case FrameState::BEGIN:
    case FrameState::FIN:
        message.clear();
    }
    ws->_frameState = FrameState::HEADER;
    return 0;
}

int WebSocket::on_frame_body(websocket_parser* parser, const char* at, size_t length)
{
    WebSocket* ws   = static_cast<WebSocket*>(parser->data);
    ws->_frameState = FrameState::BODY;
    if (parser->flags & WS_HAS_MASK)
        websocket_parser_decode(const_cast<char*>(at), at, length, parser);

    std::unique_lock<std::recursive_mutex> lck(ws->_receivedDataMtx);
    ws->_receivedData.append(at, at + length);
    return 0;
}

int WebSocket::on_frame_end(websocket_parser* parser)
{
    WebSocket* ws = static_cast<WebSocket*>(parser->data);

    ws->_frameState = FrameState::END;
    if (parser->flags & WS_FIN)
    {
        ws->_frameState = FrameState::FIN;

        std::unique_lock<std::recursive_mutex> lck(ws->_receivedDataMtx);
        switch (ws->_opcode)
        {
        case WS_OP_TEXT:
        case WS_OP_BINARY:
            AX_ASSERT(!ws->_receivedData.empty());
            ws->_eventQueue.emplace_back(new MessageEvent{std::move(ws->_receivedData), ws->_opcode == WS_OP_BINARY});
            break;
        case WS_OP_CLOSE:
            AXLOGD("WS: control frame: CLOSE");
            break;
        case WS_OP_PING:
            AXLOGD("WS: control frame: PING");
            WebSocketProtocol::sendFrame(*ws, ws->_receivedData.data(), ws->_receivedData.size(),
                                         ws::detail::opcode::pong);
            break;
        case WS_OP_PONG:
            AXLOGD("WS: control frame: PONG");
            if (ws->_receivedData.size() != 4 || 0 != memcmp(ws->_receivedData.data(), "WSWS", 4))
                AXLOGD("WS: Unsolicited PONG frame from server (possible keep-alive)\n\n");
            break;
        }
    }

    return 0;
}

/**
 *  @brief Sends string data to websocket server.
 *
 *  @param message string data.
 *  @lua sendstring
 */
void WebSocket::send(std::string_view message)
{
    if (!_transport || message.empty())
        return;
    WebSocketProtocol::sendFrame(*this, message.data(), message.length(), ws::detail::opcode::text);
}

/**
 *  @brief Sends binary data to websocket server.
 *
 *  @param data binary data.
 *  @param len the size of binary string data.
 *  @lua sendstring
 */
void WebSocket::send(const void* data, unsigned int len)
{
    if (!_transport || len == 0)
        return;
    WebSocketProtocol::sendFrame(*this, static_cast<const char*>(data), len, ws::detail::opcode::binary);
}

/**
 *  @brief Closes the connection to server synchronously.
 *  @note It's a synchronous method, it will not return until websocket thread exits.
 */
void WebSocket::close()
{
    if (_state < State::CLOSING)
    {
        if (_service->is_open(0))
        {
            _state = State::CLOSING;
            _service->close(0);

            if (_transport)
            {
                _syncCloseState = std::make_shared<std::promise<int>>();
                int status      = _syncCloseState->get_future().get();
                _syncCloseState.reset();
                AXLOGD("WebSocket close with status: {}", status);
            }
            else
            {
                _service->stop();  // stop internal service for sync close
                _state = State::CLOSED;
                AXLOGW("WebSocket::close: connection not ready!");
            }
        }
        else
            _state = State::CLOSED;
    }
}

/**
 *  @brief Closes the connection to server asynchronously.
 *  @note It's an asynchronous method, it just notifies websocket thread to exit and returns directly,
 *        If using 'closeAsync' to close websocket connection,
 *        be careful of not using destructed variables in the callback of 'onClose'.
 */
void WebSocket::closeAsync()
{
    if (_state < State::CLOSING)
    {
        if (_service->is_open(0))
        {
            _state = State::CLOSING;
            _service->close(0);
        }
        else
            _state = State::CLOSED;
    }
}

void WebSocket::generateHandshakeSecKey()
{
    char rand_key[16] = {0};
    int* p            = reinterpret_cast<int*>(rand_key);
    for (int i = 0; i < 4; ++i, ++p)
        *p = ax::random();
    _handshakeSecKey = utils::base64Encode(std::span{rand_key});

    auto signContent = _handshakeSecKey;
    signContent += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"sv;
    auto digest   = utils::computeDigest(signContent, "sha1"sv, false);
    _verifySecKey = utils::base64Encode(std::span{digest});
}

void WebSocket::handleNetworkEvent(yasio::io_event* event)
{
    int channelIndex = event->cindex();
    auto channel     = _service->channel_at(event->cindex());
    switch (event->kind())
    {
    case YEK_ON_PACKET:
        if (_state == State::CONNECTING)
        {
            auto&& pkt = event->packet_view();
            this->do_handshake(pkt.data(), pkt.size());
            if (_handshakeFinished)
            {
                ErrorCode error = ErrorCode::OK;
                if (_responseCode == 101)
                {
                    auto it = _responseHeaders.find("sec-websocket-accept");
                    if (it != _responseHeaders.end())
                    {
                        auto& secAccept = it->second;
                        if (_verifySecKey != secAccept)
                            error = ErrorCode::VERIFY_SEC_ACCEPT_FAILURE;
                    }
                    else
                        error = ErrorCode::NO_SEC_ACCEPT;
                }
                else
                    error = ErrorCode::UPGRADE_FAILURE;

                if (error == ErrorCode::OK)
                {
                    _state             = State::OPEN;
                    auto& timerForRead = channel->get_user_timer();
                    timerForRead.cancel();
                    _transport = event->transport();
                    _eventQueue.emplace_back(new WsOpenEvent());

                    // WebSocketProtocol::sendPing(*this);

                    // starts websocket heartbeat timer
                    //_heartbeatTimer->expires_from_now(std::chrono::seconds(30));
                    //_heartbeatTimer->async_wait([this](io_service& service) {
                    //    WebSocketProtocol::sendFrame(*this, "WSWS", 4, ws::detail::opcode::ping);
                    //    return false;
                    //});
                }
                else
                {
                    if (!_responseData.empty())
                        AXLOGE("WebSocket: handshake fail, detail: {}", _responseData);

                    //_state = State::CLOSING;
                    _service->close(channelIndex);
                    _eventQueue.emplace_back(new ErrorEvent(error));
                }
            }
        }
        else if (_state == State::OPEN)
        {
            auto&& pkt = event->packet_view();
            websocket_parser_execute(&_wsParser, &_wsParserSettings, pkt.data(), pkt.size());
        }  // else unreachable
        break;
    case YEK_ON_OPEN:
        if (event->status() == 0)
        {
            obstream obs;
            obs.write_bytes("GET ");

            auto& uri = _requestUri;  // response->getRequestUri();
            obs.write_bytes(uri.getPathEtc());

            obs.write_bytes(" HTTP/1.1\r\n");

            obs.write_bytes("Host: ");
            obs.write_bytes(uri.getHost());
            obs.write_bytes("\r\n");

            // process custom headers
            struct HeaderFlag
            {
                enum
                {
                    UESR_AGENT   = 1,
                    CONTENT_TYPE = 1 << 1,
                    ACCEPT       = 1 << 2,
                };
            };

            obs.write_bytes("Connection: Upgrade\r\n");
            obs.write_bytes("Upgrade: websocket\r\n");
            obs.write_bytes("Sec-WebSocket-Version: 13\r\n");

            // response: base64(sha1(key + 258EAFA5-E914-47DA-95CA-C5AB0DC85B11))
            obs.write_bytes("Sec-WebSocket-Key: ");
            obs.write_bytes(_handshakeSecKey);
            obs.write_bytes("\r\n");

            if (!_protocols.empty())
            {
                obs.write_bytes("Sec-WebSocket-Protocol: ");
                obs.write_bytes(_protocols);
                obs.write_bytes("\r\n");
            }

            for (auto&& header : _headers)
            {
                obs.write_bytes(header);
                obs.write_bytes("\r\n");
            }

            obs.write_bytes("User-Agent: yasio-ws\r\n");

            obs.write_bytes("Accept: */*;q=0.8\r\n");

            obs.write_bytes("\r\n");  // build handshake request end

            _service->write(event->transport(), std::move(obs.buffer()));

            auto& timerForRead = channel->get_user_timer();
            timerForRead.cancel();
            timerForRead.expires_from_now(std::chrono::seconds(30));
            timerForRead.async_wait_once([this, channelIndex](io_service& s) {
                _eventQueue.emplace_back(new ErrorEvent(ErrorCode::TIME_OUT));

                //_state = State::CLOSING;
                s.close(channelIndex);  // timeout
            });
        }
        else
        {
            _eventQueue.emplace_back(new ErrorEvent(ErrorCode::CONNECTION_FAILURE));
        }
        break;
    case YEK_ON_CLOSE:
        _transport = nullptr;
        if (_state == State::OPEN || _state == State::CLOSING)
        {
            _state = State::CLOSED;
            _eventQueue.emplace_back(new CloseEvent());
        }
        if (_syncCloseState)
            _syncCloseState->set_value(event->status());
        break;
    }
}

}  // namespace network

NS_AX_END
