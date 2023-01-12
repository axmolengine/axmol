/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2023 Bytedance Inc.
 
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

//
//  WebSocketTest.h
//  TestCpp
//
//  Created by James Chen on 5/31/13.
//
//

#ifndef __TestCpp__WebSocketTest__
#define __TestCpp__WebSocketTest__

#include "axmol.h"
#include "extensions/cocos-ext.h"
#include "network/WebSocket.h"
#include "network/SocketIO.h"
#include "BaseTest.h"

DEFINE_TEST_SUITE(WebSocketTests);

class WebSocketTest : public TestCase
, public ax::network::WebSocket::Delegate
{
public:
    CREATE_FUNC(WebSocketTest);

    WebSocketTest();
    virtual ~WebSocketTest();

    virtual void onExit() override;
    
    virtual void onOpen(ax::network::WebSocket* ws)override;
    virtual void onMessage(ax::network::WebSocket* ws, const ax::network::WebSocket::Data& data)override;
    virtual void onClose(ax::network::WebSocket* ws)override;
    virtual void onError(ax::network::WebSocket* ws, const ax::network::WebSocket::ErrorCode& error)override;
    
    // Menu Callbacks
    void onMenuSendTextClicked(ax::Ref *sender);
    void onMenuSendMultipleTextClicked(ax::Ref *sender);
    void onMenuSendBinaryClicked(ax::Ref *sender);

    virtual std::string title() const override { return "WebSocket Test"; }
    void startTestCallback(ax::Ref* sender);

private:
    ax::network::WebSocket* _wsiSendText;
    ax::network::WebSocket* _wsiSendBinary;
    ax::network::WebSocket* _wsiError;
    
    ax::Label* _sendTextStatus;
    ax::Label* _sendBinaryStatus;
    ax::Label* _errorStatus;
    ax::Menu* _startTestMenu;
    
    int _sendTextTimes;
    int _sendBinaryTimes;
};

class WebSocketCloseTest : public TestCase
    , public ax::network::WebSocket::Delegate
{
public:
    CREATE_FUNC(WebSocketCloseTest);

    virtual void onOpen(ax::network::WebSocket* ws)override;
    virtual void onMessage(ax::network::WebSocket* ws, const ax::network::WebSocket::Data& data)override;
    virtual void onClose(ax::network::WebSocket* ws)override;
    virtual void onError(ax::network::WebSocket* ws, const ax::network::WebSocket::ErrorCode& error)override;

    WebSocketCloseTest();
    virtual ~WebSocketCloseTest();

    virtual std::string title() const override { return "WebSocket close by resetDirector event Test"; }
    std::string subtitle() const override { return "Click close button, Program should close without crash."; }

private:
    ax::network::WebSocket* _wsiTest;
};

class WebSocketDelayTest : public TestCase
, public ax::network::WebSocket::Delegate
{
public:
    CREATE_FUNC(WebSocketDelayTest);

    WebSocketDelayTest();
    virtual ~WebSocketDelayTest();

    virtual void onExit() override;
    
    virtual void onOpen(ax::network::WebSocket* ws)override;
    virtual void onMessage(ax::network::WebSocket* ws, const ax::network::WebSocket::Data& data)override;
    virtual void onClose(ax::network::WebSocket* ws)override;
    virtual void onError(ax::network::WebSocket* ws, const ax::network::WebSocket::ErrorCode& error)override;
    
    // Menu Callbacks
    void onMenuSendTextClicked(ax::Ref *sender);

    virtual std::string title() const override { return "WebSocket Delay Test"; }
    void startTestCallback(ax::Ref* sender);

    int64_t getNowMircroSeconds()
    {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    }

    void doSendText();
    void doReceiveText();

private:
    ax::network::WebSocket* _wsiSendText;
    
    ax::Label* _sendTextStatus;
    ax::Label* _progressStatus;
    ax::Menu* _startTestMenu;
   
    int64_t _totalDelayMircoSec = 0;
    int64_t _sendTimeMircoSec = 0;
    int64_t _receiveTimeMircoSec = 0;

    int _sendTextTimes = 0;
    int _receiveTextTimes = 0;
};

class SocketIOTest: public TestCase
	, public ax::network::SocketIO::SIODelegate
{
public:
    CREATE_FUNC(SocketIOTest);

    SocketIOTest();
    virtual ~SocketIOTest();

	/**
	*  @brief Used for network level socket close (not for disconnect from the socket.io server)
	*/
	virtual void onClose(ax::network::SIOClient* client)override;
	/**
	*  @brief Used for network level socket error (not for disconnect from the socket.io server)
	**/
	virtual void onError(ax::network::SIOClient* client, std::string_view data)override;
	/**
	*  @brief Common function to call on both socket.io disconnect and websocket close
	**/
	void closedSocketAction(ax::network::SIOClient* client);
	
	// test action handlers for main Test Client that connects to default namespace "" or "/"
	void onMenuSIOClientClicked(ax::Ref *sender);
	void onMenuTestMessageClicked(ax::Ref *sender);
	void onMenuTestEventClicked(ax::Ref *sender);
	void onMenuTestClientDisconnectClicked(ax::Ref *sender);

	// test action handlers for Test Endpoint that connects to /testpoint endpoint
	void onMenuSIOEndpointClicked(ax::Ref *sender);
	void onMenuTestMessageEndpointClicked(ax::Ref *sender);
	void onMenuTestEventEndpointClicked(ax::Ref *sender);
	void onMenuTestEndpointDisconnectClicked(ax::Ref *sender);

	// custom handlers for socket.io related events

	/**
	*  @brief Socket.io event handler for custom event "testevent"
	**/
	void testevent(ax::network::SIOClient *client, std::string_view data);
	/**
	*  @brief Socket.io event handler for custom event "echoevent"
	**/
	void echotest(ax::network::SIOClient *client, std::string_view data);
	/**
	*  @brief Socket.io event handler for event "connect"
	**/
	void connect(ax::network::SIOClient* client, std::string_view data);
	/**
	*  @brief Socket.io event handler for event "disconnect"
	**/
	void disconnect(ax::network::SIOClient* client, std::string_view data);
	/**
	*  @brief Socket.io event handler for event "message"
	**/
	void message(ax::network::SIOClient* client, std::string_view data);
	/**
	*  @brief Socket.io event handler for event "json"
	*         This is only used in v 0.9.x, in 1.x this is handled as a "message" event
	**/
	void json(ax::network::SIOClient* client, std::string_view data);

    virtual std::string title() const override{ return "SocketIO Extension Test"; }

protected:

	ax::network::SIOClient *_sioClient, *_sioEndpoint;

	ax::Label *_sioClientStatus;
};


#endif /* defined(__TestCpp__WebSocketTest__) */
