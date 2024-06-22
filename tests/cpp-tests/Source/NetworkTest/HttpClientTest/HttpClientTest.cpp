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

#include "HttpClientTest.h"
#include <string>

USING_NS_AX;
using namespace ax::network;

#define CHROME_UA                                                                                                   \
    "User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/112.0.0.0 " \
    "Safari/537.36"

HttpClientTests::HttpClientTests()
{
    ADD_TEST_CASE(HttpClientTest);
    ADD_TEST_CASE(HttpClientClearRequestsTest);
}

HttpClientTest::HttpClientTest() : _labelStatusCode(nullptr)
{
    auto winSize = Director::getInstance()->getWinSize();

    auto httpClient = HttpClient::getInstance();

    auto cafile = FileUtils::getInstance()->fullPathForFilename("cacert.pem");
    httpClient->setSSLVerification(cafile);
    httpClient->enableCookies(nullptr);
    AXLOG("The http cookie will store to: %s", httpClient->getCookieFilename().data());

    const int MARGIN = 40;
    const int SPACE  = 35;

    const int LEFT  = winSize.width / 4 * 1;
    const int CENTER  = winSize.width / 2;
    const int RIGHT = winSize.width / 4 * 3;

    auto menuRequest = Menu::create();
    menuRequest->setPosition(Vec2::ZERO);
    addChild(menuRequest);

    // Get
    auto labelGet = Label::createWithTTF("Test Get", "fonts/arial.ttf", 22);
    auto itemGet  = MenuItemLabel::create(labelGet, AX_CALLBACK_1(HttpClientTest::onMenuGetTestClicked, this));
    itemGet->setPosition(CENTER, winSize.height - MARGIN - SPACE);
    menuRequest->addChild(itemGet);

    // Patch
    auto labelPatch = Label::createWithTTF("Test Patch", "fonts/arial.ttf", 22);
    auto itemPatch  = MenuItemLabel::create(labelPatch, AX_CALLBACK_1(HttpClientTest::onMenuPatchTestClicked, this));
    itemPatch->setPosition(CENTER, winSize.height - MARGIN - 2 * SPACE);
    menuRequest->addChild(itemPatch);

    // Post
    auto labelPost = Label::createWithTTF("Test Post", "fonts/arial.ttf", 22);
    auto itemPost  = MenuItemLabel::create(labelPost, AX_CALLBACK_1(HttpClientTest::onMenuPostTestClicked, this));
    itemPost->setPosition(LEFT, winSize.height - MARGIN - 3 * SPACE);
    menuRequest->addChild(itemPost);

    // Post Binary
    auto labelPostBinary = Label::createWithTTF("Test Post Binary", "fonts/arial.ttf", 22);
    auto itemPostBinary =
        MenuItemLabel::create(labelPostBinary, AX_CALLBACK_1(HttpClientTest::onMenuPostBinaryTestClicked, this));
    itemPostBinary->setPosition(RIGHT, winSize.height - MARGIN - 3 * SPACE);
    menuRequest->addChild(itemPostBinary);

    // Put
    auto labelPut = Label::createWithTTF("Test Put", "fonts/arial.ttf", 22);
    auto itemPut  = MenuItemLabel::create(labelPut, AX_CALLBACK_1(HttpClientTest::onMenuPutTestClicked, this));
    itemPut->setPosition(CENTER, winSize.height - MARGIN - 4 * SPACE);
    menuRequest->addChild(itemPut);

    // Delete
    auto labelDelete = Label::createWithTTF("Test Delete", "fonts/arial.ttf", 22);
    auto itemDelete  = MenuItemLabel::create(labelDelete, AX_CALLBACK_1(HttpClientTest::onMenuDeleteTestClicked, this));
    itemDelete->setPosition(CENTER, winSize.height - MARGIN - 5 * SPACE);
    menuRequest->addChild(itemDelete);

    // Response Code Label
    _labelStatusCode = Label::createWithTTF("HTTP Status Code", "fonts/arial.ttf", 18);
    _labelStatusCode->setPosition(winSize.width / 2, winSize.height - MARGIN - 6 * SPACE);
    addChild(_labelStatusCode);
}

HttpClientTest::~HttpClientTest()
{
    HttpClient::destroyInstance();
}

