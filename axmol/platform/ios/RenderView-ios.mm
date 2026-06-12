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
#include "axmol/platform/ios/RenderView-ios.h"
#include "axmol/platform/ios/AxmolViewController.h"
#include "axmol/platform/Application.h"
#include "axmol/platform/Device.h"
#include "axmol/base/PointerEvent.h"
#include "axmol/base/Director.h"
#include "axmol/base/InputSystem.h"

namespace ax
{

PixelFormat RenderView::_pixelFormat = PixelFormat::RGB565;
PixelFormat RenderView::_depthFormat = PixelFormat::D24S8;
int RenderView::_multisamplingCount  = 0;

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

RenderView* RenderView::create(std::string_view viewName)
{
    auto ret = new RenderView;
    if (ret->initWithFullScreen(viewName))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

RenderView* RenderView::createWithRect(std::string_view viewName,
                                       const ax::Rect& rect,
                                       float frameZoomFactor,
                                       bool resizable)
{
    auto ret = new RenderView;
    if (ret->initWithRect(viewName, rect, frameZoomFactor, resizable))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

RenderView* RenderView::createWithFullscreen(std::string_view viewName)
{
    auto ret = new RenderView();
    if (ret->initWithFullScreen(viewName))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

void RenderView::choosePixelFormats()
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

RenderView::RenderView() {}

RenderView::~RenderView() {}

bool RenderView::initWithRect(std::string_view /*viewName*/,
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

bool RenderView::initWithFullScreen(std::string_view viewName)
{
    CGRect rect = [[UIScreen mainScreen] bounds];
    Rect r;
    r.origin.x    = rect.origin.x;
    r.origin.y    = rect.origin.y;
    r.size.width  = rect.size.width;
    r.size.height = rect.size.height;

    return initWithRect(viewName, r, 1);
}

void RenderView::setMultipleTouchEnabled(bool enabled)
{
#if !defined(AX_TARGET_OS_TVOS)
    [(__bridge RenderHostView*)_hostViewHandle setMultipleTouchEnabled:enabled];
#else
    AX_UNUSED_PARAM(enabled);
#endif
}

void RenderView::showWindow(void* viewController)
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

    const auto size = resolveViewSizeToOrientation([hostView bounds].size);

    updateSurfaceMetrics(Vec2{static_cast<float>(size.width), static_cast<float>(size.height)},
                         [hostView contentScaleFactor], false);

#if !defined(AX_TARGET_OS_TVOS)
    [controller prefersStatusBarHidden];
#endif

    // Launching the app with the arguments -NSAllowsDefaultLineBreakStrategy NO to force back to the old behavior.
    if ([[UIDevice currentDevice].systemVersion floatValue] >= 13.0f)
    {
        [[NSUserDefaults standardUserDefaults] setBool:NO forKey:@"NSAllowsDefaultLineBreakStrategy"];
    }
}

void RenderView::updateSurfaceMetrics(const Vec2& viewSize, float renderScale, bool shouldNotify)
{
    _renderScale = renderScale;

    // Inform InputSystem about the platform input scale so it can apply
    // coordinate scaling centrally when dispatching input events.
    InputSystem::getInstance()->setInputScale(renderScale);

    updateRenderSurface(viewSize.width, viewSize.height, SurfaceUpdateFlag::WindowSizeChanged);

    auto flags = shouldNotify ? SurfaceUpdateFlag::RenderSizeChanged
                              : (SurfaceUpdateFlag::RenderSizeChanged | SurfaceUpdateFlag::SilentUpdate);
    updateRenderSurface(viewSize.width * _renderScale, viewSize.height * _renderScale, flags);
}

bool RenderView::isGfxContextReady()
{
    return _hostViewHandle != nullptr;
}

void RenderView::end()
{
    [CCDirectorCaller destroy];

    [(__bridge RenderHostView*)_hostViewHandle removeFromSuperview];
    release();
}

void RenderView::swapBuffers()
{
    [(__bridge RenderHostView*)_hostViewHandle swapBuffers];
}

void RenderView::setIMEKeyboardState(bool open)
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

Rect RenderView::getSafeAreaRect() const
{
    RenderHostView* hostView = (__bridge RenderHostView*)_hostViewHandle;

#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 110000
    if (@available(iOS 11.0, *))
    {
        UIEdgeInsets insets = hostView.safeAreaInsets;
        CGRect bounds       = hostView.bounds;

        // 1. Industrial-grade Orientation Fix: Forcefully project mismatched
        // vertical padding to the horizontal X-axis under strict landscape mode.
        bool isLandscape = bounds.size.width > bounds.size.height;
        float maxPadding = std::max({insets.left, insets.right, insets.top, insets.bottom});

        float nativeMinX = (isLandscape && maxPadding > 0.0f) ? maxPadding : insets.left;
        float nativeMinY = isLandscape ? 0.0f : insets.top;
        float nativeMaxX = bounds.size.width - ((isLandscape && maxPadding > 0.0f) ? maxPadding : insets.right);
        float nativeMaxY = bounds.size.height - (isLandscape ? 0.0f : insets.bottom);

        // 2. Map native logical boundaries directly to engine standard screen pixels
        auto* inputSys       = ax::InputSystem::getInstance();
        ax::Vec2 leftTop     = inputSys->nativeToScreen(ax::Vec2(nativeMinX, nativeMinY));
        ax::Vec2 rightBottom = inputSys->nativeToScreen(ax::Vec2(nativeMaxX, nativeMaxY));

        // 3. Convert standard screen pixels to standard Engine Target space via Director
        auto* director = ax::Director::getInstance();

        leftTop     = director->screenToWorld(leftTop);
        rightBottom = director->screenToWorld(rightBottom);

        // 4. Adjust points to make them inside design resolution
        float minX = std::max(leftTop.x, 0.0f);
        float minY = std::max(rightBottom.y, 0.0f);
        float maxX = std::min(rightBottom.x, _designResolutionSize.width);
        float maxY = std::min(leftTop.y, _designResolutionSize.height);

        auto safeArea = ax::Rect(minX, minY, maxX - minX, maxY - minY);

        AXLOGD("ios safe area: origin=({},{}), size=({},{})", safeArea.origin.x, safeArea.origin.y, safeArea.size.width,
               safeArea.size.height);
        return safeArea;
    }
#endif

    return RenderView::getSafeAreaRect();
}

void RenderView::showContextMenu(const Vec2& point, bool hasText, bool hasSelection, bool readOnly)
{
    auto position = InputSystem::getInstance()->screenToNative(point);

    // Implementation for showing system edit menu at the specified point
    RenderHostView* hostView = (__bridge RenderHostView*)_hostViewHandle;
    [hostView showContextMenu:CGPointMake(position.x, position.y)
                      hasText:hasText
                 hasSelection:hasSelection
                     readOnly:readOnly];
}

void RenderView::hideContextMenu()
{
    // Implementation for hiding system edit menu
    RenderHostView* hostView = (__bridge RenderHostView*)_hostViewHandle;
    [hostView hideContextMenu];
}

}  // namespace ax
