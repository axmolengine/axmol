/****************************************************************************
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
#ifndef __AXMOL__UI__WEBVIEWCOMMON_H_
#define __AXMOL__UI__WEBVIEWCOMMON_H_

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "base/Utils.h"

namespace webview_common
{
USING_NS_AX;
using namespace rapidjson;

inline std::string htmlFromUri(std::string_view s)
{
    if (s.substr(0, 15) == "data:text/html,")
    {
        return utils::urlDecode(s.substr(15));
    }
    return "";
}

inline int jsonUnescape(const char* s, size_t n, char* out)
{
    int r = 0;
    if (*s++ != '"')
    {
        return -1;
    }
    while (n > 2)
    {
        char c = *s;
        if (c == '\\')
        {
            s++;
            n--;
            switch (*s)
            {
            case 'b':
                c = '\b';
                break;
            case 'f':
                c = '\f';
                break;
            case 'n':
                c = '\n';
                break;
            case 'r':
                c = '\r';
                break;
            case 't':
                c = '\t';
                break;
            case '\\':
                c = '\\';
                break;
            case '/':
                c = '/';
                break;
            case '\"':
                c = '\"';
                break;
            default:  // TODO: support unicode decoding
                return -1;
            }
        }
        if (out != NULL)
        {
            *out++ = c;
        }
        s++;
        n--;
        r++;
    }
    if (*s != '"')
    {
        return -1;
    }
    if (out != NULL)
    {
        *out = '\0';
    }
    return r;
}

// These are the results that must be returned by this method
// assert(jsonParse(R"({"foo":"bar"})", "foo", -1) == "bar");
// assert(jsonParse(R"({"foo":""})", "foo", -1) == "");
// assert(jsonParse(R"(["foo", "bar", "baz"])", "", 0) == "foo");
// assert(jsonParse(R"(["foo", "bar", "baz"])", "", 2) == "baz");
// The following is a special case, where the exact json string is not returned due
// to how rapidjson re-creates the nested object, original: "{"bar": 1}", parsed result: "{"bar":1}"
// assert(jsonParse(R"({"foo": {"bar": 1}})", "foo", -1) == R"({"bar":1})");
inline std::string jsonParse(std::string_view s, std::string_view key, const int index)
{
    const char* value = nullptr;
    size_t value_sz{};
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);
    Document d;
    d.Parse(s.data());
    if (key.empty() && index > -1)
    {
        if (d.IsArray())
        {
            auto&& jsonArray = d.GetArray();
            if (SizeType(index) < jsonArray.Size())
            {
                auto&& arrayValue = jsonArray[SizeType(index)];
                value             = arrayValue.GetString();
                value_sz          = arrayValue.GetStringLength();
            }
        }
    }
    else
    {
        auto&& fieldItr = d.FindMember(key.data());
        if (fieldItr != d.MemberEnd())
        {
            auto&& jsonValue = fieldItr->value;
            if (jsonValue.IsString())
            {
                value    = jsonValue.GetString();
                value_sz = jsonValue.GetStringLength();
            }
            else
            {
                jsonValue.Accept(writer);
                value    = sb.GetString();
                value_sz = sb.GetLength();
            }
        }
    }

    if (value != nullptr)
    {
        if (value[0] != '"')
        {
            return std::string(value, value_sz);
        }

        const auto n = jsonUnescape(value, value_sz, nullptr);
        if (n > 0)
        {
            const auto decoded = std::unique_ptr<char[]>(new char[n + 1]);
            jsonUnescape(value, value_sz, decoded.get());
            return std::string(decoded.get(), n);
        }
    }
    return "";
}

static std::string getDataURI(const ax::Data& data, std::string_view mime_type)
{
    auto encodedData = utils::base64Encode(std::span{data.getBytes(), data.getBytes() + data.getSize()});
    return std::string{"data:"}.append(mime_type).append(";base64,").append(utils::urlEncode(encodedData));
}

}  // namespace webview_common

#endif  // __AXMOL__UI__WEBVIEWCOMMON_H_
