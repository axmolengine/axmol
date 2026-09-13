/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
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

#include "axmol/axmol.h"
#include "extensions/axmol-ext.h"
#include "axmol/network/WebSocket.h"
#include "BaseTest.h"

DEFINE_TEST_SUITE(WebSocketTests);

class WebSocketTest : public TestCase, public ax::network::WebSocket::Delegate
{
public:
    CREATE_FUNC(WebSocketTest);

    WebSocketTest();
    virtual ~WebSocketTest();

    virtual void onExit() override;

    virtual void onOpen(ax::network::WebSocket* ws) override;
    virtual void onMessage(ax::network::WebSocket* ws, const ax::network::WebSocket::Data& data) override;
    virtual void onClose(ax::network::WebSocket* ws, uint16_t code, std::string_view reason) override;
    virtual void onError(ax::network::WebSocket* ws, const ax::network::WebSocket::ErrorCode& error) override;

    // Menu Callbacks
    void onMenuSendTextClicked(ax::Object* sender);
    void onMenuSendMultipleTextClicked(ax::Object* sender);
    void onMenuSendBinaryClicked(ax::Object* sender);

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

class WebSocketCloseTest : public TestCase, public ax::network::WebSocket::Delegate
{
public:
    CREATE_FUNC(WebSocketCloseTest);

    virtual void onOpen(ax::network::WebSocket* ws) override;
    virtual void onMessage(ax::network::WebSocket* ws, const ax::network::WebSocket::Data& data) override;
    virtual void onClose(ax::network::WebSocket* ws, uint16_t code, std::string_view reason) override;
    virtual void onError(ax::network::WebSocket* ws, const ax::network::WebSocket::ErrorCode& error) override;

    WebSocketCloseTest();
    virtual ~WebSocketCloseTest();

    virtual std::string title() const override { return "WebSocket close by resetDirector event Test"; }
    std::string subtitle() const override { return "Click close button, Program should close without crash."; }

private:
    ax::network::WebSocket* _wsiTest;
};

class WebSocketDelayTest : public TestCase, public ax::network::WebSocket::Delegate
{
public:
    CREATE_FUNC(WebSocketDelayTest);

    WebSocketDelayTest();
    virtual ~WebSocketDelayTest();

    virtual void onExit() override;

    virtual void onOpen(ax::network::WebSocket* ws) override;
    virtual void onMessage(ax::network::WebSocket* ws, const ax::network::WebSocket::Data& data) override;
    virtual void onClose(ax::network::WebSocket* ws, uint16_t code, std::string_view reason) override;
    virtual void onError(ax::network::WebSocket* ws, const ax::network::WebSocket::ErrorCode& error) override;

    // Menu Callbacks
    void onMenuSendTextClicked(ax::Object* sender);

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

    int64_t _totalDelayMircoSec  = 0;
    int64_t _sendTimeMircoSec    = 0;
    int64_t _receiveTimeMircoSec = 0;

    int _sendTextTimes    = 0;
    int _receiveTextTimes = 0;
};

#endif /* defined(__TestCpp__WebSocketTest__) */
