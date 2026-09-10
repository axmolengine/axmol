/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#pragma once

#include "axmol/base/Object.h"
#include "axmol/math/Math.h"
#include "axmol/platform/RenderViewCore.h"

namespace ax
{

class AX_DLL RenderView : public RenderViewCore
{
public:
    // static function
    static RenderView* create(std::string_view viewname);
    static RenderView* createWithRect(std::string_view viewName,
                                      const Rect& rect,
                                      float zoomFactor = 1.0f,
                                      bool resizable   = false);
    static RenderView* createWithFullscreen(std::string_view viewName);

    bool isGfxContextReady() override;
    void end() override;
    void swapBuffers() override;
    void setIMEKeyboardState(bool bOpen) override;
    Rect getSafeAreaRect() const override;

    WindowPlatform getWindowPlatform() const override { return WindowPlatform::Android; }

    void* getNativeWindow() const override;
    SurfaceHandle getNativeDisplay() const override;

    [[internal]] void recreateVkSurface(bool needUpdateRenderSurface);

protected:
    RenderView();
    virtual ~RenderView();

    bool initWithRect(std::string_view viewName, const Rect& rect, float zoomFactor, bool resizable = false);
    bool initWithFullScreen(std::string_view viewName);

    void showContextMenu(const Vec2& point, bool hasText, bool hasSelection, bool readOnly) override;
    void hideContextMenu() override;

    void* _nativeWindow{nullptr};
    void* _nativeDisplay{nullptr};
};

}  // namespace ax
