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

#include "axmol/platform/RenderView.h"

#include "axmol/base/Touch.h"
#include "axmol/base/Director.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/2d/Camera.h"
#include "axmol/2d/Scene.h"
#include "axmol/renderer/Renderer.h"

namespace ax
{

namespace
{

static Touch* g_touches[EventTouch::MAX_TOUCHES] = {nullptr};
static unsigned int g_indexBitsUsed              = 0;
// System touch pointer ID (It may not be ascending order number) <-> Ascending order number from 0
static std::map<intptr_t, int> g_touchIdReorderMap;

static int getUnUsedIndex()
{
    int i;
    int temp = g_indexBitsUsed;

    for (i = 0; i < EventTouch::MAX_TOUCHES; i++)
    {
        if (!(temp & 0x00000001))
        {
            g_indexBitsUsed |= (1 << i);
            return i;
        }

        temp >>= 1;
    }

    // all bits are used
    return -1;
}

static std::vector<Touch*> getAllTouchesVector()
{
    std::vector<Touch*> ret;
    int i;
    int temp = g_indexBitsUsed;

    ret.reserve(EventTouch::MAX_TOUCHES);
    for (i = 0; i < EventTouch::MAX_TOUCHES; i++)
    {
        if (temp & 0x00000001)
        {
            ret.emplace_back(g_touches[i]);
        }
        temp >>= 1;
    }
    return ret;
}

static void removeUsedIndexBit(int index)
{
    if (index < 0 || index >= EventTouch::MAX_TOUCHES)
    {
        return;
    }

    unsigned int temp = 1 << index;
    temp              = ~temp;
    g_indexBitsUsed &= temp;
}

}  // namespace

// default context attributions are set as follows
GfxContextAttrs RenderView::_gfxContextAttrs = {8, 8, 8, 8, 24, 8, 0};

void RenderView::setGfxContextAttrs(GfxContextAttrs& gfxContextAttrs)
{
    _gfxContextAttrs = gfxContextAttrs;
}

GfxContextAttrs& RenderView::getGfxContextAttrs()
{
    return _gfxContextAttrs;
}

RenderView::RenderView()
    : _screenSize(0, 0)
    , _designResolutionSize(0, 0)
    , _scaleX(1.0f)
    , _scaleY(1.0f)
    , _resolutionPolicy(ResolutionPolicy::UNKNOWN)
    , _interactive(true)
{}

RenderView::~RenderView() {}

void RenderView::pollEvents() {}

void RenderView::updateDesignResolutionSize()
{
    if (_screenSize.width > 0 && _screenSize.height > 0 && _designResolutionSize.width > 0 &&
        _designResolutionSize.height > 0)
    {
        _scaleX = (float)_screenSize.width / _designResolutionSize.width;
        _scaleY = (float)_screenSize.height / _designResolutionSize.height;

        if (_resolutionPolicy == ResolutionPolicy::NO_BORDER)
        {
            _scaleX = _scaleY = MAX(_scaleX, _scaleY);
        }

        else if (_resolutionPolicy == ResolutionPolicy::SHOW_ALL)
        {
            _scaleX = _scaleY = MIN(_scaleX, _scaleY);
        }

        else if (_resolutionPolicy == ResolutionPolicy::FIXED_HEIGHT)
        {
            _scaleX                     = _scaleY;
            _designResolutionSize.width = ceilf(_screenSize.width / _scaleX);
        }

        else if (_resolutionPolicy == ResolutionPolicy::FIXED_WIDTH)
        {
            _scaleY                      = _scaleX;
            _designResolutionSize.height = ceilf(_screenSize.height / _scaleY);
        }

        // calculate the rect of viewport
        float viewportW = _designResolutionSize.width * _scaleX;
        float viewportH = _designResolutionSize.height * _scaleY;

        _viewportRect.setRect((_screenSize.width - viewportW) / 2, (_screenSize.height - viewportH) / 2, viewportW,
                              viewportH);

        // reset director's member variables to fit visible rect
        auto director                   = Director::getInstance();
        director->_winSizeInPoints      = getDesignResolutionSize();
        director->_isStatusLabelUpdated = true;
        director->setProjection(director->getProjection());
    }
}

void RenderView::setDesignResolutionSize(float width, float height, ResolutionPolicy resolutionPolicy)
{
    AXASSERT(resolutionPolicy != ResolutionPolicy::UNKNOWN, "should set resolutionPolicy");

    if (width == 0.0f || height == 0.0f)
    {
        return;
    }

    _designResolutionSize.set(width, height);
    _resolutionPolicy = resolutionPolicy;

    updateDesignResolutionSize();
}

const Vec2& RenderView::getDesignResolutionSize() const
{
    return _designResolutionSize;
}

Vec2 RenderView::getFrameSize() const
{
    return _screenSize;
}

void RenderView::setFrameSize(float width, float height)
{
    _screenSize = Vec2(width, height);

    // Github issue #16003 and #16485
    // only update the designResolution if it wasn't previously set
    if (_designResolutionSize.equals(Vec2::ZERO))
        _designResolutionSize = _screenSize;
}

Rect RenderView::getVisibleRect() const
{
    Rect ret;
    ret.size   = getVisibleSize();
    ret.origin = getVisibleOrigin();
    return ret;
}

Rect RenderView::getSafeAreaRect() const
{
    return getVisibleRect();
}

Vec2 RenderView::getVisibleSize() const
{
    if (_resolutionPolicy == ResolutionPolicy::NO_BORDER)
    {
        return Vec2(_screenSize.width / _scaleX, _screenSize.height / _scaleY);
    }
    else
    {
        return _designResolutionSize;
    }
}

Vec2 RenderView::getVisibleOrigin() const
{
    if (_resolutionPolicy == ResolutionPolicy::NO_BORDER)
    {
        return Vec2((_designResolutionSize.width - _screenSize.width / _scaleX) / 2,
                    (_designResolutionSize.height - _screenSize.height / _scaleY) / 2);
    }
    else
    {
        return Vec2::ZERO;
    }
}

void RenderView::setViewportInPoints(float x, float y, float w, float h)
{
    Viewport vp;
    vp.x      = (int)(x * _scaleX + _viewportRect.origin.x);
    vp.y      = (int)(y * _scaleY + _viewportRect.origin.y);
    vp.width  = (unsigned int)(w * _scaleX);
    vp.height = (unsigned int)(h * _scaleY);
    Camera::setDefaultViewport(vp);
}

void RenderView::setScissorInPoints(float x, float y, float w, float h)
{
    setScissorRect((int)(x * _scaleX + _viewportRect.origin.x), (int)(y * _scaleY + _viewportRect.origin.y),
                   (unsigned int)(w * _scaleX), (unsigned int)(h * _scaleY));
}

bool RenderView::isScissorEnabled()
{
    auto renderer = Director::getInstance()->getRenderer();
    return renderer->getScissorTest();
}

Rect RenderView::getScissorInPoints() const
{
    auto& rect = getScissorRect();

    float x = (rect.x - _viewportRect.origin.x) / _scaleX;
    float y = (rect.y - _viewportRect.origin.y) / _scaleY;
    float w = rect.width / _scaleX;
    float h = rect.height / _scaleY;
    return Rect(x, y, w, h);
}

void RenderView::setViewName(std::string_view viewname)
{
    _viewName = viewname;
}

std::string_view RenderView::getViewName() const
{
    return _viewName;
}

void RenderView::handleTouchesBegin(int num, intptr_t ids[], float xs[], float ys[])
{
    if (!_interactive)
        return;

    intptr_t id     = 0;
    float x         = 0.0f;
    float y         = 0.0f;
    int unusedIndex = 0;
    EventTouch touchEvent;

    for (int i = 0; i < num; ++i)
    {
        id = ids[i];
        x  = xs[i];
        y  = ys[i];

        auto iter = g_touchIdReorderMap.find(id);

        // it is a new touch
        if (iter == g_touchIdReorderMap.end())
        {
            unusedIndex = getUnUsedIndex();

            // The touches is more than MAX_TOUCHES ?
            if (unusedIndex == -1)
            {
                AXLOGD("The touches is more than MAX_TOUCHES, unusedIndex = {}", unusedIndex);
                continue;
            }

            Touch* touch = g_touches[unusedIndex] = new Touch();
            touch->setTouchInfo(unusedIndex, transformInputX(x), transformInputY(y));

            AXLOGV("x = {} y = {}", touch->getLocationInView().x, touch->getLocationInView().y);

            g_touchIdReorderMap.emplace(id, unusedIndex);
            touchEvent._touches.emplace_back(touch);
        }
    }

    if (touchEvent._touches.empty())
    {
        AXLOGD("touchesBegan: size = 0");
        return;
    }

    touchEvent._eventCode = EventTouch::EventCode::BEGAN;
    auto dispatcher       = Director::getInstance()->getEventDispatcher();
    dispatcher->dispatchEvent(&touchEvent);
}

void RenderView::handleTouchesMove(int num, intptr_t ids[], float xs[], float ys[])
{
    handleTouchesMove(num, ids, xs, ys, nullptr, nullptr);
}

void RenderView::handleTouchesMove(int num, intptr_t ids[], float xs[], float ys[], float fs[], float ms[])
{
    if (!_interactive)
        return;

    intptr_t id    = 0;
    float x        = 0.0f;
    float y        = 0.0f;
    float force    = 0.0f;
    float maxForce = 0.0f;
    EventTouch touchEvent;

    for (int i = 0; i < num; ++i)
    {
        id       = ids[i];
        x        = xs[i];
        y        = ys[i];
        force    = fs ? fs[i] : 0.0f;
        maxForce = ms ? ms[i] : 0.0f;

        auto iter = g_touchIdReorderMap.find(id);
        if (iter == g_touchIdReorderMap.end())
        {
            AXLOGD("if the index doesn't exist, it is an error");
            continue;
        }

        AXLOGV("Moving touches with id: {}, x={}, y={}, force={}, maxFource={}", (int)id, x, y, force, maxForce);
        Touch* touch = g_touches[iter->second];
        if (touch)
        {
            touch->setTouchInfo(iter->second, transformInputX(x), transformInputY(y), force, maxForce);

            touchEvent._touches.emplace_back(touch);
        }
        else
        {
            // It is error, should return.
            AXLOGD("Moving touches with id: {} error", static_cast<int32_t>(id));
            return;
        }
    }

    if (touchEvent._touches.empty())
    {
        AXLOGD("touchesMoved: size = 0");
        return;
    }

    touchEvent._eventCode = EventTouch::EventCode::MOVED;
    auto dispatcher       = Director::getInstance()->getEventDispatcher();
    dispatcher->dispatchEvent(&touchEvent);
}

void RenderView::handleTouchesOfEndOrCancel(EventTouch::EventCode eventCode,
                                            int num,
                                            intptr_t ids[],
                                            float xs[],
                                            float ys[])
{
    intptr_t id = 0;
    float x     = 0.0f;
    float y     = 0.0f;
    EventTouch touchEvent;

    for (int i = 0; i < num; ++i)
    {
        id = ids[i];
        x  = xs[i];
        y  = ys[i];

        auto iter = g_touchIdReorderMap.find(id);
        if (iter == g_touchIdReorderMap.end())
        {
            AXLOGD("if the index doesn't exist, it is an error");
            continue;
        }

        /* Add to the set to send to the director */
        Touch* touch = g_touches[iter->second];
        if (touch)
        {
            AXLOGV("Ending touches with id: {}, x={}, y={}", (int)id, x, y);
            touch->setTouchInfo(iter->second, transformInputX(x), transformInputY(y));

            touchEvent._touches.emplace_back(touch);

            g_touches[iter->second] = nullptr;
            removeUsedIndexBit(iter->second);

            g_touchIdReorderMap.erase(id);
        }
        else
        {
            AXLOGD("Ending touches with id: {} error", static_cast<int32_t>(id));
            return;
        }
    }

    if (touchEvent._touches.empty())
    {
        AXLOGD("touchesEnded or touchesCancel: size = 0");
        return;
    }

    touchEvent._eventCode = eventCode;
    auto dispatcher       = Director::getInstance()->getEventDispatcher();
    dispatcher->dispatchEvent(&touchEvent);

    for (auto&& touch : touchEvent._touches)
    {
        // release the touch object.
        touch->release();
    }
}

void RenderView::handleTouchesEnd(int num, intptr_t ids[], float xs[], float ys[])
{
    handleTouchesOfEndOrCancel(EventTouch::EventCode::ENDED, num, ids, xs, ys);
}

void RenderView::handleTouchesCancel(int num, intptr_t ids[], float xs[], float ys[])
{
    handleTouchesOfEndOrCancel(EventTouch::EventCode::CANCELLED, num, ids, xs, ys);
}

const Rect& RenderView::getViewPortRect() const
{
    return _viewportRect;
}

std::vector<Touch*> RenderView::getAllTouches() const
{
    return getAllTouchesVector();
}

float RenderView::getScaleX() const
{
    return _scaleX;
}

float RenderView::getScaleY() const
{
    return _scaleY;
}

void RenderView::onFrameBufferResized(uint32_t fbWidth, uint32_t fbHeight)
{
    Director::getInstance()->resizeSwapchain(fbWidth, fbHeight);

#ifdef AX_ENABLE_VR
    if (_vrRenderer) [[unlikely]]
        _vrRenderer->onRenderViewResized(this);
#endif
}

void RenderView::renderScene(Scene* scene, Renderer* renderer)
{
    AXASSERT(scene, "Invalid Scene");
    AXASSERT(renderer, "Invalid Renderer");

#ifdef AX_ENABLE_VR
    if (_vrRenderer) [[unlikely]]
    {
        _vrRenderer->render(scene, renderer);
        return;
    }
#endif

    scene->render(renderer, Mat4::IDENTITY, nullptr);
}

void RenderView::setScissorRect(float x, float y, float w, float h)
{
#ifdef AX_ENABLE_VR
    if (_vrRenderer) [[unlikely]]
    {
        _vrRenderer->setScissorRect(x, y, w, h);
        return;
    }
#endif

    Director::getInstance()->getRenderer()->setScissorRect(x, y, w, h);
}

const ScissorRect& RenderView::getScissorRect() const
{
#ifdef AX_ENABLE_VR
    if (_vrRenderer) [[unlikely]]
        return _vrRenderer->getScissorRect();
#endif

    return Director::getInstance()->getRenderer()->getScissorRect();
}

#ifdef AX_ENABLE_VR
void RenderView::setVR(std::unique_ptr<experimental::IVRRenderer>&& impl)
{
    if (_vrRenderer != impl)
    {
        if (_vrRenderer)
        {
            _vrRenderer->cleanup();
            _vrRenderer.reset();
        }

        if (impl)
            impl->init(this);

        _vrRenderer = std::move(impl);
    }
}
#endif

void RenderView::queueOperation(AsyncOperation /*op*/, void* /*param*/) {}

void RenderView::setInteractive(bool interactive)
{
    if (_interactive && !interactive)
    {
        cancelAllTouches();
    }

    _interactive = interactive;
}

void RenderView::cancelAllTouches()
{
    EventTouch touchEvent;
    touchEvent._touches   = getAllTouchesVector();
    touchEvent._eventCode = EventTouch::EventCode::CANCELLED;

    if (touchEvent._touches.empty())
    {
        AXLOGD("cancelling all touches: size = 0");
        return;
    }

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->dispatchEvent(&touchEvent);

    for (auto&& touch : touchEvent._touches)
    {
        // release the touch object.
        touch->release();
    }

    g_touchIdReorderMap.clear();
    g_indexBitsUsed = 0;

    for (int i = 0; i < EventTouch::MAX_TOUCHES; ++i)
    {
        g_touches[i] = nullptr;
    }
}

}  // namespace ax
