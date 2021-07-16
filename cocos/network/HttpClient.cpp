/****************************************************************************
 Copyright (c) 2012      greathqy
 Copyright (c) 2012      cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2021 Bytedance Inc.

 https://adxe.org

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

#include "network/HttpClient.h"
#include <errno.h>
#include "base/ccUtils.h"
#include "base/CCDirector.h"
#include "platform/CCFileUtils.h"
#include "yasio/yasio.hpp"
#include "yasio/obstream.hpp"

using namespace yasio;

NS_CC_BEGIN

namespace network {

static HttpClient* _httpClient = nullptr; // pointer to singleton

template <typename _Cont, typename _Fty>
static void __clearQueueUnsafe(_Cont& queue, _Fty pred) {
    for (auto it = queue.unsafe_begin(); it != queue.unsafe_end();) {
        if (!pred || pred((*it))) {
            (*it)->release();
            it = queue.unsafe_erase(it);
        } else {
            ++it;
        }
    }
}

// HttpClient implementation
HttpClient* HttpClient::getInstance() {
    if (_httpClient == nullptr) {
        _httpClient = new (std::nothrow) HttpClient();
    }

    return _httpClient;
}

void HttpClient::destroyInstance() {
    if (nullptr == _httpClient) {
        CCLOG("HttpClient singleton is nullptr");
        return;
    }

    CCLOG("HttpClient::destroyInstance begin");
    delete _httpClient;
    _httpClient = nullptr;

    CCLOG("HttpClient::destroyInstance() finished!");
}

void HttpClient::enableCookies(const char* cookieFile) {
    std::lock_guard<std::recursive_mutex> lock(_cookieFileMutex);
    if (cookieFile) {
        _cookieFilename = std::string(cookieFile);
    } else {
        _cookieFilename = (FileUtils::getInstance()->getNativeWritableAbsolutePath() + "cookieFile.txt");
    }
}

void HttpClient::setSSLVerification(const std::string& caFile) {
    std::lock_guard<std::recursive_mutex> lock(_sslCaFileMutex);
    _sslCaFilename = caFile;
    _service->set_option(yasio::YOPT_S_SSL_CACERT, _sslCaFilename.c_str());
}

HttpClient::HttpClient()
    : _isInited(false), _dispatchOnWorkThread(false), _timeoutForConnect(30), _timeoutForRead(60), _cookie(nullptr),
      _clearResponsePredicate(nullptr) {
    CCLOG("In the constructor of HttpClient!");
    _scheduler = Director::getInstance()->getScheduler();

    _service = new yasio::io_service(HttpClient::MAX_CHANNELS);
    _service->set_option(yasio::YOPT_S_DEFERRED_EVENT, 0);
    _service->start([=](yasio::event_ptr&& e) { handleNetworkEvent(e.get()); });

    for (int i = 0; i < HttpClient::MAX_CHANNELS; ++i) {
        _availChannelQueue.push_back(i);
    }

    _isInited = true;
}

HttpClient::~HttpClient() {
    delete _service;
    CCLOG("HttpClient destructor");
}

bool HttpClient::send(HttpRequest* request) {
    if (!request)
        return false;

    auto response = new HttpResponse(request);
    processResponse(response, request->getUrl());
    response->release();
    return true;
}

HttpResponse* HttpClient::sendSync(HttpRequest* request) {
    request->setSync(true);
    if (this->send(request))
        return request->wait();
    return nullptr;
}

int HttpClient::tryTakeAvailChannel() {
    auto lck = _availChannelQueue.get_lock();
    if (!_availChannelQueue.empty()) {
        int channel = _availChannelQueue.front();
        _availChannelQueue.pop_front();
        return channel;
    }
    return -1;
}

void HttpClient::processResponse(HttpResponse* response, const std::string& url) {
    auto channelIndex = tryTakeAvailChannel();
    response->retain();

    if (channelIndex != -1) {
        if (response->prepareForProcess(url)) {
            response->_responseHeaders.clear(); // redirect needs clear old response headers
            auto& requestUri       = response->getRequestUri();
            auto channelHandle     = _service->channel_at(channelIndex);
            channelHandle->ud_.ptr = response;
            _service->set_option(
                YOPT_C_REMOTE_ENDPOINT, channelIndex, requestUri.getHost().c_str(), (int) requestUri.getPort());
            if (requestUri.isSecure())
                _service->open(channelIndex, YCK_SSL_CLIENT);
            else
                _service->open(channelIndex, YCK_TCP_CLIENT);
        } else {
            finishResponse(response);
        }
    } else {
        _responseQueue.push_back(response);
    }
}

void HttpClient::handleNetworkEvent(yasio::io_event* event) {
    int channelIndex       = event->cindex();
    auto channel           = _service->channel_at(event->cindex());
    HttpResponse* response = (HttpResponse*) channel->ud_.ptr;
    if (!response)
        return;

    bool responseFinished = response->isFinished();
    switch (event->kind()) {
    case YEK_ON_PACKET:
        if (!responseFinished)
            response->handleInput(event->packet());

        if (response->isFinished()) {
            response->updateInternalCode(yasio::errc::eof);
            _service->close(event->cindex());
        }
        break;
    case YEK_ON_OPEN:
        if (event->status() == 0) {
            obstream obs;
            bool usePostData = false;
            auto request     = response->getHttpRequest();
            switch (request->getRequestType()) {
            case HttpRequest::Type::GET:
                obs.write_bytes("GET");
                break;
            case HttpRequest::Type::POST:
                obs.write_bytes("POST");
                usePostData = true;
                break;
            case HttpRequest::Type::DELETE:
                obs.write_bytes("DELETE");
                break;
            case HttpRequest::Type::PUT:
                obs.write_bytes("PUT");
                usePostData = true;
                break;
            default:
                obs.write_bytes("GET");
                break;
            }
            obs.write_bytes(" ");

            auto& uri = response->getRequestUri();
            obs.write_bytes(uri.getPath());
            if (!usePostData) {
                auto& query = uri.getQuery();
                if (!query.empty()) {
                    obs.write_byte('?');
                    obs.write_bytes(query);
                }
            }
            obs.write_bytes(" HTTP/1.1\r\n");

            obs.write_bytes("Host: ");
            obs.write_bytes(uri.getHost());
            obs.write_bytes("\r\n");

            // custom headers
            auto& headers = request->getHeaders();

            bool userAgentSpecified   = false;
            bool contentTypeSpecified = false;
            if (!headers.empty()) {
                using namespace cxx17; // for string_view literal
                for (auto& header : headers) {
                    obs.write_bytes(header);
                    obs.write_bytes("\r\n");

                    if (cxx20::ic::starts_with(cxx17::string_view{header}, "User-Agent:"_sv))
                        userAgentSpecified = true;
                    else if (cxx20::ic::starts_with(cxx17::string_view{header}, "Content-Type:"_sv))
                        contentTypeSpecified = true;
                }
            }

            if (!userAgentSpecified)
                obs.write_bytes("User-Agent: yasio-http\r\n");

            obs.write_bytes("Accept: */*;q=0.8\r\n");
            obs.write_bytes("Connection: Close\r\n");

            if (usePostData) {
                if (!contentTypeSpecified)
                    obs.write_bytes("Content-Type: application/x-www-form-urlencoded;charset=UTF-8\r\n");

                char strContentLength[128] = {0};
                auto requestData           = request->getRequestData();
                auto requestDataSize       = request->getRequestDataSize();
                sprintf(strContentLength, "Content-Length: %d\r\n\r\n", static_cast<int>(requestDataSize));
                obs.write_bytes(strContentLength);

                if (requestData && requestDataSize > 0)
                    obs.write_bytes(cxx17::string_view{requestData, static_cast<size_t>(requestDataSize)});
            } else {
                obs.write_bytes("\r\n");
            }

            _service->write(event->transport(), std::move(obs.buffer()));

            auto& timerForRead = channel->get_user_timer();
            timerForRead.cancel(*_service);
            timerForRead.expires_from_now(std::chrono::seconds(this->_timeoutForRead));
            timerForRead.async_wait(*_service, [=](io_service& s) {
                response->updateInternalCode(yasio::errc::read_timeout);
                s.close(channelIndex); // timeout
                return true;
            });
        } else {
            handleNetworkEOF(response, channel, event->status());
        }
        break;
    case YEK_ON_CLOSE:
        handleNetworkEOF(response, channel, event->status());
        break;
    }
}

