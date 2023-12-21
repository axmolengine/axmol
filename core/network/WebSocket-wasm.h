#pragma once

#include "platform/PlatformMacros.h"
#include <string>
#include "base/Ref.h"
#include "yasio/byte_buffer.hpp"

#include <emscripten/websocket.h>

NS_AX_BEGIN

namespace network
{
/**
 * WebSocket is wrapper of the libwebsockets-protocol, let the develop could call the websocket easily.
 * Please note that all public methods of WebSocket have to be invoked on Cocos Thread.
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

    class Event : public ax::Ref
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
     *  @param protocols The websocket protocols that agree with websocket server
     *  @param caFilePath The ca file path for wss connection
     *  @return true: Success, false: Failure.
     *  @lua NA
     */
    bool open(Delegate* delegate,
              std::string_view url,
              const char* protocols = nullptr,
              std::string_view caFilePath               = "");

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
     *  @param binaryMsg binary string data.
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
    inline std::string_view getUrl() const { return _url; }

    /**
     *  @brief Gets the protocol selected by websocket server.
     */
    inline std::string_view getProtocol() const { return _selectedProtocol; }

protected:
    static EM_BOOL em_ws_onopen(int eventType, const EmscriptenWebSocketOpenEvent *websocketEvent, void *userData);
    static EM_BOOL em_ws_onerror(int eventType, const EmscriptenWebSocketErrorEvent *websocketEvent, void *userData);
    static EM_BOOL em_ws_onclose(int eventType, const EmscriptenWebSocketCloseEvent *websocketEvent, void *userData);
    static EM_BOOL em_ws_onmessage(int eventType, const EmscriptenWebSocketMessageEvent *websocketEvent, void *userData);

protected:
    int _wsfd = -1;
    State _state = State::CLOSED;
    std::string _url;
    std::string _selectedProtocol;
};
}  // namespace network

NS_AX_END
