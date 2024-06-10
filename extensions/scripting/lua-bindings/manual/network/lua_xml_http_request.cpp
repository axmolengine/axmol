/****************************************************************************
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
#include "lua-bindings/manual/network/lua_xml_http_request.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaStack.h"
#include "lua-bindings/manual/LuaValue.h"
#include "lua-bindings/manual/LuaEngine.h"
#include "lua-bindings/manual/base/LuaScriptHandlerMgr.h"

#include "network/HttpClient.h"

#include <unordered_map>
#include <string>
#include <sstream>

#include "yasio/byte_buffer.hpp"

USING_NS_AX;
using namespace ax::network;

class LuaMinXmlHttpRequest : public ax::Object
{
public:
    enum class ResponseType
    {
        STRING,
        ARRAY_BUFFER,
        BLOB,
        DOCUMENT,
        JSON
    };

    // Ready States (http://www.w3.org/TR/XMLHttpRequest/#interface-xmlhttprequest)
    static const unsigned short UNSENT           = 0;
    static const unsigned short OPENED           = 1;
    static const unsigned short HEADERS_RECEIVED = 2;
    static const unsigned short LOADING          = 3;
    static const unsigned short DONE             = 4;

    LuaMinXmlHttpRequest();
    ~LuaMinXmlHttpRequest();

    inline void setResponseType(ResponseType type) { _responseType = type; }
    inline ResponseType getResponseType() const { return _responseType; }

    inline void setWithCredentialsValue(bool value) { _withCredentialsValue = value; }
    inline bool getWithCredentialsValue() const { return _withCredentialsValue; }

    inline void setTimeout(unsigned timeOut) { _timeout = timeOut; }
    inline unsigned getTimeout() const { return _timeout; }

    inline void setReadyState(int readyState) { _readyState = readyState; }
    inline int getReadyState() const { return _readyState; }

    inline ax::network::HttpRequest* getHttpRequest() const { return _httpRequest; }
    inline std::string_view getStatusText() const { return _statusText; }

    inline void setStatus(int status) { _status = status; }
    inline int getStatus() { return _status; }

    inline std::string_view getUrl() { return _url; }
    inline void setUrl(std::string_view url) { _url = url; }

    inline std::string_view getMethod() const { return _meth; }
    inline void setMethod(std::string_view meth) { _meth = meth; }

    inline void setAsync(bool isAsync) { _isAsync = isAsync; }
    inline void setIsNetWork(bool isNetWork) { _isNetwork = isNetWork; }

    void _setHttpRequestHeader();
    void _sendRequest();
    void setRequestHeader(const char* field, const char* value);

    const HttpResponse::ResponseHeaderMap& getHttpHeader() const { return _httpHeader; }
    void clearHttpHeader() { _httpHeader.clear(); }

    void getByteData(unsigned char* byteData) const;

    inline const char* getData() { return !_data.empty() ? _data.data() : ""; }
    inline size_t getDataSize() const { return _data.size(); }

    inline void setErrorFlag(bool errorFlag) { _errorFlag = errorFlag; }
    inline bool getErrorFlag() const { return _errorFlag; }

    inline void setAborted(bool isAborted) { _isAborted = isAborted; }
    inline bool isAborted() const { return _isAborted; }

private:
    std::string _url;
    std::string _meth;
    std::string _type;
    yasio::sbyte_buffer _data;
    int _readyState;
    int _status;
    std::string _statusText;
    ResponseType _responseType;
    unsigned _timeout;
    bool _isAsync;
    HttpRequest* _httpRequest;
    bool _isNetwork;
    bool _withCredentialsValue;
    HttpResponse::ResponseHeaderMap _httpHeader;
    std::unordered_map<std::string, std::string> _requestHeader;
    bool _errorFlag;
    bool _isAborted;
};

LuaMinXmlHttpRequest::LuaMinXmlHttpRequest()
    : _url("")
    , _meth("")
    , _type("")
    , _readyState(UNSENT)
    , _status(0)
    , _statusText("")
    , _responseType(ResponseType::STRING)
    , _timeout(0)
    , _isAsync(false)
    , _isNetwork(true)
    , _withCredentialsValue(true)
    , _errorFlag(false)
    , _isAborted(false)
{
    _httpHeader.clear();
    _requestHeader.clear();
    _httpRequest = new HttpRequest();
}

LuaMinXmlHttpRequest::~LuaMinXmlHttpRequest()
{
    _httpHeader.clear();
    _requestHeader.clear();
    AX_SAFE_RELEASE_NULL(_httpRequest);
}

/**
 *  @brief Set Request header for next call.
 *  @param field  Name of the Header to be set.
 *  @param value  Value of the Headerfield
 */
