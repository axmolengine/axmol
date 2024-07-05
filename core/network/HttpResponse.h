/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
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

#ifndef __HTTP_RESPONSE__
#define __HTTP_RESPONSE__
#include <ctype.h>
#include <map>
#include <unordered_map>
#include "network/HttpRequest.h"
#include "network/Uri.h"
#include "llhttp.h"

/**
 * @addtogroup network
 * @{
 */

NS_AX_BEGIN

namespace network
{

class HttpClient;
/**
 * @brief defines the object which users will receive at onHttpCompleted(sender, HttpResponse) callback.
 * Please refer to samples/TestCpp/Classes/ExtensionTest/NetworkTest/HttpClientTest.cpp as a sample.
 * @since v2.0.2.
 * @lua NA
 */
class AX_DLL HttpResponse : public ax::Object
{
    friend class HttpClient;

public:
    using ResponseHeaderMap = std::multimap<std::string, std::string>;

    /**
     * Constructor, it's used by HttpClient internal, users don't need to create HttpResponse manually.
     * @param request the corresponding HttpRequest which leads to this response.
     */
    HttpResponse(HttpRequest* request) : _pHttpRequest(request)
    {
        if (_pHttpRequest)
        {
            _pHttpRequest->retain();
        }
    }

    /**
     * Destructor, it will be called in HttpClient internal.
     * Users don't need to destruct HttpResponse object manually.
     */
    virtual ~HttpResponse()
    {
        if (_pHttpRequest)
        {
            _pHttpRequest->release();
        }
    }

    /**
     * Override autorelease method to prevent developers from calling it.
     * If this method is called , it would trigger AXASSERT.
     * @return ax::Object* always return nullptr.
     */
    ax::Object* autorelease()
    {
        AXASSERT(false,
                 "HttpResponse is used between network thread and ui thread \
                        therefore, autorelease is forbidden here");
        return nullptr;
    }

    // getters, will be called by users

    /**
     * Get the corresponding HttpRequest object which leads to this response.
     * There's no paired setter for it, because it's already set in class constructor
     * @return HttpRequest* the corresponding HttpRequest object which leads to this response.
     */
    HttpRequest* getHttpRequest() const { return _pHttpRequest; }

    /**
     * Get the http response data.
     * @return yasio::sbyte_buffer* the pointer that point to the _responseData.
     */
    yasio::sbyte_buffer* getResponseData() { return &_responseData; }

    bool isSucceed() const { return _responseCode == 200; }

    /**
     * Get the http response code to judge whether response is successful or not.
     * I know that you want to see the _responseCode is 200.
     * If _responseCode is not 200, you should check the meaning for _responseCode by the net.
     * @return int32_t the value of _responseCode
     */
    int getResponseCode() const { return _responseCode; }

    /*
     * The yasio error code, see yasio::errc
     */
    int getInternalCode() const { return _internalCode; }

    int getRedirectCount() const { return _redirectCount; }

    const ResponseHeaderMap& getResponseHeaders() const { return _responseHeaders; }

private:
    void setResponseCode(int value) { _responseCode = value; }

    void updateInternalCode(int value)
    {
        if (_internalCode == 0)
            _internalCode = value;
    }

    /**
     * To see if the http request is finished.
     */
    bool isFinished() const { return _finished; }

    void handleInput(const char* d, size_t n)
    {
        enum llhttp_errno err = llhttp_execute(&_context, d, n);
        if (err != HPE_OK)
        {
            _finished = true;
        }
    }

    bool tryRedirect()
    {
        if ((_redirectCount < HttpRequest::MAX_REDIRECT_COUNT))
        {
            auto iter = _responseHeaders.find("location");
            if (iter != _responseHeaders.end())
            {
                auto redirectUrl = iter->second;
                if (_responseCode == 302)
                    getHttpRequest()->setRequestType(HttpRequest::Type::GET);
                AXLOGD("Process url redirect ({}): {}", _responseCode, redirectUrl);
                return setLocation(redirectUrl, true);
            }
        }
        return false;
    }