void HttpClientTest::onMenuGetTestClicked(ax::Object* sender)
{
    // test 1
    {
        HttpRequest* request = new HttpRequest();
        request->setUrl("https://just-make-this-request-failed.com");
        request->setRequestType(HttpRequest::Type::GET);
        request->setHeaders(std::vector<std::string>{CHROME_UA});
        request->setResponseCallback(AX_CALLBACK_2(HttpClientTest::onHttpRequestCompleted, this));
        request->setTag("GET test2");
        HttpClient::getInstance()->send(request);
        request->release();
    }

    // test 2
    {
        HttpRequest* request = new HttpRequest();
        // required fields
        request->setUrl("https://httpbin.org/ip");
        request->setRequestType(HttpRequest::Type::GET);
        request->setHeaders(std::vector<std::string>{CHROME_UA});
        request->setResponseCallback(AX_CALLBACK_2(HttpClientTest::onHttpRequestCompleted, this));
        request->setTag("GET test3");
        HttpClient::getInstance()->send(request);
        // don't forget to release it, pair to new
        request->release();
    }

    // test 3
    {
        HttpRequest* request = new HttpRequest();
        request->setUrl("https://httpbin.org/get");
        request->setRequestType(HttpRequest::Type::GET);
        request->setHeaders(std::vector<std::string>{CHROME_UA});
        request->setResponseCallback(AX_CALLBACK_2(HttpClientTest::onHttpRequestCompleted, this));
        request->setTag("GET test4");
        HttpClient::getInstance()->send(request);
        request->release();
    }

    // test 4
    {
        HttpRequest* request = new HttpRequest();
        request->setUrl("https://github.com/yasio/yasio");
        request->setRequestType(HttpRequest::Type::GET);
        request->setHeaders(std::vector<std::string>{CHROME_UA});
        request->setResponseCallback(AX_CALLBACK_2(HttpClientTest::onHttpRequestCompleted, this));
        request->setTag("GET test5");
        HttpClient::getInstance()->send(request);
        request->release();
    }

    // waiting
    _labelStatusCode->setString("waiting...");
}

void HttpClientTest::onMenuPatchTestClicked(Object* sender)
{
    // test 1
    {
        HttpRequest* request = new HttpRequest();
        request->setUrl("https://httpbin.org/patch");
        request->setRequestType(HttpRequest::Type::PATCH);
        request->setResponseCallback(AX_CALLBACK_2(HttpClientTest::onHttpRequestCompleted, this));

        // write the body data
        const char* bodyData = "visitor=cocos2d&TestSuite=Extensions Test/NetworkTest";
        request->setRequestData(bodyData, strlen(bodyData));
        request->setTag("PATCH Binary test1");
        HttpClient::getInstance()->send(request);
        request->release();
    }

    // test 2: set Content-Type
    {
        HttpRequest* request = new HttpRequest();
        request->setUrl("https://httpbin.org/patch");
        request->setRequestType(HttpRequest::Type::PATCH);
        std::vector<std::string> headers;
        headers.emplace_back("Content-Type: application/json; charset=utf-8");
        request->setHeaders(headers);
        request->setResponseCallback(AX_CALLBACK_2(HttpClientTest::onHttpRequestCompleted, this));

        // write the post data
        const char* bodyData = "visitor=cocos2d&TestSuite=Extensions Test/NetworkTest";
        request->setRequestData(bodyData, strlen(bodyData));
        request->setTag("PATCH Binary test2");
        HttpClient::getInstance()->send(request);
        request->release();
    }

    // waiting
    _labelStatusCode->setString("waiting...");
}

void HttpClientTest::onMenuPostTestClicked(ax::Object* sender)
{
    // test 1
    {
        HttpRequest* request = new HttpRequest();
        request->setUrl("https://httpbin.org/post");
        request->setRequestType(HttpRequest::Type::POST);
        request->setHeaders(std::vector<std::string>{CHROME_UA});
        request->setResponseCallback(AX_CALLBACK_2(HttpClientTest::onHttpRequestCompleted, this));

        // write the post data
        const char* postData = "visitor=cocos2d&TestSuite=Extensions Test/NetworkTest";
        request->setRequestData(postData, strlen(postData));
        request->setTag("POST test1");
        HttpClient::getInstance()->send(request);
        request->release();
    }

    // test 2: set Content-Type
    {
        HttpRequest* request = new HttpRequest();
        request->setUrl("https://httpbin.org/post");
        request->setRequestType(HttpRequest::Type::POST);
        request->setHeaders(std::vector<std::string>{CHROME_UA, "Content-Type: application/json; charset=utf-8"});
        request->setResponseCallback(AX_CALLBACK_2(HttpClientTest::onHttpRequestCompleted, this));

        // write the post data
        const char* postData = "visitor=cocos2d&TestSuite=Extensions Test/NetworkTest";
        request->setRequestData(postData, strlen(postData));
        request->setTag("POST test2");
        HttpClient::getInstance()->send(request);
        request->release();
    }

    // waiting
    _labelStatusCode->setString("waiting...");
}