void LuaMinXmlHttpRequest::setRequestHeader(const char* field, const char* value)
{
    std::stringstream header_s;
    std::stringstream value_s;
    std::string header;

    auto iter = _requestHeader.find(field);

    // Concatenate values when header exists.
    if (iter != _requestHeader.end())
    {
        value_s << iter->second << "," << value;
    }
    else
    {
        value_s << value;
    }

    _requestHeader[field] = value_s.str();
}

/**
 * @brief  If headers has been set, pass them to curl.
 *
 */
void LuaMinXmlHttpRequest::_setHttpRequestHeader()
{
    std::vector<std::string> header;

    for (auto it = _requestHeader.begin(); it != _requestHeader.end(); ++it)
    {
        header.emplace_back(it->first + ": " + it->second);
    }

    if (!header.empty())
    {
        _httpRequest->setHeaders(header);
    }
}

/**
 * @brief   Send out request and fire callback when done.
 */
void LuaMinXmlHttpRequest::_sendRequest()
{
    _httpRequest->setResponseCallback(
        [this](ax::network::HttpClient* sender, ax::network::HttpResponse* response) {
            if (_isAborted)
                return;
            auto tag = response->getHttpRequest()->getTag();
            if (!tag.empty())
            {
                AXLOG("%s completed", tag.data());
            }

            int statusCode = response->getResponseCode();

            if (!response->isSucceed())
            {
                AXLOG("Response failed, statusCode: %d", statusCode);
                if (statusCode == 0)
                {
                    _errorFlag = true;
                    _status    = 0;
                    _statusText.clear();
                }
                // TODO: call back lua function
                int handler = ax::ScriptHandlerMgr::getInstance()->getObjectHandler(
                    (void*)this, ax::ScriptHandlerMgr::HandlerType::XMLHTTPREQUEST_READY_STATE_CHANGE);

                if (0 != handler)
                {
                    AXLOG("come in handler, handler is %d", handler);
                    ax::CommonScriptData data(handler, "");
                    ax::ScriptEvent event(ax::ScriptEventType::kCommonEvent, (void*)&data);
                    ax::ScriptEngineManager::sendEventToLua(event);
                }
                return;
            }

            // set header
            _httpHeader = response->getResponseHeaders();

            /** get the response data **/
            auto buffer = response->getResponseData();

            if (statusCode == 200)
            {
                // Succeeded
                _status     = 200;
                _readyState = DONE;
                _data       = std::move(*buffer);
            }
            else
            {
                _status = 0;
            }

            // TODO: call back lua function
            int handler = ax::ScriptHandlerMgr::getInstance()->getObjectHandler(
                (void*)this, ax::ScriptHandlerMgr::HandlerType::XMLHTTPREQUEST_READY_STATE_CHANGE);

            if (0 != handler)
            {
                ax::CommonScriptData data(handler, "");
                ax::ScriptEvent event(ax::ScriptEventType::kCommonEvent, (void*)&data);
                ax::ScriptEngineManager::sendEventToLua(event);
            }
            release();
        });
    network::HttpClient::getInstance()->send(_httpRequest);
    retain();
}

