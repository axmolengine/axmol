/****************************************************************************
 Copyright (c) 2015 Chris Hannon http://www.channon.us
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2023 Bytedance Inc.

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

*based on the SocketIO library created by LearnBoost at http://socket.io
*using spec version 1 found at https://github.com/LearnBoost/socket.io-spec

Usage is described below, a full working example can be found in TestCpp under ExtionsTest/NetworkTest/SocketIOTest

creating a new connection to a socket.io server running at localhost:3000

    SIOClient *client = SocketIO::connect(*delegate, "ws://localhost:3000");

the connection process will begin and if successful delegate::onOpen will be called
if the connection process results in an error, delegate::onError will be called with the err msg

sending a message to the server

    client->send("Hello!");

emitting an event to be handled by the server, argument json formatting is up to you

    client->emit("eventname", "[{\"arg\":\"value\"}]");

registering an event callback, target should be a member function in a subclass of SIODelegate
CC_CALLBACK_2 is used to wrap the callback with std::bind and store as an SIOEvent

    client->on("eventname", CC_CALLBACK_2(TargetClass::targetfunc, *targetclass_instance));

event target function should match this pattern, *this pointer will be made available

    void TargetClass::targetfunc(SIOClient *, std::string_view)

disconnect from the endpoint by calling disconnect(), onClose will be called on the delegate once complete
in the onClose method the pointer should be set to NULL or used to connect to a new endpoint

    client->disconnect();

 ****************************************************************************/

#ifndef __CC_SOCKETIO_H__
#define __CC_SOCKETIO_H__

#include <string>
#include <unordered_map>
#include <vector>
#include "platform/CCPlatformMacros.h"
#include "base/CCMap.h"

/**
 * @addtogroup network
 * @{
 */

NS_AX_BEGIN

namespace network
{

// forward declarations
class SIOClientImpl;
class SIOClient;

/**
 * Singleton and wrapper class to provide static creation method as well as registry of all sockets.
 *
 * @lua NA
 */
class AX_DLL SocketIO
{
public:
    /**
     * Get instance of SocketIO.
     *
     * @return SocketIO* the instance of SocketIO.
     */
    static SocketIO* getInstance();
    static void destroyInstance();

    /**
     * The delegate class to process socket.io events.
     * @lua NA
     */
    class SIODelegate
    {
    public:
        /** Destructor of SIODelegate. */
        virtual ~SIODelegate() {}
        /**
         * This is kept for backwards compatibility, connect is now fired as a socket.io event "connect"
         *
         * This function would be called when the related SIOClient object receive messages that mean it have connected
         * to endpoint successfully.
         *
         * @param client the connected SIOClient object.
         */
        virtual void onConnect(SIOClient* client) { AXLOG("SIODelegate onConnect fired"); };
        /**
         * This is kept for backwards compatibility, message is now fired as a socket.io event "message"
         *
         * This function would be called when the related SIOClient object receive message or json message.
         *
         * @param client the connected SIOClient object.
         * @param data the message,it could be json message
         */
        virtual void onMessage(SIOClient* client, std::string_view data)
        {
            AXLOG("SIODelegate onMessage fired with data: %s", data.data());
        };
        /**
         * Pure virtual callback function, this function should be overridden by the subclass.
         *
         * This function would be called when the related SIOClient object disconnect or receive disconnect signal.
         *
         * @param client the connected SIOClient object.
         */
        virtual void onClose(SIOClient* client) = 0;
        /**
         * Pure virtual callback function, this function should be overridden by the subclass.
         *
         * This function would be called when the related SIOClient object receive error signal or didn't connect the
         * endpoint but do some network operation, eg.,send and emit,etc.
         *
         * @param client the connected SIOClient object.
         * @param data the error message
         */
        virtual void onError(SIOClient* client, std::string_view data) = 0;
        /**
         * Fire event to script when the related SIOClient object receive the fire event signal.
         *
         * @param client the connected SIOClient object.
         * @param eventName the event's name.
         * @param data the event's data information.
         */
        virtual void fireEventToScript(SIOClient* client, std::string_view eventName, std::string_view data)
        {
            AXLOG("SIODelegate event '%s' fired with data: %s", eventName.data(), data.data());
        };
    };

