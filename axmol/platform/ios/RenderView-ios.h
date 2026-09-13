/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/base/Object.h"
#include "axmol/platform/Common.h"
#include "axmol/platform/RenderViewCore.h"

namespace ax
{

/** Class that represent the RenderView
 */
class AX_DLL RenderView : public RenderViewCore
{
public:
    /** creates a RenderView with a title name in fullscreen mode */
    static RenderView* create(std::string_view viewName);

    /** creates a RenderView with a title name, a rect and the zoom factor */
    static RenderView* createWithRect(std::string_view viewName,
                                      const Rect& rect,
                                      float zoomFactor = 1.0f,
                                      bool resizable   = false);

    /** creates a RenderView with a name in fullscreen mode */
    static RenderView* createWithFullscreen(std::string_view viewName);

    static void choosePixelFormats();
    static PixelFormat _pixelFormat;
    static PixelFormat _depthFormat;
    static int _multisamplingCount;

    /** @since axmol-2.8.0, sets multi touch enabled */
    void setMultipleTouchEnabled(bool enabled);

    void showWindow(void* viewController);

    /** @since axmol-3.0, returns the objective-c UIWindow instance */
    void* getNativeWindow() const override { return _hostWindowHandle; }

    /** @since axmol-3.0, sets the objective-c UIWindow instance */
    void setNativeWindow(void* window) { _hostWindowHandle = window; }

    /** @since axmol-3.0, returns the objective-c EARenderView instance */
    SurfaceHandle getNativeDisplay() const override { return _hostViewHandle; }

    WindowPlatform getWindowPlatform() const override { return WindowPlatform::UIKit; }

    // overrides
    bool isGfxContextReady() override;
    void end() override;
    void swapBuffers() override;
    void setIMEKeyboardState(bool bOpen) override;

    Rect getSafeAreaRect() const override;

    void showContextMenu(const Vec2& point, bool hasText, bool hasSelection, bool readOnly) override;
    void hideContextMenu() override;

    void updateSurfaceMetrics(const Vec2& viewSize, float renderScale, bool shouldNotify);

protected:
    RenderView();
    ~RenderView() override;
    bool initWithRect(std::string_view viewName, const Rect& rect, float frameZoomFactor, bool resizable = false);
    bool initWithFullScreen(std::string_view viewName);

    // the objective-c instance handles
    void* _hostViewHandle;
    void* _hostWindowHandle;
};

}  // namespace ax
