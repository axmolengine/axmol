/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
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

#include "WebSocketTest.h"
#include "../NetworkTest.h"
#include "testResource.h"

#include "WebSocketTest.h"

#include "fmt/format.h"
#include "fmt/compile.h"

// "ws://echo.websocket.org no longer avaiable: https://www.lob.com/blog/websocket-org-is-down-here-is-an-alternative
// other public test servers:
//   - "wss://socketsbay.com/wss/v2/1/demo/"
#define ECHO_SERVER_URL "ws://echo.websocket.events/.ws"


USING_NS_AX;
using namespace ax::network;
//USING_NS_AX_EXT;

WebSocketTests::WebSocketTests()
{
    ADD_TEST_CASE(WebSocketTest);
    ADD_TEST_CASE(WebSocketCloseTest);
    ADD_TEST_CASE(WebSocketDelayTest);
    ADD_TEST_CASE(SocketIOTest);
}

WebSocketTest::WebSocketTest()
: _wsiSendText(nullptr)
, _wsiSendBinary(nullptr)
, _wsiError(nullptr)
, _sendTextStatus(nullptr)
, _sendBinaryStatus(nullptr)
, _errorStatus(nullptr)
, _sendTextTimes(0)
, _sendBinaryTimes(0)
{
    auto winSize = Director::getInstance()->getWinSize();
    
    const int MARGIN = 40;
    const int SPACE = 35;
    
    auto menuRequest = Menu::create();
    menuRequest->setPosition(Vec2::ZERO);
    addChild(menuRequest);
    
    // Send Text
    auto labelSendText = Label::createWithTTF("Send Text", "fonts/arial.ttf", 20);
    auto itemSendText = MenuItemLabel::create(labelSendText, AX_CALLBACK_1(WebSocketTest::onMenuSendTextClicked, this));
    itemSendText->setPosition(Vec2(winSize.width / 2, winSize.height - MARGIN - SPACE));
    menuRequest->addChild(itemSendText);
    
    labelSendText = Label::createWithTTF("Send Multiple Text", "fonts/arial.ttf", 20);
    itemSendText = MenuItemLabel::create(labelSendText, AX_CALLBACK_1(WebSocketTest::onMenuSendMultipleTextClicked, this));
    itemSendText->setPosition(Vec2(winSize.width / 2, winSize.height - MARGIN - 2 * SPACE));
    menuRequest->addChild(itemSendText);
    
    // Send Binary
    auto labelSendBinary = Label::createWithTTF("Send Binary", "fonts/arial.ttf", 20);
    auto itemSendBinary = MenuItemLabel::create(labelSendBinary, AX_CALLBACK_1(WebSocketTest::onMenuSendBinaryClicked, this));
    itemSendBinary->setPosition(Vec2(winSize.width / 2, winSize.height - MARGIN - 3 * SPACE));
    menuRequest->addChild(itemSendBinary);
    

    // Send Text Status Label
    _sendTextStatus = Label::createWithTTF("Send Text WS is waiting...", "fonts/arial.ttf", 16, Size(160, 100), TextHAlignment::CENTER, TextVAlignment::TOP);
    _sendTextStatus->setAnchorPoint(Vec2(0, 0));
    _sendTextStatus->setPosition(Vec2(VisibleRect::left().x, VisibleRect::rightBottom().y + 25));
    this->addChild(_sendTextStatus);
    
    // Send Binary Status Label
    _sendBinaryStatus = Label::createWithTTF("Send Binary WS is waiting...", "fonts/arial.ttf", 16, Size(160, 100), TextHAlignment::CENTER, TextVAlignment::TOP);
    _sendBinaryStatus->setAnchorPoint(Vec2(0, 0));
    _sendBinaryStatus->setPosition(Vec2(VisibleRect::left().x + 160, VisibleRect::rightBottom().y + 25));
    this->addChild(_sendBinaryStatus);
    
    // Error Label
    _errorStatus = Label::createWithTTF("Error WS is waiting...", "fonts/arial.ttf", 16, Size(160, 100), TextHAlignment::CENTER, TextVAlignment::TOP);
    _errorStatus->setAnchorPoint(Vec2(0, 0));
    _errorStatus->setPosition(Vec2(VisibleRect::left().x + 320, VisibleRect::rightBottom().y + 25));
    this->addChild(_errorStatus);
    
    auto startTestLabel = Label::createWithTTF("Start Test WebSocket", "fonts/arial.ttf", 16);
    auto startTestItem = MenuItemLabel::create(startTestLabel, AX_CALLBACK_1(WebSocketTest::startTestCallback, this));
    startTestItem->setPosition(Vec2(VisibleRect::center().x - 150, VisibleRect::bottom().y + 150));
    _startTestMenu = Menu::create(startTestItem, nullptr);
    _startTestMenu->setPosition(Vec2::ZERO);
    this->addChild(_startTestMenu, 1);
}

