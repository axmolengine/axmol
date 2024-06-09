/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
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
#pragma once

#include "base/Object.h"
#include "math/Math.h"
#include "platform/GLView.h"

NS_AX_BEGIN

class AX_DLL GLViewImpl : public GLView
{
public:
    // static function
    static void loadGLES2();
    static GLViewImpl* create(std::string_view viewname);
    static GLViewImpl* createWithRect(std::string_view viewName, const Rect& rect, float frameZoomFactor = 1.0f, bool resizable = false);
    static GLViewImpl* createWithFullScreen(std::string_view viewName);

    bool isOpenGLReady() override;
    void end() override;
    void swapBuffers() override;
    void setIMEKeyboardState(bool bOpen) override;
    virtual Rect getSafeAreaRect() const override;

    void queueOperation(void (*op)(void*), void* param) override;

protected:
    GLViewImpl();
    virtual ~GLViewImpl();

    bool initWithRect(std::string_view viewName, const Rect& rect, float frameZoomFactor, bool resizable = false);
    bool initWithFullScreen(std::string_view viewName);
};

NS_AX_END
