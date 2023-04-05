/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#include "axmol.h"
#include "extensions/axmol-ext.h"
#include "network/HttpClient.h"
#include "BaseTest.h"

DEFINE_TEST_SUITE(HttpClientTests);

class HttpClientTest : public TestCase
{
public:
    CREATE_FUNC(HttpClientTest);

    HttpClientTest();
    virtual ~HttpClientTest();

    // Menu Callbacks
    void onMenuGetTestClicked(ax::Ref* sender);
    void onMenuPostTestClicked(ax::Ref* sender);
    void onMenuPostBinaryTestClicked(ax::Ref* sender);
    void onMenuPutTestClicked(ax::Ref* sender);
    void onMenuDeleteTestClicked(ax::Ref* sender);

    // Http Response Callback
    void onHttpRequestCompleted(ax::network::HttpClient* sender, ax::network::HttpResponse* response);

    virtual std::string title() const override { return "Http Request Test"; }

private:
    ax::Label* _labelStatusCode;
};

class HttpClientClearRequestsTest : public TestCase
{
public:
    CREATE_FUNC(HttpClientClearRequestsTest);

    HttpClientClearRequestsTest();
    virtual ~HttpClientClearRequestsTest();

    // Menu Callbacks
    void onMenuCancelAllClicked(ax::Ref* sender);
    void onMenuCancelSomeClicked(ax::Ref* sender);

    // Http Response Callback
    void onHttpRequestCompleted(ax::network::HttpClient* sender, ax::network::HttpResponse* response);

    virtual std::string title() const override { return "Http Request Test"; }

private:
    int _totalExpectedRequests;
    int _totalProcessedRequests;
    ax::Label* _labelTrakingData;
    ax::Label* _labelStatusCode;
};

#endif  //__HTTPREQUESTHTTP_H
