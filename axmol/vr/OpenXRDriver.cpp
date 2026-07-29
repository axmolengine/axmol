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

#include "axmol/vr/OpenXRDriver.h"

#include <string.h>
#include <stdio.h>
#include <algorithm>
#include <cmath>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "axmol/renderer/Renderer.h"
#include "axmol/renderer/RenderTexturePass.h"
#include "axmol/renderer/Texture2D.h"
#include "axmol/renderer/ProgramManager.h"
#include "axmol/rhi/GraphicsCore.h"
#include "axmol/rhi/ProgramState.h"
#include "axmol/rhi/RenderTarget.h"
#include "axmol/rhi/Texture.h"
#include "axmol/base/Director.h"
#include "axmol/base/Logging.h"
#include "axmol/base/InputSystem.h"
#include "axmol/math/Quat.h"
#include "axmol/scene/Camera.h"
#include "axmol/scene/Scene.h"
#include "axmol/platform/RenderView.h"
#include "axmol/platform/Application.h"

// Backend-specific headers (must be included before openxr_platform.h
// because openxr_platform.h uses types like ID3D11Device, VkInstance, etc.
// without including their headers.)
#if AX_ENABLE_D3D11 && defined(_WIN32)
#    include "axmol/rhi/d3d11/Driver11.h"
#    include "axmol/rhi/d3d11/Texture11.h"
#endif
#if AX_ENABLE_D3D12 && defined(_WIN32)
#    include "axmol/rhi/d3d12/Driver12.h"
#    include "axmol/rhi/d3d12/Texture12.h"
#endif
#if AX_ENABLE_VK
#    include "axmol/rhi/vulkan/DriverVK.h"
#    include "axmol/rhi/vulkan/TextureVK.h"
#endif
#if AX_ENABLE_GL
#    include "axmol/rhi/opengl/TextureGL.h"
#endif
#if AX_ENABLE_GL && AX_GLES_PROFILE && defined(__ANDROID__)
#    include "axmol/platform/GL.h"
#endif

#if AX_ENABLE_VK
#    include "axmol/vr/OpenXRVulkanInterop.h"
#endif

// Define graphics API usage for OpenXR platform types.
// These must be set before including openxr_platform.h so that it provides
// the correct backend-specific types (XrGraphicsBindingD3D11KHR,
// XrSwapchainImageVulkanKHR, etc.).
#if defined(__ANDROID__)
#    define XR_USE_PLATFORM_ANDROID
#endif
#if AX_ENABLE_D3D11 && defined(_WIN32)
#    define XR_USE_GRAPHICS_API_D3D11
#endif
#if AX_ENABLE_D3D12 && defined(_WIN32)
#    define XR_USE_GRAPHICS_API_D3D12
#endif
#if AX_ENABLE_VK
#    define XR_USE_GRAPHICS_API_VULKAN
#endif
#if AX_ENABLE_GL && AX_GLES_PROFILE && defined(__ANDROID__)
#    define XR_USE_GRAPHICS_API_OPENGL_ES
#endif

#include "openxr/openxr_platform.h"

