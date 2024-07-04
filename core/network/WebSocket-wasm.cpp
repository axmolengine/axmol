#include "network/WebSocket-wasm.h"
#include "yasio/errc.hpp"
#include "base/Logging.h"

NS_AX_BEGIN

namespace network
{

EM_BOOL WebSocket::em_ws_onopen(int eventType, const EmscriptenWebSocketOpenEvent* websocketEvent, void* userData)
{
    auto ws = static_cast<WebSocket*>(userData);
    if (!ws || !ws->_delegate)
        return EM_TRUE;
    ws->_state = WebSocket::State::OPEN;
    ws->_delegate->onOpen(ws);
    return EM_TRUE;
}

EM_BOOL WebSocket::em_ws_onerror(int eventType, const EmscriptenWebSocketErrorEvent* websocketEvent, void* userData)
{
    auto ws = static_cast<WebSocket*>(userData);
    if (!ws || !ws->_delegate)
        return EM_TRUE;
    ws->_state = WebSocket::State::CLOSED;
    ws->_delegate->onError(ws, ErrorCode::CONNECTION_FAILURE);
    return EM_TRUE;
}

EM_BOOL WebSocket::em_ws_onclose(int eventType, const EmscriptenWebSocketCloseEvent* websocketEvent, void* userData)
{
    auto ws = static_cast<WebSocket*>(userData);
    if (!ws || !ws->_delegate)
        return EM_TRUE;
    ws->_state = WebSocket::State::CLOSED;
    ws->_delegate->onClose(ws);
    return EM_TRUE;
}

EM_BOOL WebSocket::em_ws_onmessage(int eventType, const EmscriptenWebSocketMessageEvent* websocketEvent, void* userData)
{
    auto ws = static_cast<WebSocket*>(userData);
    if (!ws || !ws->_delegate)
        return EM_TRUE;
    WebSocket::Data dataView;
    dataView.bytes    = reinterpret_cast<const char*>(websocketEvent->data);
    dataView.isBinary = !websocketEvent->isText;
    dataView.len      = websocketEvent->numBytes;
    ws->_delegate->onMessage(ws, dataView);
    return EM_TRUE;
}

WebSocket::WebSocket() {}

WebSocket::~WebSocket() {}

bool WebSocket::open(Delegate* delegate, std::string_view url, std::string_view caFilePath, std::string_view protocols)
{
    if (url.empty())
    {
        AXLOGW("ws open fail, url is empty!");
        return false;
    }

    _delegate = delegate;

    _url          = url;
    _subProtocols = protocols;

    EmscriptenWebSocketCreateAttributes ws_attrs = {_url.c_str(),
                                                    _subProtocols.empty() ? nullptr : _subProtocols.c_str(), EM_TRUE};

    AXLOGD("ws open url: {}, protocols: {}", ws_attrs.url, ws_attrs.protocols);

    _state = WebSocket::State::CONNECTING;
    _wsfd = emscripten_websocket_new(&ws_attrs);

    // chrome/edge can't connect
    // firefox works with "Sec-Fetch-Site: cross-site" in request header
    // refer: https://github.com/emscripten-core/emscripten/issues/19100
    // wasm websocket callback thread same with axmol render thread
    emscripten_websocket_set_onopen_callback(_wsfd, this, em_ws_onopen);
    emscripten_websocket_set_onerror_callback(_wsfd, this, em_ws_onerror);
    emscripten_websocket_set_onclose_callback(_wsfd, this, em_ws_onclose);
    emscripten_websocket_set_onmessage_callback(_wsfd, this, em_ws_onmessage);
    return true;
}
/**
 *  @brief Sends string data to websocket server.
 *
 *  @param message string data.
 *  @lua sendstring
 */
void WebSocket::send(std::string_view message)
{
    auto error = emscripten_websocket_send_utf8_text(_wsfd, message.data());
    if (error)
        AXLOGW("Failed to emscripten_websocket_send_binary(): {}", error);
}

/**
 *  @brief Sends binary data to websocket server.
 *
 *  @param binaryMsg binary string data.
 *  @param len the size of binary string data.
 *  @lua sendstring
 */
void WebSocket::send(const void* data, unsigned int len)
{
    auto error = emscripten_websocket_send_binary(_wsfd, const_cast<void*>(data), len);
    if (error)
        AXLOGW("Failed to emscripten_websocket_send_binary(): {}", error);
}

/**
 *  @brief Closes the connection to server synchronously.
 *  @note It's a synchronous method, it will not return until websocket thread exits.
 */
void WebSocket::close()
{
    closeAsync();  // TODO
}

/**
 *  @brief Closes the connection to server asynchronously.
 *  @note It's an asynchronous method, it just notifies websocket thread to exit and returns directly,
 *        If using 'closeAsync' to close websocket connection,
 *        be careful of not using destructed variables in the callback of 'onClose'.
 */
void WebSocket::closeAsync()
{
    // close code: Uncaught DOMException: Failed to execute 'close' on 'WebSocket':
    // The code must be either 1000, or between 3000 and 4999. 1024 is neither.
    EMSCRIPTEN_RESULT error =
        emscripten_websocket_close(_wsfd, 3000 - yasio::errc::shutdown_by_localhost, "shutdown by localhost");
    if (!error)
        _state = WebSocket::State::CLOSING;
    else
        AXLOGW("Failed to emscripten_websocket_close(): {}", error);
}

}  // namespace network

NS_AX_END
