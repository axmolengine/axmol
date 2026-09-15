/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once
/// @cond DO_NOT_SHOW

#include "axmol/platform/PlatformMacros.h"

#include <string.h>
#include <string>
#include <vector>
#include <span>

namespace ax
{

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

    std::span<const CookieInfo> getCookies() const;
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

}  // namespace ax

/// @endcond