void HttpClientTest::onMenuPostBinaryTestClicked(ax::Object* sender)
{
    HttpRequest* request = new HttpRequest();
    request->setUrl("https://httpbin.org/post");
    request->setRequestType(HttpRequest::Type::POST);
    request->setResponseCallback(AX_CALLBACK_2(HttpClientTest::onHttpRequestCompleted, this));

    // write the post data
    char postData[22] = "binary=hello\0\0cocos2d";  // including \0, the strings after \0 should not be cut in response
    request->setRequestData(postData, 22);
    request->setTag("POST Binary test");
    HttpClient::getInstance()->send(request);
    request->release();

    // waiting
    _labelStatusCode->setString("waiting...");
}

void HttpClientTest::onMenuPutTestClicked(Object* sender)
{
    // test 1
    {
        HttpRequest* request = new HttpRequest();
        request->setUrl("https://httpbin.org/put");
        request->setRequestType(HttpRequest::Type::PUT);
        request->setResponseCallback(AX_CALLBACK_2(HttpClientTest::onHttpRequestCompleted, this));

        // write the post data
        const char* postData = "visitor=cocos2d&TestSuite=Extensions Test/NetworkTest";
        request->setRequestData(postData, strlen(postData));
        request->setTag("PUT Binary test1");
        HttpClient::getInstance()->send(request);
        request->release();
    }

    // test 2: set Content-Type
    {
        HttpRequest* request = new HttpRequest();
        request->setUrl("https://httpbin.org/put");
        request->setRequestType(HttpRequest::Type::PUT);
        std::vector<std::string> headers;
        headers.emplace_back("Content-Type: application/json; charset=utf-8");
        request->setHeaders(headers);
        request->setResponseCallback(AX_CALLBACK_2(HttpClientTest::onHttpRequestCompleted, this));

        // write the post data
        const char* postData = "visitor=cocos2d&TestSuite=Extensions Test/NetworkTest";
        request->setRequestData(postData, strlen(postData));
        request->setTag("PUT Binary test2");
        HttpClient::getInstance()->send(request);
        request->release();
    }

    // waiting
    _labelStatusCode->setString("waiting...");
}

void HttpClientTest::onMenuDeleteTestClicked(Object* sender)
{
    // test 1
    {
        HttpRequest* request = new HttpRequest();
        request->setUrl("https://just-make-this-request-failed.com");
        request->setRequestType(HttpRequest::Type::DELETE);
        request->setResponseCallback(AX_CALLBACK_2(HttpClientTest::onHttpRequestCompleted, this));
        request->setTag("DELETE test1");
        HttpClient::getInstance()->send(request);
        request->release();
    }

    // test 2
    {
        HttpRequest* request = new HttpRequest();
        request->setUrl("https://httpbin.org/delete");
        request->setRequestType(HttpRequest::Type::DELETE);
        request->setResponseCallback(AX_CALLBACK_2(HttpClientTest::onHttpRequestCompleted, this));
        request->setTag("DELETE test2");
        HttpClient::getInstance()->send(request);
        request->release();
    }

    // waiting
    _labelStatusCode->setString("waiting...");
}

void HttpClientTest::onHttpRequestCompleted(HttpClient* sender, HttpResponse* response)
{
    if (!response)
    {
        return;
    }

    // You can get original request type from: response->request->reqType
    auto tag = response->getHttpRequest()->getTag();
    if (!tag.empty())
    {
        ax::print("%s completed", tag.data());
    }

    int32_t statusCode    = response->getResponseCode();
    char statusString[64] = {};
    sprintf(statusString, "HTTP Status Code: %d, tag = %s", statusCode, tag.data());
    _labelStatusCode->setString(statusString);
    ax::print("response code: %d", statusCode);

    if (response->getResponseCode() != 200)
    {
        ax::print("response failed");
        // ax::print("error buffer: %s", response->getErrorBuffer());
        return;
    }

    // dump data
    auto buffer = response->getResponseData();
    buffer->push_back('\0');  // to c_str
    ax::print("Http Test, dump data: %s", buffer->data());
    ax::print("\n");
    if (response->getHttpRequest()->getReferenceCount() != 2)
    {
        ax::print("request ref count not 2, is %d", response->getHttpRequest()->getReferenceCount());
    }
}