WebSocketTest::~WebSocketTest()
{

}

void WebSocketTest::onExit()
{
    if (_wsiSendText)
    {
        _wsiSendText->closeAsync();
        AX_SAFE_DELETE(_wsiSendText);
    }

    if (_wsiSendBinary)
    {
        _wsiSendBinary->closeAsync();
        AX_SAFE_DELETE(_wsiSendBinary);
    }

    if (_wsiError)
    {
        _wsiError->closeAsync();
        AX_SAFE_DELETE(_wsiError);
    }
    Node::onExit();
}

void WebSocketTest::startTestCallback(Ref* sender)
{
    removeChild(_startTestMenu);
    _startTestMenu = nullptr;

    _wsiSendText = new network::WebSocket();
    _wsiSendBinary = new network::WebSocket();
    _wsiError = new network::WebSocket();

    std::vector<std::string> protocols;
    protocols.push_back("myprotocol_1");
    protocols.push_back("myprotocol_2");
    if (!_wsiSendText->init(*this, ECHO_SERVER_URL, &protocols, "cacert.pem"))
    {
        AX_SAFE_DELETE(_wsiSendText);
    }
    else
    {
        retain(); // Retain self to avoid WebSocketTest instance be deleted immediately, it will be released in WebSocketTest::onClose.
    }

    protocols.erase(protocols.begin());
    if (!_wsiSendBinary->init(*this, ECHO_SERVER_URL, &protocols, "cacert.pem"))
    {
        AX_SAFE_DELETE(_wsiSendBinary);
    }
    else
    {
        retain(); // Retain self to avoid WebSocketTest instance be deleted immediately, it will be released in WebSocketTest::onClose.
    }

    if (!_wsiError->init(*this, "ws://invalid.urlxxxxxxxx.com"))
    {
        AX_SAFE_DELETE(_wsiError);
    }
    else
    {
        retain(); // Retain self to avoid WebSocketTest instance be deleted immediately, it will be released in WebSocketTest::onClose.
    }
}

// Delegate methods
void WebSocketTest::onOpen(network::WebSocket* ws)
{
    char status[256] = {0};
    sprintf(status, "Opened, url: %s, protocol: %s", ws->getUrl().c_str(), ws->getProtocol().c_str());

    log("Websocket (%p) was opened, url: %s, protocol: %s", ws, ws->getUrl().c_str(), ws->getProtocol().c_str());
    if (ws == _wsiSendText)
    {
        _sendTextStatus->setString(status);
    }
    else if (ws == _wsiSendBinary)
    {
        _sendBinaryStatus->setString(status);
    }
    else if (ws == _wsiError)
    {
        AXASSERT(0, "error test will never go here.");
    }
}

void WebSocketTest::onMessage(network::WebSocket* ws, const network::WebSocket::Data& data)
{
    if (!data.isBinary)
    {
        _sendTextTimes++;
        std::string textStr = fmt::format(FMT_COMPILE("#{} response text msg: {}"), _sendTextTimes, std::string_view{data.bytes, static_cast<size_t>(data.len)});
        log("%s", textStr.c_str());
        
        _sendTextStatus->setString(textStr.c_str());
    }
    else
    {
        _sendBinaryTimes++;
        char times[100] = {0};
        sprintf(times, "%d", _sendBinaryTimes);

        std::string binaryStr = "response bin msg: ";
        
        for (int i = 0; i < data.len; ++i) {
            if (data.bytes[i] != '\0')
            {
                binaryStr += data.bytes[i];
            }
            else
            {
                binaryStr += "\'\\0\'";
            }
        }
        
        binaryStr += std::string(", ")+times;
        log("%s", binaryStr.c_str());
        _sendBinaryStatus->setString(binaryStr.c_str());
    }
}

