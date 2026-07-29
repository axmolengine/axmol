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
#include <array>
#include <cmath>
#include <vector>

#include "axmol/base/Director.h"
#include "axmol/renderer/ProgramManager.h"
#include "axmol/renderer/Renderer.h"
#include "axmol/renderer/RenderTexturePass.h"
#include "axmol/rhi/ProgramState.h"
#include "axmol/scene/Camera.h"
#include "axmol/scene/Node.h"
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

static Vec3 sampleCubicBezier(const Vec3& p0, const Vec3& p1, const Vec3& p2, const Vec3& p3, float t)
{
    const float oneMinusT  = 1.0f - t;
    const float oneMinusT2 = oneMinusT * oneMinusT;
    const float t2         = t * t;
    return p0 * (oneMinusT2 * oneMinusT) + p1 * (3.0f * oneMinusT2 * t) +
           p2 * (3.0f * oneMinusT * t2) + p3 * (t2 * t);
}

VRSceneCompositor::VRSceneCompositor()
{
    _rtPass = RefPtr<RenderTexturePass>(RenderTexturePass::obtain(), tlx::adopt_object);
    _rtPass->setCameraOverrideEnabled(false);
}

VRSceneCompositor::~VRSceneCompositor()
{
    if (_xrDriver)
        _xrDriver->setCompositorAlive(false);
    shutdownControllerRayResources();
}

bool VRSceneCompositor::isInitialized() const
{
    return _xrDriver && _xrDriver->isInitialized();
}

XrInstance VRSceneCompositor::getXrInstance() const
{
    return _xrDriver ? _xrDriver->getXrInstance() : XR_NULL_HANDLE;
}

XrSession VRSceneCompositor::getXrSession() const
{
    return _xrDriver ? _xrDriver->getXrSession() : XR_NULL_HANDLE;
}

void VRSceneCompositor::setXrDriver(OpenXRDriver* context)
{
    if (_xrDriver)
        _xrDriver->setCompositorAlive(false);
    _xrDriver = context;
    if (_xrDriver)
        _xrDriver->setCompositorAlive(true);
}

void VRSceneCompositor::pollEvents()
{
    auto xrDriver = _xrDriver;
    SceneCompositor::pollEvents();

    if (!xrDriver || !xrDriver->isCompositorAlive())
        return;

    xrDriver->setXrToSceneScale(_xrToSceneScale);
    xrDriver->pollEvents();
}

void VRSceneCompositor::resolveXrFrameInput(Scene* scene)
{
    if (!_xrDriver || !_xrDriver->isCompositorAlive())
        return;

    Camera* xrViewCamera       = scene ? resolveXrViewCamera(scene) : nullptr;
    _frameTrackingToWorld      = xrViewCamera ? xrViewCamera->getNodeToWorldTransform() : Mat4::identity;
    _frameTrackingToWorldValid = xrViewCamera != nullptr;

    _xrDriver->setXrToSceneScale(_xrToSceneScale);
    const float sceneRayMaxDistance = _controllerRayLength > 0.0f ? _controllerRayLength : _farZ;
    _xrDriver->resolveControllerPointers(_frameTrackingToWorld, sceneRayMaxDistance);
}

bool VRSceneCompositor::isVRActive() const
{
    return _xrDriver && _xrDriver->isSessionRunning();
}

