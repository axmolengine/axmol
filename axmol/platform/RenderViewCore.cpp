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

#include "axmol/platform/RenderViewCore.h"

#include "axmol/base/PointerEvent.h"
#include "axmol/base/Director.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/base/InputSystem.h"
#include "axmol/scene/Camera.h"
#include "axmol/renderer/Renderer.h"

namespace ax
{

const std::string_view RenderViewCore::EVENT_WINDOW_POSITIONED   = "_ax_window_positioned"sv;
const std::string_view RenderViewCore::EVENT_WINDOW_RESIZED      = "_ax_window_resized"sv;
const std::string_view RenderViewCore::EVENT_WINDOW_FOCUSED      = "_ax_window_focused"sv;
const std::string_view RenderViewCore::EVENT_WINDOW_UNFOCUSED    = "_ax_window_unfocused"sv;
const std::string_view RenderViewCore::EVENT_WINDOW_CLOSE        = "_ax_window_close"sv;
const std::string_view RenderViewCore::EVENT_WINDOW_CURSOR_ENTER = "_ax_window_cursor_enter"sv;

RenderViewCore::RenderViewCore()
    : _windowSize(0, 0)
    , _designResolutionSize(0, 0)
    , _viewScale(Vec2::one)
    , _resolutionPolicy(ResolutionPolicy::UNKNOWN)
{}

RenderViewCore::~RenderViewCore() {}

void RenderViewCore::pollEvents() {}

void RenderViewCore::updateDesignResolution()
{
    if (_renderSize.width > 0 && _renderSize.height > 0 && _designResolutionSize.width > 0 &&
        _designResolutionSize.height > 0)
    {
        _viewScale = _renderSize / _designResolutionSize;

        if (_resolutionPolicy == ResolutionPolicy::NO_BORDER)
        {
            _viewScale.x = _viewScale.y = (std::max)(_viewScale.x, _viewScale.y);
        }

        else if (_resolutionPolicy == ResolutionPolicy::SHOW_ALL)
        {
            _viewScale.x = _viewScale.y = (std::min)(_viewScale.x, _viewScale.y);
        }

        else if (_resolutionPolicy == ResolutionPolicy::FIXED_HEIGHT)
        {
            _viewScale.x                = _viewScale.y;
            _designResolutionSize.width = ceilf(_renderSize.width / _viewScale.x);
        }

        else if (_resolutionPolicy == ResolutionPolicy::FIXED_WIDTH)
        {
            _viewScale.y                 = _viewScale.x;
            _designResolutionSize.height = ceilf(_renderSize.height / _viewScale.y);
        }

        // calculate the rect of viewport
        float viewportW = _designResolutionSize.width * _viewScale.x;
        float viewportH = _designResolutionSize.height * _viewScale.y;

        _viewportRect.setRect((_renderSize.width - viewportW) / 2, (_renderSize.height - viewportH) / 2, viewportW,
                              viewportH);

        // reset director's member variables to fit visible rect
        auto director = Director::getInstance();
        director->setCanvasSize(getDesignResolutionSize());
        director->setProjection(director->getProjection());
    }
}

void RenderViewCore::setDesignResolutionSize(float width, float height, ResolutionPolicy resolutionPolicy)
{
    AXASSERT(resolutionPolicy != ResolutionPolicy::UNKNOWN, "should set resolutionPolicy");

    if (width == 0.0f || height == 0.0f)
    {
        return;
    }

    _designResolutionSize.set(width, height);
    _resolutionPolicy = resolutionPolicy;

    if (!_isResolutionUpdateLocked)
        updateDesignResolution();
}

const Vec2& RenderViewCore::getDesignResolutionSize() const
{
    return _designResolutionSize;
}

void RenderViewCore::updateRenderSurface(float width, float height, uint8_t updateFlag)
{
    Vec2 value{width, height};

    if (updateFlag & SurfaceUpdateFlag::WindowSizeChanged)
        _windowSize = value;

    if (updateFlag & SurfaceUpdateFlag::RenderSizeChanged)
    {
        _isResolutionUpdateLocked = true;

        _renderSize = value;

        // If designResolutionSize hasn't been set, default to renderSize
        if (_designResolutionSize.equals(Vec2::zero))
            _designResolutionSize = value;

        // Notify the application that the screen size has changed.
        // This gives the user a chance to re-layout scene content or reset designResolutionSize if needed.
        if (!(updateFlag & SurfaceUpdateFlag::SilentUpdate))
            ax::Application::getInstance()->applicationScreenSizeChanged(width, height);

        // then we update resolution and viewport
        updateDesignResolution();

        _isResolutionUpdateLocked = false;
    }

    // check does all updateed
    maybeDispatchResizeEvent(updateFlag);
}

void RenderViewCore::maybeDispatchResizeEvent(uint8_t updateFlag)
{
    const bool silentUpdate = (updateFlag & SurfaceUpdateFlag::SilentUpdate) != 0;
    updateFlag &= ~SurfaceUpdateFlag::SilentUpdate;  // Remove temporary flag

    _surfaceUpdateFlags |= updateFlag;

    constexpr uint8_t requiredFlags = SurfaceUpdateFlag::WindowSizeChanged | SurfaceUpdateFlag::RenderSizeChanged;

    const bool readyToDispatch = (_surfaceUpdateFlags == requiredFlags);

    if (readyToDispatch)
    {
        _surfaceUpdateFlags = 0;
        if (!silentUpdate)
            onSurfaceResized();
    }
}

Rect RenderViewCore::getVisibleRect() const
{
    Rect ret;
    ret.size   = getVisibleSize();
    ret.origin = getVisibleOrigin();
    return ret;
}

Rect RenderViewCore::getSafeAreaRect() const
{
    return getVisibleRect();
}

Vec2 RenderViewCore::getVisibleSize() const
{
    if (_resolutionPolicy == ResolutionPolicy::NO_BORDER)
    {
        return Vec2(_renderSize.width / _viewScale.x, _renderSize.height / _viewScale.y);
    }
    else
    {
        return _designResolutionSize;
    }
}

Vec2 RenderViewCore::getVisibleOrigin() const
{
    if (_resolutionPolicy == ResolutionPolicy::NO_BORDER)
    {
        return Vec2((_designResolutionSize.width - _renderSize.width / _viewScale.x) / 2,
                    (_designResolutionSize.height - _renderSize.height / _viewScale.y) / 2);
    }
    else
    {
        return Vec2::zero;
    }
}

void RenderViewCore::setViewportInPoints(float x, float y, float w, float h)
{
    Viewport vp;
    vp.x      = (int)(x * _viewScale.x + _viewportRect.origin.x);
    vp.y      = (int)(y * _viewScale.y + _viewportRect.origin.y);
    vp.width  = (unsigned int)(w * _viewScale.x);
    vp.height = (unsigned int)(h * _viewScale.y);
    Camera::setDefaultViewport(vp);
}

void RenderViewCore::setScissorInPoints(float x, float y, float w, float h)
{
    setScissorRect((int)(x * _viewScale.x + _viewportRect.origin.x), (int)(y * _viewScale.y + _viewportRect.origin.y),
                   (unsigned int)(w * _viewScale.x), (unsigned int)(h * _viewScale.y));
}

Rect RenderViewCore::getScissorInPoints() const
{
    auto& rect = getScissorRect();

    float x = (rect.x - _viewportRect.origin.x) / _viewScale.x;
    float y = (rect.y - _viewportRect.origin.y) / _viewScale.y;
    float w = rect.width / _viewScale.x;
    float h = rect.height / _viewScale.y;
    return Rect(x, y, w, h);
}

bool RenderViewCore::isScissorEnabled()
{
    auto renderer = Director::getInstance()->getRenderer();
    return renderer->getScissorTest();
}

void RenderViewCore::setViewName(std::string_view viewname)
{
    _viewName = viewname;
}

std::string_view RenderViewCore::getViewName() const
{
    return _viewName;
}

const Rect& RenderViewCore::getViewportRect() const
{
    return _viewportRect;
}

void RenderViewCore::onSurfaceResized()
{
    int screenWidth  = static_cast<uint32_t>(_renderSize.width);
    int screenHeight = static_cast<uint32_t>(_renderSize.height);

    AXLOGD("RenderViewCore::onSurfaceResized: ({}x{})", screenWidth, screenHeight);
    auto director = Director::getInstance();
    auto renderer = director->getRenderer();
    if (renderer)
        renderer->updateSurface(getNativeDisplay(), screenWidth, screenHeight);
    director->getSceneRenderer()->onRenderViewChanged(this);
}

void RenderViewCore::setScissorRect(float x, float y, float w, float h)
{
    Director::getInstance()->getSceneRenderer()->setScissorRect(x, y, w, h);
}

const ScissorRect& RenderViewCore::getScissorRect() const
{
    return Director::getInstance()->getSceneRenderer()->getScissorRect();
}

}  // namespace ax