void LuaMinXmlHttpRequest::getByteData(unsigned char* byteData) const
{
    if (!_data.empty())
        memcpy((char*)byteData, _data.data(), _data.size());
}

/* function to regType */
static void lua_reg_xml_http_request(lua_State* L)
{
    tolua_usertype(L, "ax.XMLHttpRequest");
}

static int lua_collect_xml_http_request(lua_State* L)
{
    LuaMinXmlHttpRequest* self = (LuaMinXmlHttpRequest*)tolua_tousertype(L, 1, 0);
    self->release();
    return 0;
}

static int axlua_XMLHttpRequest_constructor(lua_State* L)
{
    int argc                   = 0;
    LuaMinXmlHttpRequest* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

    argc = lua_gettop(L) - 1;
    if (argc == 0)
    {
        self = new LuaMinXmlHttpRequest();
        tolua_pushusertype(L, (void*)self, "ax.XMLHttpRequest");
        tolua_register_gc(L, lua_gettop(L));
        return 1;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "XMLHttpRequest", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_XMLHttpRequest_constructor'.", &tolua_err);
    return 0;
#endif
}

static int lua_get_XMLHttpRequest_responseType(lua_State* L)
{
    LuaMinXmlHttpRequest* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.XMLHttpRequest", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (LuaMinXmlHttpRequest*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'lua_get_XMLHttpRequest_responseType'\n", nullptr);
        return 0;
    }
#endif

    tolua_pushnumber(L, (lua_Number)self->getResponseType());
    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'lua_get_XMLHttpRequest_responseType'.", &tolua_err);
    return 0;
#endif
}

static int lua_set_XMLHttpRequest_responseType(lua_State* L)
{
    int argc                   = 0;
    LuaMinXmlHttpRequest* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.XMLHttpRequest", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (LuaMinXmlHttpRequest*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'lua_set_XMLHttpRequest_responseType'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isnumber(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        int responseType = (int)tolua_tonumber(L, 2, 0);

        self->setResponseType((LuaMinXmlHttpRequest::ResponseType)responseType);

        return 0;
    }

    luaL_error(L, "'setResponseType' function of XMLHttpRequest wrong number of arguments: %d, was expecting %d\n",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'lua_set_XMLHttpRequest_responseType'.", &tolua_err);
    return 0;
#endif
}

static int lua_get_XMLHttpRequest_withCredentials(lua_State* L)
{
    LuaMinXmlHttpRequest* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.XMLHttpRequest", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (LuaMinXmlHttpRequest*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'lua_get_XMLHttpRequest_withCredentials'\n", nullptr);
        return 0;
    }
#endif

    tolua_pushboolean(L, self->getWithCredentialsValue());
    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'lua_get_XMLHttpRequest_withCredentials'.", &tolua_err);
    return 0;
#endif
}

static int lua_set_XMLHttpRequest_withCredentials(lua_State* L)
{
    int argc                   = 0;
    LuaMinXmlHttpRequest* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.XMLHttpRequest", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (LuaMinXmlHttpRequest*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'lua_set_XMLHttpRequest_withCredentials'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isboolean(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        self->setWithCredentialsValue((bool)tolua_toboolean(L, 2, 0));
        return 0;
    }

    luaL_error(L, "'setWithCredentials' function of XMLHttpRequest wrong number of arguments: %d, was expecting %d\n",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'lua_set_XMLHttpRequest_withCredentials'.", &tolua_err);
    return 0;
#endif
}

static int lua_get_XMLHttpRequest_timeout(lua_State* L)
{
    LuaMinXmlHttpRequest* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.XMLHttpRequest", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (LuaMinXmlHttpRequest*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'lua_get_XMLHttpRequest_timeout'\n", nullptr);
        return 0;
    }
#endif

    tolua_pushnumber(L, (lua_Number)self->getTimeout());
    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'lua_get_XMLHttpRequest_timeout'.", &tolua_err);
    return 0;