void VRSceneCompositor::onRenderViewChanged(RenderViewCore* rv)
{
    SceneCompositor::onRenderViewChanged(rv);
    if (_xrDriver)
        _xrDriver->onRenderViewChanged(rv);
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
    if (_xrDriver)
        _xrDriver->setXrToSceneScale(_xrToSceneScale);
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

Camera* VRSceneCompositor::resolveXrViewCamera(Scene* scene) const
{
    if (!scene)
        return nullptr;

    Camera* fallback = nullptr;
    for (auto* camera : scene->getCameras())
    {
        if (!camera || !camera->isVisible())
            continue;

        if (!fallback)
            fallback = camera;

        if (camera->getCameraMode() != CameraMode::Classic)
            return camera;
    }

    return fallback ? fallback : scene->getDefaultCamera();
}

void VRSceneCompositor::drawControllerRays(Renderer* renderer, const SceneViewData& view)
{
    if (!_controllerRayVisible || !renderer || !_xrDriver)
        return;

    ensureControllerRayResources();
    if (!_controllerRayResourcesInitialized)
        return;

    const auto* controllers = _xrDriver->getControllers();
    if (!controllers)
        return;

    // _controllerRayLength is a scene-world visual cap. Input rays themselves
    // are already scene-world rays.
    const float visualMaxDistance = _controllerRayLength > 0.0f ? _controllerRayLength : _farZ;

    for (uint32_t hand = 0; hand < 2; ++hand)
    {
        const auto& ctrl = controllers[hand];
        if (!ctrl.poseValid)
            continue;

        auto& command = _controllerRayCommands[hand];

        constexpr size_t curvePointCount    = 16;
        constexpr size_t curveVertexCount   = (curvePointCount - 1) * 2;
        constexpr size_t reticleVertexCount = 4;
        std::array<V3F_C4F, curveVertexCount + reticleVertexCount> vertices;
        size_t vertexCount = 0;

        const bool useSubtleCurve = _controllerRayVisualStyle == ControllerRayVisualStyle::SubtleCurve;
        const Ray& visualSourceRay =
            useSubtleCurve && ctrl.rawWorldRayValid ? ctrl.rawWorldRay : ctrl.currentRay;
        const Vec3 rayOrigin = visualSourceRay.origin;
        const Vec3 rayEnd    = ctrl.rayHitValid
                                   ? ctrl.rayHitPoint
                                   : ctrl.currentRay.origin + ctrl.currentRay.direction * visualMaxDistance;

        const Color rayColor = ctrl.triggerPressed ? _controllerRayPressedColor : _controllerRayIdleColor;

        const float lineLength = rayOrigin.distance(rayEnd);
        if (lineLength > 1e-5f)
        {
            if (useSubtleCurve)
            {
                constexpr float hitCurveRatio       = 0.35f;
                constexpr float freeCurveRatio      = 0.18f;
                constexpr float curveBlend          = 0.45f;
                const float curveRatio              = ctrl.rayHitValid ? hitCurveRatio : freeCurveRatio;
                const float maxControlDistance      = std::max(1.0f, visualMaxDistance * 0.2f);
                const float controlDistance         = std::min(lineLength * curveRatio, maxControlDistance);
                const Vec3 quadraticControl         = rayOrigin + visualSourceRay.direction * controlDistance;
                const Vec3 endpointVector           = rayEnd - rayOrigin;

                // Keep the Unity-like controller curve subtle; hit-test still uses ctrl.currentRay.
                const Vec3 control1 = (rayOrigin + quadraticControl * 2.0f) / 3.0f;
                const Vec3 control2 = (quadraticControl * 2.0f + rayEnd) / 3.0f;

                Vec3 previousPoint = rayOrigin;
                for (size_t pointIndex = 1; pointIndex < curvePointCount; ++pointIndex)
                {
                    const float t =
                        static_cast<float>(pointIndex) / static_cast<float>(curvePointCount - 1);
                    const Vec3 straightPoint = rayOrigin + endpointVector * t;
                    const Vec3 curvePoint = sampleCubicBezier(rayOrigin, control1, control2, rayEnd, t);
                    const Vec3 point = straightPoint + (curvePoint - straightPoint) * curveBlend;
                    vertices[vertexCount++] = {previousPoint, rayColor};
                    vertices[vertexCount++] = {point, rayColor};
                    previousPoint           = point;
                }
            }
            else
            {
                vertices[vertexCount++] = {rayOrigin, rayColor};
                vertices[vertexCount++] = {rayEnd, rayColor};
            }
        }

        if (ctrl.rayHitValid)
        {
            const Vec3& center = rayEnd;

            Vec3 facing = view.position - center;
            if (facing.lengthSquared() < 1e-8f)
                facing = -ctrl.currentRay.direction;
            facing.normalize();

            const Vec3 upReference =
                std::abs(facing.dot(Vec3::yAxis)) > 0.95f ? Vec3::xAxis : Vec3::yAxis;

            Vec3 reticleRight;
            Vec3::cross(upReference, facing, &reticleRight);
            if (reticleRight.lengthSquared() < 1e-8f)
                reticleRight = Vec3::xAxis;
            reticleRight.normalize();

            Vec3 reticleUp;
            Vec3::cross(facing, reticleRight, &reticleUp);
            reticleUp.normalize();

            constexpr float reticleAngularRadius = 0.008f;
            const float viewDistance             = std::max(view.position.distance(center), 0.001f);
            const float reticleRadius            = viewDistance * std::tan(reticleAngularRadius);

            const Vec3 rightOffset = reticleRight * reticleRadius;
            const Vec3 upOffset    = reticleUp * reticleRadius;

            vertices[vertexCount++] = {
                center - rightOffset,
                _controllerRayHitColor,
            };
            vertices[vertexCount++] = {
                center + rightOffset,
                _controllerRayHitColor,
            };
            vertices[vertexCount++] = {
                center - upOffset,
                _controllerRayHitColor,
            };
            vertices[vertexCount++] = {
                center + upOffset,
                _controllerRayHitColor,
            };
        }

        if (vertexCount == 0)
            continue;

        if (command.getVertexCapacity() < vertexCount)
        {
            command.createVertexBuffer(sizeof(V3F_C4F), vertexCount, CustomCommand::BufferUsage::DYNAMIC);
        }

        command.unsafePS()->setUniform(_controllerRayMVPLocation, view.viewProjection.m, sizeof(view.viewProjection.m));

        command.updateVertexBuffer(vertices.data(), vertexCount * sizeof(V3F_C4F));

        command.setVertexDrawInfo(0, vertexCount);
        command.init(0.0f);

        renderer->addCommand(&command);
    }
}

void VRSceneCompositor::renderScene(Renderer* renderer, Scene* scene)
{
    if (!_xrDriver || !_xrDriver->isSessionRunning() || !_xrDriver->isInitialized())
    {
        SceneCompositor::renderScene(renderer, scene);
        return;
    }

    resolveXrFrameInput(scene);

    // For debugging, don't remove
#ifndef NDEBUG
    SceneCompositor::renderScene(renderer, scene);
#endif

    if (!_xrDriver->beginRenderFrame())
        return;

    uint32_t viewCountOutput = 0;
    if (!_xrDriver->locateViews(viewCountOutput))
        return;

    std::vector<OpenXRDriver::AcquiredSwapchain> acquired;
    if (!_xrDriver->acquireSwapchains(acquired))
    {
        _xrDriver->endFrameEmpty();
        return;
    }

    const Color& clearColor       = _director->getClearColor();
    const auto& transform         = scene->getNodeToParentTransform();
    const auto sourceScissorSize  = _director->getRenderView()->getViewportRect().size;
    const auto& views             = _xrDriver->getViews();
    const uint32_t eyeRenderCount = std::min<uint32_t>(viewCountOutput, static_cast<uint32_t>(acquired.size()));
    const Mat4 trackingToWorld = _frameTrackingToWorld;

    for (uint32_t eyeIdx = 0; eyeIdx < eyeRenderCount; ++eyeIdx)
    {
        const auto& swapchain       = acquired[eyeIdx];
        const XrView& view          = views[eyeIdx];
        const float eyeW            = static_cast<float>(swapchain.width);
        const float eyeH            = static_cast<float>(swapchain.height);
        const auto scissorTransform = makeVRScissorTransform(eyeW, eyeH, sourceScissorSize);

        _rtPass->setTarget(swapchain.renderTarget);
        _rtPass->setViewport(Viewport(0, 0, static_cast<int>(eyeW), static_cast<int>(eyeH)));

        const Mat4 eyeToTracking =
            OpenXRDriver::xrPoseToMat4(scaleXrPosePosition(view.pose, _xrToSceneScale));
        const Mat4 projection = OpenXRDriver::xrFovToProjection(view.fov, _nearZ, _farZ);
        const Mat4 primaryEyeToWorld = trackingToWorld * eyeToTracking;
        const Mat4 primaryEyeView    = primaryEyeToWorld.getInversed();

        Vec3 primaryEyePosition = Vec3::zero;
        primaryEyeToWorld.getTranslation(&primaryEyePosition);
        const auto primaryEyeViewData = SceneViewData::fromMatrices(primaryEyeView, projection, primaryEyePosition);

        int passCount = 0;

        for (const auto& camera : scene->getCameras())
        {
            if (!camera || !camera->isVisible())
                continue;

            const Mat4 cameraEyeToWorld = camera->getNodeToWorldTransform() * eyeToTracking;
            const Mat4 cameraEyeView    = cameraEyeToWorld.getInversed();

            Vec3 cameraEyePosition = Vec3::zero;
            cameraEyeToWorld.getTranslation(&cameraEyePosition);
            const auto cameraEyeViewData =
                SceneViewData::fromMatrices(cameraEyeView, projection, cameraEyePosition);

            SceneRenderState renderState(renderer, camera, cameraEyeViewData);

            _rtPass->begin();

            if (passCount++ == 0)
            {
                _rtPass->clear(ClearFlag::COLOR | ClearFlag::DEPTH_AND_STENCIL,
                               {.color = clearColor, .depth = 1.0f, .stencil = 0});
                camera->clearBackground(renderState);
            }
            else
            {
                _rtPass->clear(ClearFlag::DEPTH_AND_STENCIL, {.depth = 1.0f, .stencil = 0});
            }

            renderer->addCallbackCommand([this, scissorTransform]() { _scissorTransformStack.push(scissorTransform); });

            scene->visit(renderState, transform, 0);

            renderer->addCallbackCommand([this]() {
                if (!_scissorTransformStack.empty())
                    _scissorTransformStack.pop();
            });

            _rtPass->end();
            renderer->render();
        }

        if (_controllerRayVisible)
        {
            _rtPass->begin();
            drawControllerRays(renderer, primaryEyeViewData);
            _rtPass->end();
            renderer->render();
        }
    }

    renderer->submitCurrentFrameCommands(true);
    _xrDriver->releaseSwapchains(acquired);
    _xrDriver->endFrameWithProjectionLayer(acquired, eyeRenderCount);
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