    /**
     * Set new request location with url
     * @param url the actually url to request
     * @param redirect wither redirect
     */
    bool setLocation(std::string_view url, bool redirect)
    {
        if (redirect)
        {
            ++_redirectCount;
            _requestUri.invalid();
        }

        if (!_requestUri.isValid())
        {
            Uri uri = Uri::parse(url);
            if (!uri.isValid())
                return false;
            _requestUri = std::move(uri);

            /* Resets response status */
            _responseHeaders.clear();
            _finished = false;
            _responseData.clear();
            _currentHeader.clear();
            _responseCode = -1;
            _internalCode = 0;

            /* Initialize user callbacks and settings */
            llhttp_settings_init(&_contextSettings);

            /* Initialize the parser in HTTP_BOTH mode, meaning that it will select between
             * HTTP_REQUEST and HTTP_RESPONSE parsing automatically while reading the first
             * input.
             */
            llhttp_init(&_context, HTTP_RESPONSE, &_contextSettings);

            _context.data = this;

            /* Set user callbacks */
            _contextSettings.on_header_field          = on_header_field;
            _contextSettings.on_header_field_complete = on_header_field_complete;
            _contextSettings.on_header_value          = on_header_value;
            _contextSettings.on_header_value_complete = on_header_value_complete;
            _contextSettings.on_body                  = on_body;
            _contextSettings.on_message_complete      = on_complete;
        }

        return true;
    }

    bool validateUri() const { return _requestUri.isValid(); }

    const Uri& getRequestUri() const { return _requestUri; }

    static int on_header_field(llhttp_t* context, const char* at, size_t length)
    {
        auto thiz = (HttpResponse*)context->data;
        thiz->_currentHeader.insert(thiz->_currentHeader.end(), at, at + length);
        return 0;
    }
    static int on_header_field_complete(llhttp_t* context)
    {
        auto thiz = (HttpResponse*)context->data;
        std::transform(thiz->_currentHeader.begin(), thiz->_currentHeader.end(), thiz->_currentHeader.begin(),
                       ::tolower);
        return 0;
    }
    static int on_header_value(llhttp_t* context, const char* at, size_t length)
    {
        auto thiz = (HttpResponse*)context->data;
        thiz->_currentHeaderValue.insert(thiz->_currentHeaderValue.end(), at, at + length);
        return 0;
    }
    static int on_header_value_complete(llhttp_t* context)
    {
        auto thiz = (HttpResponse*)context->data;
        thiz->_responseHeaders.emplace(std::move(thiz->_currentHeader), std::move(thiz->_currentHeaderValue));
        return 0;
    }
    static int on_body(llhttp_t* context, const char* at, size_t length)
    {
        auto thiz = (HttpResponse*)context->data;
        thiz->_responseData.insert(thiz->_responseData.end(), at, at + length);
        return 0;
    }
    static int on_complete(llhttp_t* context)
    {
        auto thiz           = (HttpResponse*)context->data;
        thiz->_responseCode = context->status_code;
        thiz->_finished     = true;
        return 0;
    }

protected:
    // properties
    HttpRequest* _pHttpRequest;  /// the corresponding HttpRequest pointer who leads to this response
    int _redirectCount = 0;

    Uri _requestUri;
    bool _finished = false;             /// to indicate if the http request is successful simply
    yasio::sbyte_buffer _responseData;  /// the returned raw data. You can also dump it as a string
    std::string _currentHeader;
    std::string _currentHeaderValue;
    ResponseHeaderMap _responseHeaders;  /// the returned raw header data. You can also dump it as a string
    int _responseCode = -1;              /// the status code returned from server, e.g. 200, 404
    int _internalCode = 0;               /// the ret code of perform
    llhttp_t _context;
    llhttp_settings_t _contextSettings;
};

}  // namespace network

NS_AX_END

// end group
/// @}

#endif  //__HTTP_RESPONSE_H__
