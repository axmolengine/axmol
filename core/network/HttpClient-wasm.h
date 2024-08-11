/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev

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

#pragma once

#include "base/Vector.h"
#include "network/HttpRequest.h"
#include "network/HttpResponse.h"
#include "network/HttpCookie.h"

struct emscripten_fetch_t;

/**
 * @addtogroup network
 * @{
 */

NS_AX_BEGIN

namespace network {



/** Singleton that handles asynchronous http requests.
 *
 * Once the request completed, a callback will issued in main thread when it provided during make request.
 *
 * @lua NA
 */
class AX_DLL HttpClient
{
public:
    /**
    * The buffer size of _responseMessage
    */
    static const int RESPONSE_BUFFER_SIZE = 256;

    /**
     * Get instance of HttpClient.
     *
     * @return the instance of HttpClient.
     */
    static HttpClient* getInstance();

    /**
     * Release the instance of HttpClient.
     */
    static void destroyInstance();

    /**
     * Enable cookie support.
     *
     * @param [nullable] cookieFile the filepath of cookie file.
     */
    void enableCookies(const char* cookieFile);

    /**
     * Get the cookie filename
     *
     * @return the cookie filename
     */
    std::string_view getCookieFilename();

    /**
     * Set root certificate path for SSL verification.
     *
     * @param caFile a full path of root certificate.if it is empty, SSL verification is disabled.
     */
    void setSSLVerification(std::string_view caFile);

    /**
     * Get the ssl CA filename
     *
     * @return the ssl CA filename
     */
    std::string_view getSSLVerification();

    /**
     * Add a get request to task queue
     *
     * @param request a HttpRequest object, which includes url, response callback etc.
                      please make sure request->_requestData is clear before calling "send" here.
     */
    void send(HttpRequest* request);

    /**
     * Immediate send a request
     *
     * @param request a HttpRequest object, which includes url, response callback etc.
                      please make sure request->_requestData is clear before calling "sendImmediate" here.
     */
    void sendImmediate(HttpRequest* request);

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

    typedef std::function<bool(HttpRequest*)> ClearRequestPredicate;
    typedef std::function<bool(HttpResponse*)> ClearResponsePredicate;

    /**
     * Clears the pending http responses and http requests
     * If defined, the method uses the ClearRequestPredicate and ClearResponsePredicate
     * to check for each request/response which to delete
     */
    void clearResponseAndRequestQueue(); 

    void clearResponseQueue() { clearResponseAndRequestQueue(); }

    /**
    * Sets a predicate function that is going to be called to determine if we proceed
    * each of the pending requests
    *
    * @param predicate function that will be called 
    */
    void setClearRequestPredicate(ClearRequestPredicate predicate) { _clearRequestPredicate = predicate; }

    /**
     Sets a predicate function that is going to be called to determine if we proceed
    * each of the pending requests
    *
    * @param cb predicate function that will be called 
    */
    void setClearResponsePredicate(ClearResponsePredicate predicate) { _clearResponsePredicate = predicate; }

        
private:
    HttpClient();
    virtual ~HttpClient();
    
    void processResponse(HttpResponse* response, bool isAlone);
    static void onRequestComplete(emscripten_fetch_t *fetch);
    void increaseThreadCount();
    void decreaseThreadCountAndMayDeleteThis();

private:
    int _timeoutForConnect;
    
    int _timeoutForRead;
    
    int _threadCount;
    
    Vector<HttpRequest*>  _requestQueue;
    
    std::string _cookieFilename;
    
    std::string _sslCaFilename;
    
    HttpCookie* _cookie;

    ClearRequestPredicate _clearRequestPredicate;
    ClearResponsePredicate _clearResponsePredicate;
};

} // namespace network

NS_AX_END

// end group
/// @}