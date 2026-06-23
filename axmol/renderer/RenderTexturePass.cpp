/****************************************************************************
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
#include "axmol/renderer/RenderTexturePass.h"

#include "axmol/base/Director.h"
#include "axmol/renderer/Renderer.h"
#include "axmol/rhi/DriverContext.h"
#include "axmol/scene/Camera.h"
#include "axmol/renderer/RenderTexture.h"

namespace ax
{

static constexpr float RT_PASS_BEGIN_ORDER = -std::numeric_limits<float>::max();
static constexpr float RT_PASS_CLEAR_ORDER = -std::numeric_limits<float>::max() * 0.5f;
static constexpr float RT_PASS_END_ORDER   = std::numeric_limits<float>::max();

RenderTexturePass* RenderTexturePass::obtain(RenderTexture* rt)
{
    RenderTexturePass* pass = new RenderTexturePass(rt);
    // ref count = 1 from Object ctor, caller owns the reference
    return pass;
}

RenderTexturePass::RenderTexturePass() : _renderer(Director::getInstance()->getRenderer()) {}

RenderTexturePass::RenderTexturePass(RenderTexture* renderTexture) : RenderTexturePass()
{
    setTarget(renderTexture);
}

RenderTexturePass::~RenderTexturePass()
{
    if (_active)
        end();
}

void RenderTexturePass::setTarget(RenderTexture* rt)
{
    AXASSERT(!_active, "Cannot change target while pass is active");
    _renderTexture = rt;
    updateViewport();
}

void RenderTexturePass::updateViewport()
{
    if (!_renderTexture)
    {
        _viewport.reset();
        return;
    }

    auto size = _renderTexture->getPixelSize();
    _viewport.emplace(0, 0, static_cast<int>(size.x), static_cast<int>(size.y));
}

void RenderTexturePass::setViewport(std::optional<Viewport> vp)
{
    _viewport = vp;
}

Viewport RenderTexturePass::makeVirtualViewport(const Vec2& rtBegin,
                                                const Rect& fullRect,
                                                const Rect& fullViewport) const
{
    Viewport viewport;
    if (fullViewport.size.width <= 0 || fullViewport.size.height <= 0 || fullRect.size.width <= 0 ||
        fullRect.size.height <= 0)
    {
        return viewport;
    }

    const auto viewportRectWidthRatio  = fullViewport.size.width / fullRect.size.width;
    const auto viewportRectHeightRatio = fullViewport.size.height / fullRect.size.height;

    viewport.x      = static_cast<int>((fullRect.origin.x - rtBegin.x) * viewportRectWidthRatio);
    viewport.y      = static_cast<int>((fullRect.origin.y - rtBegin.y) * viewportRectHeightRatio);
    viewport.width  = static_cast<int>(fullViewport.size.width);
    viewport.height = static_cast<int>(fullViewport.size.height);
    return viewport;
}

void RenderTexturePass::begin(const Camera* camera)
{
    AXASSERT(_renderTexture && !_active,
             "RenderTexturePass::begin requires a valid RenderTexture and not in active state");
    if (!_renderTexture || _active)
        return;

    if (_cameraOverrideEnabled)
    {
        // Fallback chain: explicit camera -> visiting camera -> default camera -> offscreen camera
        if (!camera)
        {
            camera = Camera::getVisitingCamera();
            if (!camera)
                camera = Camera::getDefaultCamera();
            if (!camera)
                camera = Director::getInstance()->getOffscreenCamera();
        }

        // Save visiting camera
        _savedCamera = Camera::getVisitingCamera();
        Camera::setVisitingCamera(const_cast<Camera*>(camera));

        // Save and flip visiting camera's projection on OpenGL to keep texture upright
        if (_autoFlipY && rhi::DriverContext::isOpenGL())
        {
            auto cam = const_cast<Camera*>(Camera::getVisitingCamera());
            if (cam)
            {
                _savedProjection = cam->getProjectionMatrix();
                Mat4 flipY;
                Mat4::createScale(1.f, -1.f, 1.f, &flipY);
                cam->setProjectionMatrix(flipY * *_savedProjection);
            }
        }
    }

    // Push group to isolate rendered content
    auto groupCmd = _renderer->getNextGroupCommand();
    groupCmd->init(0);
    _renderer->addCommand(groupCmd);
    _renderer->pushGroup(groupCmd->getRenderQueueID());

    // Begin callback: save current render target / viewport, set new ones
    auto beginCmd = _renderer->nextCallbackCommand();
    beginCmd->init(RT_PASS_BEGIN_ORDER);
    beginCmd->func = [target = _renderTexture->getRenderTarget(), vp = _viewport,
                      self = RefPtr<RenderTexturePass>(this), this]() {
        SavedState state;
        state.renderTarget = _renderer->getRenderTarget();
        if (vp.has_value())
        {
            state.viewport = _renderer->getViewport();
            _renderer->setViewport(vp->x, vp->y, vp->width, vp->height);
        }
        _renderer->setRenderTarget(target);
        _savedStates.push_back(state);
    };
    _renderer->addCommand(beginCmd);

    _active = true;
}

void RenderTexturePass::clear(ClearFlag flags, const ClearValue& value)
{
    AXASSERT(_active, "RenderTexturePass::clear requires an active pass");
    if (!_active || flags == ClearFlag::NONE)
        return;

    _renderer->clear(flags, value.color, value.depth, value.stencil, RT_PASS_CLEAR_ORDER);
}

void RenderTexturePass::end()
{
    if (!_active)
        return;

    // End callback: restore previous render target / viewport
    auto endCmd = _renderer->nextCallbackCommand();
    endCmd->init(RT_PASS_END_ORDER);
    endCmd->func = [self = RefPtr<RenderTexturePass>(this), this]() {
        if (_savedStates.empty())
            return;
        auto state = _savedStates.back();
        _savedStates.pop_back();
        _renderer->setRenderTarget(state.renderTarget);
        if (state.viewport.has_value())
        {
            _renderer->setViewport(state.viewport->x, state.viewport->y, state.viewport->width, state.viewport->height);
        }
    };
    _renderer->addCommand(endCmd);
    _renderer->popGroup();

    // Restore visiting camera's projection immediately
    if (_savedProjection.has_value())
    {
        auto cam = const_cast<Camera*>(Camera::getVisitingCamera());
        if (cam)
            cam->setProjectionMatrix(*_savedProjection);
        _savedProjection.reset();
    }

    // Restore visiting camera immediately
    if (_savedCamera)
        Camera::setVisitingCamera(const_cast<Camera*>(_savedCamera));

    _active = false;
}

}  // namespace ax
