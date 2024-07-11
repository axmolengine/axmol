/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
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
#include "platform/android/GLViewImpl-android.h"
#include "base/Director.h"
#include "base/Macros.h"
#include "platform/android/jni/JniHelper.h"
#include "platform/GL.h"

#include <stdlib.h>
#include <android/log.h>

#define DEFAULT_MARGIN_ANDROID 30.0f
#define WIDE_SCREEN_ASPECT_RATIO_ANDROID 2.0f


NS_AX_BEGIN
void GLViewImpl::loadGLES2()
{
    auto glesVer = gladLoaderLoadGLES2();
    if (glesVer)
        AXLOGI("Load GLES success, version: {}", glesVer);
    else
        throw std::runtime_error("Load GLES fail");
}

GLViewImpl* GLViewImpl::createWithRect(std::string_view viewName, const Rect& rect, float frameZoomFactor, bool resizable)
{
    auto ret = new GLViewImpl;
    if (ret && ret->initWithRect(viewName, rect, frameZoomFactor, resizable))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

GLViewImpl* GLViewImpl::create(std::string_view viewName)
{
    auto ret = new GLViewImpl;
    if (ret && ret->initWithFullScreen(viewName))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

GLViewImpl* GLViewImpl::createWithFullScreen(std::string_view viewName)
{
    auto ret = new GLViewImpl();
    if (ret && ret->initWithFullScreen(viewName))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

GLViewImpl::GLViewImpl()
{
}

GLViewImpl::~GLViewImpl() {}

bool GLViewImpl::initWithRect(std::string_view /*viewName*/, const Rect& /*rect*/, float /*frameZoomFactor*/, bool /*resizable*/)
{
    return true;
}

bool GLViewImpl::initWithFullScreen(std::string_view viewName)
{
    return true;
}

bool GLViewImpl::isOpenGLReady()
{
    return (_screenSize.width != 0 && _screenSize.height != 0);
}

void GLViewImpl::end()
{
    JniHelper::callStaticVoidMethod("org.axmol.lib.AxmolEngine", "onExit");
    release();
}

void GLViewImpl::swapBuffers() {}

void GLViewImpl::setIMEKeyboardState(bool bOpen)
{
    if (bOpen)
    {
        JniHelper::callStaticVoidMethod("org.axmol.lib.AxmolGLSurfaceView", "openIMEKeyboard");
    }
    else
    {
        JniHelper::callStaticVoidMethod("org.axmol.lib.AxmolGLSurfaceView", "closeIMEKeyboard");
    }
}

Rect GLViewImpl::getSafeAreaRect() const
{
    Rect safeAreaRect       = GLView::getSafeAreaRect();
    float deviceAspectRatio = 0;
    if (safeAreaRect.size.height > safeAreaRect.size.width)
    {
        deviceAspectRatio = safeAreaRect.size.height / safeAreaRect.size.width;
    }
    else
    {
        deviceAspectRatio = safeAreaRect.size.width / safeAreaRect.size.height;
    }

    float marginX = DEFAULT_MARGIN_ANDROID / _scaleX;
    float marginY = DEFAULT_MARGIN_ANDROID / _scaleY;

    bool isScreenRound   = JniHelper::callStaticBooleanMethod("org/axmol/lib/AxmolEngine", "isScreenRound");
    bool hasSoftKeys     = JniHelper::callStaticBooleanMethod("org/axmol/lib/AxmolEngine", "hasSoftKeys");
    bool isCutoutEnabled = JniHelper::callStaticBooleanMethod("org/axmol/lib/AxmolEngine", "isCutoutEnabled");

    float insetTop = 0.0f;
    float insetBottom = 0.0f;
    float insetLeft = 0.0f;
    float insetRight = 0.0f;

    static int* cornerRadii =
            JniHelper::callStaticIntArrayMethod("org/axmol/lib/AxmolEngine", "getDeviceCornerRadii");

    if (isScreenRound)
    {
        // edge screen (ex. Samsung Galaxy s7, s9, s9+, Note 9, Nokia 8 Sirocco, Sony Xperia XZ3, Oppo Find X...)
        if (safeAreaRect.size.width < safeAreaRect.size.height)
        {
            safeAreaRect.origin.y += marginY * 2.f;
            safeAreaRect.size.height -= (marginY * 2.f);

            safeAreaRect.origin.x += marginX;
            safeAreaRect.size.width -= (marginX * 2.f);
        }
        else
        {
            safeAreaRect.origin.y += marginY;
            safeAreaRect.size.height -= (marginY * 2.f);

            // landscape: no changes with X-coords
        }
    }
    else if (deviceAspectRatio >= WIDE_SCREEN_ASPECT_RATIO_ANDROID || cornerRadii != nullptr)
    {
        // almost all devices on the market have round corners if
        // deviceAspectRatio more than 2 (@see "android.max_aspect" parameter in AndroidManifest.xml)

        // cornerRadii is only available in API31+ (Android 12+)
        if (cornerRadii != nullptr)
        {
            float radiiBottom = cornerRadii[0] / _scaleY;
            float radiiLeft   = cornerRadii[1] / _scaleX;
            float radiiRight  = cornerRadii[2] / _scaleX;
            float radiiTop    = cornerRadii[3] / _scaleY;

            if (safeAreaRect.size.width < safeAreaRect.size.height)
            {
                if (hasSoftKeys)
                {
                    safeAreaRect.origin.y += marginY;
                    safeAreaRect.size.height -= (marginY * 2);
                }

                // portrait
                insetTop = radiiTop;
                insetBottom = radiiBottom;
            }
            else
            {
                // landscape
                insetLeft = radiiLeft;
                insetRight = radiiRight;
            }
        }
        else
        {
            float bottomMarginIfPortrait = 0;
            if (hasSoftKeys)
            {
                bottomMarginIfPortrait = marginY * 2.f;
            }

            if (safeAreaRect.size.width < safeAreaRect.size.height)
            {
                // portrait: double margin space if device has soft menu
                safeAreaRect.origin.y += bottomMarginIfPortrait;
                safeAreaRect.size.height -= (bottomMarginIfPortrait + marginY);
            }
            else
            {
                // landscape: ignore double margin at the bottom in any cases
                // prepare single margin for round corners
                safeAreaRect.origin.y += marginY;
                safeAreaRect.size.height -= (marginY * 2.f);
            }
        }
    }
    else
    {
        if (hasSoftKeys && (safeAreaRect.size.width < safeAreaRect.size.height))
        {
            // portrait: preserve only for soft system menu
            safeAreaRect.origin.y += marginY * 2.f;
            safeAreaRect.size.height -= (marginY * 2.f);
        }
    }

    if (isCutoutEnabled)
    {
        // screen with enabled cutout area (ex. Google Pixel 3 XL, Huawei P20, Asus ZenFone 5, etc)
        static int* safeInsets =
            JniHelper::callStaticIntArrayMethod("org/axmol/lib/AxmolEngine", "getSafeInsets");
        if (safeInsets != nullptr)
        {
            float safeInsetBottom = safeInsets[0] / _scaleY;
            float safeInsetLeft   = safeInsets[1] / _scaleX;
            float safeInsetRight  = safeInsets[2] / _scaleX;
            float safeInsetTop    = safeInsets[3] / _scaleY;

            // fit safe area rect with safe insets
            auto maxInsetBottom = std::max(safeInsetBottom, insetBottom);
            safeAreaRect.origin.y += maxInsetBottom;
            safeAreaRect.size.height -= maxInsetBottom;

            auto maxInsetLeft = std::max(safeInsetLeft, insetLeft);
            safeAreaRect.origin.x += maxInsetLeft;
            safeAreaRect.size.width -= maxInsetLeft;

            safeAreaRect.size.width -= std::max(safeInsetRight, insetRight);
            safeAreaRect.size.height -= std::max(safeInsetTop, insetTop);
        }
    }

    return safeAreaRect;
}

void GLViewImpl::queueOperation(void (*op)(void*), void* param)
{
    JniHelper::callStaticVoidMethod("org.axmol.lib.AxmolEngine", "queueOperation", (jlong)(uintptr_t)op,
                                    (jlong)(uintptr_t)param);
}

NS_AX_END
