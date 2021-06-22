/****************************************************************************
 Copyright (c) 2012      greathqy
 Copyright (c) 2012      cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2021 Bytedance Inc.
 
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

#include "network/HttpClient.h"
#include <errno.h>
#include "base/CCDirector.h"
#include "platform/CCFileUtils.h"
#include "yasio/yasio.hpp"
#include "yasio/obstream.hpp"

using namespace yasio;

NS_CC_BEGIN

namespace network {

static HttpClient* _httpClient = nullptr; // pointer to singleton

template<typename _Cont, typename _Fty>
static void __clearQueueUnsafe(_Cont& queue, _Fty pred) {
    for (auto it = queue.unsafe_begin(); it != queue.unsafe_end();)
    {
        if (!pred || pred((*it)))
        {
            (*it)->release();
            it = queue.unsafe_erase(it);
        }
        else
        {
            ++it;
        }
    }
}

static std::string urlencodeQuery(cxx17::string_view input) {
    std::string output;
    for (size_t ix = 0; ix < input.size(); ix++) {
        uint8_t buf[4];
        memset(buf, 0, 4);
        if (isalnum((uint8_t) input[ix]) || input[ix] == '=' || input[ix] == '&') {
            buf[0] = input[ix];
        } else if (isspace((uint8_t) input[ix])) {
            buf[0] = '+';
        } else {
            buf[0] = '%';
            buf[1] = CC_HEX2CHAR((uint8_t) input[ix] >> 4);
            buf[2] = CC_HEX2CHAR((uint8_t) input[ix] % 16);
        }
        output += (char*) buf;
    }
    return output;
};

// HttpClient implementation
HttpClient* HttpClient::getInstance()
{
    if (_httpClient == nullptr)
    {
        _httpClient = new (std::nothrow) HttpClient();
    }
    
    return _httpClient;
}

void HttpClient::destroyInstance()
{
    if (nullptr == _httpClient)
    {
        CCLOG("HttpClient singleton is nullptr");
        return;
    }

    CCLOG("HttpClient::destroyInstance begin");
    delete _httpClient;
    _httpClient = nullptr;

    CCLOG("HttpClient::destroyInstance() finished!");
}

void HttpClient::enableCookies(const char* cookieFile)
{
    std::lock_guard<std::recursive_mutex> lock(_cookieFileMutex);
    if (cookieFile)
    {
        _cookieFilename = std::string(cookieFile);
    }
    else
    {
        _cookieFilename = (FileUtils::getInstance()->getNativeWritableAbsolutePath() + "cookieFile.txt");
    }
}
    
void HttpClient::setSSLVerification(const std::string& caFile)
{
    std::lock_guard<std::recursive_mutex> lock(_sslCaFileMutex);
    _sslCaFilename = caFile;
    _service->set_option(yasio::YOPT_S_SSL_CACERT, _sslCaFilename.c_str());
}

HttpClient::HttpClient()
: _isInited(false)
, _dispatchOnWorkThread(false)
, _timeoutForConnect(30)
, _timeoutForRead(10)
, _cookie(nullptr)
, _clearResponsePredicate(nullptr)
{
    CCLOG("In the constructor of HttpClient!");
    _scheduler = Director::getInstance()->getScheduler();

    _service = new yasio::io_service(HttpClient::MAX_CHANNELS);
    _service->set_option(yasio::YOPT_S_DEFERRED_EVENT, 0);
    _service->start([=](yasio::event_ptr&& e) { handleNetworkEvent(e.get()); });

    for (int i = 0; i < HttpClient::MAX_CHANNELS; ++i) {
        _availChannelQueue.push_back(i);
    }

    _timerForRead = std::make_shared<highp_timer>();

    _isInited = true;
}

HttpClient::~HttpClient()
{
    _timerForRead->cancel(*_service);
    _timerForRead.reset();
    CC_SAFE_DELETE(_service);
    CCLOG("HttpClient destructor");
}

//Lazy create semaphore & mutex & thread
bool HttpClient::lazyInitService() {
    return true;
}

//Add a get task to queue
bool HttpClient::send(HttpRequest* request)
{
    if (!request)
        return false;

    auto response = new HttpResponse(request);
    processResponse(response, request->getUrl());
    response->release();
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

// Process Response
void HttpClient::processResponse(HttpResponse* response, const std::string& url) {
    auto channelIndex = tryTakeAvailChannel();
    if (channelIndex != -1) {
        response->retain();

        if (response->prepareForProcess(url)) {
            auto& requestUri = response->getRequestUri();
            auto channelHandle = _service->channel_at(channelIndex);
            channelHandle->ud_.ptr = response;
            _service->set_option(YOPT_C_REMOTE_ENDPOINT, channelIndex, requestUri.getHost().c_str(), (int) requestUri.getPort());
            if (requestUri.isSecure())
                _service->open(channelIndex, YCK_SSL_CLIENT);
            else
                _service->open(channelIndex, YCK_TCP_CLIENT);
        } else {
            finishResponse(response);
        }
    } else {
        response->retain();
        _responseQueue.push_back(response);
    }
}

void HttpClient::handleNetworkEvent(yasio::io_event* event) {
    int channelIndex     = event->cindex();
    auto channel         = _service->channel_at(event->cindex());
    HttpResponse* response = (HttpResponse*) channel->ud_.ptr;
    if (!response)
        return;

    bool responseFinished = response->isFinished();
    switch (event->kind()) {
    case YEK_ON_PACKET:
        if (!responseFinished)
            response->handleInput(event->packet());

        if (response->isFinished())
            _service->close(event->cindex());
        break;
    case YEK_ON_OPEN:
        if (event->status() == 0) {
            obstream obs;
            bool ispost  = false;
            auto request = response->getHttpRequest();
            auto& uri = response->getRequestUri();
            switch (request->getRequestType()) { 
            case HttpRequest::Type::GET:
                obs.write_bytes("GET");
                break;
            case HttpRequest::Type::POST:
                obs.write_bytes("POST");
                ispost = true;
                break;
            case HttpRequest::Type::DELETE:
                obs.write_bytes("DELETE");
                break;
            case HttpRequest::Type::PUT:
                obs.write_bytes("PUT");
                ispost = true;
                break;
            default: // other, TODO: PUT,DELETE
                obs.write_bytes("GET");
                break;
            }
            obs.write_bytes(" ");
            obs.write_bytes(uri.getPath());
            auto& query = uri.getQuery();
            if (!query.empty()) {
                auto encodedQuery = urlencodeQuery(query);
                obs.write_byte('?');
                obs.write_bytes(encodedQuery);
            }
            obs.write_bytes(" HTTP/1.1\r\n");

            std::string encodedRequestData;
            if (ispost) {
                // obs.write_bytes("Origin: yasio\r\n");
                obs.write_bytes("Content-Type: application/x-www-form-urlencoded;charset=UTF-8\r\n");
                auto requestData = request->getRequestData();
                auto requestDataSize = request->getRequestDataSize();
                if (requestData && request->getRequestDataSize() > 0) {
                    encodedRequestData = urlencodeQuery(cxx17::string_view(requestData, requestDataSize));
                    auto lenField      = yasio::strfmt(63, "Content-Length: %d\r\n", (int) encodedRequestData.size());
                    obs.write_bytes(lenField);
                }
            }
            obs.write_bytes("Host: ");
            obs.write_bytes(uri.getHost());
            obs.write_bytes("\r\n");
            obs.write_bytes("User-Agent: ");
            obs.write_bytes("yasio-http");
            obs.write_bytes("\r\n");

            // custom headers
            auto& headers = request->getHeaders();
            if(!headers.empty()) {
                for(auto& header: headers) {
                    obs.write_bytes(header);
                    obs.write_bytes("\r\n");
                }
            }

            obs.write_bytes("Accept: */*;q=0.8\r\n");
            obs.write_bytes("Connection: Close\r\n\r\n");

            if (ispost && !encodedRequestData.empty()) {
                obs.write_bytes(encodedRequestData);
            }

            _service->write(event->transport(), std::move(obs.buffer()));
            _timerForRead->cancel(*_service);
            _timerForRead->expires_from_now(std::chrono::seconds(this->_timeoutForRead));
            _timerForRead->async_wait(*_service, [=](io_service& s) {
                s.close(channelIndex); // timeout
                return true;
                });
        } else {
            response->setInternalCode(event->status());
            handleNetworkEOF(response, channelIndex);
        }
        break;
    case YEK_ON_CLOSE:
        handleNetworkEOF(response, channelIndex);
        break;
    }
}

