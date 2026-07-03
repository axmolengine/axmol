/****************************************************************************
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

#include "axmol/vr/VRSceneCompositor.h"

#include <algorithm>
#include <cmath>
#include <vector>

#include "axmol/base/Director.h"
#include "axmol/base/Logging.h"
#include "axmol/math/Vec4.h"
#include "axmol/renderer/ProgramManager.h"
#include "axmol/renderer/Renderer.h"
#include "axmol/renderer/RenderTexturePass.h"
#include "axmol/rhi/ProgramState.h"
#include "axmol/rhi/RenderTarget.h"
#include "axmol/scene/Camera.h"
#include "axmol/scene/Scene.h"
#include "axmol/platform/RenderView.h"

namespace ax
{
inline namespace experimental
{

static VRScissorTransform makeVRScissorTransform(float eyeWidth, float eyeHeight, const Size& sourceSize)
{
    VRScissorTransform xf;
    if (sourceSize.width <= 0.0f || sourceSize.height <= 0.0f)
        return xf;

    xf.sx = eyeWidth / sourceSize.width;
    xf.sy = eyeHeight / sourceSize.height;
    return xf;
}

static XrPosef scaleXrPosePosition(const XrPosef& pose, float scale)
{
    XrPosef scaledPose = pose;
    scaledPose.position.x *= scale;
    scaledPose.position.y *= scale;
    scaledPose.position.z *= scale;
    return scaledPose;
}

struct VRProjectedPoint
{
    Vec4 clip;
    Vec3 ndc;
};

static VRProjectedPoint projectControllerRayPoint(const Mat4& mvp, const Vec3& point)
{
    VRProjectedPoint projected;
    projected.clip.set(point.x, point.y, point.z, 1.0f);
    mvp.transformVector(&projected.clip);

    if (std::abs(projected.clip.w) > 0.000001f)
    {
        projected.ndc.set(projected.clip.x / projected.clip.w, projected.clip.y / projected.clip.w,
                          projected.clip.z / projected.clip.w);
    }

    return projected;
}

VRSceneCompositor::VRSceneCompositor()
{
    _context.setXrToSceneScale(_xrToSceneScale);
    _rtPass = RefPtr<RenderTexturePass>(RenderTexturePass::obtain(), tlx::adopt_object);
    _rtPass->setCameraOverrideEnabled(false);
}

VRSceneCompositor::~VRSceneCompositor()
{
    shutdownControllerRayResources();
}

bool VRSceneCompositor::isInitialized() const
{
    return _context.isInitialized();
}

XrInstance VRSceneCompositor::getXrInstance() const
{
    return _context.getXrInstance();
}

XrSession VRSceneCompositor::getXrSession() const
{
    return _context.getXrSession();
}

void VRSceneCompositor::prepareFrame()
{
#ifndef NDEBUG
    if (auto renderView = _director->getRenderView())
        renderView->pollEvents();
#endif

    _context.setXrToSceneScale(_xrToSceneScale);
    _context.ensurePointerRayCamera(_nearZ, _farZ);
    _context.pollEvents();
}

bool VRSceneCompositor::isVRActive() const
{
    return _context.isSessionRunning();
}

void VRSceneCompositor::onRenderViewChanged(RenderViewCore* rv)
{
    _context.onRenderViewChanged(rv);
}

void VRSceneCompositor::setControllerRayColors(const Color& idle, const Color& pressed, const Color& hit)
{
    _controllerRayIdleColor    = idle;
    _controllerRayPressedColor = pressed;
    _controllerRayHitColor     = hit;
}

void VRSceneCompositor::setXrToSceneScale(float scale)
{
    _xrToSceneScale = scale > 0.0f ? scale : 1.0f;
    _context.setXrToSceneScale(_xrToSceneScale);
}

void VRSceneCompositor::ensureControllerRayResources()
{
    if (_controllerRayResourcesInitialized)
        return;

    auto program = axpm->getBuiltinProgram(rhi::ProgramType::POSITION_COLOR);
    if (!program)
        return;

    _controllerRayMVPLocation = program->getUniformLocation("u_MVPMatrix");

    for (auto& command : _controllerRayCommands)
    {
        command.setPrimitiveType(CustomCommand::PrimitiveType::LINE);
        command.setDrawType(CustomCommand::DrawType::ARRAY);
        command.setBeforeCallback(AX_CALLBACK_0(VRSceneCompositor::onBeforeControllerRayDraw, this));
        command.setAfterCallback(AX_CALLBACK_0(VRSceneCompositor::onAfterControllerRayDraw, this));
        command.setOwnPSVL(new rhi::ProgramState(program), program->getVertexLayout(), RenderCommand::ADOPT_FLAG_PS);

        auto& blend                  = command.blendDesc();
        blend.blendEnabled           = true;
        blend.sourceAlphaBlendFactor = blend.sourceRGBBlendFactor = rhi::BlendFactor::SRC_ALPHA;
        blend.destinationAlphaBlendFactor = blend.destinationRGBBlendFactor = rhi::BlendFactor::ONE_MINUS_SRC_ALPHA;
    }

    _controllerRayResourcesInitialized = true;
}

void VRSceneCompositor::shutdownControllerRayResources()
{
    if (!_controllerRayResourcesInitialized)
        return;

    for (auto& command : _controllerRayCommands)
        command.releasePSVL();

    _controllerRayResourcesInitialized = false;
    _controllerRayMVPLocation.reset();
}

void VRSceneCompositor::onBeforeControllerRayDraw()
{
    auto* renderer = Director::getInstance()->getRenderer();
    if (!renderer)
        return;

    _controllerRayOldDepthTest  = renderer->getDepthTest();
    _controllerRayOldDepthWrite = renderer->getDepthWrite();
    renderer->setDepthTest(false);
    renderer->setDepthWrite(false);
}

void VRSceneCompositor::onAfterControllerRayDraw()
{
    auto* renderer = Director::getInstance()->getRenderer();
    if (!renderer)
        return;

    renderer->setDepthTest(_controllerRayOldDepthTest);
    renderer->setDepthWrite(_controllerRayOldDepthWrite);
}

void VRSceneCompositor::drawControllerRays(Renderer* renderer, uint32_t eyeIdx, const XrView& view)
{
    if (!_controllerRayVisible || !renderer)
        return;

    ensureControllerRayResources();
    if (!_controllerRayResourcesInitialized)
        return;

    auto camera = Camera::getVisitingCamera();
    if (!camera)
        return;

    const auto* controllers = _context.getControllers();
    const Mat4& mvp         = camera->getViewProjectionMatrix();

    for (uint32_t hand = 0; hand < 2; ++hand)
    {
        const auto& ctrl = controllers[hand];
        if (!ctrl.poseValid)
            continue;

        auto& command = _controllerRayCommands[hand];
        V3F_C4F vertices[6];
        size_t vertexCount = 0;

        const float sceneScale     = std::max(1.0f, _director->getZEye());
        const float sceneRayLength = _controllerRayLength * sceneScale;
        const Vec3 start           = ctrl.visualRayStartValid ? ctrl.visualRayStart : ctrl.currentRay.origin;
        const Vec3 maxEnd          = start + ctrl.currentRay.direction * sceneRayLength;
        const Vec3 reticlePoint    = ctrl.rayHitValid ? ctrl.rayHitPoint : maxEnd;
        const Vec3 end             = reticlePoint;
        const Color rayColor       = ctrl.triggerPressed ? _controllerRayPressedColor : _controllerRayIdleColor;

        vertices[vertexCount++] = {start, rayColor};
        vertices[vertexCount++] = {end, rayColor};

        if (ctrl.rayHitValid)
        {
            const float reticleRadius = std::max(0.025f * sceneScale, 3.0f);
            vertices[vertexCount++]   = {reticlePoint + Vec3(-reticleRadius, 0.0f, 0.0f), _controllerRayHitColor};
            vertices[vertexCount++]   = {reticlePoint + Vec3(reticleRadius, 0.0f, 0.0f), _controllerRayHitColor};
            vertices[vertexCount++]   = {reticlePoint + Vec3(0.0f, -reticleRadius, 0.0f), _controllerRayHitColor};
            vertices[vertexCount++]   = {reticlePoint + Vec3(0.0f, reticleRadius, 0.0f), _controllerRayHitColor};
        }

        if (command.getVertexCapacity() < vertexCount)
            command.createVertexBuffer(sizeof(V3F_C4F), vertexCount, CustomCommand::BufferUsage::DYNAMIC);

        command.unsafePS()->setUniform(_controllerRayMVPLocation, mvp.m, sizeof(mvp.m));
        command.updateVertexBuffer(vertices, vertexCount * sizeof(V3F_C4F));
        command.setVertexDrawInfo(0, vertexCount);
        command.init(0.0f);
        renderer->addCommand(&command);
    }
}

void VRSceneCompositor::renderScene(Renderer* renderer, Scene* scene)
{
    if (!_context.isSessionRunning() || !_context.isInitialized())
    {
        SceneCompositor::renderScene(renderer, scene);
        return;
    }

    // For debugging, don't remove
#ifndef NDEBUG
    SceneCompositor::renderScene(renderer, scene);
#endif

    if (!_context.beginRenderFrame())
        return;

    uint32_t viewCountOutput = 0;
    if (!_context.locateViews(viewCountOutput))
        return;

    std::vector<OpenXRContext::AcquiredSwapchain> acquired;
    if (!_context.acquireSwapchains(acquired))
    {
        _context.endFrameEmpty();
        return;
    }

    const Color& clearColor       = _director->getClearColor();
    const auto& transform         = scene->getNodeToParentTransform();
    const auto sourceScissorSize  = _director->getRenderView()->getViewportRect().size;
    const auto& views             = _context.getViews();
    const uint32_t eyeRenderCount = std::min<uint32_t>(viewCountOutput, static_cast<uint32_t>(acquired.size()));

    for (uint32_t eyeIdx = 0; eyeIdx < eyeRenderCount; ++eyeIdx)
    {
        const auto& swapchain       = acquired[eyeIdx];
        const XrView& view          = views[eyeIdx];
        const float eyeW            = static_cast<float>(swapchain.width);
        const float eyeH            = static_cast<float>(swapchain.height);
        const auto scissorTransform = makeVRScissorTransform(eyeW, eyeH, sourceScissorSize);

        _rtPass->setTarget(swapchain.renderTarget);
        _rtPass->setViewport(Viewport(0, 0, static_cast<int>(eyeW), static_cast<int>(eyeH)));

        Mat4 eyeTransform = OpenXRContext::xrPoseToMat4(scaleXrPosePosition(view.pose, _xrToSceneScale));

        int passCount          = 0;
        bool renderedScenePass = false;
        for (const auto& camera : scene->getCameras())
        {
            if (!camera->isVisible())
                continue;

            Camera::setVisitingCamera(camera);

            const Mat4 originalProjection = camera->getProjectionMatrix();
            camera->setAdditionalTransform(eyeTransform);
            camera->setProjectionMatrix(OpenXRContext::xrFovToProjection(view.fov, _nearZ, _farZ));
            camera->updateViewProjectionState();

            _rtPass->begin();
            if (passCount++ == 0)
                _rtPass->clear(ClearFlag::COLOR | ClearFlag::DEPTH_AND_STENCIL,
                               {.color = clearColor, .depth = 1.0f, .stencil = 0});
            else
                _rtPass->clear(ClearFlag::DEPTH_AND_STENCIL, {.depth = 1.0f, .stencil = 0});
            renderer->addCallbackCommand([this, scissorTransform]() { _scissorTransformStack.push(scissorTransform); });
            scene->visit(renderer, transform, 0);
            renderer->addCallbackCommand([this]() {
                if (!_scissorTransformStack.empty())
                    _scissorTransformStack.pop();
            });
            _rtPass->end();

            renderer->render();
            renderedScenePass = true;

            camera->setProjectionMatrix(originalProjection);
            camera->setAdditionalTransform(Mat4::identity);
        }

        auto rayCamera = _context.ensurePointerRayCamera(_nearZ, _farZ);
        if (rayCamera && rayCamera->isVisible())
        {
            Camera::setVisitingCamera(rayCamera);

            const Mat4 originalProjection = rayCamera->getProjectionMatrix();
            rayCamera->setAdditionalTransform(eyeTransform);
            rayCamera->setProjectionMatrix(OpenXRContext::xrFovToProjection(view.fov, _nearZ, _farZ));
            rayCamera->updateViewProjectionState();

            _rtPass->begin();
            drawControllerRays(renderer, eyeIdx, view);
            _rtPass->end();
            renderer->render();

            rayCamera->setProjectionMatrix(originalProjection);
            rayCamera->setAdditionalTransform(Mat4::identity);
            Camera::setVisitingCamera(nullptr);
        }
    }

    Camera::setVisitingCamera(nullptr);

    renderer->submitCurrentFrameCommands(true);
    _context.releaseSwapchains(acquired);
    _context.endFrameWithProjectionLayer(acquired, eyeRenderCount);
}

void VRSceneCompositor::setScissorRect(float x, float y, float w, float h)
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

    _director->getRenderer()->setScissorRect(x, y, w, h);
}

const ScissorRect& VRSceneCompositor::getScissorRect() const
{
    return _sourceScissorRect;
}

}  // namespace experimental
}  // namespace ax