void WebSocketTest::onClose(network::WebSocket* ws)
{
    log("onClose: websocket instance (%p) closed.", ws);
    if (ws == _wsiSendText)
    {
        // _wsiSendText = nullptr;
        _sendTextStatus->setString("Send Text WS was closed");
    }
    else if (ws == _wsiSendBinary)
    {
        // _wsiSendBinary = nullptr;
        _sendBinaryStatus->setString("Send Binary WS was closed");
    }
    else if (ws == _wsiError)
    {
        // _wsiError = nullptr;
        _errorStatus->setString("Test invalid URL WS was closed");
    }
    // Delete websocket instance.
    // AX_SAFE_DELETE(ws);
    log("WebSocketTest ref: %u", _referenceCount);
    release();
}

void WebSocketTest::onError(network::WebSocket* ws, const network::WebSocket::ErrorCode& error)
{
    log("Error was fired, error code: %d", static_cast<int>(error));
    char buf[100] = {0};
    sprintf(buf, "An error was fired, code: %d", static_cast<int>(error));

    if (ws == _wsiSendText)
    {
        _sendTextStatus->setString(buf);
    }
    else if (ws == _wsiSendBinary)
    {
        _sendBinaryStatus->setString(buf);
    }
    else if (ws == _wsiError)
    {
        _errorStatus->setString(buf);
    }
}

// Menu Callbacks
void WebSocketTest::onMenuSendTextClicked(ax::Ref *sender)
{
    if (! _wsiSendText)
    {
        return;
    }

    if (_wsiSendText->getReadyState() == network::WebSocket::State::OPEN)
    {
        _sendTextStatus->setString("Send Text WS is waiting...");
        _wsiSendText->send("Hello WebSocket, I'm a text message from axmol-1.0.0 game engine.");
    }
    else
    {
        std::string warningStr = "send text websocket instance wasn't ready...";
        log("%s", warningStr.c_str());
        _sendTextStatus->setString(warningStr.c_str());
    }
}

void WebSocketTest::onMenuSendMultipleTextClicked(ax::Ref *sender)
{
    if (! _wsiSendText)
    {
        return;
    }
    
    if (_wsiSendText->getReadyState() == network::WebSocket::State::OPEN)
    {
        _sendTextStatus->setString("Send Multiple Text WS is waiting...");
        for (int index = 0; index < 15; ++index) {
            _wsiSendText->send(StringUtils::format("Hello WebSocket, text message index:%d", index));
        }
    }
    else
    {
        std::string warningStr = "send text websocket instance wasn't ready...";
        log("%s", warningStr.c_str());
        _sendTextStatus->setString(warningStr.c_str());
    }
}

void WebSocketTest::onMenuSendBinaryClicked(ax::Ref *sender)
{
    if (! _wsiSendBinary) {
        return;
    }

    if (_wsiSendBinary->getReadyState() == network::WebSocket::State::OPEN)
    {
        _sendBinaryStatus->setString("Send Binary WS is waiting...");
        char buf[] = "Hello WebSocket,\0 I'm\0 a\0 binary\0 message\0.";
        _wsiSendBinary->send((unsigned char*)buf, sizeof(buf));
    }
    else
    {
        std::string warningStr = "send binary websocket instance wasn't ready...";
        log("%s", warningStr.c_str());
        _sendBinaryStatus->setString(warningStr.c_str());
    }
}

