/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmolengine.github.io/
 
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
#include "extensions/axmol-ext.h"
#include "network/WebSocket.h"
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
    void onMenuSendTextClicked(ax::Object *sender);
    void onMenuSendMultipleTextClicked(ax::Object *sender);
    void onMenuSendBinaryClicked(ax::Object *sender);

    virtual std::string title() const override { return "WebSocket Test"; }
    void startTestCallback(ax::Object* sender);

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
    void onMenuSendTextClicked(ax::Object *sender);

    virtual std::string title() const override { return "WebSocket Delay Test"; }
    void startTestCallback(ax::Object* sender);

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

#endif /* defined(__TestCpp__WebSocketTest__) */
