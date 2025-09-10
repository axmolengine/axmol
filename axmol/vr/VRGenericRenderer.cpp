/****************************************************************************
 Copyright (c) 2016 Chukong Technologies Inc.
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

#include "axmol/platform/PlatformMacros.h"
#include "axmol/vr/VRGenericRenderer.h"
#include "axmol/vr/VRDistortionMesh.h"
#include "axmol/vr/VRDistortion.h"
#include "axmol/vr/VRGenericHeadTracker.h"
#include "axmol/renderer/Renderer.h"
#include "axmol/rhi/ProgramState.h"
#include "axmol/base/Director.h"
#include "axmol/2d/Scene.h"
#include "axmol/2d/Camera.h"
#include "axmol/2d/Sprite.h"
#include "axmol/platform/RenderView.h"

namespace ax::experimental
{

static RasterTransform makeEyeScissorTransform(const Viewport& eyeViewport,
                                               const Viewport& screenViewport,
                                               const Size& rtSize)
{
    const float subW = rtSize.width * 0.5f;
    const float subH = rtSize.height;

    const float sx = (subW) / screenViewport.width;
    const float sy = (subH) / screenViewport.height;

    RasterTransform xf;
    xf.sx = sx;
    xf.sy = sy;

    xf.ox = eyeViewport.x - (screenViewport.x) * sx;
    xf.oy = eyeViewport.y - (screenViewport.y) * sy;

    return xf;
}

VRGenericRenderer::VRGenericRenderer()
{
    _headTracker = new VRGenericHeadTracker();
    _director    = Director::getInstance();

    setupProgram();
}

VRGenericRenderer::~VRGenericRenderer()
{
    cleanup();
    AX_SAFE_RELEASE(_programState);
    AX_SAFE_RELEASE(_vertexLayout);
    AX_SAFE_DELETE(_headTracker);
}

IVRHeadTracker* VRGenericRenderer::getHeadTracker()
{
    return _headTracker;
}

void VRGenericRenderer::cleanup()
{
    AX_SAFE_RELEASE_NULL(_renderTexture);
    AX_SAFE_DELETE(_distortion);
    AX_SAFE_DELETE(_leftDistortionMesh);
    AX_SAFE_DELETE(_rightDistortionMesh);
}

void VRGenericRenderer::onRenderViewResized(RenderView* rv)
{
    cleanup();
    init(rv);
}

void VRGenericRenderer::setScissorRect(float x, float y, float w, float h)
{
    _sourceScissorRect.set(x, y, w, h);

    if (!_stStack.empty())
    {
        const auto& xf = _stStack.top();
        x              = x * xf.sx + xf.ox;
        y              = y * xf.sy + xf.oy;
        w              = w * xf.sx;
        h              = h * xf.sy;
    }

    Director::getInstance()->getRenderer()->setScissorRect(x, y, w, h);
}
const ScissorRect& VRGenericRenderer::getScissorRect() const
{
    return _sourceScissorRect;
}

void VRGenericRenderer::init(RenderView* rv)
{
    const auto windowSize = rv->getWindowSize();
    _renderTexture        = RenderTexture::create(windowSize.width, windowSize.height);
    _renderTexture->retain();

    // DO NOT offset eye viewports by default viewport.
    // Eye viewports in RT must be exact halves to match distortion mesh sampling.
    fillEyeViewports(rv, windowSize);

    // Scissor transform: scale only, no translation in VR path.
    auto& screenVP = Camera::getDefaultViewport();
    auto& rtSize   = _renderTexture->getContentSize();
    _xfLeft        = makeEyeScissorTransform(_leftEye.viewport, screenVP, rtSize);
    _xfRight       = makeEyeScissorTransform(_rightEye.viewport, screenVP, rtSize);

    _distortion          = new Distortion();
    _leftDistortionMesh  = createDistortionMesh(VREye::EyeType::LEFT, windowSize);
    _rightDistortionMesh = createDistortionMesh(VREye::EyeType::RIGHT, windowSize);

    _leftEyeCmd.init(0);
    _leftEyeCmd.setVertexBuffer(_leftDistortionMesh->_vbo);
    _leftEyeCmd.setIndexBuffer(_leftDistortionMesh->_ebo, CustomCommand::IndexFormat::U_SHORT);
    _leftEyeCmd.setDrawType(CustomCommand::DrawType::ELEMENT);
    _leftEyeCmd.setPrimitiveType(CustomCommand::PrimitiveType::TRIANGLE_STRIP);
    _leftEyeCmd.setIndexDrawInfo(0, _leftDistortionMesh->_indices);

    _rightEyeCmd.init(0);
    _rightEyeCmd.setVertexBuffer(_rightDistortionMesh->_vbo);
    _rightEyeCmd.setIndexBuffer(_rightDistortionMesh->_ebo, CustomCommand::IndexFormat::U_SHORT);
    _rightEyeCmd.setDrawType(CustomCommand::DrawType::ELEMENT);
    _rightEyeCmd.setPrimitiveType(CustomCommand::PrimitiveType::TRIANGLE_STRIP);
    _rightEyeCmd.setIndexDrawInfo(0, _rightDistortionMesh->_indices);
}

void VRGenericRenderer::fillEyeViewports(RenderView* rv, const Vec2& screenSize)
{
    const auto& rtSize = _renderTexture->getContentSize();
    if (screenSize.x <= 0 || screenSize.y <= 0 || rtSize.width <= 0 || rtSize.height <= 0)
        return;

    // Per-eye half frame
    const float subW = rtSize.width / 2;
    const float subH = rtSize.height;

    // SHOW_ALL: decide by aspect
    float viewportW, viewportH;
    if (subW * screenSize.y > subH * screenSize.x)
    {
        // Half frame is wider: fit height
        viewportH = subH;
        viewportW = subH * (screenSize.x / screenSize.y);
    }
    else
    {
        // Half frame is narrower: fit width
        viewportW = subW;
        viewportH = subW / (screenSize.x / screenSize.y);
    }

    // Center inside half frame
    const float offsetX = (subW - viewportW) / 2;
    const float offsetY = (subH - viewportH) / 2;

    // Left eye
    _leftEye.type            = VREye::EyeType::LEFT;
    _leftEye.viewport.x      = offsetX;
    _leftEye.viewport.y      = offsetY;
    _leftEye.viewport.width  = viewportW;
    _leftEye.viewport.height = viewportH;

    // Right eye
    _rightEye.type            = VREye::EyeType::RIGHT;
    _rightEye.viewport.x      = subW + offsetX;
    _rightEye.viewport.y      = offsetY;
    _rightEye.viewport.width  = viewportW;
    _rightEye.viewport.height = viewportH;
}

void VRGenericRenderer::render(Scene* scene, Renderer* renderer)
{
    const float eyeOffset = 0.5f;

    // Get head rotation and compute view transforms for both eyes
    auto headRotation = !_debugIgnoreHeadTracker ? _headTracker->getLocalRotation() : Mat4::IDENTITY;

    Mat4 leftTransform;
    Mat4::createTranslation(eyeOffset, 0, 0, &leftTransform);
    leftTransform *= headRotation;

    Mat4 rightTransform;
    Mat4::createTranslation(-eyeOffset, 0, 0, &rightTransform);
    rightTransform *= headRotation;

    auto texture = _renderTexture->getSprite()->getTexture();

    auto& clearColor = _director->getClearColor();

    // --- Render left eye
    _renderTexture->beginWithClear(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    renderer->addCallbackCommand(AX_CALLBACK_0(VRGenericRenderer::pushLeftRasterTransform, this, renderer));
    _renderTexture->setVirtualViewport(
        Vec2(0, 0),  // Start position on target texture
        Rect(_leftEye.viewport.x, _leftEye.viewport.y, _leftEye.viewport.w, _leftEye.viewport.h),
        Rect(_leftEye.viewport.x, _leftEye.viewport.y, _leftEye.viewport.w, _leftEye.viewport.h));

    scene->render(renderer, leftTransform);
    renderer->addCallbackCommand(AX_CALLBACK_0(VRGenericRenderer::popRasterTransform, this, renderer));
    _renderTexture->end();

    // --- Render right eye
    _renderTexture->begin();
    renderer->addCallbackCommand(AX_CALLBACK_0(VRGenericRenderer::pushRightRasterTransform, this, renderer));
    _renderTexture->setVirtualViewport(
        Vec2(0, 0),  // Start position on target texture
        Rect(_rightEye.viewport.x, _rightEye.viewport.y, _rightEye.viewport.w, _rightEye.viewport.h),
        Rect(_rightEye.viewport.x, _rightEye.viewport.y, _rightEye.viewport.w, _rightEye.viewport.h));
    scene->render(renderer, rightTransform);
    renderer->addCallbackCommand(AX_CALLBACK_0(VRGenericRenderer::popRasterTransform, this, renderer));
    _renderTexture->end();

#pragma region Submit distortion draw commands to screen
    // Hacker: due to GroupCommand, scene->render internally calls renderer->render()
    // to finish drawing into offscreen RT. We need an empty begin/end to restore the default screen RT.
    _renderTexture->begin();
    _renderTexture->end();

    // Bind the rendered texture to the distortion shader
    _programState->setTexture(texture->getRHITexture());

    // Restore viewport for distortion rendering
    auto defaultVP = Camera::getDefaultViewport();
    renderer->addCallbackCommand(
        [=]() { renderer->setViewport(defaultVP.x, defaultVP.y, defaultVP.width, defaultVP.height); });

    // Submit distortion draw commands for both eyes
    renderer->addCommand(&_leftEyeCmd);
    renderer->addCommand(&_rightEyeCmd);
#pragma endregion
}

void VRGenericRenderer::pushLeftRasterTransform(Renderer* renderer)
{
    pushScissorTransform(_xfLeft);
}

void VRGenericRenderer::pushRightRasterTransform(Renderer* renderer)
{
    pushScissorTransform(_xfRight);
}

void VRGenericRenderer::popRasterTransform(Renderer* renderer)
{
    popScissorTransform();
}

DistortionMesh* VRGenericRenderer::createDistortionMesh(VREye::EyeType eyeType, const Size& screenSize)
{
    auto vp = Camera::getDefaultViewport();

    const float screenWidth      = screenSize.width;
    const float screenHeight     = screenSize.height;
    const float xEyeOffsetScreen = (eyeType == VREye::EyeType::LEFT) ? screenWidth / 4 : screenWidth * 3 / 4;
    const float yEyeOffsetScreen = screenHeight / 2;

    const float textureWidth      = screenSize.width;
    const float textureHeight     = screenSize.height;
    const float xEyeOffsetTexture = (eyeType == VREye::EyeType::LEFT) ? screenSize.width / 4 : screenSize.width * 3 / 4;
    const float yEyeOffsetTexture = screenSize.height / 2;

    const float viewportX = (eyeType == VREye::EyeType::LEFT) ? 0 : textureWidth / 2;
    const float viewportY = 0;
    const float viewportW = textureWidth / 2;
    const float viewportH = textureHeight;

    return new DistortionMesh(_distortion, screenWidth, screenHeight, xEyeOffsetScreen, yEyeOffsetScreen, textureWidth,
                              textureHeight, xEyeOffsetTexture, yEyeOffsetTexture, viewportX, viewportY, viewportW,
                              viewportH, _vignetteEnabled);
}

void VRGenericRenderer::setupProgram()
{
    auto program = axpm->loadProgram("vr_vs", "vr_fs");

    _programState = new ProgramState(program);

    auto desc = axvlm->allocateVertexLayoutDesc();

    desc.startLayout(3);

    desc.addAttrib("a_position", program->getVertexInputDesc("a_position"), rhi::VertexFormat::FLOAT2, 0, false);
    desc.addAttrib("a_texCoord", program->getVertexInputDesc("a_texCoord"), rhi::VertexFormat::FLOAT2, sizeof(Vec2),
                   false);
    desc.addAttrib("a_vignette", program->getVertexInputDesc("a_vignette"), rhi::VertexFormat::FLOAT,
                   sizeof(Vec2) + sizeof(Vec2), false);

    desc.endLayout();

    _vertexLayout = axvlm->acquireVertexLayout(std::move(desc));

    _leftEyeCmd.setWeakPSVL(_programState, _vertexLayout);
    _rightEyeCmd.setWeakPSVL(_programState, _vertexLayout);
}

}  // namespace ax::experimental