#endif
}

static int lua_set_XMLHttpRequest_timeout(lua_State* L)
{
    int argc                   = 0;
    LuaMinXmlHttpRequest* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.XMLHttpRequest", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (LuaMinXmlHttpRequest*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'lua_set_XMLHttpRequest_timeout'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isnumber(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        self->setTimeout((unsigned)tolua_tonumber(L, 2, 0));
        return 0;
    }

    luaL_error(L, "'setTimeout' function of XMLHttpRequest wrong number of arguments: %d, was expecting %d\n", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'lua_set_XMLHttpRequest_timeout'.", &tolua_err);
    return 0;
#endif
}

static int lua_get_XMLHttpRequest_readyState(lua_State* L)
{
    LuaMinXmlHttpRequest* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.XMLHttpRequest", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (LuaMinXmlHttpRequest*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'lua_get_XMLHttpRequest_readyState'\n", nullptr);
        return 0;
    }
#endif

    lua_pushinteger(L, (lua_Integer)self->getReadyState());

    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'lua_get_XMLHttpRequest_readyState'.", &tolua_err);
    return 0;
#endif
}

static int lua_get_XMLHttpRequest_status(lua_State* L)
{
    LuaMinXmlHttpRequest* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.XMLHttpRequest", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (LuaMinXmlHttpRequest*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'lua_get_XMLHttpRequest_status'\n", nullptr);
        return 0;
    }
#endif

    lua_pushinteger(L, (lua_Integer)self->getStatus());

    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'lua_get_XMLHttpRequest_status'.", &tolua_err);
    return 0;
#endif
}

static int lua_get_XMLHttpRequest_statusText(lua_State* L)
{
    LuaMinXmlHttpRequest* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.XMLHttpRequest", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (LuaMinXmlHttpRequest*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'lua_get_XMLHttpRequest_statusText'\n", nullptr);
        return 0;
    }
#endif

    tolua_pushsv(L, self->getStatusText());

    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'lua_get_XMLHttpRequest_statusText'.", &tolua_err);
    return 0;
#endif
}

static int lua_get_XMLHttpRequest_responseText(lua_State* L)
{
    LuaMinXmlHttpRequest* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.XMLHttpRequest", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (LuaMinXmlHttpRequest*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'lua_get_XMLHttpRequest_responseText'\n", nullptr);
        return 0;
    }
#endif
    lua_pushlstring(L, self->getData(), self->getDataSize());
    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'lua_get_XMLHttpRequest_responseText'.", &tolua_err);
    return 0;
#endif
}

static int lua_get_XMLHttpRequest_response(lua_State* L)
{
    LuaMinXmlHttpRequest* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.XMLHttpRequest", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (LuaMinXmlHttpRequest*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'lua_get_XMLHttpRequest_response'\n", nullptr);
        return 0;
    }
#endif

    if (self->getResponseType() == LuaMinXmlHttpRequest::ResponseType::JSON)
    {
        if (self->getReadyState() != LuaMinXmlHttpRequest::DONE || self->getErrorFlag())
            return 0;

        lua_pushlstring(L, self->getData(), self->getDataSize());
        return 1;
    }
    else if (self->getResponseType() == LuaMinXmlHttpRequest::ResponseType::ARRAY_BUFFER)
    {
        if (self->getReadyState() != LuaMinXmlHttpRequest::DONE || self->getErrorFlag())
            return 0;

        LuaStack* pStack = LuaEngine::getInstance()->getLuaStack();
        if (NULL == pStack)
        {
            return 0;
        }

        lua_State* tolua_s = pStack->getLuaState();
        if (NULL == tolua_s)
        {
            return 0;
        }

        LuaValueArray array;

        uint8_t* tmpData = new uint8_t[self->getDataSize()];
        self->getByteData(tmpData);

        for (size_t i = 0; i < self->getDataSize(); i++)
        {
            LuaValue value = LuaValue::intValue(tmpData[i]);
            array.emplace_back(value);
        }

        pStack->pushLuaValueArray(array);

        AX_SAFE_DELETE_ARRAY(tmpData);
        return 1;
    }
    else
    {
        lua_pushlstring(L, self->getData(), self->getDataSize());
        return 1;
    }

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'lua_get_XMLHttpRequest_response'.", &tolua_err);
    return 0;
