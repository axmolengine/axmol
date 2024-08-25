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

#include "network/HttpCookie.h"
#include "network/Uri.h"
#include "platform/FileUtils.h"
#include "yasio/utils.hpp"
#include "yasio/string_view.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <locale>
#include <iomanip>
#include <sstream>

#include "base/axstd.h"
#include "fmt/compile.h"

namespace ax
{

namespace network
{
void HttpCookie::readFile()
{
    enum
    {
        DOMAIN_INDEX = 0,
        TAILMATCH_INDEX,
        PATH_INDEX,
        SECURE_INDEX,
        EXPIRES_INDEX,
        NAME_INDEX,
        VALUE_INDEX,
    };

    using namespace std::literals::string_view_literals;

    std::string inString = FileUtils::getInstance()->getStringFromFile(_cookieFileName);
    if (!inString.empty())
    {
        axstd::split_cb(&inString.front(), inString.length(), '\n', [this](char* s, char* e) {
            if (*s == '#')  // skip comment
                return;
            int count = 0;
            CookieInfo cookieInfo;
            using namespace cxx17;
            axstd::split_cb(s, e - s, '\t', [&, this](char* ss, char* ee) {
                auto ch = *ee;  // store
                *ee     = '\0';
                switch (count)
                {
                case DOMAIN_INDEX:
                    cookieInfo.domain.assign(ss, ee - ss);
                    break;
                case PATH_INDEX:
                    cookieInfo.path.assign(ss, ee - ss);
                    break;
                case SECURE_INDEX:
                    cookieInfo.secure = cxx17::string_view{ss, (size_t)(ee - ss)} == "TRUE"_sv;
                    break;
                case EXPIRES_INDEX:
                    cookieInfo.expires = static_cast<time_t>(strtoll(ss, nullptr, 10));
                    break;
                case NAME_INDEX:
                    cookieInfo.name.assign(ss, ee - ss);
                    break;
                case VALUE_INDEX:
                    cookieInfo.value.assign(ss, ee - ss);
                    break;
                }
                *ee = ch;  // restore
                ++count;
            });
            if (count >= 7)
                _cookies.push_back(std::move(cookieInfo));
        });
    }
}

const std::vector<CookieInfo>* HttpCookie::getCookies() const
{
    return &_cookies;
}

const CookieInfo* HttpCookie::getMatchCookie(const Uri& uri) const
{
    for (auto&& cookie : _cookies)
    {
        if (cxx20::ends_with(uri.getHost(), cookie.domain) && cxx20::starts_with(uri.getPath(), cookie.path))
            return &cookie;
    }

    return nullptr;
}

void HttpCookie::updateOrAddCookie(CookieInfo* cookie)
{
    for (auto&& _cookie : _cookies)
    {
        if (cookie->isSame(_cookie))
        {
            _cookie.updateValue(*cookie);
            return;
        }
    }
    _cookies.push_back(std::move(*cookie));
}

std::string HttpCookie::checkAndGetFormatedMatchCookies(const Uri& uri)
{
    std::string ret;
    for (auto iter = _cookies.begin(); iter != _cookies.end();)
    {
        auto& cookie = *iter;
        if (cxx20::ends_with(uri.getHost(), cookie.domain) && cxx20::starts_with(uri.getPath(), cookie.path))
        {
            if (yasio::time_now() >= cookie.expires)
            {
                iter = _cookies.erase(iter);
                continue;
            }

            if (!ret.empty())
                ret += "; ";

            ret += cookie.name;
            ret.push_back('=');
            ret += cookie.value;
        }
        ++iter;
    }
    return ret;
}

bool HttpCookie::updateOrAddCookie(std::string_view cookie, const Uri& uri)
{
    using namespace std::literals::string_view_literals;

    unsigned int count = 0;
    CookieInfo info;
    axstd::split_cb(cookie.data(), cookie.length(), ';', [&](const char* start, const char* end) {
        unsigned int count_ = 0;
        while (*start == ' ')
            ++start;  // skip ws
        if (++count > 1)
        {
            cxx17::string_view key;
            cxx17::string_view value;
            axstd::split_cb(start, end - start, '=', [&](const char* s, const char* e) {
                switch (++count_)
                {
                case 1:
                    key = cxx17::string_view(s, e - s);
                    break;
                case 2:
                    value = cxx17::string_view(s, e - s);
                    break;
                }
            });

            using namespace cxx17;
            if (cxx20::ic::iequals(key, "domain"_sv))
            {
                if (!value.empty())
                    info.domain.assign(value.data(), value.length());
            }
            else if (cxx20::ic::iequals(key, "path"_sv))
            {
                if (!value.empty())
                    info.path.assign(value.data(), value.length());
            }
            else if (cxx20::ic::iequals(key, "expires"_sv))
            {
                std::string expires_ctime(!value.empty() ? value.data() : "", value.length());
                if (cxx20::ends_with(expires_ctime, " GMT"_sv))
                    expires_ctime.resize(expires_ctime.length() - sizeof(" GMT") + 1);
                if (expires_ctime.empty())
                    return;
                size_t off = 0;
                auto p     = expires_ctime.find_first_of(',');
                if (p != std::string::npos)
                {
                    p = expires_ctime.find_first_not_of(' ', p + 1);  // skip ws
                    if (p != std::string::npos)
                        off = p;
                }

                struct tm dt = {0};
                std::stringstream ss(&expires_ctime[off]);
                ss >> std::get_time(&dt, "%d %b %Y %H:%M:%S");
                if (!ss.fail())
                    info.expires = mktime(&dt);
                else
                {
                    ss.str("");
                    ss.clear();
                    ss << (&expires_ctime[off]);
                    ss >> std::get_time(&dt, "%d-%b-%Y %H:%M:%S");
                    if (!ss.fail())
                        info.expires = mktime(&dt);
                }
            }
            else if (cxx20::ic::iequals(key, "secure"_sv))
            {
                info.secure = true;
            }
        }
        else
        {  // first is cookie name
            axstd::split_cb(start, end - start, '=', [&](const char* s, const char* e) {
                switch (++count_)
                {
                case 1:
                    info.name.assign(s, e - s);
                    break;
                case 2:
                    info.value.assign(s, e - s);
                    break;
                }
            });
        }
    });
    if (info.path.empty())
        info.path.push_back('/');

    if (info.domain.empty())
        info.domain += uri.getHost();

    if (info.expires <= 0)
        info.expires = (std::numeric_limits<time_t>::max)();

    updateOrAddCookie(&info);
    return true;
}

void HttpCookie::writeFile()
{
    constexpr auto cookieText =
        "# Netscape HTTP Cookie File\n"
        "# http://curl.haxx.se/docs/http-cookies.html\n"
        "# This file was generated by axmol! Edit at your own risk.\n"
        "# Test axmol cookie write.\n\n"sv;
    constexpr auto endLine = "\n"sv;

    auto out = FileUtils::getInstance()->openFileStream(_cookieFileName, IFileStream::Mode::WRITE);
    out->write(cookieText.data(), cookieText.size());

    std::string line;

    char expires[32] = {0};  // LONGLONG_STRING_SIZE=20
    for (auto&& cookie : _cookies)
    {
        line.clear();
        line.append(cookie.domain);
        line.append(1, '\t');
        cookie.tailmatch ? line.append("TRUE") : line.append("FALSE");
        line.append(1, '\t');
        line.append(cookie.path);
        line.append(1, '\t');
        cookie.secure ? line.append("TRUE") : line.append("FALSE");
        line.append(1, '\t');
        fmt::format_to(std::back_inserter(line), FMT_COMPILE("{}"), cookie.expires);
        line.append(1, '\t');
        line.append(cookie.name);
        line.append(1, '\t');
        line.append(cookie.value);
        // line.append(1, '\n');

        out->write(line.c_str(), static_cast<unsigned int>(line.length()));
        out->write(endLine.data(), static_cast<unsigned int>(endLine.size()));
    }

    out->close();
}

void HttpCookie::setCookieFileName(std::string_view filename)
{
    _cookieFileName = filename;
}

}  // namespace network

}
