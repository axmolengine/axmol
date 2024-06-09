/****************************************************************************
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

#ifndef __VISIBLERECT_H__
#define __VISIBLERECT_H__

#include "axmol.h"

class VisibleRect
{
public:
    static ax::Rect getVisibleRect();

    static ax::Vec2 left();
    static ax::Vec2 right();
    static ax::Vec2 top();
    static ax::Vec2 bottom();
    static ax::Vec2 center();
    static ax::Vec2 leftTop();
    static ax::Vec2 rightTop();
    static ax::Vec2 leftBottom();
    static ax::Vec2 rightBottom();

private:
    static void lazyInit();
    static ax::Rect s_visibleRect;
};

#endif /* __VISIBLERECT_H__ */
