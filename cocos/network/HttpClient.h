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

#ifndef __CCHTTPCLIENT_H__
#define __CCHTTPCLIENT_H__

#include <thread>
#include <condition_variable>
#include <deque>
#include "base/CCScheduler.h"
#include "network/HttpRequest.h"
#include "network/HttpResponse.h"
#include "network/HttpCookie.h"
#include "network/Uri.h"
#include "yasio/yasio_fwd.hpp"
#include "base/ConcurrentDeque.h"

/**
 * @addtogroup network
 * @{
 */

NS_CC_BEGIN

namespace network {



/** Singleton that handles asynchronous http requests.
 *
 * Once the request completed, a callback will issued in main thread when it provided during make request.
 *
 * @lua NA
 */
class CC_DLL HttpClient
{
public:
    /**
    * How many requests could be perform concurrency.
    */
    static const int MAX_CHANNELS = 21;
    static const int MAX_REDIRECT_COUNT = 3;

    /**
     * Get instance of HttpClient.
     *
     * @return the instance of HttpClient.
     */
    static HttpClient *getInstance();

    /**
     * Release the instance of HttpClient.
     */
    static void destroyInstance();

    /**
     * Enable cookie support.
     *
     * @param cookieFile the filepath of cookie file.
     */
    void enableCookies(const char* cookieFile);

    /**
     * Get the cookie filename
     *
     * @return the cookie filename
     */
    const std::string& getCookieFilename();

    /**
     * Set root certificate path for SSL verification.
     *
     * @param caFile a full path of root certificate.if it is empty, SSL verification is disabled.
     */
    void setSSLVerification(const std::string& caFile);

    /**
     * Get the ssl CA filename
     *
     * @return the ssl CA filename
     */
    const std::string& getSSLVerification();

    /**
     * Add a get request to task queue
     *
     * @param request a HttpRequest object, which includes url, response callback etc.
                      please make sure request->_requestData is clear before calling "send" here.
     */
    bool send(HttpRequest* request);

    /**
     * Set the timeout value for connecting.
     *
     * @param value the timeout value for connecting.
     */
    void setTimeoutForConnect(int value);

    /**
     * Get the timeout value for connecting.
     *
     * @return int the timeout value for connecting.
     */
    int getTimeoutForConnect();

    /**
     * Set the timeout value for reading.
     *
     * @param value the timeout value for reading.
     */
    void setTimeoutForRead(int value);

    /**
     * Get the timeout value for reading.
     *
     * @return int the timeout value for reading.
     */
    int getTimeoutForRead();

    HttpCookie* getCookie() const {return _cookie; }

    std::recursive_mutex& getCookieFileMutex() {return _cookieFileMutex;}

    std::recursive_mutex& getSSLCaFileMutex() {return _sslCaFileMutex;}
    
    typedef std::function<bool(HttpResponse*)> ClearResponsePredicate;

    /**
     * Clears the pending http response
     */
    void clearResponseQueue(); 

    /**
     Sets a predicate function that is going to be called to determine if we proceed
    * each of the pending requests
    *
    * @param cb predicate function that will be called 
    */
    void setClearResponsePredicate(ClearResponsePredicate predicate) { _clearResponsePredicate = predicate; }

    void setDispatchOnWorkThread(bool bVal) { _dispatchOnWorkThread = bVal; }
    bool isDispatchOnWorkThread() const { return _dispatchOnWorkThread; }
        
private:
    HttpClient();
    virtual ~HttpClient();

    void processResponse(HttpResponse* response, const std::string& url);

    int tryTakeAvailChannel();

    void handleNetworkEvent(yasio::io_event* event);

    void handleNetworkEOF(HttpResponse* response, yasio::io_channel* channel, int internalErrorCode);

    void finishResponse(HttpResponse* response);

private:
    bool _isInited;

    yasio::io_service* _service;
    
    bool _dispatchOnWorkThread;

    int _timeoutForConnect;
    std::recursive_mutex _timeoutForConnectMutex;

    int _timeoutForRead;
    std::recursive_mutex _timeoutForReadMutex;

    Scheduler* _scheduler;
    std::recursive_mutex _schedulerMutex;

    ConcurrentDeque<HttpResponse*> _responseQueue;

    ConcurrentDeque<int> _availChannelQueue;

    std::string _cookieFilename;
    std::recursive_mutex _cookieFileMutex;

    std::string _sslCaFilename;
    std::recursive_mutex _sslCaFileMutex;

    HttpCookie* _cookie;

    ClearResponsePredicate _clearResponsePredicate;
};

} // namespace network

NS_CC_END

// end group
/// @}

#endif //__CCHTTPCLIENT_H__