#endif
}

static int axlua_XMLHttpRequest_open(lua_State* L)
{
    int argc                   = 0;
    LuaMinXmlHttpRequest* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.XMLHttpRequest", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (LuaMinXmlHttpRequest*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_XMLHttpRequest_open'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (argc >= 2)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isstring(L, 2, 0, &tolua_err) || !tolua_isstring(L, 3, 0, &tolua_err))
            goto tolua_lerror;
#endif

        std::string method = tolua_tostring(L, 2, "");
        std::string url    = tolua_tostring(L, 3, "");
        bool async         = true;
        if (argc > 2)
        {
#if _AX_DEBUG >= 1
            if (!tolua_isboolean(L, 4, 0, &tolua_err))
                goto tolua_lerror;
#endif
            async = tolua_toboolean(L, 4, 0);
        }

        self->setUrl(url);
        self->setMethod(method);
        self->setReadyState(1);
        self->setAsync(async);

        if (url.length() > 5 && url.compare(url.length() - 5, 5, ".json") == 0)
        {
            self->setResponseType(LuaMinXmlHttpRequest::ResponseType::JSON);
        }

        if (nullptr != self->getHttpRequest())
        {
            if (method.compare("post") == 0 || method.compare("POST") == 0)
            {
                self->getHttpRequest()->setRequestType(network::HttpRequest::Type::POST);
            }
            else if (method.compare("get") == 0 || method.compare("GET") == 0)
            {
                self->getHttpRequest()->setRequestType(network::HttpRequest::Type::GET);
            }
            else if (method.compare("patch") == 0 || method.compare("PATCH") == 0)
            {
                self->getHttpRequest()->setRequestType(network::HttpRequest::Type::PATCH);
            }
            else if (method.compare("put") == 0 || method.compare("PUT") == 0)
            {
                self->getHttpRequest()->setRequestType(network::HttpRequest::Type::PUT);
            }
            else if (method.compare("delete") == 0 || method.compare("DELETE") == 0)
            {
                self->getHttpRequest()->setRequestType(network::HttpRequest::Type::DELETE);
            }
            else
            {
                self->getHttpRequest()->setRequestType(network::HttpRequest::Type::UNKNOWN);
            }

            self->getHttpRequest()->setUrl(url);
        }

        self->setIsNetWork(true);
        self->setReadyState(LuaMinXmlHttpRequest::OPENED);
        self->setStatus(0);
        self->setAborted(false);

        return 0;
    }

    luaL_error(L, "'open' function of XMLHttpRequest wrong number of arguments: %d, was expecting %d\n", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_XMLHttpRequest_open'.", &tolua_err);
    return 0;
#endif
}

static int axlua_XMLHttpRequest_send(lua_State* L)
{
    int argc                   = 0;
    LuaMinXmlHttpRequest* self = nullptr;
    // std::string data = "";
    const char* data = NULL;
    size_t size      = 0;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.XMLHttpRequest", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (LuaMinXmlHttpRequest*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_XMLHttpRequest_send'\n", nullptr);
        return 0;
    }
#endif
    self->clearHttpHeader();
    self->setErrorFlag(false);

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isstring(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        // data = tolua_tostring(L, 2, "");
        data = (const char*)lua_tolstring(L, 2, &size);
    }

    if (size > 0 &&
        (self->getMethod().compare("post") == 0 || self->getMethod().compare("POST") == 0 ||
         self->getMethod().compare("put") == 0 || self->getMethod().compare("PUT") == 0) &&
        nullptr != self->getHttpRequest())
    {
        self->getHttpRequest()->setRequestData(data, size);
    }

    self->_setHttpRequestHeader();
    self->_sendRequest();
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_XMLHttpRequest_send'.", &tolua_err);
    return 0;
