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

#ifndef HTTP_COOKIE_H
#define HTTP_COOKIE_H
/// @cond DO_NOT_SHOW

#include "platform/PlatformMacros.h"

#include <string.h>
#include <string>
#include <vector>

NS_AX_BEGIN

namespace network
{

class Uri;
struct CookieInfo
{
    CookieInfo()                  = default;
    CookieInfo(const CookieInfo&) = default;
    CookieInfo(CookieInfo&& rhs)
        : domain(std::move(rhs.domain))
        , tailmatch(rhs.tailmatch)
        , path(std::move(rhs.path))
        , secure(rhs.secure)
        , name(std::move(rhs.name))
        , value(std::move(rhs.value))
        , expires(rhs.expires)
    {}

    CookieInfo& operator=(CookieInfo&& rhs)
    {
        domain    = std::move(rhs.domain);
        tailmatch = rhs.tailmatch;
        path      = std::move(rhs.path);
        secure    = rhs.secure;
        name      = std::move(rhs.name);
        value     = std::move(rhs.value);
        expires   = rhs.expires;
        return *this;
    }

    bool isSame(const CookieInfo& rhs) { return name == rhs.name && domain == rhs.domain; }

    void updateValue(const CookieInfo& rhs)
    {
        value   = rhs.value;
        expires = rhs.expires;
        path    = rhs.path;
    }

    std::string domain;
    bool tailmatch = true;
    std::string path;
    bool secure = false;
    std::string name;
    std::string value;
    time_t expires = 0;
};

class HttpCookie
{
public:
    void readFile();

    void writeFile();
    void setCookieFileName(std::string_view fileName);

    const std::vector<CookieInfo>* getCookies() const;
    const CookieInfo* getMatchCookie(const Uri& uri) const;
    void updateOrAddCookie(CookieInfo* cookie);

    // Check match cookies for http request
    std::string checkAndGetFormatedMatchCookies(const Uri& uri);
    bool updateOrAddCookie(std::string_view cookie, const Uri& uri);

private:
    std::string _cookieFileName;
    std::vector<CookieInfo> _cookies;
};
}  // namespace network

NS_AX_END

/// @endcond
#endif /* HTTP_COOKIE_H */