void HttpClient::handleNetworkEOF(HttpResponse* response, yasio::io_channel* channel, int internalErrorCode) {
    channel->get_user_timer().cancel(*_service);
    response->updateInternalCode(internalErrorCode);
    auto responseCode = response->getResponseCode();
    switch (responseCode) {
    case 301:
    case 307:
    case 302:
        if (response->increaseRedirectCount() < HttpClient::MAX_REDIRECT_COUNT) {
            auto iter = response->_responseHeaders.find("LOCATION");
            if (iter != response->_responseHeaders.end()) {
                _availChannelQueue.push_back(channel->index());
                processResponse(response, iter->second);
                response->release();
                return;
            }
        }
    }

    finishResponse(response);

    // recycle channel
    _availChannelQueue.push_back(channel->index());

    // try process pending response
    auto lck = _responseQueue.get_lock();
    if (!_responseQueue.unsafe_empty()) {
        auto pendingResponse = _responseQueue.unsafe_front();
        _responseQueue.unsafe_pop_front();
        lck.unlock();

        processResponse(pendingResponse, pendingResponse->getHttpRequest()->getUrl());
        pendingResponse->release();
    }
}

void HttpClient::finishResponse(HttpResponse* response) {
    auto request   = response->getHttpRequest();
    auto syncState = request->getSyncState();
    if (!syncState) {
        auto cbNotify = [=]() {
            HttpRequest* request                  = response->getHttpRequest();
            const ccHttpRequestCallback& callback = request->getCallback();
            Ref* pTarget                          = request->getTarget();
            SEL_HttpResponse pSelector            = request->getSelector();

            if (callback != nullptr) {
                callback(this, response);
            } else if (pTarget && pSelector) {
                (pTarget->*pSelector)(this, response);
            }

            response->release();
        };

        if (_dispatchOnWorkThread || std::this_thread::get_id() == Director::getInstance()->getCocos2dThreadId())
            cbNotify();
        else
            _scheduler->performFunctionInCocosThread(cbNotify);
    } else {
        syncState->set_value(response);
    }
}

