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

#include "platform/CCPlatformConfig.h"

#include "YAsioTest.h"
#include "stdio.h"
#include "stdlib.h"
#include "yasio/ibstream.hpp"
#include "yasio/yasio.hpp"

USING_NS_CC;

using namespace yasio;
using namespace yasio::inet;

void yasioTest()
{
    yasio::inet::io_hostent endpoints[] = { {"www.baidu.com", 443} };

    io_service service(endpoints, 1);

    resolv_fn_t resolv = [&](std::vector<ip::endpoint>& endpoints, const char* hostname,
        unsigned short port) {
            return service.__builtin_resolv(endpoints, hostname, port);
    };
    service.set_option(YOPT_S_RESOLV_FN, &resolv);

    std::vector<transport_handle_t> transports;

    deadline_timer udpconn_delay(service);
    deadline_timer udp_heartbeat(service);
    int total_bytes_transferred = 0;

    int max_request_count = 3;

    std::string respRawData;
    service.start_service([&](event_ptr&& event) {
        switch (event->kind())
        {
        case YEK_PACKET: {
            auto packet = std::move(event->packet());
            total_bytes_transferred += static_cast<int>(packet.size());
            respRawData.insert(respRawData.end(), packet.begin(), packet.end());
            break;
        }
        case YEK_CONNECT_RESPONSE:
            if (event->status() == 0)
            {
                auto transport = event->transport();
                if (event->cindex() == 0)
                {
                    obstream obs;
                    obs.write_bytes("GET / HTTP/1.1\r\n");

                    obs.write_bytes("Host: www.baidu.com\r\n");

                    obs.write_bytes("User-Agent: Mozilla/5.0 (Windows NT 10.0; "
                        "WOW64) AppleWebKit/537.36 (KHTML, like Gecko) "
                        "Chrome/78.0.3904.108 Safari/537.36\r\n");
                    obs.write_bytes("Accept: */*;q=0.8\r\n");
                    obs.write_bytes("Connection: Close\r\n\r\n");

                    service.write(transport, std::move(obs.buffer()));
                }

                transports.push_back(transport);
            }
            break;
        case YEK_CONNECTION_LOST:
            cocos2d::log("yasio-ssl recv data: %s\n---> %d bytes transferred", respRawData.c_str(), total_bytes_transferred);
            respRawData.clear();

            total_bytes_transferred = 0;
            if (--max_request_count > 0)
            {
                udpconn_delay.expires_from_now(std::chrono::seconds(1));
                udpconn_delay.async_wait([&]() { service.open(0, YCM_SSL_CLIENT); });
            }
            else
                service.stop_service();
            break;
        }
        });

    /*
    ** If after 5 seconds no data interaction at application layer,
    ** send a heartbeat per 10 seconds when no response, try 2 times
    ** if no response, then he connection will shutdown by driver.
    ** At windows will close with error: 10054
    */
    service.set_option(YOPT_S_TCP_KEEPALIVE, 5, 10, 2);

    service.open(0, YCM_SSL_CLIENT); // open http client

    time_t duration = 0;
    while (service.is_running())
    {
        service.dispatch();
        if (duration >= 6000000)
        {
            for (auto transport : transports)
                service.close(transport);
            break;
        }
        duration += 50;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

YAsioTests::YAsioTests()
{
    ADD_TEST_CASE(YAsioTest);
}

YAsioTest::YAsioTest()
{
    auto label = Label::createWithTTF("yasio Test", "fonts/arial.ttf", 28);
    addChild(label, 0);
    label->setPosition(VisibleRect::center().x, VisibleRect::top().y-50);

    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesEnded = CC_CALLBACK_2(YAsioTest::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    // create a label to display the tip string
    _label = Label::createWithTTF("Touch the screen to connect", "fonts/arial.ttf", 22);
    _label->setPosition(VisibleRect::center());
    addChild(_label, 0);
    
    _label->retain();
}

// the test code is
// https://github.com/simdsoft/yasio/blob/master/tests/ssl/main.cpp
void YAsioTest::onTouchesEnded(const std::vector<Touch*>& touches, Event  *event)
{
    std::thread t([=] {
        Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
            _label->setString("Starting yasio https request test, see the output!");
            });

        yasioTest();

        Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
            _label->setString("See the output!");
            });
        });

    t.detach();
}

YAsioTest::~YAsioTest()
{
    _label->release();
}

