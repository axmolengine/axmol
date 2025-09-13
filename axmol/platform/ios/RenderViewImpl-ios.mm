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
#import <UIKit/UIKit.h>

#include "axmol/platform/ios/EARenderView-ios.h"
#include "axmol/platform/ios/DirectorCaller-ios.h"
#include "axmol/platform/ios/RenderViewImpl-ios.h"
#include "axmol/rhi/DriverBase.h"
#include "axmol/base/Touch.h"
#include "axmol/base/Director.h"

namespace ax
{

PixelFormat RenderViewImpl::_pixelFormat = PixelFormat::RGB565;
PixelFormat RenderViewImpl::_depthFormat = PixelFormat::D24S8;
int RenderViewImpl::_multisamplingCount  = 0;

RenderViewImpl* RenderViewImpl::create(std::string_view viewName)
{
    auto ret = new RenderViewImpl;
    if (ret->initWithFullScreen(viewName))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

RenderViewImpl* RenderViewImpl::createWithRect(std::string_view viewName,
                                               const ax::Rect& rect,
                                               float frameZoomFactor,
                                               bool resizable)
{
    auto ret = new RenderViewImpl;
    if (ret->initWithRect(viewName, rect, frameZoomFactor, resizable))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

RenderViewImpl* RenderViewImpl::createWithFullscreen(std::string_view viewName)
{
    auto ret = new RenderViewImpl();
    if (ret->initWithFullScreen(viewName))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

void RenderViewImpl::choosePixelFormats()
{
    const auto& contextAttrs = rhi::DriverBase::getContextAttrs();

    if (contextAttrs.redBits == 8 && contextAttrs.greenBits == 8 && contextAttrs.blueBits == 8 &&
        contextAttrs.alphaBits == 8)
    {
        _pixelFormat = PixelFormat::RGBA8;
    }
    else if (contextAttrs.redBits == 5 && contextAttrs.greenBits == 6 && contextAttrs.blueBits == 5 &&
             contextAttrs.alphaBits == 0)
    {
        _pixelFormat = PixelFormat::RGB565;
    }
    else
    {
        AXASSERT(0, "Unsupported render buffer pixel format. Using default");
    }

    if (contextAttrs.depthBits == 24 && contextAttrs.stencilBits == 8)
    {
        _depthFormat = PixelFormat::D24S8;
    }
    else if (contextAttrs.depthBits == 0 && contextAttrs.stencilBits == 0)
    {
        _depthFormat = PixelFormat::NONE;
    }
    else
    {
        AXASSERT(0, "Unsupported format for depth and stencil buffers. Using default");
    }

    _multisamplingCount = contextAttrs.multisamplingCount;
}

RenderViewImpl::RenderViewImpl() {}

RenderViewImpl::~RenderViewImpl()
{
    // auto eaView = (__bridge EARenderView*) _eaViewHandle;
    //[eaView release];
}

bool RenderViewImpl::initWithRect(std::string_view /*viewName*/,
                                  const Rect& rect,
                                  float frameZoomFactor,
                                  bool /*resizable*/)
{
    CGRect r = CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
    choosePixelFormats();

    // create platform window
    _eaWindowHandle = [[UIWindow alloc] initWithFrame:r];

    // create platform render view
    EARenderView* eaView = [EARenderView viewWithFrame:r
                                           pixelFormat:(int)_pixelFormat
                                           depthFormat:(int)_depthFormat
                                    preserveBackbuffer:NO
                                            sharegroup:nil
                                         multiSampling:_multisamplingCount > 0 ? YES : NO
                                       numberOfSamples:_multisamplingCount];

    // Not available on tvOS
#if !defined(AX_TARGET_OS_TVOS)
    [eaView setMultipleTouchEnabled:YES];
#endif

    RenderView::setWindowSize([eaView getWidth], [eaView getHeight]);
    //    _viewScale.x = _viewScale.y = [eaView contentScaleFactor];

    _eaViewHandle = eaView;

    return true;
}

bool RenderViewImpl::initWithFullScreen(std::string_view viewName)
{
    CGRect rect = [[UIScreen mainScreen] bounds];
    Rect r;
    r.origin.x    = rect.origin.x;
    r.origin.y    = rect.origin.y;
    r.size.width  = rect.size.width;
    r.size.height = rect.size.height;

    return initWithRect(viewName, r, 1);
}

void RenderViewImpl::setMultipleTouchEnabled(bool enabled)
{
#if !defined(AX_TARGET_OS_TVOS)
    [(__bridge EARenderView*)_eaViewHandle setMultipleTouchEnabled:enabled];
#else
    AX_UNUSED_PARAM(enabled);
#endif
}

void RenderViewImpl::showWindow(void* viewController)
{
    auto window     = (__bridge UIWindow*)_eaWindowHandle;
    auto controller = (__bridge UIViewController*)viewController;

#if !defined(AX_TARGET_OS_TVOS)
    controller.extendedLayoutIncludesOpaqueBars = YES;
#endif
    auto view       = (__bridge EARenderView*)_eaViewHandle;
    controller.view = view;

    // Set RootViewController to window
    if ([[UIDevice currentDevice].systemVersion floatValue] < 6.0)
    {
        // warning: addSubView doesn't work on iOS6
        [window addSubview:controller.view];
    }
    else
    {
        // use this method on ios6
        [window setRootViewController:controller];
    }

    [window makeKeyAndVisible];

#if !defined(AX_TARGET_OS_TVOS)
    [controller prefersStatusBarHidden];
#endif

    // Launching the app with the arguments -NSAllowsDefaultLineBreakStrategy NO to force back to the old behavior.
    if ([[UIDevice currentDevice].systemVersion floatValue] >= 13.0f)
    {
        [[NSUserDefaults standardUserDefaults] setBool:NO forKey:@"NSAllowsDefaultLineBreakStrategy"];
    }
}

bool RenderViewImpl::isGfxContextReady()
{
    return _eaViewHandle != nullptr;
}

bool RenderViewImpl::setContentScaleFactor(float contentScaleFactor)
{
    AX_ASSERT(_resolutionPolicy == ResolutionPolicy::UNKNOWN);  // cannot enable retina mode
    _viewScale.x = _viewScale.y = contentScaleFactor;

    [(__bridge EARenderView*)_eaViewHandle setNeedsLayout];

    return true;
}

float RenderViewImpl::getContentScaleFactor() const
{
    return [(__bridge EARenderView*)_eaViewHandle contentScaleFactor];
}

void RenderViewImpl::end()
{
    [CCDirectorCaller destroy];

    [(__bridge EARenderView*)_eaViewHandle removeFromSuperview];
    release();
}

void RenderViewImpl::swapBuffers()
{
    [(__bridge EARenderView*)_eaViewHandle swapBuffers];
}

void RenderViewImpl::setIMEKeyboardState(bool open)
{
    auto eaView = (__bridge EARenderView*)_eaViewHandle;
    if (open)
    {
        [eaView showKeyboard];
    }
    else
    {
        [eaView hideKeyboard];
    }
}

Rect RenderViewImpl::getSafeAreaRect() const
{
    EARenderView* eaView = (__bridge EARenderView*)_eaViewHandle;

#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 110000
    float version = [[UIDevice currentDevice].systemVersion floatValue];
    if (version >= 11.0f)
    {
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wpartial-availability"
        UIEdgeInsets safeAreaInsets = eaView.safeAreaInsets;
#    pragma clang diagnostic pop

        // Multiply contentScaleFactor since safeAreaInsets return points.
        safeAreaInsets.left *= eaView.contentScaleFactor;
        safeAreaInsets.right *= eaView.contentScaleFactor;
        safeAreaInsets.top *= eaView.contentScaleFactor;
        safeAreaInsets.bottom *= eaView.contentScaleFactor;

        // Get leftBottom and rightTop point in UI coordinates
        Vec2 leftBottom = Vec2(safeAreaInsets.left, _windowSize.height - safeAreaInsets.bottom);
        Vec2 rightTop   = Vec2(_windowSize.width - safeAreaInsets.right, safeAreaInsets.top);

        // Convert a point from UI coordinates to which in design resolution coordinate.
        leftBottom.x = (leftBottom.x - _viewportRect.origin.x) / _viewScale.x,
        leftBottom.y = (leftBottom.y - _viewportRect.origin.y) / _viewScale.y;
        rightTop.x   = (rightTop.x - _viewportRect.origin.x) / _viewScale.x,
        rightTop.y   = (rightTop.y - _viewportRect.origin.y) / _viewScale.y;

        // Adjust points to make them inside design resolution
        leftBottom.x = MAX(leftBottom.x, 0);
        leftBottom.y = MIN(leftBottom.y, _designResolutionSize.height);
        rightTop.x   = MIN(rightTop.x, _designResolutionSize.width);
        rightTop.y   = MAX(rightTop.y, 0);

        // Convert to GL coordinates
        leftBottom = Director::getInstance()->screenToWorld(leftBottom);
        rightTop   = Director::getInstance()->screenToWorld(rightTop);

        return Rect(leftBottom.x, leftBottom.y, rightTop.x - leftBottom.x, rightTop.y - leftBottom.y);
    }
#endif

    // If running on iOS devices lower than 11.0, return visiable rect instead.
    return RenderView::getSafeAreaRect();
}

void RenderViewImpl::queueOperation(void (*op)(void*), void* param)
{
    [[NSOperationQueue mainQueue] addOperationWithBlock:^(void) {
      op(param);
    }];
}

}  // namespace ax