WebSocketCloseTest::WebSocketCloseTest()
: _wsiTest(nullptr)
{
    auto winSize = Director::getInstance()->getWinSize();

    _wsiTest = new network::WebSocket();

    if (!_wsiTest->init(*this, ECHO_SERVER_URL))
    {
        delete _wsiTest;
        _wsiTest = nullptr;
    }

    auto closeItem = MenuItemImage::create(s_pathClose, s_pathClose, [](Ref* sender){
        Director::getInstance()->end();
    });
    closeItem->setPosition(VisibleRect::right().x / 2, VisibleRect::top().y * 2 / 3);

    auto menu = Menu::create(closeItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    addChild(menu, 1);

    auto notifyLabel = Label::createWithTTF("See log window, when enter there's should have\n'Websocket opened' log,\nwhen close there's should have'websocket closed' log", "fonts/arial.ttf", 20);
    notifyLabel->setPosition(VisibleRect::right().x / 2, VisibleRect::top().y / 3);
    notifyLabel->setAlignment(TextHAlignment::CENTER);
    addChild(notifyLabel, 1);
}

WebSocketCloseTest::~WebSocketCloseTest()
{
    if (_wsiTest != nullptr)
    {
        _wsiTest->close();
        delete _wsiTest;
        _wsiTest = nullptr;
    }
}

// Delegate methods
void WebSocketCloseTest::onOpen(network::WebSocket* ws)
{
    log("Websocket (%p) opened", ws);
}

void WebSocketCloseTest::onMessage(network::WebSocket* ws, const network::WebSocket::Data& data)
{
    log("Websocket get message from %p", ws);
}

void WebSocketCloseTest::onClose(network::WebSocket* ws)
{
    log("websocket (%p) closed.", ws);
    // if (ws == _wsiTest) {
    //     _wsiTest = nullptr;
    // }
    // AX_SAFE_DELETE(ws);
}

void WebSocketCloseTest::onError(network::WebSocket* ws, const network::WebSocket::ErrorCode& error)
{
    log("Error was fired, error code: %d", static_cast<int>(error));
}


#define SEND_TEXT_TIMES 100

WebSocketDelayTest::WebSocketDelayTest()
: _wsiSendText(nullptr)
, _sendTextStatus(nullptr)
, _progressStatus(nullptr)
, _sendTextTimes(0)
{
    auto winSize = Director::getInstance()->getWinSize();
    
    const int MARGIN = 40;
    const int SPACE = 35;
    
    auto menuRequest = Menu::create();
    menuRequest->setPosition(Vec2::ZERO);
    addChild(menuRequest);
    
    // Send Text
    char cmdLabel[60] = { 0 };
    snprintf(cmdLabel, 60, "Send %d Text", SEND_TEXT_TIMES);
    auto labelSendText = Label::createWithTTF(cmdLabel, "fonts/arial.ttf", 20);
    auto itemSendText = MenuItemLabel::create(labelSendText, AX_CALLBACK_1(WebSocketDelayTest::onMenuSendTextClicked, this));
    itemSendText->setPosition(Vec2(winSize.width / 2, winSize.height - MARGIN - SPACE));
    menuRequest->addChild(itemSendText);
    
    // Send Text Status Label
    _sendTextStatus = Label::createWithTTF("Waiting connection...", "fonts/arial.ttf", 16, Size(160, 100), TextHAlignment::CENTER, TextVAlignment::TOP);
    _sendTextStatus->setAnchorPoint(Vec2(0, 0));
    _sendTextStatus->setPosition(Vec2(VisibleRect::left().x, VisibleRect::rightBottom().y + 25));
    this->addChild(_sendTextStatus);
   

    // Error Label
    _progressStatus = Label::createWithTTF(".", "fonts/arial.ttf", 16, Size(160, 100), TextHAlignment::CENTER, TextVAlignment::TOP);
    _progressStatus->setAnchorPoint(Vec2(0, 0));
    _progressStatus->setPosition(Vec2(VisibleRect::left().x + 320, VisibleRect::rightBottom().y + 25));
    this->addChild(_progressStatus);
    
    auto startTestLabel = Label::createWithTTF("DO Connect!", "fonts/arial.ttf", 16);
    auto startTestItem = MenuItemLabel::create(startTestLabel, AX_CALLBACK_1(WebSocketDelayTest::startTestCallback, this));
    startTestItem->setPosition(Vec2(VisibleRect::center().x - 150, VisibleRect::bottom().y + 150));
    _startTestMenu = Menu::create(startTestItem, nullptr);
    _startTestMenu->setPosition(Vec2::ZERO);
    this->addChild(_startTestMenu, 1);
}

WebSocketDelayTest::~WebSocketDelayTest()
{

}

void WebSocketDelayTest::onExit()
{
    if (_wsiSendText)
    {
        _wsiSendText->close();
        delete _wsiSendText;
        _wsiSendText = nullptr;
    }

    Node::onExit();
}

void WebSocketDelayTest::startTestCallback(Ref* sender)
{
    removeChild(_startTestMenu);
    _startTestMenu = nullptr;

    _wsiSendText = new network::WebSocket();
    
    std::vector<std::string> protocols;
    protocols.push_back("myprotocol_1");
    protocols.push_back("myprotocol_2");
    if (!_wsiSendText->init(*this, ECHO_SERVER_URL, &protocols, "cacert.pem"))
    {
        AX_SAFE_DELETE(_wsiSendText);
    }
    else
    {
        retain(); // Retain self to avoid WebSocketDelayTest instance be deleted immediately, it will be released in WebSocketDelayTest::onClose.

    }

}

void WebSocketDelayTest::doSendText()
{
    _sendTextTimes += 1;
    if (_sendTextTimes > SEND_TEXT_TIMES) 
    {
        _sendTextStatus->setString("Test Done!");
        return;
    }

    char statueBuffer[80] = { 0 };
    snprintf(statueBuffer, 80, "Sending #%d/%d text", _sendTextTimes, SEND_TEXT_TIMES);
    _sendTextStatus->setString(statueBuffer);
    _sendTimeMircoSec = getNowMircroSeconds();
    _wsiSendText->send("Hello WebSocket, I'm a text message.");
}

void WebSocketDelayTest::doReceiveText()
{
    _receiveTimeMircoSec = getNowMircroSeconds();
    if(_sendTimeMircoSec > 0)
        _totalDelayMircoSec += (_receiveTimeMircoSec - _sendTimeMircoSec);
    doSendText(); //send next 
}

// Delegate methods
void WebSocketDelayTest::onOpen(network::WebSocket* ws)
{
    char status[256] = {0};
    sprintf(status, "Opened, url: %s, protocol: %s", ws->getUrl().c_str(), ws->getProtocol().c_str());

    log("Websocket (%p) was opened, url: %s, protocol: %s", ws, ws->getUrl().c_str(), ws->getProtocol().c_str());
    if (ws == _wsiSendText)
    {
        _sendTextStatus->setString(status);

    }
}

void WebSocketDelayTest::onMessage(network::WebSocket* ws, const network::WebSocket::Data& data)
{
    if (!data.isBinary)
    {
        _receiveTextTimes++;
        char times[100] = {0};
        sprintf(times, "%d", _receiveTextTimes);
        std::string textStr = std::string("response text msg: ")+data.bytes+", "+times;
        log("%s", textStr.c_str());
        doReceiveText();
        memset(times, 0, 100);
        snprintf(times, 100, "total delay %f seconds", (float)(_totalDelayMircoSec/ 1000000.0));
        _progressStatus->setString(times);
    }
}

void WebSocketDelayTest::onClose(network::WebSocket* ws)
{
    log("onClose: websocket instance (%p) closed.", ws);
    if (ws == _wsiSendText)
    {
        // delete _wsiSendText;
        // _wsiSendText = nullptr;
        _sendTextStatus->setString("Send Text WS was closed");
    }
    log("WebSocketDelayTest ref: %u", _referenceCount);
    release();
}

void WebSocketDelayTest::onError(network::WebSocket* ws, const network::WebSocket::ErrorCode& error)
{
    log("Error was fired, error code: %d", static_cast<int>(error));
    char buf[100] = {0};
    sprintf(buf, "An error was fired, code: %d", static_cast<int>(error));

    if (ws == _wsiSendText)
    {
        _sendTextStatus->setString(buf);
    }

    _sendTimeMircoSec = 0;
}

// Menu Callbacks
void WebSocketDelayTest::onMenuSendTextClicked(ax::Ref *sender)
{
    if (! _wsiSendText)
    {
        return;
    }

    if (_wsiSendText->getReadyState() == network::WebSocket::State::OPEN)
    {
        
        _sendTextTimes = 0;
        _receiveTextTimes = 0;
        doSendText();
    }
    else
    {
        std::string warningStr = "send text websocket instance wasn't ready...";
        log("%s", warningStr.c_str());
        _sendTextStatus->setString(warningStr.c_str());
    }
}

SocketIOTest::SocketIOTest()
	: _sioClient(nullptr)
	, _sioEndpoint(nullptr)
	, _sioClientStatus(nullptr)
{//set the clients to nullptr until we are ready to connect

	Size winSize = Director::getInstance()->getWinSize();

	const int MARGIN = 40;
	const int SPACE = 35;

	auto menuRequest = Menu::create();
	menuRequest->setPosition(Vec2::ZERO);
	addChild(menuRequest);

	// Test to create basic client in the default namespace
	auto labelSIOClient = Label::createWithTTF("Open SocketIO Client", "fonts/arial.ttf", 22);
	auto itemSIOClient = MenuItemLabel::create(labelSIOClient, AX_CALLBACK_1(SocketIOTest::onMenuSIOClientClicked, this));
	itemSIOClient->setPosition(Vec2(VisibleRect::left().x + labelSIOClient->getContentSize().width / 2 + 5, winSize.height - MARGIN - SPACE));
	menuRequest->addChild(itemSIOClient);

	// Test to create a client at the endpoint '/testpoint'
	auto labelSIOEndpoint = Label::createWithTTF("Open SocketIO Endpoint", "fonts/arial.ttf", 22);
	auto itemSIOEndpoint = MenuItemLabel::create(labelSIOEndpoint, AX_CALLBACK_1(SocketIOTest::onMenuSIOEndpointClicked, this));
	itemSIOEndpoint->setPosition(Vec2(VisibleRect::right().x - labelSIOEndpoint->getContentSize().width / 2 - 5, winSize.height - MARGIN - SPACE));
	menuRequest->addChild(itemSIOEndpoint);

	// Test sending message to default namespace
	auto labelTestMessage = Label::createWithTTF("Send Test Message", "fonts/arial.ttf", 22);
	auto itemTestMessage = MenuItemLabel::create(labelTestMessage, AX_CALLBACK_1(SocketIOTest::onMenuTestMessageClicked, this));
	itemTestMessage->setPosition(Vec2(VisibleRect::left().x + labelTestMessage->getContentSize().width / 2 + 5, winSize.height - MARGIN - 2 * SPACE));
	menuRequest->addChild(itemTestMessage);

	// Test sending message to the endpoint '/testpoint'
	auto labelTestMessageEndpoint = Label::createWithTTF("Test Endpoint Message", "fonts/arial.ttf", 22);
	auto itemTestMessageEndpoint = MenuItemLabel::create(labelTestMessageEndpoint, AX_CALLBACK_1(SocketIOTest::onMenuTestMessageEndpointClicked, this));
	itemTestMessageEndpoint->setPosition(Vec2(VisibleRect::right().x - labelTestMessageEndpoint->getContentSize().width / 2 - 5, winSize.height - MARGIN - 2 * SPACE));
	menuRequest->addChild(itemTestMessageEndpoint);

	// Test sending event 'echotest' to default namespace
	auto labelTestEvent = Label::createWithTTF("Send Test Event", "fonts/arial.ttf", 22);
	auto itemTestEvent = MenuItemLabel::create(labelTestEvent, AX_CALLBACK_1(SocketIOTest::onMenuTestEventClicked, this));
	itemTestEvent->setPosition(Vec2(VisibleRect::left().x + labelTestEvent->getContentSize().width / 2 + 5, winSize.height - MARGIN - 3 * SPACE));
	menuRequest->addChild(itemTestEvent);

	// Test sending event 'echotest' to the endpoint '/testpoint'
	auto labelTestEventEndpoint = Label::createWithTTF("Test Endpoint Event", "fonts/arial.ttf", 22);
	auto itemTestEventEndpoint = MenuItemLabel::create(labelTestEventEndpoint, AX_CALLBACK_1(SocketIOTest::onMenuTestEventEndpointClicked, this));
	itemTestEventEndpoint->setPosition(Vec2(VisibleRect::right().x - labelTestEventEndpoint->getContentSize().width / 2 - 5, winSize.height - MARGIN - 3 * SPACE));
	menuRequest->addChild(itemTestEventEndpoint);

	// Test disconnecting basic client
	auto labelTestClientDisconnect = Label::createWithTTF("Disconnect Socket", "fonts/arial.ttf", 22);
	auto itemClientDisconnect = MenuItemLabel::create(labelTestClientDisconnect, AX_CALLBACK_1(SocketIOTest::onMenuTestClientDisconnectClicked, this));
	itemClientDisconnect->setPosition(Vec2(VisibleRect::left().x + labelTestClientDisconnect->getContentSize().width / 2 + 5, winSize.height - MARGIN - 4 * SPACE));
	menuRequest->addChild(itemClientDisconnect);

	// Test disconnecting the endpoint '/testpoint'
	auto labelTestEndpointDisconnect = Label::createWithTTF("Disconnect Endpoint", "fonts/arial.ttf", 22);
	auto itemTestEndpointDisconnect = MenuItemLabel::create(labelTestEndpointDisconnect, AX_CALLBACK_1(SocketIOTest::onMenuTestEndpointDisconnectClicked, this));
	itemTestEndpointDisconnect->setPosition(Vec2(VisibleRect::right().x - labelTestEndpointDisconnect->getContentSize().width / 2 - 5, winSize.height - MARGIN - 4 * SPACE));
	menuRequest->addChild(itemTestEndpointDisconnect);

	// Shared Status Label
	_sioClientStatus = Label::createWithTTF("Not connected...", "fonts/arial.ttf", 14, Size(320, 100), TextHAlignment::LEFT);
	_sioClientStatus->setAnchorPoint(Vec2(0, 0));
	_sioClientStatus->setPosition(Vec2(VisibleRect::left().x, VisibleRect::rightBottom().y));
	this->addChild(_sioClientStatus);
}


SocketIOTest::~SocketIOTest()
{
}

//test event callback handlers, these will be registered with socket.io
void SocketIOTest::testevent(SIOClient *client, const std::string& data) {

    AXLOGINFO("SocketIOTest::testevent called with data: %s", data.c_str());

	std::stringstream s;
	s << client->getTag() << " received event testevent with data: " << data.c_str();

	_sioClientStatus->setString(s.str().c_str());

}

void SocketIOTest::echotest(SIOClient *client, const std::string& data) {

    AXLOGINFO("SocketIOTest::echotest called with data: %s", data.c_str());

	std::stringstream s;
	s << client->getTag() << " received event echotest with data: " << data.c_str();

	_sioClientStatus->setString(s.str().c_str());

}

// onMessage is no longer a required override from the delegate class
// 'message' events and handlers are now registered in the same way that other events are
void SocketIOTest::message(network::SIOClient* client, const std::string& data)
{
    AXLOGINFO("SocketIOTest::message received: %s", data.c_str());

	std::stringstream s;
	s << client->getTag() << " received message with content: " << data.c_str();
	_sioClientStatus->setString(s.str().c_str());

}

void SocketIOTest::json(network::SIOClient* client, const std::string& data)
{
    AXLOGINFO("SocketIOTest::json received: %s", data.c_str());

	std::stringstream s;
	s << client->getTag() << " received json message with content: " << data.c_str();
	_sioClientStatus->setString(s.str().c_str());

}

void SocketIOTest::connect(network::SIOClient* client, const std::string& data)
{
    AXLOGINFO("SocketIOTest::connect called");

	std::stringstream s;
	s << client->getTag() << " connected!";
	_sioClientStatus->setString(s.str().c_str());

}

void SocketIOTest::disconnect(network::SIOClient* client, const std::string& data)
{
    AXLOGINFO("SocketIOTest::disconnect called");

	std::stringstream s;
	s << client->getTag() << " disconnected by server!";
	_sioClientStatus->setString(s.str().c_str());

	this->closedSocketAction(client);

}

void SocketIOTest::closedSocketAction(network::SIOClient* client)
{
	//set the local pointer to nullptr or connect to another client
	//the client object will be released on its own after this method completes
	if (client == _sioClient) {

		_sioClient = nullptr;
	}
	else if (client == _sioEndpoint) {

		_sioEndpoint = nullptr;
	}
}

void SocketIOTest::onMenuSIOClientClicked(ax::Ref *sender)
{
	//create a client by using this static method, url does not need to contain the protocol
	_sioClient = SocketIO::connect("ws://tools.itharbors.com:4000", *this);
	//you may set a tag for the client for reference in callbacks
	_sioClient->setTag("Test Client");

	//register event callbacks using the AX_CALLBACK_2() macro and passing the instance of the target class
	_sioClient->on("testevent", AX_CALLBACK_2(SocketIOTest::testevent, this));
	_sioClient->on("echotest", AX_CALLBACK_2(SocketIOTest::echotest, this));
	_sioClient->on("message", AX_CALLBACK_2(SocketIOTest::message, this));
	_sioClient->on("json", AX_CALLBACK_2(SocketIOTest::json, this));
	_sioClient->on("connect", AX_CALLBACK_2(SocketIOTest::connect, this));
	_sioClient->on("disconnect", AX_CALLBACK_2(SocketIOTest::disconnect, this));

}

void SocketIOTest::onMenuSIOEndpointClicked(ax::Ref *sender)
{
	//repeat the same connection steps for the namespace "testpoint"
	_sioEndpoint = SocketIO::connect("ws://tools.itharbors.com:4000/testpoint", *this); 
	//a tag to differentiate in shared callbacks
	_sioEndpoint->setTag("Test Endpoint");

	//demonstrating how callbacks can be shared within a delegate
	_sioEndpoint->on("testevent", AX_CALLBACK_2(SocketIOTest::testevent, this));
	_sioEndpoint->on("echotest", AX_CALLBACK_2(SocketIOTest::echotest, this));
	_sioEndpoint->on("message", AX_CALLBACK_2(SocketIOTest::message, this));
	_sioEndpoint->on("json", AX_CALLBACK_2(SocketIOTest::json, this));
	_sioEndpoint->on("connect", AX_CALLBACK_2(SocketIOTest::connect, this));
	_sioEndpoint->on("disconnect", AX_CALLBACK_2(SocketIOTest::disconnect, this));

}

void SocketIOTest::onMenuTestMessageClicked(ax::Ref *sender)
{
	//check that the socket is != nullptr before sending or emitting events
	//the client should be nullptr either before initialization and connection or after disconnect
	if (_sioClient != nullptr) _sioClient->send("Hello Socket.IO!");

}

void SocketIOTest::onMenuTestMessageEndpointClicked(ax::Ref *sender)
{

	if (_sioEndpoint != nullptr) _sioEndpoint->send("Hello Socket.IO!");

}

void SocketIOTest::onMenuTestEventClicked(ax::Ref *sender)
{
	//check that the socket is != nullptr before sending or emitting events
	//the client should be nullptr either before initialization and connection or after disconnect
	if (_sioClient != nullptr) _sioClient->emit("echotest", "{\"name\":\"myname\",\"type\":\"mytype\"}");

}

void SocketIOTest::onMenuTestEventEndpointClicked(ax::Ref *sender)
{

	if (_sioEndpoint != nullptr) _sioEndpoint->emit("echotest", "{\"name\":\"myname\",\"type\":\"mytype\"}");

}

void SocketIOTest::onMenuTestClientDisconnectClicked(ax::Ref *sender)
{
	// Disconnecting from the default namespace "" or "/" will also disconnect all other endpoints
	std::stringstream s;

	if (_sioClient != nullptr) {
		s << _sioClient->getTag() << " manually closed!";
		_sioClient->disconnect();
		_sioClient = nullptr;
	}
	else {
		s << "Socket.io Test Client not initialized!";
	}

	_sioClientStatus->setString(s.str().c_str());

}

void SocketIOTest::onMenuTestEndpointDisconnectClicked(ax::Ref *sender)
{
	std::stringstream s;

	if (_sioEndpoint != nullptr) {
		s << _sioEndpoint->getTag() << " manually closed!";
		_sioEndpoint->disconnect();
		_sioEndpoint = nullptr;
	}
	else {
		s << "Socket.io Test Endpoint not initialized!";
	}

	_sioClientStatus->setString(s.str().c_str());

}

// SIODelegate methods to catch network/socket level events outside of the socket.io events

void SocketIOTest::onClose(network::SIOClient* client)
{
    AXLOGINFO("SocketIOTest::onClose called");

	std::stringstream s;
	s << client->getTag() << " closed!";
	_sioClientStatus->setString(s.str().c_str());

	this->closedSocketAction(client);

}

void SocketIOTest::onError(network::SIOClient* client, const std::string& data)
{
	AXLOGERROR("SocketIOTest::onError received: %s", data.c_str());

	std::stringstream s;
	s << client->getTag() << " received error with content: " << data.c_str();
	_sioClientStatus->setString(s.str().c_str());
}