#endif
}

/**
 * @brief abort function Placeholder!
 */
static int axlua_XMLHttpRequest_abort(lua_State* L)
{
    int argc                   = 0;
    LuaMinXmlHttpRequest* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.XMLHttpRequest", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (LuaMinXmlHttpRequest*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_XMLHttpRequest_send'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (0 == argc)
    {
        self->setAborted(true);
        self->setReadyState(LuaMinXmlHttpRequest::UNSENT);
    }
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_XMLHttpRequest_send'.", &tolua_err);
    return 0;
#endif
}

static int axlua_XMLHttpRequest_setRequestHeader(lua_State* L)
{
    int argc                   = 0;
    LuaMinXmlHttpRequest* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.XMLHttpRequest", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (LuaMinXmlHttpRequest*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_XMLHttpRequest_setRequestHeader'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (2 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isstring(L, 2, 0, &tolua_err) || !tolua_isstring(L, 3, 0, &tolua_err))
            goto tolua_lerror;
#endif

        const char* field = tolua_tostring(L, 2, "");
        const char* value = tolua_tostring(L, 3, "");
        self->setRequestHeader(field, value);
        return 0;
    }

    luaL_error(L, "'setRequestHeader' function of XMLHttpRequest wrong number of arguments: %d, was expecting %d\n",
               argc, 2);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_XMLHttpRequest_setRequestHeader'.", &tolua_err);
    return 0;
#endif
}

static int axlua_XMLHttpRequest_getAllResponseHeaders(lua_State* L)
{
    int argc                   = 0;
    LuaMinXmlHttpRequest* self = nullptr;

    std::stringstream responseheaders;
    std::string responseheader;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.XMLHttpRequest", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (LuaMinXmlHttpRequest*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_XMLHttpRequest_getAllResponseHeaders'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (0 == argc)
    {
        const auto& httpHeader = self->getHttpHeader();

        for (auto it = httpHeader.begin(); it != httpHeader.end(); ++it)
        {
            responseheaders << it->first << ": " << it->second << "\n";
        }

        responseheader = responseheaders.str();
        tolua_pushstring(L, responseheader.c_str());
        return 1;
    }

    luaL_error(L,
               "'getAllResponseHeaders' function of XMLHttpRequest wrong number of arguments: %d, was expecting %d\n",
               argc, 0);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_XMLHttpRequest_getAllResponseHeaders'.", &tolua_err);
    return 0;
#endif
}

static int axlua_XMLHttpRequest_getResponseHeader(lua_State* L)
{
    int argc                   = 0;
    LuaMinXmlHttpRequest* self = nullptr;

    std::string responseheader;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.XMLHttpRequest", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (LuaMinXmlHttpRequest*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_XMLHttpRequest_getAllResponseHeaders'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isstring(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        responseheader = tolua_tostring(L, 2, "");

        const auto& headers = self->getHttpHeader();
        auto iter           = headers.find(responseheader);
        if (iter != headers.end())
        {
            tolua_pushstring(L, (iter->second).c_str());
        }
        else
        {
            lua_pushnil(L);
        }
        return 1;
    }

    luaL_error(L, "'getResponseHeader' function of XMLHttpRequest wrong number of arguments: %d, was expecting %d\n",
               argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_XMLHttpRequest_getAllResponseHeaders'.", &tolua_err);
    return 0;
#endif
}

