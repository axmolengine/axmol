/****************************************************************************
 Copyright (c) 2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software and to permit persons to whom the Software is
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
#include "axmol/renderer/ProgramManager.h"
#include "axmol/renderer/VertexLayoutManager.h"
#include "axmol/renderer/Shaders.h"
#include "axmol/rhi/ProgramState.h"
#include "axmol/base/Director.h"
#include "axmol/scene/Scene.h"
#include "axmol/scene/Camera.h"
#include "axmol/platform/RenderView.h"

namespace ax
{
inline namespace experimental
{

static ScissorTransform makeEyeScissorTransform(const Viewport& eyeViewport, const Size& sourceSize)
{
    ScissorTransform xf;
    if (sourceSize.width <= 0 || sourceSize.height <= 0)
        return xf;

    xf.sx = eyeViewport.width / sourceSize.width;
    xf.sy = eyeViewport.height / sourceSize.height;
    xf.ox = eyeViewport.x;
    xf.oy = eyeViewport.y;

    return xf;
}

VRGenericRenderer::VRGenericRenderer()
{
    _headTracker = new VRGenericHeadTracker();
    _director    = Director::getInstance();
    setupDistortionProgram();
}

VRGenericRenderer::~VRGenericRenderer()
{
    AX_SAFE_RELEASE_NULL(_renderTexture);
    AX_SAFE_DELETE(_distortion);
    AX_SAFE_DELETE(_leftDistortionMesh);
    AX_SAFE_DELETE(_rightDistortionMesh);
    AX_SAFE_RELEASE(_programState);
    AX_SAFE_RELEASE(_vertexLayout);
    AX_SAFE_DELETE(_headTracker);
}

IVRHeadTracker* VRGenericRenderer::getHeadTracker()
{
    return _headTracker;
}

void VRGenericRenderer::onRenderViewChanged(RenderViewCore* rv)
{
    reset(rv);
}

void VRGenericRenderer::setRenderScale(float renderScale)
{
    if (renderScale <= 0.0f || !std::isfinite(renderScale))
        renderScale = 1.0f;

    if (_renderScale == renderScale)
        return;

    _renderScale = renderScale;

    reset(_director->getRenderView());
}

void VRGenericRenderer::reset(RenderViewCore* rv)
{
    AX_SAFE_RELEASE_NULL(_renderTexture);
    AX_SAFE_DELETE(_distortion);
    AX_SAFE_DELETE(_leftDistortionMesh);
    AX_SAFE_DELETE(_rightDistortionMesh);

    auto& leftEye  = _eyes[LeftEyeIndex];
    auto& rightEye = _eyes[RightEyeIndex];

    // Ensure VR render view uses the same resolution policy as the normal render view by basing it on the viewport size
    auto& outputSize = rv->getViewportRect().size;

    _renderTexture = RenderTexture::create(outputSize * _renderScale, PixelFormat::RGBA8, PixelFormat::D24S8);
    _renderTexture->retain();

    _rtPass = RefPtr<RenderTexturePass>(RenderTexturePass::obtain(_renderTexture), tlx::adopt_object);
    _rtPass->setCameraOverrideEnabled(false);

    // DO NOT offset eye viewports by default viewport.
    // Eye viewports in RT must be exact halves to match distortion mesh sampling.
    prepareEyesViewport(outputSize, _renderTexture->getRenderSize());

    // Scissor rectangles are reported in the visible viewport's local pixel
    // space. Map that source size into each eye sub-viewport so clipped
    // components such as ScrollView keep the same visible clipping area.
    leftEye.scissorTransform  = makeEyeScissorTransform(leftEye.viewport, outputSize);
    rightEye.scissorTransform = makeEyeScissorTransform(rightEye.viewport, outputSize);

    _distortion          = new Distortion();
    _leftDistortionMesh  = createDistortionMesh(VREye::EyeType::LEFT, outputSize);
    _rightDistortionMesh = createDistortionMesh(VREye::EyeType::RIGHT, outputSize);

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

    // Bind the rendered texture to the distortion shader
    _programState->setTexture(_renderTexture->getRHITexture());
}

void VRGenericRenderer::prepareEyesViewport(const Vec2& outputSize, const Vec2& textureSize)
{
    if (outputSize.x <= 0 || outputSize.y <= 0 || textureSize.width <= 0 || textureSize.height <= 0)
        return;

    auto& leftEye  = _eyes[LeftEyeIndex];
    auto& rightEye = _eyes[RightEyeIndex];

    // Per-eye half frame
    const float subW = textureSize.width / 2;
    const float subH = textureSize.height;

    // SHOW_ALL: decide by aspect
    float viewportW, viewportH;
    if (subW * outputSize.y > subH * outputSize.x)
    {
        // Half frame is wider: fit height
        viewportH = subH;
        viewportW = subH * (outputSize.x / outputSize.y);
    }
    else
    {
        // Half frame is narrower: fit width
        viewportW = subW;
        viewportH = subW / (outputSize.x / outputSize.y);
    }

    // Center inside half frame
    const float offsetX = (subW - viewportW) / 2;
    const float offsetY = (subH - viewportH) / 2;

    // Left eye
    leftEye.type            = VREye::EyeType::LEFT;
    leftEye.viewport.x      = offsetX;
    leftEye.viewport.y      = offsetY;
    leftEye.viewport.width  = viewportW;
    leftEye.viewport.height = viewportH;

    // Right eye
    rightEye.type            = VREye::EyeType::RIGHT;
    rightEye.viewport.x      = subW + offsetX;
    rightEye.viewport.y      = offsetY;
    rightEye.viewport.width  = viewportW;
    rightEye.viewport.height = viewportH;
}

void VRGenericRenderer::renderScene(Renderer* renderer, Scene* scene)
{
    clearEyeRenderTexture(renderer, _director->getClearColor());

    const float eyeOffset = _eyeSeparation * 0.5f;

    // Get head rotation and compute view transforms for both eyes
    auto headRotation = !_debugIgnoreHeadTracker ? _headTracker->getLocalRotation() : Mat4::identity;

    Mat4 leftEyeTransform;
    Mat4::createTranslation(eyeOffset, 0, 0, &leftEyeTransform);
    leftEyeTransform *= headRotation;

    Mat4 rightEyeTransform;
    Mat4::createTranslation(-eyeOffset, 0, 0, &rightEyeTransform);
    rightEyeTransform *= headRotation;

    renderSceneToEye(renderer, scene, LeftEyeIndex, leftEyeTransform);
    renderSceneToEye(renderer, scene, RightEyeIndex, rightEyeTransform);
    renderDistortionPass(renderer);

    Camera::setVisitingCamera(nullptr);
}

void VRGenericRenderer::renderDistortionPass(Renderer* renderer)
{
    AXASSERT(renderer, "Invalid Renderer");

    // Restore the screen viewport before drawing the distortion meshes.
    // The eye rendering path flushes per camera and leaves the renderer state
    // restored to the default render target, but the viewport should still be
    // explicitly reset for the final screen-space distortion pass.
    auto defaultVP = Camera::getDefaultViewport();
    renderer->addCallbackCommand(
        [=]() { renderer->setViewport(defaultVP.x, defaultVP.y, defaultVP.width, defaultVP.height); });

    // Submit distortion draw commands for both eyes. The scene has already been
    // rendered into the VR render texture; these meshes sample that texture and
    // present the final distorted image to the screen.
    renderer->addCommand(&_leftEyeCmd);
    renderer->addCommand(&_rightEyeCmd);

    renderer->render();
}

void VRGenericRenderer::renderSceneToEye(Renderer* renderer, Scene* scene, EyeIndex eyeIndex, const Mat4& eyeTransform)
{
    AXASSERT(renderer, "Invalid Renderer");
    AXASSERT(scene, "Invalid Scene");

    auto& eye = _eyes[eyeIndex];

    const auto& transform = scene->getNodeToParentTransform();
    for (const auto& camera : scene->getCameras())
    {
        if (!camera->isVisible())
            continue;

        _rtPass->setViewport(Viewport(static_cast<int>(eye.viewport.x), static_cast<int>(eye.viewport.y),
                                      static_cast<int>(eye.viewport.w), static_cast<int>(eye.viewport.h)));
        _rtPass->begin();

        _rtPass->clear(ClearFlag::DEPTH_AND_STENCIL, {});

        Camera::setVisitingCamera(camera);

        // VR rendering reuses the original scene cameras for both eyes.
        // Each eye applies a temporary view override through Camera::setAdditionalTransform(),
        // avoiding duplicated camera objects and keeping camera ownership in the scene.
        camera->setAdditionalTransform(eyeTransform.getInversed());

        // For VR rendering, call updateViewProjectionState() instead of
        // camera->apply(). Axmol currently uses a shared default viewport for
        // all scene cameras, and camera->apply() would reapply that viewport.
        // The per-eye viewport has already been set by RenderTexturePass.
        camera->updateViewProjectionState();

        // Scissor changes are executed by renderer commands, not immediately
        // during scene traversal. Queue push/pop callbacks around the scene
        // commands so clipped UI is transformed at execution time.
        renderer->addCallbackCommand(
            [this, eyeIndex]() { _scissorTransformStack.push(_eyes[eyeIndex].scissorTransform); });
        scene->visit(renderer, transform, 0);
        renderer->addCallbackCommand([this]() { _scissorTransformStack.pop(); });

        _rtPass->end();

        // Flush per camera to preserve SceneRenderer's camera boundary semantics,
        // while keeping each flush fully bracketed by RenderTexturePass begin/end.
        renderer->render();
    }
}

void VRGenericRenderer::clearEyeRenderTexture(Renderer* renderer, const Color& clearColor)
{
    const auto rtSize = _renderTexture->getPixelSize();
    _rtPass->setViewport(Viewport(0, 0, static_cast<int>(rtSize.width), static_cast<int>(rtSize.height)));
    _rtPass->begin();
    _rtPass->clear(ClearFlag::COLOR | ClearFlag::DEPTH_AND_STENCIL, {.color = clearColor});
    _rtPass->end();
    renderer->render();
}

void VRGenericRenderer::setScissorRect(float x, float y, float w, float h)
{
    _sourceScissorRect.set(x, y, w, h);

    if (!_scissorTransformStack.empty())
    {
        const auto& xf = _scissorTransformStack.top();
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

DistortionMesh* VRGenericRenderer::createDistortionMesh(VREye::EyeType eyeType, const Size& outputSize)
{
    const float screenWidth      = outputSize.width;
    const float screenHeight     = outputSize.height;
    const float xEyeOffsetScreen = (eyeType == VREye::EyeType::LEFT) ? screenWidth / 4 : screenWidth * 3 / 4;
    const float yEyeOffsetScreen = screenHeight / 2;

    const float textureWidth      = outputSize.width;
    const float textureHeight     = outputSize.height;
    const float xEyeOffsetTexture = (eyeType == VREye::EyeType::LEFT) ? outputSize.width / 4 : outputSize.width * 3 / 4;
    const float yEyeOffsetTexture = outputSize.height / 2;

    const float viewportX = (eyeType == VREye::EyeType::LEFT) ? 0 : textureWidth / 2;
    const float viewportY = 0;
    const float viewportW = textureWidth / 2;
    const float viewportH = textureHeight;

    return new DistortionMesh(_distortion, screenWidth, screenHeight, xEyeOffsetScreen, yEyeOffsetScreen, textureWidth,
                              textureHeight, xEyeOffsetTexture, yEyeOffsetTexture, viewportX, viewportY, viewportW,
                              viewportH, _vignetteEnabled);
}

void VRGenericRenderer::setupDistortionProgram()
{
    auto program = axpm->loadProgram("vr_vs", "vr_fs");

    _programState = new rhi::ProgramState(program);

    auto desc = axvlm->allocateVertexLayoutDesc();

    desc.startLayout(3);

    desc.addAttrib("a_position", program->getVertexInputDesc("a_position"), rhi::VertexFormat::FLOAT2, 0, false);
    desc.addAttrib("a_texCoord", program->getVertexInputDesc("a_texCoord"), rhi::VertexFormat::FLOAT2, sizeof(Vec2),
                   false);
    desc.addAttrib("a_vignette", program->getVertexInputDesc("a_vignette"), rhi::VertexFormat::FLOAT,
                   sizeof(Vec2) + sizeof(Vec2), false);

    desc.endLayout();

    Object::assign(_vertexLayout, axvlm->getVertexLayout(std::move(desc)));

    _leftEyeCmd.setWeakPSVL(_programState, _vertexLayout);
    _rightEyeCmd.setWeakPSVL(_programState, _vertexLayout);
}

}  // namespace experimental
}  // namespace ax
