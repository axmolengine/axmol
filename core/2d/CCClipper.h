/****************************************************************************
 Copyright (c) 2013      Zynga Inc.
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2021 Bytedance Inc.

 https://axys1.github.io/

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

#ifndef _CCClipper_h_
#define _CCClipper_h_

/// @cond DO_NOT_SHOW

//#include <string>
//#include "base/ccTypes.h"
//#include "base/ccUTF8.h"

#include "clipper.h"

NS_AX_BEGIN

class AX_DLL Clipper : public Node
{
public:

    inline Clipper2Lib::Path64 MakeStar(const Clipper2Lib::Point64& center, int radius, int points);
    inline Clipper2Lib::Path64 MakeRandomPoly(int width, int height, unsigned vertCnt);


    Clipper2Lib::Paths64  getPaths64(Clipper2Lib::Paths64 subject, Clipper2Lib::Paths64 clip, int Type, Clipper2Lib::FillRule);


    Clipper2Lib::Paths64 subject, clip, solution;
    const std::vector<Vec2> _subject;
    const std::vector<Vec2> _clip;
    const std::vector<Vec2> _solution;
};

NS_AX_END

/// @endcond
#endif