void HttpClient::handleNetworkEOF(HttpResponse* response, int channelIndex) {
    auto responseCode = response->getResponseCode();
    switch (responseCode) {
    case 301:
    case 307:
    case 302:
        if (response->increaseRedirectCount() < HttpClient::MAX_REDIRECT_COUNT) {
            auto iter = response->_responseHeaders.find("LOCATION");
            if (iter != response->_responseHeaders.end()) {
                _availChannelQueue.push_back(channelIndex); 
                processResponse(response, iter->second);
                response->release();
                return;
            }
        }
    }

    finishResponse(response);

    // recycle channel
    _availChannelQueue.push_back(channelIndex); 

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
}
    
void HttpClient::clearResponseQueue() {
    auto lck = _responseQueue.get_lock();
    __clearQueueUnsafe(_responseQueue, ClearResponsePredicate{});
}

void HttpClient::setTimeoutForConnect(int value)
{
    std::lock_guard<std::recursive_mutex> lock(_timeoutForConnectMutex);
    _timeoutForConnect = value;
    _service->set_option(YOPT_S_CONNECT_TIMEOUT, value);
}
    
int HttpClient::getTimeoutForConnect()
{
    std::lock_guard<std::recursive_mutex> lock(_timeoutForConnectMutex);
    return _timeoutForConnect;
}
    
void HttpClient::setTimeoutForRead(int value)
{
    std::lock_guard<std::recursive_mutex> lock(_timeoutForReadMutex);
    _timeoutForRead = value;
}
    
int HttpClient::getTimeoutForRead()
{
    std::lock_guard<std::recursive_mutex> lock(_timeoutForReadMutex);
    return _timeoutForRead;
}
    
const std::string& HttpClient::getCookieFilename()
{
    std::lock_guard<std::recursive_mutex> lock(_cookieFileMutex);
    return _cookieFilename;
}
    
const std::string& HttpClient::getSSLVerification()
{
    std::lock_guard<std::recursive_mutex> lock(_sslCaFileMutex);
    return _sslCaFilename;
}

}

NS_CC_END


