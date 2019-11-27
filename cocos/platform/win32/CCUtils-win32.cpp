/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
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
#include "base/ccMacros.h"
#include "platform/CCPlatformMacros.h"
#include "platform/win32/CCUtils-win32.h"

#include <sstream>

NS_CC_BEGIN

std::wstring StringUtf8ToWideChar(const std::string& strUtf8)
{
    std::wstring ret;
    if (!strUtf8.empty())
    {
        int nNum = MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, nullptr, 0);
        if (nNum)
        {
            WCHAR* wideCharString = new WCHAR[nNum + 1];
            wideCharString[0] = 0;

            nNum = MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, wideCharString, nNum + 1);

            ret = wideCharString;
            delete[] wideCharString;
        }
        else
        {
            CCLOG("Wrong convert to WideChar code:0x%x", GetLastError());
        }
    }
    return ret;
}

std::string StringWideCharToMultiByte(const std::wstring& strWideChar, unsigned int cp)
{
    int cchMultiByte = WideCharToMultiByte(cp, 0, strWideChar.c_str(), static_cast<int>(strWideChar.length()), NULL, 0, NULL, NULL);
    std::string buffer(cchMultiByte, '\0');
    WideCharToMultiByte(cp, 0, strWideChar.c_str(), static_cast<int>(strWideChar.length()), &buffer.front(), cchMultiByte, NULL, NULL);
    return buffer;
}

std::string UTF8StringToMultiByte(const std::string& strUtf8)
{
    std::string ret;
    if (!strUtf8.empty())
    {
        std::wstring strWideChar = StringUtf8ToWideChar(strUtf8);
        int nNum = WideCharToMultiByte(CP_ACP, 0, strWideChar.c_str(), -1, nullptr, 0, nullptr, FALSE);
        if (nNum)
        {
            char* ansiString = new char[nNum + 1];
            ansiString[0] = 0;

            nNum = WideCharToMultiByte(CP_ACP, 0, strWideChar.c_str(), -1, ansiString, nNum + 1, nullptr, FALSE);

            ret = ansiString;
            delete[] ansiString;
        }
        else
        {
            CCLOG("Wrong convert to Ansi code:0x%x", GetLastError());
        }
    }

    return ret;
}

NS_CC_END