namespace ax
{
inline namespace experimental
{
static bool checkXr(XrResult result, const char* operation)
{
    if (XR_SUCCEEDED(result))
        return true;

    switch (result)
    {
    case XR_ERROR_RUNTIME_FAILURE:
        AXLOGW("[OpenXR] VR runtime is not available.");
        break;
    case XR_ERROR_FORM_FACTOR_UNAVAILABLE:
        AXLOGW("[OpenXR] HMD is not available.");
        break;
    default:
        AXLOGW("[OpenXR] {} failed, ec:{}", operation, static_cast<int>(result));
    }
    return false;
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static XrPosef makeXrPoseIdentity()
{
    XrPosef pose{};
    pose.orientation.w = 1.0f;
    return pose;
}

static XrPosef scaleXrPosePosition(const XrPosef& pose, float scale)
{
    XrPosef scaledPose = pose;
    scaledPose.position.x *= scale;
    scaledPose.position.y *= scale;
    scaledPose.position.z *= scale;
    return scaledPose;
}

static XrExtent2Di makeXrRect(uint32_t w, uint32_t h)
{
    XrExtent2Di r;
    r.width  = static_cast<int>(w);
    r.height = static_cast<int>(h);
    return r;
}

struct TextureFormat
{
    rhi::PixelFormat pixelFormat{rhi::PixelFormat::NONE};
    rhi::ColorSpace colorSpace{rhi::ColorSpace::Linear};
};

/*
 * axmol only support Linear color space for now, so we treat SRGB as Linear
 * even though openxr not provide linear color space format, and vulkan validation layer will report error if we use
 * Linear RGBA format for swapchain image, but it still works fine, so we just ignore the validation layer error for
 * now.
 */
static bool xrFormatToTextureFormat(int64_t format, TextureFormat& outFormat)
{
#if (AX_ENABLE_D3D11 || AX_ENABLE_D3D12) && defined(_WIN32)
    switch (format)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        outFormat = {rhi::PixelFormat::RGBA8, rhi::ColorSpace::Linear};
        return true;
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        outFormat = {rhi::PixelFormat::BGRA8, rhi::ColorSpace::Linear};
        break;
    }
#endif

#if AX_ENABLE_VK
    switch (format)
    {
    case VK_FORMAT_R8G8B8A8_UNORM:
    case VK_FORMAT_R8G8B8A8_SRGB:
        outFormat = {rhi::PixelFormat::RGBA8, rhi::ColorSpace::Linear};
        return true;
    case VK_FORMAT_B8G8R8A8_UNORM:
    case VK_FORMAT_B8G8R8A8_SRGB:
        outFormat = {rhi::PixelFormat::BGRA8, rhi::ColorSpace::Linear};
        return true;
    }
#endif

#if AX_ENABLE_GL
    if (format == GL_RGBA8)
    {
        outFormat = {rhi::PixelFormat::RGBA8, rhi::ColorSpace::Linear};
        return true;
    }
#    if defined(GL_BGRA8_EXT)
    if (format == GL_BGRA8_EXT)
    {
        outFormat = {rhi::PixelFormat::BGRA8, rhi::ColorSpace::Linear};
        return true;
    }
#    endif
#endif

    return false;
}

// ---------------------------------------------------------------------------
// XrPosef -> axmol Mat4
//   XrPosef has orientation (quat) + position (vec3).
//   We build a pose matrix that transforms from pose-local space to the
//   OpenXR reference space.
// ---------------------------------------------------------------------------
Mat4 OpenXRDriver::xrPoseToMat4(const XrPosef& pose)
{
    Mat4 rot;
    Mat4::createRotation(Quat(pose.orientation.x, pose.orientation.y, pose.orientation.z, pose.orientation.w), &rot);

    Mat4 trans;
    Mat4::createTranslation(pose.position.x, pose.position.y, pose.position.z, &trans);

    return trans * rot;
}

// ---------------------------------------------------------------------------
// XrFovf -> axmol projection matrix
//   XrFovf contains the four tangent angles (left, right, up, down) of the
//   asymmetric frustum, relative to the eye.  This builds a classic
//   off-center / asymmetric perspective projection matrix.
// ---------------------------------------------------------------------------
Mat4 OpenXRDriver::xrFovToProjection(const XrFovf& fov, float nearZ, float farZ)
{
    Mat4 out;
    Mat4::createPerspectiveOffCenter(nearZ * std::tan(fov.angleLeft), nearZ * std::tan(fov.angleRight),
                                     nearZ * std::tan(fov.angleDown), nearZ * std::tan(fov.angleUp), nearZ, farZ, &out);

    return out;
}

// ---------------------------------------------------------------------------
// XrPosef -> Ray (for controller aiming)
//   The aim pose's orientation + position defines a ray in world space,
//   where -Z is the pointing direction (OpenXR convention).
// ---------------------------------------------------------------------------
Ray OpenXRDriver::xrPoseToRay(const XrPosef& pose)
{
    const Mat4 poseTransform = xrPoseToMat4(pose);

    Vec3 origin(Vec3::zero);
    poseTransform.transformPoint(&origin);

    Vec3 direction(0.0f, 0.0f, -1.0f);
    poseTransform.transformVector(&direction);
    direction.normalize();

    return Ray(origin, direction);
}

Vec2 OpenXRDriver::xrToVec2(const XrVector2f& v)
{
    return Vec2(v.x, v.y);
}

static Vec3 slerpDirection(const Vec3& from, const Vec3& to, float t)
{
    float cosTheta = std::clamp(from.dot(to), -1.0f, 1.0f);
    const float theta = std::acos(cosTheta);
    if (theta < 1e-5f)
        return from;

    const float sinTheta = std::sin(theta);
    if (std::abs(sinTheta) < 1e-5f)
    {
        Vec3 direction = from + (to - from) * t;
        direction.normalize();
        return direction;
    }

    Vec3 direction = from * (std::sin((1.0f - t) * theta) / sinTheta) + to * (std::sin(t * theta) / sinTheta);
    direction.normalize();
    return direction;
}

static Ray makeRayFromXrPose(const XrPosef& pose)
{
    const Mat4 poseTransform = OpenXRDriver::xrPoseToMat4(pose);

    Vec3 origin(Vec3::zero);
    poseTransform.transformPoint(&origin);

    Vec3 direction(0.0f, 0.0f, -1.0f);
    poseTransform.transformVector(&direction);
    direction.normalize();

    return Ray(origin, direction);
}

static float calculateStabilizedLerp(float distance, float deltaTime)
{
    constexpr float frameTime90Hz = 1.0f / 90.0f;

    if (distance >= 1.0f)
        return 1.0f;
    if (distance <= 0.0f)
        return 0.0f;

    const float doubleFrameLerp = distance - distance * distance;
    const float tripleFrameLerp = doubleFrameLerp * doubleFrameLerp;
    const float timeSlice       = deltaTime / frameTime90Hz;

    return distance * std::clamp(timeSlice, 0.0f, 1.0f) +
           doubleFrameLerp * std::clamp(timeSlice - 1.0f, 0.0f, 1.0f) +
           tripleFrameLerp * std::clamp(timeSlice - 2.0f, 0.0f, 1.0f);
}

static float directionAngle(const Vec3& from, const Vec3& to)
{
    return std::acos(std::clamp(from.dot(to), -1.0f, 1.0f));
}

static Ray stabilizeControllerRay(OpenXRDriver::ControllerState& ctrl, const Ray& rawRay, float deltaTime)
{
    constexpr float pi                          = 3.14159265358979323846f;
    constexpr float positionStabilizationMeters = 0.25f;
    constexpr float angleStabilizationRadians   = 20.0f * pi / 180.0f;

    if (!ctrl.stabilizedRayValid)
    {
        ctrl.stabilizedTrackingRay = rawRay;
        ctrl.stabilizedRayValid    = true;
        return ctrl.stabilizedTrackingRay;
    }

    const Ray previousRay = ctrl.stabilizedTrackingRay;

    const Vec3 positionOffset             = rawRay.origin - previousRay.origin;
    const float positionDistance          = positionOffset.length();
    const float directionDistance         = directionAngle(previousRay.direction, rawRay.direction);
    constexpr float directionResetRadians = 30.0f * pi / 180.0f;
    if (positionDistance >= positionStabilizationMeters || directionDistance >= directionResetRadians)
    {
        ctrl.stabilizedTrackingRay = rawRay;
        return ctrl.stabilizedTrackingRay;
    }

    const float positionLerp =
        calculateStabilizedLerp(positionDistance / positionStabilizationMeters, deltaTime);
    const Vec3 stabilizedOrigin = previousRay.origin + positionOffset * positionLerp;

    const float referenceDistance = std::max(ctrl.stabilizationReferenceDistance, 0.001f);
    const Vec3 previousEndpoint   = previousRay.origin + previousRay.direction * referenceDistance;

    Vec3 endpointPreservingDirection = previousEndpoint - stabilizedOrigin;
    if (endpointPreservingDirection.lengthSquared() < 1e-8f)
        endpointPreservingDirection = previousRay.direction;
    else
        endpointPreservingDirection.normalize();

    const float distanceScale     = 1.0f + std::log(std::max(referenceDistance, 1.0f));
    const float endpointAngleSpan = angleStabilizationRadians * std::clamp(distanceScale, 1.0f, 3.0f);
    const float directError       = directionAngle(previousRay.direction, rawRay.direction);
    const float endpointError     = directionAngle(endpointPreservingDirection, rawRay.direction);
    const float directRatio       = directError / angleStabilizationRadians;
    const float endpointRatio     = endpointError / endpointAngleSpan;

    Vec3 stabilizedDirection;
    if (endpointRatio < directRatio)
    {
        const float directionLerp = calculateStabilizedLerp(endpointRatio, deltaTime * distanceScale);
        stabilizedDirection = slerpDirection(endpointPreservingDirection, rawRay.direction, directionLerp);
    }
    else
    {
        const float directionLerp = calculateStabilizedLerp(directRatio, deltaTime * distanceScale);
        stabilizedDirection = slerpDirection(previousRay.direction, rawRay.direction, directionLerp);
    }

    ctrl.stabilizedTrackingRay = Ray(stabilizedOrigin, stabilizedDirection);

    return ctrl.stabilizedTrackingRay;
}

// ---------------------------------------------------------------------------
// OpenXR event polling (session state management)
// ---------------------------------------------------------------------------
bool OpenXRDriver::xrPollEvents()
{
    XrEventDataBuffer event{XR_TYPE_EVENT_DATA_BUFFER};
    while (xrPollEvent(_xrInstance, &event) == XR_SUCCESS)
    {
        switch (event.type)
        {
        case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED:
        {
            auto* sessionEvent      = reinterpret_cast<XrEventDataSessionStateChanged*>(&event);
            XrSessionState oldState = _sessionState;
            _sessionState           = sessionEvent->state;
            AXLOGI("[OpenXR] Session state: {} -> {}", static_cast<int>(oldState), static_cast<int>(_sessionState));

            switch (_sessionState)
            {
            case XR_SESSION_STATE_READY:
            {
                // Some runtimes deliver READY only after the session-created
                // event is polled, so xrBeginSession in onRenderViewChanged
                // may have failed. Retry here when needed.
                if (!_sessionRunning)
                {
                    XrSessionBeginInfo beginInfo{XR_TYPE_SESSION_BEGIN_INFO};
                    beginInfo.primaryViewConfigurationType = _viewConfigType;
                    _sessionRunning = checkXr(xrBeginSession(_xrSession, &beginInfo), "xrBeginSession");
                }
                break;
            }
            case XR_SESSION_STATE_SYNCHRONIZED:
            case XR_SESSION_STATE_VISIBLE:
            case XR_SESSION_STATE_FOCUSED:
                break;
            case XR_SESSION_STATE_STOPPING:
                _sessionRunning = false;
                break;
            case XR_SESSION_STATE_EXITING:
                _sessionRunning = false;
                _initialized    = false;
                break;
            case XR_SESSION_STATE_LOSS_PENDING:
                _sessionRunning = false;
                _initialized    = false;
                break;
            default:
                break;
            }
            break;
        }
        case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING:
        {
            _sessionRunning = false;
            _initialized    = false;
            break;
        }
        case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED:
        {
            AXLOGI("[OpenXR] Interaction profile changed");
            logXrInteractionProfiles();
            break;
        }
        default:
            break;
        }

        event.type = XR_TYPE_EVENT_DATA_BUFFER;
    }
    return _sessionRunning && _initialized;
}

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------
OpenXRDriver::OpenXRDriver(std::string_view appName) : _appName(appName)
{
    _director = Director::getInstance();
}

OpenXRDriver::~OpenXRDriver()
{
    GraphicsCore::setVulkanInterop(nullptr);
    shutdownXr();
}

// ---------------------------------------------------------------------------
// XR frame driver
// ---------------------------------------------------------------------------

bool OpenXRDriver::registerVulkanInterop()
{
    if (!initXrInstance() || !initXrSystem())
        return false;

#if AX_ENABLE_VK
    _vulkanInterop = std::make_unique<rhi::OpenXRVulkanInterop>();
    _vulkanInterop->setXrHandles(_xrInstance, _xrSystem);
    GraphicsCore::setVulkanInterop(_vulkanInterop.get());
#endif
    return true;
}

void OpenXRDriver::onRenderViewChanged(RenderViewCore* rv)
{
    AX_UNUSED_PARAM(rv);

    if (!_initialized)
    {
        if (_xrInstance == XR_NULL_HANDLE)
        {
            if (!initXrInstance() || !initXrSystem())
                return;
        }

        _initialized = initXrSession() && initXrSwapchains() && initXrSpaces() && initXrActions();
        if (!_initialized)
            return;

        XrSessionBeginInfo beginInfo{XR_TYPE_SESSION_BEGIN_INFO};
        beginInfo.primaryViewConfigurationType = _viewConfigType;
        _sessionRunning                        = checkXr(xrBeginSession(_xrSession, &beginInfo), "xrBeginSession");
    }
}

void OpenXRDriver::pollEvents()
{
    _frameReady             = false;
    _viewsLocated           = false;
    _locatedViewCount       = 0;
    _locatedViewsPoseValid  = false;
    _headPoseTransformValid = false;
    _headPoseTransform      = Mat4::identity;

    if (_initialized && _xrInstance != XR_NULL_HANDLE)
        xrPollEvents();

    if (!_sessionRunning || !_initialized || _xrSession == XR_NULL_HANDLE)
        return;

    XrFrameWaitInfo frameWaitInfo{XR_TYPE_FRAME_WAIT_INFO};
    XrFrameState frameState{XR_TYPE_FRAME_STATE};
    if (XR_FAILED(xrWaitFrame(_xrSession, &frameWaitInfo, &frameState)))
        return;

    _frameState = frameState;
    _frameReady = true;

    if (_frameState.shouldRender)
    {
        _locatedViewCount      = 0;
        _locatedViewsPoseValid = false;

        XrViewState viewState{XR_TYPE_VIEW_STATE};
        XrViewLocateInfo viewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
        viewLocateInfo.viewConfigurationType = _viewConfigType;
        viewLocateInfo.displayTime           = _frameState.predictedDisplayTime;
        viewLocateInfo.space                 = _localSpace;

        if (XR_SUCCEEDED(xrLocateViews(_xrSession, &viewLocateInfo, &viewState, static_cast<uint32_t>(_views.size()),
                                       &_locatedViewCount, _views.data())))
        {
            _locatedViewsPoseValid = _locatedViewCount > 0 &&
                                     (viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT) &&
                                     (viewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT);

            _viewsLocated = true;
            if (_locatedViewsPoseValid)
                updateHeadPoseTransform(_locatedViewCount);
        }
    }

    pollXrActions(_frameState.predictedDisplayTime);
}

bool OpenXRDriver::beginRenderFrame()
{
    if (!_frameReady || _inFrame)
        return false;

    XrFrameBeginInfo frameBeginInfo{XR_TYPE_FRAME_BEGIN_INFO};
    if (XR_FAILED(xrBeginFrame(_xrSession, &frameBeginInfo)))
    {
        _frameReady = false;
        return false;
    }

    _inFrame    = true;
    _frameReady = false;
    if (_frameState.shouldRender != XR_TRUE)
    {
        endFrameEmpty();
        return false;
    }

    return true;
}

bool OpenXRDriver::locateViews(uint32_t& viewCountOutput)
{
    viewCountOutput = 0;

    if (!_inFrame)
        return false;

    if (!_viewsLocated)
    {
        endFrameEmpty();
        return false;
    }

    viewCountOutput = _locatedViewCount;
    return true;
}

void OpenXRDriver::updateHeadPoseTransform(uint32_t viewCount)
{
    _headPoseTransformValid = false;
    _headPoseTransform      = Mat4::identity;

    if (viewCount == 0 || _views.empty())
        return;

    XrPosef centerPose = _views[0].pose;
    if (viewCount >= 2)
    {
        centerPose.position.x = (_views[0].pose.position.x + _views[1].pose.position.x) * 0.5f;
        centerPose.position.y = (_views[0].pose.position.y + _views[1].pose.position.y) * 0.5f;
        centerPose.position.z = (_views[0].pose.position.z + _views[1].pose.position.z) * 0.5f;
    }

    centerPose = scaleXrPosePosition(centerPose, _xrToSceneScale);

    _headPoseTransform      = xrPoseToMat4(centerPose);
    _headPoseTransformValid = true;
}

bool OpenXRDriver::acquireSwapchains(std::vector<AcquiredSwapchain>& acquired)
{
    acquired.clear();
    acquired.reserve(_colorSwapchains.size());

    for (size_t i = 0; i < _colorSwapchains.size(); ++i)
    {
        AcquiredSwapchain acq{};
        acq.handle = _colorSwapchains[i].handle;
        acq.width  = _colorSwapchains[i].width;
        acq.height = _colorSwapchains[i].height;

        XrSwapchainImageAcquireInfo acquireInfo{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
        if (XR_FAILED(xrAcquireSwapchainImage(acq.handle, &acquireInfo, &acq.index)))
        {
            releaseSwapchains(acquired);
            acquired.clear();
            return false;
        }

        XrSwapchainImageWaitInfo waitInfo{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
        waitInfo.timeout = XR_INFINITE_DURATION;
        if (XR_FAILED(xrWaitSwapchainImage(acq.handle, &waitInfo)))
        {
            XrSwapchainImageReleaseInfo releaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
            xrReleaseSwapchainImage(acq.handle, &releaseInfo);
            releaseSwapchains(acquired);
            acquired.clear();
            return false;
        }

        if (acq.index >= _colorSwapchains[i].textures.size() || acq.index >= _colorSwapchains[i].renderTargets.size())
        {
            XrSwapchainImageReleaseInfo releaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
            xrReleaseSwapchainImage(acq.handle, &releaseInfo);
            releaseSwapchains(acquired);
            acquired.clear();
            return false;
        }

        acq.texture      = _colorSwapchains[i].textures[acq.index];
        acq.renderTarget = _colorSwapchains[i].renderTargets[acq.index];

#if AX_ENABLE_VK
        if (acq.texture && rhi::GraphicsCore::currentDriverType() == rhi::DriverType::Vulkan)
        {
            static_cast<rhi::vk::TextureImpl*>(acq.texture)->setKnownLayout(VK_IMAGE_LAYOUT_UNDEFINED);
        }
#endif

        acquired.push_back(acq);
    }

    return true;
}

void OpenXRDriver::releaseSwapchains(const std::vector<AcquiredSwapchain>& acquired)
{
    for (auto& acq : acquired)
    {
        XrSwapchainImageReleaseInfo releaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
        xrReleaseSwapchainImage(acq.handle, &releaseInfo);
    }
}

void OpenXRDriver::endFrameEmpty()
{
    if (!_inFrame)
    {
        _frameReady = false;
        return;
    }

    XrFrameEndInfo endInfo{XR_TYPE_FRAME_END_INFO};
    endInfo.displayTime          = _frameState.predictedDisplayTime;
    endInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    endInfo.layerCount           = 0;
    endInfo.layers               = nullptr;
    xrEndFrame(_xrSession, &endInfo);

    _inFrame    = false;
    _frameReady = false;
}

void OpenXRDriver::endFrameWithProjectionLayer(const std::vector<AcquiredSwapchain>& acquired, uint32_t viewCountOutput)
{
    if (!_inFrame)
        return;

    std::vector<XrCompositionLayerProjectionView> projViews;
    projViews.reserve(viewCountOutput);

    for (uint32_t i = 0; i < viewCountOutput && i < acquired.size(); ++i)
    {
        XrCompositionLayerProjectionView pv{XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};
        pv.pose                     = _views[i].pose;
        pv.fov                      = _views[i].fov;
        pv.subImage.swapchain       = acquired[i].handle;
        pv.subImage.imageRect       = {{0, 0}, makeXrRect(acquired[i].width, acquired[i].height)};
        pv.subImage.imageArrayIndex = 0;
        projViews.push_back(pv);
    }

    XrCompositionLayerProjection layer{XR_TYPE_COMPOSITION_LAYER_PROJECTION};
    layer.space     = _localSpace;
    layer.viewCount = static_cast<uint32_t>(projViews.size());
    layer.views     = projViews.data();

    const XrCompositionLayerBaseHeader* layerPtr = reinterpret_cast<const XrCompositionLayerBaseHeader*>(&layer);

    XrFrameEndInfo endInfo{XR_TYPE_FRAME_END_INFO};
    endInfo.displayTime          = _frameState.predictedDisplayTime;
    endInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    endInfo.layerCount           = 1;
    endInfo.layers               = &layerPtr;
    checkXr(xrEndFrame(_xrSession, &endInfo), "xrEndFrame");

    _inFrame    = false;
    _frameReady = false;
}

// ---------------------------------------------------------------------------
// OpenXR lifecycle
// ---------------------------------------------------------------------------

static std::vector<const char*> getXrExtensions()
{
    std::vector<const char*> extensions;

    uint32_t extCount = 0;
    if (!checkXr(xrEnumerateInstanceExtensionProperties(nullptr, 0, &extCount, nullptr),
                 "xrEnumerateInstanceExtensionProperties(count)"))
    {
        return extensions;
    }

    std::vector<XrExtensionProperties> extProps(extCount, {XR_TYPE_EXTENSION_PROPERTIES});
    if (!checkXr(xrEnumerateInstanceExtensionProperties(nullptr, extCount, &extCount, extProps.data()),
                 "xrEnumerateInstanceExtensionProperties(list)"))
    {
        return extensions;
    }

    auto hasExt = [&](const char* name) -> bool {
        for (auto& ep : extProps)
            if (strcmp(ep.extensionName, name) == 0)
                return true;
        return false;
    };

    // Enable ALL graphics extensions that the runtime supports and the engine
    // was compiled with.  XrInstance allows enabling multiple graphics extensions
    // simultaneously; the runtime only validates the actual one used in
    // xrCreateSession.  This avoids a circular dependency with GraphicsCore
    // (the driver type isn't known until after makeCurrentDriver()).
#if AX_ENABLE_D3D11
    if (hasExt(XR_KHR_D3D11_ENABLE_EXTENSION_NAME))
        extensions.push_back(XR_KHR_D3D11_ENABLE_EXTENSION_NAME);
#endif
#if AX_ENABLE_D3D12
    if (hasExt(XR_KHR_D3D12_ENABLE_EXTENSION_NAME))
        extensions.push_back(XR_KHR_D3D12_ENABLE_EXTENSION_NAME);
#endif
#if AX_ENABLE_VK
    if (hasExt(XR_KHR_VULKAN_ENABLE_EXTENSION_NAME))
        extensions.push_back(XR_KHR_VULKAN_ENABLE_EXTENSION_NAME);
#endif
#if AX_ENABLE_GL && AX_GLES_PROFILE && AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
    if (hasExt(XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME))
        extensions.push_back(XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME);
#endif

    if (extensions.empty())
        AXLOGW("[OpenXR] No graphics extension found in the runtime. OpenXR will not be usable.");
    return extensions;
}

bool OpenXRDriver::initXrInstance()
{
    AXLOGI("[OpenXR] Header API version: {}.{}.{}", XR_VERSION_MAJOR(XR_CURRENT_API_VERSION),
           XR_VERSION_MINOR(XR_CURRENT_API_VERSION), XR_VERSION_PATCH(XR_CURRENT_API_VERSION));

    // Query available layers (optional, for debug)
    uint32_t layerCount = 0;
    if (!checkXr(xrEnumerateApiLayerProperties(0, &layerCount, nullptr), "xrEnumerateApiLayerProperties(count)"))
        return false;
    std::vector<XrApiLayerProperties> layers(layerCount, {XR_TYPE_API_LAYER_PROPERTIES});
    if (!checkXr(xrEnumerateApiLayerProperties(layerCount, &layerCount, layers.data()),
                 "xrEnumerateApiLayerProperties(list)"))
    {
        return false;
    }

    // Check for debug layer
    bool hasDebugLayer = false;
    for (auto& l : layers)
    {
        if (strcmp(l.layerName, "XR_APILAYER_LUNARG_core_validation") == 0 ||
            strcmp(l.layerName, "XR_APILAYER_LUNARG_core_validation") == 0)
        {
            hasDebugLayer = true;
            break;
        }
    }

    // Enable API layers
    std::vector<const char*> apiLayers;
#if defined(_DEBUG) || AX_DEBUG
    if (hasDebugLayer)
        apiLayers.push_back("XR_APILAYER_LUNARG_core_validation");
#endif

    auto extensions = getXrExtensions();

    // Engine info
    XrApplicationInfo appInfo{};
    tlx::strlcpy(appInfo.applicationName, _appName);
    tlx::strlcpy(appInfo.engineName, "axmol3");
    appInfo.applicationVersion = 1;
    appInfo.engineVersion      = 1;
    appInfo.apiVersion         = XR_API_VERSION_1_0;

    XrInstanceCreateInfo createInfo{XR_TYPE_INSTANCE_CREATE_INFO};
    createInfo.createFlags           = 0;
    createInfo.applicationInfo       = appInfo;
    createInfo.enabledApiLayerCount  = static_cast<uint32_t>(apiLayers.size());
    createInfo.enabledApiLayerNames  = apiLayers.data();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.enabledExtensionNames = extensions.data();

    AXLOGI("[OpenXR] Requested API version: {}.{}.{}", XR_VERSION_MAJOR(appInfo.apiVersion),
           XR_VERSION_MINOR(appInfo.apiVersion), XR_VERSION_PATCH(appInfo.apiVersion));

    if (!checkXr(xrCreateInstance(&createInfo, &_xrInstance), "xrCreateInstance"))
        return false;

    XrInstanceProperties instanceProps{XR_TYPE_INSTANCE_PROPERTIES};
    if (XR_SUCCEEDED(xrGetInstanceProperties(_xrInstance, &instanceProps)))
    {
        AXLOGI("[OpenXR] Runtime: '{}' version {}.{}.{}", instanceProps.runtimeName,
               XR_VERSION_MAJOR(instanceProps.runtimeVersion), XR_VERSION_MINOR(instanceProps.runtimeVersion),
               XR_VERSION_PATCH(instanceProps.runtimeVersion));
    }

    return true;
}

bool OpenXRDriver::initXrSystem()
{
    XrSystemGetInfo getInfo{XR_TYPE_SYSTEM_GET_INFO};
    getInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

    if (!checkXr(xrGetSystem(_xrInstance, &getInfo, &_xrSystem), "xrGetSystem"))
        return false;

    // Enumerate view configurations
    uint32_t viewConfigCount = 0;
    if (!checkXr(xrEnumerateViewConfigurations(_xrInstance, _xrSystem, 0, &viewConfigCount, nullptr),
                 "xrEnumerateViewConfigurations(count)"))
    {
        return false;
    }
    std::vector<XrViewConfigurationType> viewConfigs(viewConfigCount);
    if (!checkXr(xrEnumerateViewConfigurations(_xrInstance, _xrSystem, viewConfigCount, &viewConfigCount,
                                               viewConfigs.data()),
                 "xrEnumerateViewConfigurations(list)"))
    {
        return false;
    }

    // Find stereo
    bool foundStereo = false;
    for (auto vt : viewConfigs)
    {
        if (vt == XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO)
        {
            _viewConfigType = vt;
            foundStereo     = true;
            break;
        }
    }
    if (!foundStereo)
    {
        AXLOGW("[OpenXR] XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO is not available");
        return false;
    }

    // Enumerate view configuration views to get recommended resolution
    uint32_t viewCount = 0;
    if (!checkXr(xrEnumerateViewConfigurationViews(_xrInstance, _xrSystem, _viewConfigType, 0, &viewCount, nullptr),
                 "xrEnumerateViewConfigurationViews(count)"))
    {
        return false;
    }
    _viewConfigViews.resize(viewCount);
    _views.resize(viewCount);
    for (auto& vcv : _viewConfigViews)
        vcv.type = XR_TYPE_VIEW_CONFIGURATION_VIEW;
    for (auto& v : _views)
        v.type = XR_TYPE_VIEW;

    if (!checkXr(xrEnumerateViewConfigurationViews(_xrInstance, _xrSystem, _viewConfigType, viewCount, &viewCount,
                                                   _viewConfigViews.data()),
                 "xrEnumerateViewConfigurationViews(list)"))
    {
        return false;
    }
    _viewCount = viewCount;

    // View configuration views are stored, swapchain format is selected
    // after session creation (in initXrSwapchains).

    return true;
}

bool OpenXRDriver::initXrSession()
{
    if (!checkGraphicsRequirements())
        return false;
    const void* graphicsBinding = createGraphicsBinding();
    if (!graphicsBinding)
        return false;

    XrSessionCreateInfo createInfo{XR_TYPE_SESSION_CREATE_INFO};
    createInfo.next     = graphicsBinding;
    createInfo.systemId = _xrSystem;

    return checkXr(xrCreateSession(_xrInstance, &createInfo, &_xrSession), "xrCreateSession");
}

bool OpenXRDriver::initXrSwapchains()
{
    destroySwapchains();

    // ---- Enumerate swapchain formats (must be done after session creation) ----
    uint32_t formatCount = 0;
    if (!checkXr(xrEnumerateSwapchainFormats(_xrSession, 0, &formatCount, nullptr),
                 "xrEnumerateSwapchainFormats(count)"))
    {
        return false;
    }
    std::vector<int64_t> formats(formatCount);
    if (!checkXr(xrEnumerateSwapchainFormats(_xrSession, formatCount, &formatCount, formats.data()),
                 "xrEnumerateSwapchainFormats(list)"))
    {
        return false;
    }

    // Pick the first runtime-compatible 8-bit RGBA/BGRA color format.
    _colorFormat = 0;
    TextureFormat colorTextureFormat;
    for (auto fmt : formats)
    {
        TextureFormat candidateFormat;
        if (xrFormatToTextureFormat(fmt, candidateFormat))
        {
            _colorFormat       = fmt;
            colorTextureFormat = candidateFormat;
            break;
        }
    }

    if (_colorFormat == 0 || colorTextureFormat.pixelFormat == rhi::PixelFormat::NONE)
    {
        AXLOGW("[OpenXR] No supported color swapchain format.");
        for (auto fmt : formats)
            AXLOGW("[OpenXR] Runtime format candidate: {}", static_cast<int64_t>(fmt));
        return false;
    }

    _colorSwapchains.resize(_viewCount);

    for (uint32_t i = 0; i < _viewCount; ++i)
    {
        auto& vcv = _viewConfigViews[i];

        if (!createSwapchain(vcv.recommendedImageRectWidth, vcv.recommendedImageRectHeight, _colorFormat,
                             &_colorSwapchains[i].handle, &_colorSwapchains[i].width, &_colorSwapchains[i].height))
        {
            return false;
        }

        // Enumerate swapchain images and wrap them in axmol textures
        uint32_t imageCount = 0;
        if (!checkXr(xrEnumerateSwapchainImages(_colorSwapchains[i].handle, 0, &imageCount, nullptr),
                     "xrEnumerateSwapchainImages(count)"))
        {
            return false;
        }

        const auto driverType = rhi::GraphicsCore::currentDriverType();
        rhi::TextureDesc colorDesc;
        colorDesc.width        = static_cast<uint16_t>(_colorSwapchains[i].width);
        colorDesc.height       = static_cast<uint16_t>(_colorSwapchains[i].height);
        colorDesc.pixelFormat  = colorTextureFormat.pixelFormat;
        colorDesc.textureUsage = rhi::TextureUsage::RENDER_TARGET;
        colorDesc.textureType  = rhi::TextureType::TEXTURE_2D;
        colorDesc.colorSpace   = colorTextureFormat.colorSpace;
        colorDesc.arraySize    = 1;
        colorDesc.mipLevels    = 1;

#if AX_ENABLE_D3D11
        if (driverType == rhi::DriverType::D3D11)
        {
            std::vector<XrSwapchainImageD3D11KHR> xrImages(imageCount, {XR_TYPE_SWAPCHAIN_IMAGE_D3D11_KHR});
            if (!checkXr(xrEnumerateSwapchainImages(_colorSwapchains[i].handle, imageCount, &imageCount,
                                                    reinterpret_cast<XrSwapchainImageBaseHeader*>(xrImages.data())),
                         "xrEnumerateSwapchainImages(D3D11)"))
            {
                return false;
            }

            for (auto& xrImg : xrImages)
            {
                rhi::ExternalTextureDesc externalDesc;
                externalDesc.desc          = colorDesc;
                externalDesc.nativeTexture = xrImg.texture;
                auto tex                   = axdrv->createTextureFromNativeHandle(externalDesc);
                _colorSwapchains[i].textures.push_back(tex);
            }
        }
        else
#endif
#if AX_ENABLE_D3D12
            if (driverType == rhi::DriverType::D3D12)
        {
            std::vector<XrSwapchainImageD3D12KHR> xrImages(imageCount, {XR_TYPE_SWAPCHAIN_IMAGE_D3D12_KHR});
            if (!checkXr(xrEnumerateSwapchainImages(_colorSwapchains[i].handle, imageCount, &imageCount,
                                                    reinterpret_cast<XrSwapchainImageBaseHeader*>(xrImages.data())),
                         "xrEnumerateSwapchainImages(D3D12)"))
            {
                return false;
            }

            for (auto& xrImg : xrImages)
            {
                rhi::ExternalTextureDesc externalDesc;
                externalDesc.desc             = colorDesc;
                externalDesc.nativeTexture    = xrImg.texture;
                externalDesc.nativeState      = D3D12_RESOURCE_STATE_COMMON;
                externalDesc.nativeFinalState = D3D12_RESOURCE_STATE_COMMON;
                auto tex                      = axdrv->createTextureFromNativeHandle(externalDesc);
                _colorSwapchains[i].textures.push_back(tex);
            }
        }
        else
#endif
#if AX_ENABLE_VK
            if (driverType == rhi::DriverType::Vulkan)
        {
            std::vector<XrSwapchainImageVulkanKHR> xrImages(imageCount, {XR_TYPE_SWAPCHAIN_IMAGE_VULKAN_KHR});
            if (!checkXr(xrEnumerateSwapchainImages(_colorSwapchains[i].handle, imageCount, &imageCount,
                                                    reinterpret_cast<XrSwapchainImageBaseHeader*>(xrImages.data())),
                         "xrEnumerateSwapchainImages(Vulkan)"))
            {
                return false;
            }

            for (auto& xrImg : xrImages)
            {
                rhi::ExternalTextureDesc externalDesc;
                externalDesc.desc             = colorDesc;
                externalDesc.nativeTexture    = xrImg.image;
                externalDesc.nativeUsage      = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                externalDesc.nativeState      = VK_IMAGE_LAYOUT_UNDEFINED;
                externalDesc.nativeFinalState = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                auto tex                      = axdrv->createTextureFromNativeHandle(externalDesc);
                _colorSwapchains[i].textures.push_back(tex);
            }
        }
        else
#endif
#if AX_ENABLE_GL
            if (driverType == rhi::DriverType::OpenGL)
        {
#    if AX_GLES_PROFILE && AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
            std::vector<XrSwapchainImageOpenGLESKHR> xrImages(imageCount, {XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR});
            if (!checkXr(xrEnumerateSwapchainImages(_colorSwapchains[i].handle, imageCount, &imageCount,
                                                    reinterpret_cast<XrSwapchainImageBaseHeader*>(xrImages.data())),
                         "xrEnumerateSwapchainImages(OpenGLES)"))
            {
                return false;
            }

            for (auto& xrImg : xrImages)
            {
                rhi::ExternalTextureDesc externalDesc;
                externalDesc.desc          = colorDesc;
                externalDesc.nativeTexture = static_cast<uint64_t>(xrImg.image);
                auto tex                   = axdrv->createTextureFromNativeHandle(externalDesc);
                _colorSwapchains[i].textures.push_back(tex);
            }
#    else
            AXLOGW("[OpenXR] OpenGL swapchain images are unsupported for this platform/profile.");
            return false;
#    endif
        }
        else
#endif
        {
            // No supported graphics backend
            return false;
        }

        auto depthTex = createDepthTexture(_colorSwapchains[i].width, _colorSwapchains[i].height);
        if (!depthTex)
            return false;

        _colorSwapchains[i].renderTargets.reserve(_colorSwapchains[i].textures.size());
        for (auto texture : _colorSwapchains[i].textures)
        {
            if (!texture)
                return false;

            auto rt = axdrv->createRenderTarget(texture, depthTex);
            _colorSwapchains[i].renderTargets.push_back(rt);
        }

        depthTex->release();
    }

    return true;
}

bool OpenXRDriver::initXrSpaces()
{
    // Create local reference space
    XrReferenceSpaceCreateInfo spaceInfo{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
    spaceInfo.referenceSpaceType   = XR_REFERENCE_SPACE_TYPE_LOCAL;
    spaceInfo.poseInReferenceSpace = makeXrPoseIdentity();
    return checkXr(xrCreateReferenceSpace(_xrSession, &spaceInfo, &_localSpace), "xrCreateReferenceSpace");
}

void OpenXRDriver::shutdownXr()
{
    if (_localSpace != XR_NULL_HANDLE)
    {
        xrDestroySpace(_localSpace);
        _localSpace = XR_NULL_HANDLE;
    }

    shutdownXrActions();
    destroySwapchains();

    if (_xrSession != XR_NULL_HANDLE)
    {
        xrDestroySession(_xrSession);
        _xrSession = XR_NULL_HANDLE;
    }

    if (_graphicsBindingStorage)
    {
        delete static_cast<uint8_t*>(_graphicsBindingStorage);
        _graphicsBindingStorage = nullptr;
    }

    if (_xrInstance != XR_NULL_HANDLE)
    {
        xrDestroyInstance(_xrInstance);
        _xrInstance = XR_NULL_HANDLE;
    }

    _sessionRunning = false;
    _initialized    = false;
}

// ---------------------------------------------------------------------------
// OpenXR Input Actions
// ---------------------------------------------------------------------------

bool OpenXRDriver::initXrActions()
{
    // ---- 1. Create action set ----
    XrActionSetCreateInfo actionSetInfo{XR_TYPE_ACTION_SET_CREATE_INFO};
    tlx::strlcpy(actionSetInfo.actionSetName, "gameplay");
    tlx::strlcpy(actionSetInfo.localizedActionSetName, "Gameplay");
    actionSetInfo.priority = 0;
    if (!checkXr(xrCreateActionSet(_xrInstance, &actionSetInfo, &_inputActionSet), "xrCreateActionSet"))
        return false;

    // ---- 2. Set up subaction paths for left/right hands ----
    auto strToPath = [&](const char* str) -> XrPath {
        XrPath path{XR_NULL_PATH};
        xrStringToPath(_xrInstance, str, &path);
        return path;
    };

    XrPath handLeftPath         = strToPath("/user/hand/left");
    XrPath handRightPath        = strToPath("/user/hand/right");
    XrPath handSubactionPaths[] = {handLeftPath, handRightPath};

    // ---- 3. Create actions ----
    auto makeAction = [&](const char* name, XrActionType type, XrAction& outAction) -> bool {
        XrActionCreateInfo info{XR_TYPE_ACTION_CREATE_INFO};
        tlx::strlcpy(info.actionName, name);
        tlx::strlcpy(info.localizedActionName, name);
        info.actionType          = type;
        info.countSubactionPaths = 2;
        info.subactionPaths      = handSubactionPaths;
        return checkXr(xrCreateAction(_inputActionSet, &info, &outAction), "xrCreateAction");
    };

    if (!makeAction("trigger", XR_ACTION_TYPE_BOOLEAN_INPUT, _triggerAction))
        return false;
    if (!makeAction("trigger_value", XR_ACTION_TYPE_FLOAT_INPUT, _triggerValueAction))
        return false;
    if (!makeAction("grip", XR_ACTION_TYPE_BOOLEAN_INPUT, _gripAction))
        return false;
    if (!makeAction("grip_value", XR_ACTION_TYPE_FLOAT_INPUT, _gripValueAction))
        return false;
    if (!makeAction("thumbstick", XR_ACTION_TYPE_VECTOR2F_INPUT, _thumbstickAction))
        return false;
    if (!makeAction("thumbstick_click", XR_ACTION_TYPE_BOOLEAN_INPUT, _thumbstickClickAction))
        return false;
    if (!makeAction("menu", XR_ACTION_TYPE_BOOLEAN_INPUT, _menuAction))
        return false;
    if (!makeAction("a_button", XR_ACTION_TYPE_BOOLEAN_INPUT, _aAction))
        return false;
    if (!makeAction("b_button", XR_ACTION_TYPE_BOOLEAN_INPUT, _bAction))
        return false;
    if (!makeAction("x_button", XR_ACTION_TYPE_BOOLEAN_INPUT, _xAction))
        return false;
    if (!makeAction("y_button", XR_ACTION_TYPE_BOOLEAN_INPUT, _yAction))
        return false;
    if (!makeAction("aim_pose", XR_ACTION_TYPE_POSE_INPUT, _aimPoseAction))
        return false;
    if (!makeAction("grip_pose", XR_ACTION_TYPE_POSE_INPUT, _gripPoseAction))
        return false;

    // Create action spaces for each hand
    auto createActionSpace = [&](XrAction action, XrPath subactionPath, XrSpace& outSpace) -> bool {
        XrActionSpaceCreateInfo info{XR_TYPE_ACTION_SPACE_CREATE_INFO};
        info.action            = action;
        info.subactionPath     = subactionPath;
        info.poseInActionSpace = makeXrPoseIdentity();
        return checkXr(xrCreateActionSpace(_xrSession, &info, &outSpace), "xrCreateActionSpace");
    };

    if (!createActionSpace(_aimPoseAction, handLeftPath, _controllers[0].aimSpace))
        return false;
    if (!createActionSpace(_aimPoseAction, handRightPath, _controllers[1].aimSpace))
        return false;
    if (!createActionSpace(_gripPoseAction, handLeftPath, _controllers[0].gripSpace))
        return false;
    if (!createActionSpace(_gripPoseAction, handRightPath, _controllers[1].gripSpace))
        return false;

    // Initialize controller state
    for (uint32_t i = 0; i < 2; ++i)
    {
        _controllers[i].handPath  = (i == 0) ? handLeftPath : handRightPath;
        _controllers[i].pointerId = 2 + i;  // mouse=0, touch=1, left_ctrl=2, right_ctrl=3
    }

    // ---- 4. Suggest interaction profile bindings ----
    // Bind multiple common profiles so the runtime picks the one matching the hardware.
    // Each profile only defines the paths it actually supports.
    auto strPath = [&](const char* s) -> XrPath { return strToPath(s); };

    auto suggestProfile = [&](const char* profilePath, std::vector<XrActionSuggestedBinding> bindings) {
        XrPath profile = strToPath(profilePath);
        XrInteractionProfileSuggestedBinding info{XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
        info.interactionProfile     = profile;
        info.suggestedBindings      = bindings.data();
        info.countSuggestedBindings = static_cast<uint32_t>(bindings.size());
        XrResult result             = xrSuggestInteractionProfileBindings(_xrInstance, &info);
        if (XR_SUCCEEDED(result))
        {
            AXLOGI("[OpenXR] Interaction profile '{}' bound successfully", profilePath);
        }
        else
        {
            AXLOGI("[OpenXR] Interaction profile '{}' not supported (ec:{}), skipping", profilePath,
                   static_cast<int>(result));
        }
    };

    auto bind = [&](XrAction action, const char* binding) -> XrActionSuggestedBinding {
        return {action, strPath(binding)};
    };

    // khr/simple_controller: only select (trigger click), menu, and poses
    suggestProfile(
        "/interaction_profiles/khr/simple_controller",
        {bind(_triggerAction, "/user/hand/left/input/select/click"),
         bind(_triggerAction, "/user/hand/right/input/select/click"),
         bind(_menuAction, "/user/hand/left/input/menu/click"), bind(_menuAction, "/user/hand/right/input/menu/click"),
         bind(_aimPoseAction, "/user/hand/left/input/aim/pose"),
         bind(_aimPoseAction, "/user/hand/right/input/aim/pose"),
         bind(_gripPoseAction, "/user/hand/left/input/grip/pose"),
         bind(_gripPoseAction, "/user/hand/right/input/grip/pose")});

    // microsoft/motion_controller (covers DPVR, mixed reality, etc.)
    suggestProfile(
        "/interaction_profiles/microsoft/motion_controller",
        {bind(_triggerValueAction, "/user/hand/left/input/trigger/value"),
         bind(_triggerValueAction, "/user/hand/right/input/trigger/value"),
         bind(_gripAction, "/user/hand/left/input/squeeze/click"),
         bind(_gripAction, "/user/hand/right/input/squeeze/click"),
         bind(_thumbstickAction, "/user/hand/left/input/thumbstick"),
         bind(_thumbstickAction, "/user/hand/right/input/thumbstick"),
         bind(_thumbstickAction, "/user/hand/left/input/trackpad"),
         bind(_thumbstickAction, "/user/hand/right/input/trackpad"),
         bind(_thumbstickClickAction, "/user/hand/left/input/thumbstick/click"),
         bind(_thumbstickClickAction, "/user/hand/right/input/thumbstick/click"),
         bind(_thumbstickClickAction, "/user/hand/left/input/trackpad/click"),
         bind(_thumbstickClickAction, "/user/hand/right/input/trackpad/click"),
         bind(_menuAction, "/user/hand/left/input/menu/click"), bind(_menuAction, "/user/hand/right/input/menu/click"),
         bind(_aimPoseAction, "/user/hand/left/input/aim/pose"),
         bind(_aimPoseAction, "/user/hand/right/input/aim/pose"),
         bind(_gripPoseAction, "/user/hand/left/input/grip/pose"),
         bind(_gripPoseAction, "/user/hand/right/input/grip/pose")});

    // oculus/touch_controller
    suggestProfile(
        "/interaction_profiles/oculus/touch_controller",
        {bind(_triggerValueAction, "/user/hand/left/input/trigger/value"),
         bind(_triggerValueAction, "/user/hand/right/input/trigger/value"),
         bind(_gripValueAction, "/user/hand/left/input/squeeze/value"),
         bind(_gripValueAction, "/user/hand/right/input/squeeze/value"),
         bind(_thumbstickAction, "/user/hand/left/input/thumbstick"),
         bind(_thumbstickAction, "/user/hand/right/input/thumbstick"),
         bind(_thumbstickClickAction, "/user/hand/left/input/thumbstick/click"),
         bind(_thumbstickClickAction, "/user/hand/right/input/thumbstick/click"),
         bind(_menuAction, "/user/hand/left/input/menu/click"), bind(_aAction, "/user/hand/right/input/a/click"),
         bind(_bAction, "/user/hand/right/input/b/click"), bind(_xAction, "/user/hand/left/input/x/click"),
         bind(_yAction, "/user/hand/left/input/y/click"), bind(_aimPoseAction, "/user/hand/left/input/aim/pose"),
         bind(_aimPoseAction, "/user/hand/right/input/aim/pose"),
         bind(_gripPoseAction, "/user/hand/left/input/grip/pose"),
         bind(_gripPoseAction, "/user/hand/right/input/grip/pose")});

    // valve/index_controller
    suggestProfile("/interaction_profiles/valve/index_controller",
                   {bind(_triggerAction, "/user/hand/left/input/trigger/click"),
                    bind(_triggerAction, "/user/hand/right/input/trigger/click"),
                    bind(_triggerValueAction, "/user/hand/left/input/trigger/value"),
                    bind(_triggerValueAction, "/user/hand/right/input/trigger/value"),
                    bind(_gripValueAction, "/user/hand/left/input/squeeze/value"),
                    bind(_gripValueAction, "/user/hand/right/input/squeeze/value"),
                    bind(_thumbstickAction, "/user/hand/left/input/thumbstick"),
                    bind(_thumbstickAction, "/user/hand/right/input/thumbstick"),
                    bind(_thumbstickAction, "/user/hand/left/input/trackpad"),
                    bind(_thumbstickAction, "/user/hand/right/input/trackpad"),
                    bind(_thumbstickClickAction, "/user/hand/left/input/thumbstick/click"),
                    bind(_thumbstickClickAction, "/user/hand/right/input/thumbstick/click"),
                    bind(_aAction, "/user/hand/left/input/a/click"),
                    bind(_aAction, "/user/hand/right/input/a/click"),
                    bind(_bAction, "/user/hand/left/input/b/click"),
                    bind(_bAction, "/user/hand/right/input/b/click"),
                    bind(_aimPoseAction, "/user/hand/left/input/aim/pose"),
                    bind(_aimPoseAction, "/user/hand/right/input/aim/pose"),
                    bind(_gripPoseAction, "/user/hand/left/input/grip/pose"),
                    bind(_gripPoseAction, "/user/hand/right/input/grip/pose")});

    auto suggestTouchLikeProfile = [&](const char* profilePath, bool hasTriggerClick, bool hasSqueezeClick) {
        std::vector<XrActionSuggestedBinding> bindings{
            bind(_triggerValueAction, "/user/hand/left/input/trigger/value"),
            bind(_triggerValueAction, "/user/hand/right/input/trigger/value"),
            bind(_gripValueAction, "/user/hand/left/input/squeeze/value"),
            bind(_gripValueAction, "/user/hand/right/input/squeeze/value"),
            bind(_thumbstickAction, "/user/hand/left/input/thumbstick"),
            bind(_thumbstickAction, "/user/hand/right/input/thumbstick"),
            bind(_thumbstickClickAction, "/user/hand/left/input/thumbstick/click"),
            bind(_thumbstickClickAction, "/user/hand/right/input/thumbstick/click"),
            bind(_menuAction, "/user/hand/left/input/menu/click"),
            bind(_aAction, "/user/hand/right/input/a/click"),
            bind(_bAction, "/user/hand/right/input/b/click"),
            bind(_xAction, "/user/hand/left/input/x/click"),
            bind(_yAction, "/user/hand/left/input/y/click"),
            bind(_aimPoseAction, "/user/hand/left/input/aim/pose"),
            bind(_aimPoseAction, "/user/hand/right/input/aim/pose"),
            bind(_gripPoseAction, "/user/hand/left/input/grip/pose"),
            bind(_gripPoseAction, "/user/hand/right/input/grip/pose"),
        };

        if (hasTriggerClick)
        {
            bindings.push_back(bind(_triggerAction, "/user/hand/left/input/trigger/click"));
            bindings.push_back(bind(_triggerAction, "/user/hand/right/input/trigger/click"));
        }
        if (hasSqueezeClick)
        {
            bindings.push_back(bind(_gripAction, "/user/hand/left/input/squeeze/click"));
            bindings.push_back(bind(_gripAction, "/user/hand/right/input/squeeze/click"));
        }

        suggestProfile(profilePath, std::move(bindings));
    };

    suggestTouchLikeProfile("/interaction_profiles/bytedance/pico4_controller", true, true);
    suggestTouchLikeProfile("/interaction_profiles/bytedance/pico_ultra_controller_bd", true, true);
    suggestTouchLikeProfile("/interaction_profiles/htc/vive_focus3_controller", true, true);
    suggestTouchLikeProfile("/interaction_profiles/yvr/touch_controller_yvr", false, true);
    suggestTouchLikeProfile("/interaction_profiles/meta/touch_controller_plus", false, false);
    suggestTouchLikeProfile("/interaction_profiles/meta/touch_plus_controller", false, false);
    suggestTouchLikeProfile("/interaction_profiles/meta/touch_pro_controller", false, false);

    suggestProfile("/interaction_profiles/bytedance/pico_neo3_controller",
                   {bind(_triggerAction, "/user/hand/left/input/trigger/click"),
                    bind(_triggerAction, "/user/hand/right/input/trigger/click"),
                    bind(_triggerValueAction, "/user/hand/left/input/trigger/value"),
                    bind(_triggerValueAction, "/user/hand/right/input/trigger/value"),
                    bind(_gripAction, "/user/hand/left/input/squeeze/click"),
                    bind(_gripAction, "/user/hand/right/input/squeeze/click"),
                    bind(_gripValueAction, "/user/hand/left/input/squeeze/value"),
                    bind(_gripValueAction, "/user/hand/right/input/squeeze/value"),
                    bind(_thumbstickAction, "/user/hand/left/input/thumbstick"),
                    bind(_thumbstickAction, "/user/hand/right/input/thumbstick"),
                    bind(_thumbstickClickAction, "/user/hand/left/input/thumbstick/click"),
                    bind(_thumbstickClickAction, "/user/hand/right/input/thumbstick/click"),
                    bind(_menuAction, "/user/hand/left/input/menu/click"),
                    bind(_menuAction, "/user/hand/right/input/menu/click"),
                    bind(_aAction, "/user/hand/right/input/a/click"),
                    bind(_bAction, "/user/hand/right/input/b/click"),
                    bind(_xAction, "/user/hand/left/input/x/click"),
                    bind(_yAction, "/user/hand/left/input/y/click"),
                    bind(_aimPoseAction, "/user/hand/left/input/aim/pose"),
                    bind(_aimPoseAction, "/user/hand/right/input/aim/pose"),
                    bind(_gripPoseAction, "/user/hand/left/input/grip/pose"),
                    bind(_gripPoseAction, "/user/hand/right/input/grip/pose")});

    suggestProfile(
        "/interaction_profiles/bytedance/pico_g3_controller",
        {bind(_triggerAction, "/user/hand/left/input/trigger/click"),
         bind(_triggerAction, "/user/hand/right/input/trigger/click"),
         bind(_triggerValueAction, "/user/hand/left/input/trigger/value"),
         bind(_triggerValueAction, "/user/hand/right/input/trigger/value"),
         bind(_thumbstickAction, "/user/hand/left/input/thumbstick"),
         bind(_thumbstickAction, "/user/hand/right/input/thumbstick"),
         bind(_thumbstickClickAction, "/user/hand/left/input/thumbstick/click"),
         bind(_thumbstickClickAction, "/user/hand/right/input/thumbstick/click"),
         bind(_menuAction, "/user/hand/left/input/menu/click"), bind(_menuAction, "/user/hand/right/input/menu/click"),
         bind(_aimPoseAction, "/user/hand/left/input/aim/pose"),
         bind(_aimPoseAction, "/user/hand/right/input/aim/pose"),
         bind(_gripPoseAction, "/user/hand/left/input/grip/pose"),
         bind(_gripPoseAction, "/user/hand/right/input/grip/pose")});

    suggestProfile(
        "/interaction_profiles/hp/mixed_reality_controller",
        {bind(_triggerValueAction, "/user/hand/left/input/trigger/value"),
         bind(_triggerValueAction, "/user/hand/right/input/trigger/value"),
         bind(_gripValueAction, "/user/hand/left/input/squeeze/value"),
         bind(_gripValueAction, "/user/hand/right/input/squeeze/value"),
         bind(_thumbstickAction, "/user/hand/left/input/thumbstick"),
         bind(_thumbstickAction, "/user/hand/right/input/thumbstick"),
         bind(_thumbstickClickAction, "/user/hand/left/input/thumbstick/click"),
         bind(_thumbstickClickAction, "/user/hand/right/input/thumbstick/click"),
         bind(_menuAction, "/user/hand/left/input/menu/click"), bind(_menuAction, "/user/hand/right/input/menu/click"),
         bind(_aAction, "/user/hand/right/input/a/click"), bind(_bAction, "/user/hand/right/input/b/click"),
         bind(_xAction, "/user/hand/left/input/x/click"), bind(_yAction, "/user/hand/left/input/y/click"),
         bind(_aimPoseAction, "/user/hand/left/input/aim/pose"),
         bind(_aimPoseAction, "/user/hand/right/input/aim/pose"),
         bind(_gripPoseAction, "/user/hand/left/input/grip/pose"),
         bind(_gripPoseAction, "/user/hand/right/input/grip/pose")});

    suggestProfile(
        "/interaction_profiles/khr/generic_controller",
        {bind(_triggerValueAction, "/user/hand/left/input/trigger/value"),
         bind(_triggerValueAction, "/user/hand/right/input/trigger/value"),
         bind(_gripValueAction, "/user/hand/left/input/squeeze/value"),
         bind(_gripValueAction, "/user/hand/right/input/squeeze/value"),
         bind(_thumbstickAction, "/user/hand/left/input/thumbstick"),
         bind(_thumbstickAction, "/user/hand/right/input/thumbstick"),
         bind(_thumbstickClickAction, "/user/hand/left/input/thumbstick/click"),
         bind(_thumbstickClickAction, "/user/hand/right/input/thumbstick/click"),
         bind(_xAction, "/user/hand/left/input/primary/click"), bind(_aAction, "/user/hand/right/input/primary/click"),
         bind(_yAction, "/user/hand/left/input/secondary/click"),
         bind(_bAction, "/user/hand/right/input/secondary/click"),
         bind(_aimPoseAction, "/user/hand/left/input/aim/pose"),
         bind(_aimPoseAction, "/user/hand/right/input/aim/pose"),
         bind(_gripPoseAction, "/user/hand/left/input/grip/pose"),
         bind(_gripPoseAction, "/user/hand/right/input/grip/pose")});

    // Attach action set to session
    XrSessionActionSetsAttachInfo attachInfo{XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};
    attachInfo.countActionSets = 1;
    attachInfo.actionSets      = &_inputActionSet;
    if (!checkXr(xrAttachSessionActionSets(_xrSession, &attachInfo), "xrAttachSessionActionSets"))
        return false;

    logXrInteractionProfiles();

    return true;
}

void OpenXRDriver::pollXrActions(XrTime predictedDisplayTime)
{
    if (_inputActionSet == XR_NULL_HANDLE)
        return;

    // Input actions are only available when the session has focus
    if (_sessionState != XR_SESSION_STATE_FOCUSED)
    {
        for (auto& ctrl : _controllers)
        {
            ctrl.poseValid                = false;
            ctrl.rayHitValid              = false;
            ctrl.stabilizedRayValid       = false;
            ctrl.rawWorldRayValid         = false;
            ctrl.rawAimPoseUpdated        = false;
            ctrl.aimTracked               = false;
            ctrl.rawAimSampleTime         = 0;
            ctrl.lastStabilizedSampleTime = 0;
            ctrl.rawGripPoseValid         = false;
            ctrl.triggerPrevious          = false;
            ctrl.gripPrevious             = false;
            ctrl.thumbstickClickPrevious  = false;
            ctrl.menuPrevious             = false;
            ctrl.aPrevious                = false;
            ctrl.bPrevious                = false;
            ctrl.xPrevious                = false;
            ctrl.yPrevious                = false;
            ctrl.thumbstickActivePrevious = false;
            ctrl.posePrevious             = false;
        }
        return;
    }

    // Sync actions
    XrActiveActionSet activeSet{};
    activeSet.actionSet     = _inputActionSet;
    activeSet.subactionPath = XR_NULL_PATH;

    XrActionsSyncInfo syncInfo{XR_TYPE_ACTIONS_SYNC_INFO};
    syncInfo.countActiveActionSets = 1;
    syncInfo.activeActionSets      = &activeSet;
    XrResult syncResult            = xrSyncActions(_xrSession, &syncInfo);
    if (XR_FAILED(syncResult))
    {
        AXLOGW("[OpenXR] xrSyncActions failed, ec:{}", static_cast<int>(syncResult));
        return;
    }

    // Poll buttons and raw tracking-space poses only. Scene-world pointer rays
    // are resolved after the scene update, once the frame view snapshot is known.
    for (uint32_t hand = 0; hand < 2; ++hand)
    {
        auto& ctrl = _controllers[hand];

        // Poll trigger
        XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo.subactionPath = ctrl.handPath;

        auto pollBooleanAction = [&](XrAction action) -> bool {
            if (action == XR_NULL_HANDLE)
                return false;

            getInfo.action = action;
            XrActionStateBoolean state{XR_TYPE_ACTION_STATE_BOOLEAN};
            if (XR_SUCCEEDED(xrGetActionStateBoolean(_xrSession, &getInfo, &state)) && state.isActive == XR_TRUE)
                return state.currentState == XR_TRUE;
            return false;
        };

        auto pollFloatAction = [&](XrAction action) -> float {
            if (action == XR_NULL_HANDLE)
                return 0.0f;

            getInfo.action = action;
            XrActionStateFloat state{XR_TYPE_ACTION_STATE_FLOAT};
            if (XR_SUCCEEDED(xrGetActionStateFloat(_xrSession, &getInfo, &state)) && state.isActive == XR_TRUE)
                return state.currentState;
            return 0.0f;
        };

        constexpr float triggerPressedThreshold = 0.5f;
        ctrl.triggerPressed =
            pollBooleanAction(_triggerAction) || pollFloatAction(_triggerValueAction) >= triggerPressedThreshold;

        // Poll grip
        constexpr float gripPressedThreshold = 0.5f;
        ctrl.gripPressed = pollBooleanAction(_gripAction) || pollFloatAction(_gripValueAction) >= gripPressedThreshold;

        ctrl.thumbstickClickPressed = pollBooleanAction(_thumbstickClickAction);
        ctrl.menuPressed            = pollBooleanAction(_menuAction);
        ctrl.aPressed               = pollBooleanAction(_aAction);
        ctrl.bPressed               = pollBooleanAction(_bAction);
        ctrl.xPressed               = pollBooleanAction(_xAction);
        ctrl.yPressed               = pollBooleanAction(_yAction);

        // Poll thumbstick
        getInfo.action = _thumbstickAction;
        XrActionStateVector2f thumbstickState{XR_TYPE_ACTION_STATE_VECTOR2F};
        if (XR_SUCCEEDED(xrGetActionStateVector2f(_xrSession, &getInfo, &thumbstickState)))
        {
            ctrl.thumbstick       = thumbstickState.currentState;
            ctrl.thumbstickActive = thumbstickState.isActive == XR_TRUE;
        }
        else
        {
            ctrl.thumbstick       = {};
            ctrl.thumbstickActive = false;
        }

        auto dispatchXRButton = [&](XRInputEvent::Input input, bool pressed, bool previous) {
            if (pressed == previous)
                return;

            XRInputEvent::State state;
            state.eventType          = XRInputEvent::EventType::Button;
            state.hand               = hand == 0 ? XRInputEvent::Hand::Left : XRInputEvent::Hand::Right;
            state.input              = input;
            state.phase              = pressed ? XRInputEvent::Phase::Pressed : XRInputEvent::Phase::Released;
            state.value              = pressed ? 1.0f : 0.0f;
            state.interactionProfile = ctrl.interactionProfile;
            InputSystem::getInstance()->handleXRInput(state);
        };

        dispatchXRButton(XRInputEvent::Input::Trigger, ctrl.triggerPressed, ctrl.triggerPrevious);
        dispatchXRButton(XRInputEvent::Input::Grip, ctrl.gripPressed, ctrl.gripPrevious);
        dispatchXRButton(XRInputEvent::Input::ThumbstickClick, ctrl.thumbstickClickPressed,
                         ctrl.thumbstickClickPrevious);
        dispatchXRButton(XRInputEvent::Input::Menu, ctrl.menuPressed, ctrl.menuPrevious);
        dispatchXRButton(XRInputEvent::Input::A, ctrl.aPressed, ctrl.aPrevious);
        dispatchXRButton(XRInputEvent::Input::B, ctrl.bPressed, ctrl.bPrevious);
        dispatchXRButton(XRInputEvent::Input::X, ctrl.xPressed, ctrl.xPrevious);
        dispatchXRButton(XRInputEvent::Input::Y, ctrl.yPressed, ctrl.yPrevious);

        constexpr float thumbstickEventEpsilon = 0.001f;
        const Vec2 thumbstickValue{ctrl.thumbstick.x, ctrl.thumbstick.y};
        const Vec2 thumbstickPrevious{ctrl.thumbstickPrevious.x, ctrl.thumbstickPrevious.y};
        if (ctrl.thumbstickActive || ctrl.thumbstickActivePrevious)
        {
            const bool thumbstickChanged =
                std::abs(thumbstickValue.x - thumbstickPrevious.x) > thumbstickEventEpsilon ||
                std::abs(thumbstickValue.y - thumbstickPrevious.y) > thumbstickEventEpsilon ||
                ctrl.thumbstickActive != ctrl.thumbstickActivePrevious;
            if (thumbstickChanged)
            {
                XRInputEvent::State state;
                state.eventType          = XRInputEvent::EventType::Axis;
                state.hand               = hand == 0 ? XRInputEvent::Hand::Left : XRInputEvent::Hand::Right;
                state.input              = XRInputEvent::Input::Thumbstick;
                state.phase              = XRInputEvent::Phase::Changed;
                state.axis               = thumbstickValue;
                state.value              = thumbstickValue.length();
                state.interactionProfile = ctrl.interactionProfile;
                InputSystem::getInstance()->handleXRInput(state);
            }
        }

        // Poll aim pose. VALID can contain an inferred or last-known pose, so
        // only TRACKED samples are allowed to advance the pointer ray.
        getInfo.action = _aimPoseAction;
        XrActionStatePose poseState{XR_TYPE_ACTION_STATE_POSE};
        const XrResult poseResult = xrGetActionStatePose(_xrSession, &getInfo, &poseState);
        const bool poseActive     = XR_SUCCEEDED(poseResult) && poseState.isActive == XR_TRUE;

        ctrl.rawAimPoseUpdated   = false;
        ctrl.aimTracked          = false;
        ctrl.rawGripPoseValid    = false;

        if (poseActive && ctrl.aimSpace != XR_NULL_HANDLE)
        {
            XrSpaceLocation location{XR_TYPE_SPACE_LOCATION};
            const XrResult locateResult =
                xrLocateSpace(ctrl.aimSpace, _localSpace, predictedDisplayTime, &location);

            constexpr XrSpaceLocationFlags requiredValidFlags =
                XR_SPACE_LOCATION_ORIENTATION_VALID_BIT | XR_SPACE_LOCATION_POSITION_VALID_BIT;
            constexpr XrSpaceLocationFlags requiredTrackedFlags =
                XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT | XR_SPACE_LOCATION_POSITION_TRACKED_BIT;

            const bool locationValid =
                XR_SUCCEEDED(locateResult) &&
                (location.locationFlags & requiredValidFlags) == requiredValidFlags;
            const bool locationTracked =
                locationValid && (location.locationFlags & requiredTrackedFlags) == requiredTrackedFlags;

            if (locationTracked)
            {
                ctrl.rawAimPose             = location.pose;
                ctrl.rawAimPoseUpdated      = true;
                ctrl.aimTracked             = true;
                ctrl.poseValid              = true;
                ctrl.rawAimSampleTime       = predictedDisplayTime;
                ctrl.invalidPoseFrameCount  = 0;

                if (ctrl.gripSpace != XR_NULL_HANDLE)
                {
                    XrSpaceLocation gripLocation{XR_TYPE_SPACE_LOCATION};
                    const XrResult gripLocateResult =
                        xrLocateSpace(ctrl.gripSpace, _localSpace, predictedDisplayTime, &gripLocation);
                    if (XR_SUCCEEDED(gripLocateResult) &&
                        (gripLocation.locationFlags & requiredValidFlags) == requiredValidFlags &&
                        (gripLocation.locationFlags & requiredTrackedFlags) == requiredTrackedFlags)
                    {
                        ctrl.rawGripPose      = gripLocation.pose;
                        ctrl.rawGripPoseValid = true;
                    }
                }
            }
        }

        if (!ctrl.aimTracked)
        {
            constexpr uint8_t trackingLossGraceFrames = 6;
            if (ctrl.stabilizedRayValid && ctrl.invalidPoseFrameCount < trackingLossGraceFrames)
            {
                ++ctrl.invalidPoseFrameCount;
                ctrl.poseValid = true;
            }
            else
            {
                ctrl.poseValid                = false;
                ctrl.rayHitValid              = false;
                ctrl.stabilizedRayValid       = false;
                ctrl.rawWorldRayValid         = false;
                ctrl.lastStabilizedSampleTime = 0;
            }
        }
    }
}

void OpenXRDriver::resolveControllerPointers(const Mat4& trackingToWorld, float sceneRayMaxDistance)
{
    const float xrToSceneScale = _xrToSceneScale;
    const float fallbackTrackingDistance =
        std::max(sceneRayMaxDistance / std::max(xrToSceneScale, 0.0001f), 0.25f);

    for (uint32_t hand = 0; hand < 2; ++hand)
    {
        auto& ctrl = _controllers[hand];

        if (ctrl.poseValid)
        {
            if (!ctrl.rayHitValid)
                ctrl.stabilizationReferenceDistance = fallbackTrackingDistance;

            if (ctrl.rawAimPoseUpdated)
            {
                Ray rawRay = makeRayFromXrPose(ctrl.rawAimPose);
                rawRay.direction.normalize();

                float deltaTime = 1.0f / 90.0f;
                if (ctrl.lastStabilizedSampleTime > 0 &&
                    ctrl.rawAimSampleTime > ctrl.lastStabilizedSampleTime)
                {
                    constexpr double xrTimeToSeconds = 1.0e-9;
                    deltaTime = static_cast<float>(
                        static_cast<double>(ctrl.rawAimSampleTime - ctrl.lastStabilizedSampleTime) * xrTimeToSeconds);
                    deltaTime = std::clamp(deltaTime, 1.0f / 240.0f, 1.0f / 30.0f);
                }

                ctrl.rawTrackingRay          = rawRay;
                stabilizeControllerRay(ctrl, rawRay, deltaTime);
                ctrl.lastStabilizedSampleTime = ctrl.rawAimSampleTime;
            }

            const Ray& trackingRay = ctrl.stabilizedTrackingRay;

            PointerRayContext rayContext;
            rayContext.trackingRay            = trackingRay;
            rayContext.primaryTrackingToWorld = trackingToWorld;
            rayContext.trackingScale          = xrToSceneScale;

            Ray eventRay = trackingRay;
            eventRay.origin *= xrToSceneScale;
            eventRay.transform(trackingToWorld);
            eventRay.direction.normalize();

            ctrl.currentRay = eventRay;

            Ray rawWorldRay = ctrl.rawTrackingRay;
            rawWorldRay.origin *= xrToSceneScale;
            rawWorldRay.transform(trackingToWorld);
            rawWorldRay.direction.normalize();
            ctrl.rawWorldRay      = rawWorldRay;
            ctrl.rawWorldRayValid = true;

            bool gripPoseValid = false;
            Mat4 gripPose      = Mat4::identity;
            if (ctrl.rawGripPoseValid)
            {
                gripPose      = xrPoseToMat4(scaleXrPosePosition(ctrl.rawGripPose, xrToSceneScale));
                gripPose      = trackingToWorld * gripPose;
                gripPoseValid = true;
            }

            XRInputEvent::State poseState;
            poseState.eventType          = XRInputEvent::EventType::Pose;
            poseState.hand               = hand == 0 ? XRInputEvent::Hand::Left : XRInputEvent::Hand::Right;
            poseState.input              = XRInputEvent::Input::AimPose;
            poseState.phase              = XRInputEvent::Phase::Active;
            poseState.poseValid          = true;
            poseState.aimRay             = eventRay;
            poseState.gripPoseValid      = gripPoseValid;
            poseState.gripPose           = gripPose;
            poseState.interactionProfile = ctrl.interactionProfile;
            InputSystem::getInstance()->handleXRInput(poseState);

            InputPhase phase = InputPhase::PointerMove;
            if (ctrl.triggerPressed && !ctrl.triggerPrevious)
                phase = InputPhase::PointerDown;
            else if (!ctrl.triggerPressed && ctrl.triggerPrevious)
                phase = InputPhase::PointerUp;

            PointerInputState inputState;
            inputState.id             = ctrl.pointerId;
            inputState.pressure       = ctrl.triggerPressed ? 1.0f : 0.0f;
            inputState.button         = (phase == InputPhase::PointerDown || phase == InputPhase::PointerUp)
                                            ? InputButton::Primary
                                            : InputButton::None;
            inputState.pressedButtons = ctrl.triggerPressed ? (1u << InputButton::Primary) : 0;
            inputState.type           = PointerType::Controller;

            Vec2 centerPoint(_director->getCanvasSize().width * 0.5f, _director->getCanvasSize().height * 0.5f);

            PointerHitResult hitResult;
            bool hasHitResult = false;
            InputSystem::getInstance()->handleVRPointerEvent(phase, centerPoint, eventRay, inputState, &rayContext);

            hitResult    = InputSystem::getInstance()->hitTestVRPointer(centerPoint, eventRay, inputState, &rayContext);
            hasHitResult = true;

            constexpr float thumbstickScrollDeadzone = 0.0001f;
            if (std::abs(ctrl.thumbstick.y) > thumbstickScrollDeadzone)
            {
                PointerInputState scrollState;
                scrollState.id             = ctrl.pointerId;
                scrollState.pressure       = 0.0f;
                scrollState.button         = InputButton::None;
                scrollState.pressedButtons = inputState.pressedButtons;
                scrollState.type           = PointerType::Controller;
                InputSystem::getInstance()->handleVRPointerScroll(centerPoint, Vec2{0.0f, -ctrl.thumbstick.y}, eventRay,
                                                                  scrollState, &rayContext);
            }

            ctrl.rayHitValid = hasHitResult && hitResult.hit;
            if (ctrl.rayHitValid)
            {
                ctrl.rayHitPoint = hitResult.visualPointValid ? hitResult.visualPoint : hitResult.worldPoint;

                const float sceneHitDistance = ctrl.rayHitPoint.distance(eventRay.origin);
                ctrl.stabilizationReferenceDistance =
                    std::max(sceneHitDistance / std::max(xrToSceneScale, 0.0001f), 0.25f);
            }
            else
            {
                ctrl.stabilizationReferenceDistance = fallbackTrackingDistance;
            }
        }
        else
        {
            if (ctrl.posePrevious)
            {
                XRInputEvent::State poseState;
                poseState.eventType          = XRInputEvent::EventType::Pose;
                poseState.hand               = hand == 0 ? XRInputEvent::Hand::Left : XRInputEvent::Hand::Right;
                poseState.input              = XRInputEvent::Input::AimPose;
                poseState.phase              = XRInputEvent::Phase::Inactive;
                poseState.poseValid          = false;
                poseState.interactionProfile = ctrl.interactionProfile;
                InputSystem::getInstance()->handleXRInput(poseState);
            }
            ctrl.rayHitValid                     = false;
            ctrl.stabilizedRayValid              = false;
            ctrl.rawWorldRayValid                = false;
            ctrl.lastStabilizedSampleTime        = 0;
            ctrl.stabilizationReferenceDistance = fallbackTrackingDistance;
        }

        ctrl.triggerPrevious          = ctrl.triggerPressed;
        ctrl.gripPrevious             = ctrl.gripPressed;
        ctrl.thumbstickClickPrevious  = ctrl.thumbstickClickPressed;
        ctrl.menuPrevious             = ctrl.menuPressed;
        ctrl.aPrevious                = ctrl.aPressed;
        ctrl.bPrevious                = ctrl.bPressed;
        ctrl.xPrevious                = ctrl.xPressed;
        ctrl.yPrevious                = ctrl.yPressed;
        ctrl.thumbstickPrevious       = ctrl.thumbstick;
        ctrl.thumbstickActivePrevious = ctrl.thumbstickActive;
        ctrl.posePrevious             = ctrl.poseValid;
    }
}

void OpenXRDriver::logXrInteractionProfiles()
{
    if (_xrSession == XR_NULL_HANDLE)
        return;

    auto pathToString = [&](XrPath path) -> std::string {
        if (path == XR_NULL_PATH)
            return {};

        uint32_t count = 0;
        if (XR_FAILED(xrPathToString(_xrInstance, path, 0, &count, nullptr)) || count == 0)
            return {};

        std::string value(count, '\0');
        if (XR_FAILED(xrPathToString(_xrInstance, path, count, &count, value.data())))
            return {};
        if (!value.empty() && value.back() == '\0')
            value.pop_back();
        return value;
    };

    for (uint32_t hand = 0; hand < 2; ++hand)
    {
        auto& ctrl = _controllers[hand];
        if (ctrl.handPath == XR_NULL_PATH)
            continue;

        XrInteractionProfileState state{XR_TYPE_INTERACTION_PROFILE_STATE};
        XrResult result = xrGetCurrentInteractionProfile(_xrSession, ctrl.handPath, &state);
        auto handPath   = pathToString(ctrl.handPath);
        auto profile    = pathToString(state.interactionProfile);
        if (XR_SUCCEEDED(result))
            ctrl.interactionProfile = profile;
        AXLOGI("[OpenXR] Current interaction profile for '{}': '{}' (ec:{})", handPath, profile,
               static_cast<int>(result));
    }
}

void OpenXRDriver::shutdownXrActions()
{
    for (auto& ctrl : _controllers)
    {
        if (ctrl.aimSpace != XR_NULL_HANDLE)
        {
            xrDestroySpace(ctrl.aimSpace);
            ctrl.aimSpace = XR_NULL_HANDLE;
        }
        if (ctrl.gripSpace != XR_NULL_HANDLE)
        {
            xrDestroySpace(ctrl.gripSpace);
            ctrl.gripSpace = XR_NULL_HANDLE;
        }
    }

    if (_triggerAction != XR_NULL_HANDLE)
    {
        xrDestroyAction(_triggerAction);
        _triggerAction = XR_NULL_HANDLE;
    }
    if (_triggerValueAction != XR_NULL_HANDLE)
    {
        xrDestroyAction(_triggerValueAction);
        _triggerValueAction = XR_NULL_HANDLE;
    }
    if (_gripAction != XR_NULL_HANDLE)
    {
        xrDestroyAction(_gripAction);
        _gripAction = XR_NULL_HANDLE;
    }
    if (_gripValueAction != XR_NULL_HANDLE)
    {
        xrDestroyAction(_gripValueAction);
        _gripValueAction = XR_NULL_HANDLE;
    }
    if (_thumbstickAction != XR_NULL_HANDLE)
    {
        xrDestroyAction(_thumbstickAction);
        _thumbstickAction = XR_NULL_HANDLE;
    }
    if (_thumbstickClickAction != XR_NULL_HANDLE)
    {
        xrDestroyAction(_thumbstickClickAction);
        _thumbstickClickAction = XR_NULL_HANDLE;
    }
    if (_menuAction != XR_NULL_HANDLE)
    {
        xrDestroyAction(_menuAction);
        _menuAction = XR_NULL_HANDLE;
    }
    if (_aAction != XR_NULL_HANDLE)
    {
        xrDestroyAction(_aAction);
        _aAction = XR_NULL_HANDLE;
    }
    if (_bAction != XR_NULL_HANDLE)
    {
        xrDestroyAction(_bAction);
        _bAction = XR_NULL_HANDLE;
    }
    if (_xAction != XR_NULL_HANDLE)
    {
        xrDestroyAction(_xAction);
        _xAction = XR_NULL_HANDLE;
    }
    if (_yAction != XR_NULL_HANDLE)
    {
        xrDestroyAction(_yAction);
        _yAction = XR_NULL_HANDLE;
    }
    if (_aimPoseAction != XR_NULL_HANDLE)
    {
        xrDestroyAction(_aimPoseAction);
        _aimPoseAction = XR_NULL_HANDLE;
    }
    if (_gripPoseAction != XR_NULL_HANDLE)
    {
        xrDestroyAction(_gripPoseAction);
        _gripPoseAction = XR_NULL_HANDLE;
    }

    if (_inputActionSet != XR_NULL_HANDLE)
    {
        xrDestroyActionSet(_inputActionSet);
        _inputActionSet = XR_NULL_HANDLE;
    }
}

// ---------------------------------------------------------------------------
// Swapchain helpers
// ---------------------------------------------------------------------------

bool OpenXRDriver::createSwapchain(uint32_t width,
                                   uint32_t height,
                                   int64_t format,
                                   XrSwapchain* outSwapchain,
                                   uint32_t* outWidth,
                                   uint32_t* outHeight)
{
    XrSwapchainCreateInfo swapchainInfo{XR_TYPE_SWAPCHAIN_CREATE_INFO};
    swapchainInfo.createFlags = 0;
    swapchainInfo.usageFlags  = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.format      = format;
    swapchainInfo.sampleCount = 1;
    swapchainInfo.width       = width;
    swapchainInfo.height      = height;
    swapchainInfo.faceCount   = 1;
    swapchainInfo.arraySize   = 1;
    swapchainInfo.mipCount    = 1;

    if (!checkXr(xrCreateSwapchain(_xrSession, &swapchainInfo, outSwapchain), "xrCreateSwapchain"))
        return false;

    *outWidth  = width;
    *outHeight = height;
    return true;
}

void OpenXRDriver::destroySwapchains()
{
    if (_colorSwapchains.empty())
        return;
    for (auto& sc : _colorSwapchains)
    {
        for (auto& rt : sc.renderTargets)
            AX_SAFE_RELEASE(rt);
        sc.renderTargets.clear();

        for (auto& texture : sc.textures)
            AX_SAFE_RELEASE(texture);
        sc.textures.clear();

        if (sc.handle != XR_NULL_HANDLE)
        {
            xrDestroySwapchain(sc.handle);
            sc.handle = XR_NULL_HANDLE;
        }
    }
    _colorSwapchains.clear();
}

rhi::Texture* OpenXRDriver::createDepthTexture(uint32_t width, uint32_t height)
{
    rhi::TextureDesc desc;
    desc.width        = static_cast<uint16_t>(width);
    desc.height       = static_cast<uint16_t>(height);
    desc.pixelFormat  = rhi::PixelFormat::D24S8;
    desc.textureUsage = rhi::TextureUsage::RENDER_TARGET;
    desc.textureType  = rhi::TextureType::TEXTURE_2D;
    desc.arraySize    = 1;
    desc.mipLevels    = 1;

    return axdrv->createTexture(desc);
}

// ---------------------------------------------------------------------------
// Graphics binding (backend-specific)
// ---------------------------------------------------------------------------

const void* OpenXRDriver::createGraphicsBinding()
{
    // We allocate a persistent structure and store the pointer.
    // This structure must remain valid for the lifetime of the session.
    const auto driverType = rhi::GraphicsCore::currentDriverType();

#if AX_ENABLE_D3D11
    if (driverType == rhi::DriverType::D3D11)
    {
        auto storage            = new XrGraphicsBindingD3D11KHR{XR_TYPE_GRAPHICS_BINDING_D3D11_KHR};
        storage->device         = static_cast<rhi::d3d11::DriverImpl*>(axdrv)->getDevice();
        _graphicsBindingStorage = storage;
        return storage;
    }
#endif

#if AX_ENABLE_D3D12
    if (driverType == rhi::DriverType::D3D12)
    {
        auto storage            = new XrGraphicsBindingD3D12KHR{XR_TYPE_GRAPHICS_BINDING_D3D12_KHR};
        storage->device         = static_cast<rhi::d3d12::DriverImpl*>(axdrv)->getDevice();
        storage->queue          = static_cast<rhi::d3d12::DriverImpl*>(axdrv)->getGraphicsQueue();
        _graphicsBindingStorage = storage;
        return storage;
    }
#endif

#if AX_ENABLE_VK
    if (driverType == rhi::DriverType::Vulkan)
    {
        auto storage              = new XrGraphicsBindingVulkanKHR{XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR};
        auto vkDriver             = static_cast<rhi::vk::DriverImpl*>(axdrv);
        storage->instance         = vkDriver->getInstance();
        storage->physicalDevice   = vkDriver->getPhysical();
        storage->device           = vkDriver->getDevice();
        storage->queueFamilyIndex = vkDriver->getGraphicsQueueFamily();
        storage->queueIndex       = 0;
        _graphicsBindingStorage   = storage;
        return storage;
    }
#endif

#if AX_ENABLE_GL
    if (driverType == rhi::DriverType::OpenGL)
    {
#    if AX_GLES_PROFILE && AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
        EGLDisplay display = eglGetCurrentDisplay();
        EGLContext context = eglGetCurrentContext();
        EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);
        if (display == EGL_NO_DISPLAY || context == EGL_NO_CONTEXT || surface == EGL_NO_SURFACE)
        {
            AXLOGW("[OpenXR] Failed to create OpenGL ES graphics binding: no current EGL display/context/surface");
            return nullptr;
        }

        EGLint configId = 0;
        if (!eglQuerySurface(display, surface, EGL_CONFIG_ID, &configId))
        {
            AXLOGW("[OpenXR] Failed to query EGL config id for OpenGL ES graphics binding");
            return nullptr;
        }

        EGLConfig config             = nullptr;
        EGLint configCount           = 0;
        const EGLint configAttribs[] = {EGL_CONFIG_ID, configId, EGL_NONE};
        if (!eglChooseConfig(display, configAttribs, &config, 1, &configCount) || configCount <= 0 || !config)
        {
            AXLOGW("[OpenXR] Failed to resolve EGL config for OpenGL ES graphics binding");
            return nullptr;
        }

        auto storage     = new XrGraphicsBindingOpenGLESAndroidKHR{XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR};
        storage->display = display;
        storage->config  = config;
        storage->context = context;
        _graphicsBindingStorage = storage;
        return storage;
#    else
        AXLOGW(
            "[OpenXR] OpenGL graphics binding is unsupported for this platform/profile. "
            "Windows ANGLE GLES and desktop OpenGL are not valid OpenXR backends in axmol.");
        return nullptr;
#    endif
    }
#endif

    AXLOGW("[OpenXR] Failed to create graphics binding for RHI driver type: {}", static_cast<int>(driverType));
    return nullptr;
}

bool OpenXRDriver::checkVulkanGraphicsDevice()
{
#if AX_ENABLE_VK
    auto vkDriver = static_cast<rhi::vk::DriverImpl*>(axdrv);
    if (!vkDriver || vkDriver->getInstance() == VK_NULL_HANDLE || vkDriver->getPhysical() == VK_NULL_HANDLE)
    {
        AXLOGW("[OpenXR] Vulkan graphics device check failed: RHI Vulkan instance or physical device is null");
        return false;
    }

    PFN_xrGetVulkanGraphicsDeviceKHR getGraphicsDevice = nullptr;
    if (!checkXr(xrGetInstanceProcAddr(_xrInstance, "xrGetVulkanGraphicsDeviceKHR",
                                       reinterpret_cast<PFN_xrVoidFunction*>(&getGraphicsDevice)),
                 "xrGetInstanceProcAddr(xrGetVulkanGraphicsDeviceKHR)"))
    {
        return false;
    }

    VkPhysicalDevice xrPhysicalDevice = VK_NULL_HANDLE;
    if (!checkXr(getGraphicsDevice(_xrInstance, _xrSystem, vkDriver->getInstance(), &xrPhysicalDevice),
                 "xrGetVulkanGraphicsDeviceKHR"))
    {
        return false;
    }

    if (xrPhysicalDevice != vkDriver->getPhysical())
    {
        AXLOGW("[OpenXR] Vulkan physical device mismatch: OpenXR runtime selected {}, RHI selected {}",
               static_cast<const void*>(xrPhysicalDevice), static_cast<const void*>(vkDriver->getPhysical()));
        return false;
    }

    return true;
#else
    return true;
#endif
}

bool OpenXRDriver::checkGraphicsRequirements()
{
    const auto driverType = rhi::GraphicsCore::currentDriverType();

#if AX_ENABLE_D3D11
    if (driverType == rhi::DriverType::D3D11)
    {
        PFN_xrGetD3D11GraphicsRequirementsKHR getRequirements = nullptr;
        if (!checkXr(xrGetInstanceProcAddr(_xrInstance, "xrGetD3D11GraphicsRequirementsKHR",
                                           reinterpret_cast<PFN_xrVoidFunction*>(&getRequirements)),
                     "xrGetInstanceProcAddr(xrGetD3D11GraphicsRequirementsKHR)"))
            return false;

        XrGraphicsRequirementsD3D11KHR requirements{XR_TYPE_GRAPHICS_REQUIREMENTS_D3D11_KHR};
        return checkXr(getRequirements(_xrInstance, _xrSystem, &requirements), "xrGetD3D11GraphicsRequirementsKHR");
    }
#endif

#if AX_ENABLE_D3D12
    if (driverType == rhi::DriverType::D3D12)
    {
        PFN_xrGetD3D12GraphicsRequirementsKHR getRequirements = nullptr;
        if (!checkXr(xrGetInstanceProcAddr(_xrInstance, "xrGetD3D12GraphicsRequirementsKHR",
                                           reinterpret_cast<PFN_xrVoidFunction*>(&getRequirements)),
                     "xrGetInstanceProcAddr(xrGetD3D12GraphicsRequirementsKHR)"))
            return false;

        XrGraphicsRequirementsD3D12KHR requirements{XR_TYPE_GRAPHICS_REQUIREMENTS_D3D12_KHR};
        return checkXr(getRequirements(_xrInstance, _xrSystem, &requirements), "xrGetD3D12GraphicsRequirementsKHR");
    }
#endif

#if AX_ENABLE_VK
    if (driverType == rhi::DriverType::Vulkan)
    {
        PFN_xrGetVulkanGraphicsRequirementsKHR getRequirements = nullptr;
        if (!checkXr(xrGetInstanceProcAddr(_xrInstance, "xrGetVulkanGraphicsRequirementsKHR",
                                           reinterpret_cast<PFN_xrVoidFunction*>(&getRequirements)),
                     "xrGetInstanceProcAddr(xrGetVulkanGraphicsRequirementsKHR)"))
            return false;

        XrGraphicsRequirementsVulkanKHR requirements{XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR};
        if (!checkXr(getRequirements(_xrInstance, _xrSystem, &requirements), "xrGetVulkanGraphicsRequirementsKHR"))
            return false;

        return checkVulkanGraphicsDevice();
    }
#endif

#if AX_ENABLE_GL
    if (driverType == rhi::DriverType::OpenGL)
    {
#    if AX_GLES_PROFILE && AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
        PFN_xrGetOpenGLESGraphicsRequirementsKHR getRequirements = nullptr;
        if (!checkXr(xrGetInstanceProcAddr(_xrInstance, "xrGetOpenGLESGraphicsRequirementsKHR",
                                           reinterpret_cast<PFN_xrVoidFunction*>(&getRequirements)),
                     "xrGetInstanceProcAddr(xrGetOpenGLESGraphicsRequirementsKHR)"))
            return false;

        XrGraphicsRequirementsOpenGLESKHR requirements{XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_ES_KHR};
        return checkXr(getRequirements(_xrInstance, _xrSystem, &requirements), "xrGetOpenGLESGraphicsRequirementsKHR");
#    else
        AXLOGW(
            "[OpenXR] OpenGL graphics requirements are unsupported for this platform/profile. "
            "Windows ANGLE GLES and desktop OpenGL are not valid OpenXR backends in axmol.");
        return false;
#    endif
    }
#endif

    AXLOGW("[OpenXR] Graphics requirements check is not implemented for RHI driver type: {}",
           static_cast<int>(driverType));
    return false;
}

}  // namespace experimental
}  // namespace ax