static int axlua_XMLHttpRequest_registerScriptHandler(lua_State* L)
{
    int argc                   = 0;
    LuaMinXmlHttpRequest* self = nullptr;

    std::string responseheader;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.XMLHttpRequest", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (LuaMinXmlHttpRequest*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_XMLHttpRequest_registerScriptHandler'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
            goto tolua_lerror;
#endif

        int handler = (toluafix_ref_function(L, 2, 0));
        ax::ScriptHandlerMgr::getInstance()->addObjectHandler(
            (void*)self, handler, ax::ScriptHandlerMgr::HandlerType::XMLHTTPREQUEST_READY_STATE_CHANGE);
        return 0;
    }

    luaL_error(L,
               "'registerScriptHandler' function of XMLHttpRequest wrong number of arguments: %d, was expecting %d\n",
               argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_XMLHttpRequest_registerScriptHandler'.", &tolua_err);
    return 0;
#endif
}

static int axlua_XMLHttpRequest_unregisterScriptHandler(lua_State* L)
{
    int argc                   = 0;
    LuaMinXmlHttpRequest* self = nullptr;

    std::string responseheader;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.XMLHttpRequest", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (LuaMinXmlHttpRequest*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_XMLHttpRequest_unregisterScriptHandler'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (0 == argc)
    {
        ax::ScriptHandlerMgr::getInstance()->removeObjectHandler(
            (void*)self, ax::ScriptHandlerMgr::HandlerType::XMLHTTPREQUEST_READY_STATE_CHANGE);

        return 0;
    }

    luaL_error(L,
               "'unregisterScriptHandler' function of XMLHttpRequest wrong number of arguments: %d, was expecting %d\n",
               argc, 0);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_XMLHttpRequest_unregisterScriptHandler'.", &tolua_err);
    return 0;
#endif
}

TOLUA_API int register_xml_http_request(lua_State* L)
{
    tolua_open(L);
    lua_reg_xml_http_request(L);
    tolua_module(L, "ax", 0);
    tolua_beginmodule(L, "ax");
    tolua_cclass(L, "XMLHttpRequest", "ax.XMLHttpRequest", "ax.Object", lua_collect_xml_http_request);
    tolua_beginmodule(L, "XMLHttpRequest");
    tolua_variable(L, "responseType", lua_get_XMLHttpRequest_responseType, lua_set_XMLHttpRequest_responseType);
    tolua_variable(L, "withCredentials", lua_get_XMLHttpRequest_withCredentials,
                   lua_set_XMLHttpRequest_withCredentials);
    tolua_variable(L, "timeout", lua_get_XMLHttpRequest_timeout, lua_set_XMLHttpRequest_timeout);
    tolua_variable(L, "readyState", lua_get_XMLHttpRequest_readyState, nullptr);
    tolua_variable(L, "status", lua_get_XMLHttpRequest_status, nullptr);
    tolua_variable(L, "statusText", lua_get_XMLHttpRequest_statusText, nullptr);
    tolua_variable(L, "responseText", lua_get_XMLHttpRequest_responseText, nullptr);
    tolua_variable(L, "response", lua_get_XMLHttpRequest_response, nullptr);
    tolua_function(L, "new", axlua_XMLHttpRequest_constructor);
    tolua_function(L, "open", axlua_XMLHttpRequest_open);
    tolua_function(L, "send", axlua_XMLHttpRequest_send);
    tolua_function(L, "abort", axlua_XMLHttpRequest_abort);
    tolua_function(L, "setRequestHeader", axlua_XMLHttpRequest_setRequestHeader);
    tolua_function(L, "getAllResponseHeaders", axlua_XMLHttpRequest_getAllResponseHeaders);
    tolua_function(L, "getResponseHeader", axlua_XMLHttpRequest_getResponseHeader);
    tolua_function(L, "registerScriptHandler", axlua_XMLHttpRequest_registerScriptHandler);
    tolua_function(L, "unregisterScriptHandler", axlua_XMLHttpRequest_unregisterScriptHandler);
    tolua_endmodule(L);
    tolua_endmodule(L);
    return 1;
}
