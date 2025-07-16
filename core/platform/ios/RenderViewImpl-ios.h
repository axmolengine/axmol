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
#include "platform/Common.h"
#include "platform/RenderView.h"

namespace ax
{

/** Class that represent the OpenGL View
 */
class AX_DLL RenderViewImpl : public RenderView
{
public:

#ifndef AX_CORE_PROFILE
    /** DEPRECATED creates a RenderViewImpl with a objective-c EARenderViewImpl instance */
    AX_DEPRECATED(2.8) static RenderViewImpl* createWithEARenderView(void* viewHandle);
#endif

    /** creates a RenderViewImpl with a title name in fullscreen mode */
    static RenderViewImpl* create(std::string_view viewName);

    /** creates a RenderViewImpl with a title name, a rect and the zoom factor */
    static RenderViewImpl* createWithRect(std::string_view viewName,
                                      const Rect& rect,
                                      float frameZoomFactor = 1.0f,
                                      bool resizable        = false);

    /** creates a RenderViewImpl with a name in fullscreen mode */
    static RenderViewImpl* createWithFullScreen(std::string_view viewName);
#ifndef AX_CORE_PROFILE
    AX_DEPRECATED(2.8) static void convertAttrs() { choosePixelFormats(); }
#endif
    static void choosePixelFormats();
    static PixelFormat _pixelFormat;
    static PixelFormat _depthFormat;
    static int _multisamplingCount;
    
    /** @since axmol-2.8.0, sets multi touch enabled */
    void setMultipleTouchEnabled(bool enabled);
    
    void showWindow(void* viewController);

    /** sets the content scale factor */
    bool setContentScaleFactor(float contentScaleFactor) override;

    /** returns the content scale factor */
    float getContentScaleFactor() const override;

    /** returns whether or not the view is in Retina Display mode */
    bool isRetinaDisplay() const override { return getContentScaleFactor() == 2.0; }

    /** @since axmol-2.8.0, returns the objective-c UIWindow instance */
    void* getEAWindow() const override { return _eaWindowHandle; }

    /** @since axmol-2.8.0, returns the objective-c EARenderView instance */
    void* getEARenderView() const override { return _eaViewHandle; }

    // overrides
    bool isGfxContextReady() override;
    void end() override;
    void swapBuffers() override;
    void setIMEKeyboardState(bool bOpen) override;

    Rect getSafeAreaRect() const override;

    void queueOperation(void (*op)(void*), void* param) override;

protected:
    RenderViewImpl();
    ~RenderViewImpl() override;
#ifndef AX_CORE_PROFILE
    AX_DEPRECATED(2.8) bool initWithEARenderView(void* viewHandle);
#endif
    bool initWithRect(std::string_view viewName, const Rect& rect, float frameZoomFactor, bool resizable = false);
    bool initWithFullScreen(std::string_view viewName);

    // the objective-c instance handles
    void* _eaViewHandle;
    void* _eaWindowHandle;
};

#ifndef AX_CORE_PROFILE
AX_DEPRECATED(2.8) typedef RenderViewImpl GLViewImpl;
#endif

}
