/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "NetworkTest.h"
#include "../testResource.h"
#include "HttpClientTest/HttpClientTest.h"
#include "WebSocketTest/WebSocketTest.h"
#include "DownloaderTest/DownloaderTest.h"

NetworkTests::NetworkTests()
{
    addTest("HttpClientTest", []() { return new HttpClientTests; });
    addTest("WebSocketTest", []() { return new WebSocketTests; });
    addTest("DownloaderTest", []() { return new DownloaderTests; });
}
