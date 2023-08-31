/****************************************************************************
 Copyright (c) 2012      greathqy
 Copyright (c) 2012      cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
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

#include "network/HttpClient-emscripten.h"
#include <queue>
#include "base/Director.h"
#include "platform/FileUtils.h"

#if EMSCRIPTEN 
#include <emscripten/fetch.h>
#include <emscripten.h>
#endif

NS_AX_BEGIN

namespace network
{

    struct fetchUserData
    {
        bool isAlone;
        HttpResponse *response;
    };

    static HttpClient *_httpClient = nullptr; // pointer to singleton

    // HttpClient implementation
    HttpClient *HttpClient::getInstance()
    {
        if (_httpClient == nullptr)
        {
            _httpClient = new (std::nothrow) HttpClient();
        }

        return _httpClient;
    }

    HttpClient::HttpClient()
    : _timeoutForConnect(30)
    , _timeoutForRead(60)
    , _threadCount(0)
    , _cookie(nullptr)
    , _clearRequestPredicate(nullptr)
    , _clearResponsePredicate(nullptr)
    {
        AXLOG("In the constructor of HttpClient!");
        increaseThreadCount();
    }

    HttpClient::~HttpClient()
    {
        AXLOG("HttpClient destructor");
    }

    void HttpClient::destroyInstance()
    {
        if (nullptr == _httpClient)
        {
            AXLOG("HttpClient singleton is nullptr");
        }

        auto thiz = _httpClient;
        _httpClient = nullptr;

        Vector<HttpRequest *> requestQueue = thiz->_requestQueue;
        for (auto it = requestQueue.begin(); it != requestQueue.end();)
        {
            (*it)->release();
            it = requestQueue.erase(it);
        }

        thiz->decreaseThreadCountAndMayDeleteThis();
    }

    void HttpClient::enableCookies(const char *cookieFile)
    {
        if (cookieFile)
        {
            _cookieFilename = std::string(cookieFile);
        }
        else
        {
            _cookieFilename = (FileUtils::getInstance()->getWritablePath() + "cookieFile.txt");
        }
    }

    void HttpClient::setSSLVerification(const std::string &caFile)
    {
        AXLOG("HttpClient::setSSLVerification not supported on Emscripten");
        _sslCaFilename = caFile;
    }

    //Add a get task to queue
    void HttpClient::send(HttpRequest *request)
    {
        if (!request)
        {
            return;
        }

        request->retain();

        if (_threadCount <= 1)
        {
            increaseThreadCount();
            HttpResponse *response = new (std::nothrow) HttpResponse(request);
            processResponse(response, false);
        }
        else
        {
            _requestQueue.pushBack(request);
        }
    }

    void HttpClient::sendImmediate(HttpRequest *request)
    {
        if (!request)
        {
            return;
        }

        request->retain();
        HttpResponse *response = new (std::nothrow) HttpResponse(request);
        processResponse(response, true);
    }

    // Process Response
    void HttpClient::processResponse(HttpResponse *response, bool isAlone)
    {
        // copy cookie back to document.cookie in case it is changed
        std::string_view cookieFilename = HttpClient::getInstance()->getCookieFilename();
        if (!cookieFilename.empty())
        {
            EM_ASM_ARGS({
                document.cookie = FS.readFile(UTF8ToString($0));
            }, cookieFilename.data());
        }

        auto request = response->getHttpRequest();
        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);

        // set http method
        switch (request->getRequestType())
        {
        case HttpRequest::Type::GET:
            strcpy(attr.requestMethod, "GET");
            break;

        case HttpRequest::Type::POST:
            strcpy(attr.requestMethod, "POST");
            break;

        case HttpRequest::Type::PUT:
            strcpy(attr.requestMethod, "PUT");
            break;

        case HttpRequest::Type::DELETE:
            strcpy(attr.requestMethod, "DELETE");
            break;

        default:
            AXASSERT(false, "CCHttpClient: unknown request type, only GET, POST, PUT or DELETE is supported");
            break;
        }

        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;

        auto userData = new (std::nothrow) fetchUserData();
        userData->isAlone = isAlone;
        userData->response = response;

        // set header
        std::vector<std::string> headers;
        for (auto &header : request->getHeaders())
        {
            size_t pos = header.find(":");
            if (pos != std::string::npos)
            {
                std::string key = header.substr(0, pos);
                std::string value = header.substr(pos + 1);
                headers.push_back(key);
                headers.push_back(value);
            }
        }
        
        std::vector<const char*> headersCharptr;
        headersCharptr.reserve(headers.size() + 1);
        for (auto &header : headers)
        {
            headersCharptr.push_back(header.c_str());
        }
        headersCharptr.push_back(0);
        attr.requestHeaders = &headersCharptr[0];

        // post data
        if (request->getRequestDataSize())
        {
            attr.requestData = request->getRequestData();
            attr.requestDataSize = request->getRequestDataSize();
        }

        attr.onsuccess = onRequestComplete;
        attr.onerror = onRequestComplete;
        attr.timeoutMSecs = (HttpClient::getInstance()->getTimeoutForConnect() + HttpClient::getInstance()->getTimeoutForRead()) * 1000;
        std::string_view url = response->getHttpRequest()->getUrl();
        emscripten_fetch_t *fetch = emscripten_fetch(&attr, url.data());
        fetch->userData = userData;
    }

    void HttpClient::onRequestComplete(emscripten_fetch_t *fetch)
    {
        fetchUserData *userData = reinterpret_cast<fetchUserData *>(fetch->userData);
        HttpResponse *response = userData->response;
        HttpRequest *request = response->getHttpRequest();

        // get response
        response->setResponseCode(fetch->status);
        response->setErrorBuffer(fetch->statusText);
        response->getResponseData()->assign(sttaic_cast<const char *>(fetch->data), static_cast<const char *>(fetch->data) + fetch->numBytes);
        emscripten_fetch_close(fetch);

        // write cookie back
        std::string cookieFilename = HttpClient::getInstance()->getCookieFilename();
        if (!cookieFilename.empty())
        {
            EM_ASM_ARGS({
                FS.writeFile(UTF8ToString($0), document.cookie);
            }, cookieFilename.c_str());
        }

        if (_httpClient)
        {
            // call back
            const ccHttpRequestCallback &callback = request->getCallback();
            Ref *pTarget = request->getTarget();
            SEL_HttpResponse pSelector = request->getSelector();

            if (callback != nullptr)
            {
                callback(HttpClient::getInstance(), response);
            }
            else if (pTarget && pSelector)
            {
                (pTarget->*pSelector)(HttpClient::getInstance(), response);
            }

            // call next request
            if (!userData->isAlone)
            {
                Vector<HttpRequest *> requestQueue = _httpClient->_requestQueue;
                if (!requestQueue.empty())
                {
                    HttpRequest *request = requestQueue.at(0);
                    requestQueue.erase(0);

                    HttpResponse *response = new (std::nothrow) HttpResponse(request);
                    _httpClient->processResponse(response, false);
                }
                else
                {
                    _httpClient->decreaseThreadCountAndMayDeleteThis();
                }
            }
        }

        response->release();
        request->release();
        delete userData;
    }

    void HttpClient::clearResponseAndRequestQueue()
    {
        for (auto it = _requestQueue.begin(); it != _requestQueue.end();)
        {
            if (!_clearRequestPredicate ||
                _clearRequestPredicate((*it)))
            {
                (*it)->release();
                it = _requestQueue.erase(it);
            }
            else
            {
                it++;
            }
        }
    }

    void HttpClient::increaseThreadCount()
    {
        ++_threadCount;
    }

    void HttpClient::decreaseThreadCountAndMayDeleteThis()
    {
        --_threadCount;
        if (0 == _threadCount)
        {
            delete this;
        }
    }

    void HttpClient::setTimeoutForConnect(int value)
    {
        _timeoutForConnect = value;
    }

    int HttpClient::getTimeoutForConnect()
    {
        return _timeoutForConnect;
    }

    void HttpClient::setTimeoutForRead(int value)
    {
        _timeoutForRead = value;
    }

    int HttpClient::getTimeoutForRead()
    {
        return _timeoutForRead;
    }

    const std::string &HttpClient::getCookieFilename()
    {
        return _cookieFilename;
    }

    const std::string &HttpClient::getSSLVerification()
    {
        return _sslCaFilename;
    }

} // namespace network

NS_AX_END