    /**
     *  Static client creation method, similar to socketio.connect(uri) in JS.
     *  @param  uri      the URI of the socket.io server.
     *  @param  delegate the delegate which want to receive events from the socket.io client.
     *  @return SIOClient* an initialized SIOClient if connected successfully, otherwise nullptr.
     */
    static SIOClient* connect(std::string_view uri, SocketIO::SIODelegate& delegate);

    /**
     *  Static client creation method, similar to socketio.connect(uri) in JS.
     *  @param  uri      the URI of the socket.io server.
     *  @param  delegate the delegate which want to receive events from the socket.io client.
     *  @param caFilePath The ca file path for wss connection
     *  @return SIOClient* an initialized SIOClient if connected successfully, otherwise nullptr.
     */
    static SIOClient* connect(std::string_view uri, SocketIO::SIODelegate& delegate, std::string_view caFilePath);

private:
    SocketIO();
    virtual ~SocketIO();

    static SocketIO* _inst;

    hlookup::string_map<std::weak_ptr<SIOClientImpl>> _sockets;

    std::shared_ptr<SIOClientImpl> getSocket(const std::string_view& uri);
    void addSocket(const std::string_view& uri, std::shared_ptr<SIOClientImpl>& socket);
    void removeSocket(const std::string_view& uri);

    friend class SIOClientImpl;

private:
    AX_DISALLOW_COPY_AND_ASSIGN(SocketIO)
};

// c++11 style callbacks entities will be created using CC_CALLBACK (which uses std::bind)
typedef std::function<void(SIOClient*, std::string_view)> SIOEvent;
// c++11 map to callbacks
typedef hlookup::string_map<SIOEvent> EventRegistry;

/**
 * A single connection to a socket.io endpoint.
 *
 * @lua NA
 */
class AX_DLL SIOClient : public ax::Ref
{
private:
    friend class SocketIO;  // Only SocketIO class could contruct a SIOClient instance.

    std::string _path, _tag;
    bool _connected;
    std::shared_ptr<SIOClientImpl> _socket;

    SocketIO::SIODelegate* _delegate = nullptr;

    EventRegistry _eventRegistry;

    void fireEvent(std::string_view eventName, std::string_view data);

    void onOpen();
    void onConnect();
    void socketClosed();

    friend class SIOClientImpl;

    bool isConnected() const;
    void setConnected(bool);

    /**
     * Constructor of SIOClient class.
     *
     * @param host the string that represent the host address.
     * @param port the int value represent the port number.
     * @param path the string that represent endpoint.
     * @param impl the SIOClientImpl object.
     * @param delegate the SIODelegate object.
     */
    SIOClient(const std::string_view& path, std::shared_ptr<SIOClientImpl>& impl, SocketIO::SIODelegate& delegate);
    /**
     * Destructor of SIOClient class.
     */
    virtual ~SIOClient();

public:
    /**
     * Get the delegate for the client
     * @return the delegate object for the client
     */
    SocketIO::SIODelegate* getDelegate() { return _delegate; };

    /**
     * Disconnect from the endpoint, onClose will be called for the delegate when complete
     */
    void disconnect();
    /**
     * Send a message to the socket.io server.
     *
     * @param s message.
     */
    void send(std::string_view s);
    void send(const std::vector<std::string_view>& s);

    /**
     *  Emit the eventname and the args to the endpoint that _path point to.
     * @param eventname
     * @param args
     */
    void emit(std::string_view eventname, std::string_view args);
    void emit(std::string_view eventname, const std::vector<std::string_view>& args);

    /**
     * Used to register a socket.io event callback.
     * Event argument should be passed using CC_CALLBACK2(&Base::function, this).
     * @param eventName the name of event.
     * @param e the callback function.
     */
    void on(std::string_view eventName, SIOEvent e);

    /**
     * Set tag of SIOClient.
     * The tag is used to distinguish the various SIOClient objects.
     * @param tag string object.
     */
    void setTag(std::string_view tag);

    /**
     * Get tag of SIOClient.
     * @return const char* the pointer point to the _tag.
     */
    std::string_view getTag() { return _tag; }
};

}  // namespace network

NS_AX_END

// end group
/// @}

#endif /* defined(__CC_JSB_SOCKETIO_H__) */