void HttpClient::clearResponseQueue() {
    auto lck = _responseQueue.get_lock();
    __clearQueueUnsafe(_responseQueue, ClearResponsePredicate{});
}

void HttpClient::setTimeoutForConnect(int value) {
    std::lock_guard<std::recursive_mutex> lock(_timeoutForConnectMutex);
    _timeoutForConnect = value;
    _service->set_option(YOPT_S_CONNECT_TIMEOUT, value);
}

int HttpClient::getTimeoutForConnect() {
    std::lock_guard<std::recursive_mutex> lock(_timeoutForConnectMutex);
    return _timeoutForConnect;
}

void HttpClient::setTimeoutForRead(int value) {
    std::lock_guard<std::recursive_mutex> lock(_timeoutForReadMutex);
    _timeoutForRead = value;
}

int HttpClient::getTimeoutForRead() {
    std::lock_guard<std::recursive_mutex> lock(_timeoutForReadMutex);
    return _timeoutForRead;
}

const std::string& HttpClient::getCookieFilename() {
    std::lock_guard<std::recursive_mutex> lock(_cookieFileMutex);
    return _cookieFilename;
}

const std::string& HttpClient::getSSLVerification() {
    std::lock_guard<std::recursive_mutex> lock(_sslCaFileMutex);
    return _sslCaFilename;
}

} // namespace network

NS_CC_END
