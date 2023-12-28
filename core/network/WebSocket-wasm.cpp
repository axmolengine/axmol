#include "network/WebSocket-wasm.h"
#include "yasio/errc.hpp"
#include "base/Console.h"

NS_AX_BEGIN

namespace network
{

EM_BOOL WebSocket::em_ws_onopen(int eventType, const EmscriptenWebSocketOpenEvent* websocketEvent, void* userData)
{
    ax::print("### ----------- em_ws_onopen ");
    return EM_TRUE;
}

EM_BOOL WebSocket::em_ws_onerror(int eventType, const EmscriptenWebSocketErrorEvent* websocketEvent, void* userData)
{
    ax::print("### ----------- em_ws_onerror ");
    return EM_TRUE;
}

EM_BOOL WebSocket::em_ws_onclose(int eventType, const EmscriptenWebSocketCloseEvent* websocketEvent, void* userData)
{
    return EM_TRUE;
}

EM_BOOL WebSocket::em_ws_onmessage(int eventType, const EmscriptenWebSocketMessageEvent* websocketEvent, void* userData)
{
    return EM_TRUE;
}

WebSocket::WebSocket() {}

// TODO:
WebSocket::~WebSocket() {}

bool WebSocket::open(Delegate* delegate,
                     std::string_view url,
                     std::string_view caFilePath,
                     const char* protocols)
{
    if (url.empty())
    {
        AXLOG("ws open fail, url is empty!");
        return false;
    }

    EmscriptenWebSocketCreateAttributes ws_attrs = {url.data(), protocols, EM_TRUE};

    AXLOG("ws open url: %s, protocols: %s", ws_attrs.url, ws_attrs.protocols);

    _wsfd = emscripten_websocket_new(&ws_attrs);

    // chrome/edge can't connect
    // firefox works with "Sec-Fetch-Site: cross-site" in request header
    // refer: https://github.com/emscripten-core/emscripten/issues/19100
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
    this->send(message.data(), static_cast<unsigned int>(message.size()));
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
    auto result = emscripten_websocket_send_binary(_wsfd, (void*)data, len);
    if (result)
    {
        AXLOG("Failed to emscripten_websocket_send_binary(): %d", result);
    }
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
    EMSCRIPTEN_RESULT result = emscripten_websocket_close(_wsfd, 3000 - yasio::errc::shutdown_by_localhost, "shutdown by localhost");
    if (result)
    {
        AXLOG("Failed to emscripten_websocket_close(): %d", result);
    }
}

}  // namespace network

NS_AX_END
