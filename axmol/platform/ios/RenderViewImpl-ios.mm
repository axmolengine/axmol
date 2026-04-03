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

#include "axmol/platform/ios/RenderHostView-ios.h"
#include "axmol/platform/ios/DirectorCaller-ios.h"
#include "axmol/platform/ios/RenderViewImpl-ios.h"
#include "axmol/platform/ios/AxmolViewController.h"
#include "axmol/platform/Application.h"
#include "axmol/platform/Device.h"
#include "axmol/base/Touch.h"
#include "axmol/base/Director.h"

namespace ax
{

PixelFormat RenderViewImpl::_pixelFormat = PixelFormat::RGB565;
PixelFormat RenderViewImpl::_depthFormat = PixelFormat::D24S8;
int RenderViewImpl::_multisamplingCount  = 0;

/**
 * Adjusts a UIView's size to match the resolved device orientation.
 *
 * UIKit reports UIView sizes based on the system's initial orientation assumptions.
 * At app launch, even if the device is physically in landscape, iOS may still report
 * the view size in portrait layout-especially when Info.plist declares support for
 * both portrait and landscape orientations.
 *
 * This function corrects that mismatch by comparing the aspect ratio of the input
 * UIView size with the resolved device orientation. If the orientation and size basis
 * disagree, it swaps width and height to produce a logical size that matches the
 * actual screen orientation.
 *
 * This is especially useful before layoutSubviews is triggered, when view.bounds
 * may not yet reflect the final orientation.
 *
 * @param viewSize The raw size of a UIView, typically from view.bounds or view.frame
 * @return A CGSize representing the logical screen size in the resolved orientation
 */
static CGSize resolveViewSizeToOrientation(CGSize viewSize)
{
    auto resolvedOrientation = Device::resolveOrientation();

    bool isLandscapeSize   = viewSize.width > viewSize.height;
    bool shouldBeLandscape = (resolvedOrientation == Device::Orientation::Landscape ||
                              resolvedOrientation == Device::Orientation::ReverseLandscape);

    if ((shouldBeLandscape && !isLandscapeSize) || (!shouldBeLandscape && isLandscapeSize))
        std::swap(viewSize.width, viewSize.height);

    return viewSize;
}

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
    const auto& contextAttrs = Application::getContextAttrs();

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

RenderViewImpl::~RenderViewImpl() {}

bool RenderViewImpl::initWithRect(std::string_view /*viewName*/,
                                  const Rect& rect,
                                  float frameZoomFactor,
                                  bool /*resizable*/)
{
    CGRect r = CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
    choosePixelFormats();

    // create platform window
    _hostWindowHandle = [[UIWindow alloc] initWithFrame:r];

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
    [(__bridge RenderHostView*)_hostViewHandle setMultipleTouchEnabled:enabled];
#else
    AX_UNUSED_PARAM(enabled);
#endif
}

void RenderViewImpl::showWindow(void* viewController)
{
    auto window     = (__bridge UIWindow*)_hostWindowHandle;
    auto controller = (__bridge AxmolViewController*)viewController;

#if !defined(AX_TARGET_OS_TVOS)
    controller.extendedLayoutIncludesOpaqueBars = YES;
#endif

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

    // Calling makeKeyAndVisible triggers the AxmolViewController lifecycle:
    // loadView -> viewDidLoad -> viewWillAppear
    [window makeKeyAndVisible];

    // After lifecycle completes, controller.view is initialized with RenderHostView
    auto hostView   = controller.view;
    _hostViewHandle = controller.view;

    const auto size               = resolveViewSizeToOrientation([hostView bounds].size);
    const auto backingScaleFactor = [hostView contentScaleFactor];

    // simply set renderSize, renderSize to framebufferSize with renderScale=1.0
    updateRenderSurface(size.width * backingScaleFactor, size.height * backingScaleFactor,
                        SurfaceUpdateFlag::AllUpdatesSilently);

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
    return _hostViewHandle != nullptr;
}

float RenderViewImpl::getRenderScale() const
{
    return [(__bridge RenderHostView*)_hostViewHandle contentScaleFactor];
}

void RenderViewImpl::end()
{
    [CCDirectorCaller destroy];

    [(__bridge RenderHostView*)_hostViewHandle removeFromSuperview];
    release();
}

void RenderViewImpl::swapBuffers()
{
    [(__bridge RenderHostView*)_hostViewHandle swapBuffers];
}

void RenderViewImpl::setIMEKeyboardState(bool open)
{
    auto hostView = (__bridge RenderHostView*)_hostViewHandle;
    if (open)
    {
        [hostView showKeyboard];
    }
    else
    {
        [hostView hideKeyboard];
    }
}

Rect RenderViewImpl::getSafeAreaRect() const
{
    RenderHostView* hostView = (__bridge RenderHostView*)_hostViewHandle;

#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 110000
    float version = [[UIDevice currentDevice].systemVersion floatValue];
    if (version >= 11.0f)
    {
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wpartial-availability"
        UIEdgeInsets safeAreaInsets = hostView.safeAreaInsets;
#    pragma clang diagnostic pop

        // Multiply contentScaleFactor since safeAreaInsets return points.
        safeAreaInsets.left *= hostView.contentScaleFactor;
        safeAreaInsets.right *= hostView.contentScaleFactor;
        safeAreaInsets.top *= hostView.contentScaleFactor;
        safeAreaInsets.bottom *= hostView.contentScaleFactor;

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