HttpClientClearRequestsTest::HttpClientClearRequestsTest() : _labelStatusCode(nullptr)
{
    auto winSize = Director::getInstance()->getWinSize();

    const int MARGIN = 40;
    const int SPACE  = 35;

    const int CENTER = winSize.width / 2;

    auto menuRequest = Menu::create();
    menuRequest->setPosition(Vec2::ZERO);
    addChild(menuRequest);

    // Get
    auto labelGet = Label::createWithTTF("Test Clear all Get", "fonts/arial.ttf", 22);
    auto itemGet =
        MenuItemLabel::create(labelGet, AX_CALLBACK_1(HttpClientClearRequestsTest::onMenuCancelAllClicked, this));
    itemGet->setPosition(CENTER, winSize.height - MARGIN - SPACE);
    menuRequest->addChild(itemGet);

    // Post
    auto labelPost = Label::createWithTTF("Test Clear but only with the tag DELETE", "fonts/arial.ttf", 22);
    auto itemPost =
        MenuItemLabel::create(labelPost, AX_CALLBACK_1(HttpClientClearRequestsTest::onMenuCancelSomeClicked, this));
    itemPost->setPosition(CENTER, winSize.height - MARGIN - 2 * SPACE);
    menuRequest->addChild(itemPost);

    // Response Code Label
    _labelStatusCode = Label::createWithTTF("HTTP Status Code", "fonts/arial.ttf", 18);
    _labelStatusCode->setPosition(winSize.width / 2, winSize.height - MARGIN - 6 * SPACE);
    addChild(_labelStatusCode);

    // Tracking Data Label
    _labelTrakingData = Label::createWithTTF("Got 0 of 0 expected http requests", "fonts/arial.ttf", 16);
    _labelTrakingData->setPosition(CENTER, winSize.height - MARGIN - 5 * SPACE);
    addChild(_labelTrakingData);

    _totalExpectedRequests  = 0;
    _totalProcessedRequests = 0;
}

HttpClientClearRequestsTest::~HttpClientClearRequestsTest()
{
    HttpClient::destroyInstance();
}

void HttpClientClearRequestsTest::onMenuCancelAllClicked(ax::Object* sender)
{
    for (int i = 0; i < 10; i++)
    {
        HttpRequest* request = new HttpRequest();
        std::stringstream url;
        url << "https://cocos2d-x.org/images/logo.png?id=" << std::to_string(i);
        request->setUrl(url.str());
        request->setRequestType(HttpRequest::Type::GET);
        request->setResponseCallback(AX_CALLBACK_2(HttpClientClearRequestsTest::onHttpRequestCompleted, this));

        url.str("");
        url << "TEST_" << std::to_string(i);
        request->setTag(url.str());
        HttpClient::getInstance()->send(request);
        request->release();
    }

    _totalProcessedRequests = 0;
    _totalExpectedRequests  = 1;

    HttpClient::getInstance()->setClearResponsePredicate(nullptr);
    HttpClient::getInstance()->clearResponseQueue();

    // waiting
    _labelStatusCode->setString("waiting...");
}

void HttpClientClearRequestsTest::onMenuCancelSomeClicked(ax::Object* sender)
{
    // test 1
    for (int i = 0; i < 10; i++)
    {
        HttpRequest* request = new HttpRequest();
        std::stringstream url;
        url << "https://cocos2d-x.org/images/logo.png?id=" << std::to_string(i);
        request->setUrl(url.str());
        request->setRequestType(HttpRequest::Type::GET);
        request->setResponseCallback(AX_CALLBACK_2(HttpClientClearRequestsTest::onHttpRequestCompleted, this));

        url.str("");
        if (i < 5)
        {
            url << "TEST_" << std::to_string(i);
            _totalExpectedRequests++;
        }
        else
        {
            url << "DELETE_" << std::to_string(i);
        }
        request->setTag(url.str());
        HttpClient::getInstance()->send(request);
        request->release();
    }

    /* HttpClient::getInstance()->setClearRequestPredicate([&](HttpRequest* req)
                                                          {
                                                              auto r = !!strstr(req->getTag(), "DELETE_");
                                                              return r;
                                                          });*/
    HttpClient::getInstance()->setClearResponsePredicate(nullptr);
    HttpClient::getInstance()->clearResponseQueue();

    // waiting
    _labelStatusCode->setString("waiting...");
}

void HttpClientClearRequestsTest::onHttpRequestCompleted(HttpClient* sender, HttpResponse* response)
{
    if (!response)
    {
        return;
    }

    // You can get original request type from: response->request->reqType
    auto tag = response->getHttpRequest()->getTag();
    if (!tag.empty())
    {
        ax::print("%s completed", tag.data());
    }

    int32_t statusCode    = response->getResponseCode();
    char statusString[64] = {};
    sprintf(statusString, "HTTP Status Code: %d, tag = %s", statusCode, tag.data());
    _labelStatusCode->setString(statusString);
    ax::print("response code: %d", statusCode);

    _totalProcessedRequests++;
    sprintf(statusString, "Got %d of %d expected http requests", _totalProcessedRequests, _totalExpectedRequests);
    _labelTrakingData->setString(statusString);

    if (!response->isSucceed())
    {
        ax::print("response failed");
        // ax::print("error buffer: %s", response->getErrorBuffer());
    }
}
