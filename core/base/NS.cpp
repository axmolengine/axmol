/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2017 Chukong Technologies
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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
#include "base/NS.h"
#include <string>
#include <vector>
#include <string.h>
#include <stdlib.h>

#include "base/Utils.h"

using namespace std;

namespace ax
{

typedef std::vector<std::string> strArray;

// string toolkit
static inline void split(std::string_view src, std::string_view token, strArray& vect)
{
    size_t nend      = 0;
    size_t nbegin    = 0;
    size_t tokenSize = token.size();
    while (nend != std::string::npos)
    {
        nend = src.find(token, nbegin);
        if (nend == std::string::npos)
            vect.emplace_back(std::string{src.substr(nbegin, src.length() - nbegin)});
        else
            vect.emplace_back(std::string{src.substr(nbegin, nend - nbegin)});
        nbegin = nend + tokenSize;
    }
}

// first, judge whether the form of the string like this: {x,y}
// if the form is right,the string will be split into the parameter strs;
// or the parameter strs will be empty.
// if the form is right return true,else return false.
static bool splitWithForm(std::string_view content, strArray& strs)
{
    bool bRet = false;

    do
    {
        AX_BREAK_IF(content.empty());

        size_t nPosLeft  = content.find('{');
        size_t nPosRight = content.find('}');

        // don't have '{' and '}'
        AX_BREAK_IF(nPosLeft == std::string::npos || nPosRight == std::string::npos);
        // '}' is before '{'
        AX_BREAK_IF(nPosLeft > nPosRight);

        auto pointStr = content.substr(nPosLeft + 1, nPosRight - nPosLeft - 1);
        // nothing between '{' and '}'
        AX_BREAK_IF(pointStr.empty());

        size_t nPos1 = pointStr.find('{');
        size_t nPos2 = pointStr.find('}');
        // contain '{' or '}'
        AX_BREAK_IF(nPos1 != std::string::npos || nPos2 != std::string::npos);

        split(pointStr, ",", strs);
        if (strs.size() != 2 || strs[0].empty() || strs[1].empty())
        {
            strs.clear();
            break;
        }

        bRet = true;
    } while (0);

    return bRet;
}

// implement the functions

Rect RectFromString(std::string_view str)
{
    Rect result = Rect::ZERO;

    do
    {
        AX_BREAK_IF(str.empty());
        auto content = str;

        // find the first '{' and the third '}'
        size_t nPosLeft  = content.find('{');
        size_t nPosRight = content.find('}');
        for (int i = 1; i < 3; ++i)
        {
            if (nPosRight == std::string::npos)
            {
                break;
            }
            nPosRight = content.find('}', nPosRight + 1);
        }
        AX_BREAK_IF(nPosLeft == std::string::npos || nPosRight == std::string::npos);

        content          = content.substr(nPosLeft + 1, nPosRight - nPosLeft - 1);
        size_t nPointEnd = content.find('}');
        AX_BREAK_IF(nPointEnd == std::string::npos);
        nPointEnd = content.find(',', nPointEnd);
        AX_BREAK_IF(nPointEnd == std::string::npos);

        // get the point string and size string
        auto pointStr = content.substr(0, nPointEnd);
        auto sizeStr  = content.substr(nPointEnd + 1, content.length() - nPointEnd);

        // split the string with ','
        strArray pointInfo;
        AX_BREAK_IF(!splitWithForm(pointStr, pointInfo));
        strArray sizeInfo;
        AX_BREAK_IF(!splitWithForm(sizeStr, sizeInfo));

        float x      = (float)utils::atof(pointInfo[0].c_str());
        float y      = (float)utils::atof(pointInfo[1].c_str());
        float width  = (float)utils::atof(sizeInfo[0].c_str());
        float height = (float)utils::atof(sizeInfo[1].c_str());

        result = Rect(x, y, width, height);
    } while (0);

    return result;
}

Vec2 PointFromString(std::string_view str)
{
    Vec2 ret;

    do
    {
        strArray strs;
        AX_BREAK_IF(!splitWithForm(str, strs));

        float x = (float)utils::atof(strs[0].c_str());
        float y = (float)utils::atof(strs[1].c_str());

        ret.set(x, y);
    } while (0);

    return ret;
}

Vec2 SizeFromString(std::string_view pszContent)
{
    Vec2 ret = Vec2::ZERO;

    do
    {
        strArray strs;
        AX_BREAK_IF(!splitWithForm(pszContent, strs));

        float width  = (float)utils::atof(strs[0].c_str());
        float height = (float)utils::atof(strs[1].c_str());

        ret = Vec2(width, height);
    } while (0);

    return ret;
}

}
