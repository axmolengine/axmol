/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#include "axmol/axmol.h"
#include "extensions/axmol-ext.h"
#include "axmol/network/HttpClient.h"
#include "BaseTest.h"

DEFINE_TEST_SUITE(HttpClientTests);

class HttpClientTest : public TestCase
{
public:
    CREATE_FUNC(HttpClientTest);

    HttpClientTest();
    virtual ~HttpClientTest();

    // Menu Callbacks
    void onMenuGetTestClicked(ax::Object* sender);
    void onMenuPatchTestClicked(ax::Object* sender);
    void onMenuPostTestClicked(ax::Object* sender);
    void onMenuPostBinaryTestClicked(ax::Object* sender);
    void onMenuPutTestClicked(ax::Object* sender);
    void onMenuDeleteTestClicked(ax::Object* sender);

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
    void onMenuCancelAllClicked(ax::Object* sender);
    void onMenuCancelSomeClicked(ax::Object* sender);

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
