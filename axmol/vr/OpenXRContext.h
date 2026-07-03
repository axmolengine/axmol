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

#pragma once

#include <string>
#include <vector>

#include "axmol/3d/Ray.h"
#include "axmol/base/RefPtr.h"
#include "axmol/math/Mat4.h"
#include "axmol/platform/PlatformMacros.h"

#include "openxr/openxr.h"

namespace ax
{

class Director;
class RenderViewCore;
class Camera;

namespace rhi
{
class RenderTarget;
class Texture;
}  // namespace rhi

inline namespace experimental
{

/**
 * @brief Long-lived OpenXR backend context for VRSceneCompositor.
 *
 * OpenXRContext owns OpenXR instance/session/space/swapchain/action state and
 * wraps swapchain images for the active RHI backend. It is not a per-frame data
 * object and is intentionally separate from axmol's RHI RenderContext types.
 * `pollEvents()` is the single pre-update entry point; it handles OpenXR events,
 * waits frame timing, and syncs frame-bound input. `_inFrame` tracks only the
 * xrBeginFrame/xrEndFrame pairing state.
 */
class AX_DLL OpenXRContext
{
public:
    struct ControllerState
    {
        XrPath handPath{XR_NULL_PATH};
        XrPath aimPath{XR_NULL_PATH};
        XrPath gripPath{XR_NULL_PATH};
        XrSpace aimSpace{XR_NULL_HANDLE};
        XrSpace gripSpace{XR_NULL_HANDLE};

        bool triggerPressed{false};
        bool triggerPrevious{false};
        bool gripPressed{false};
        bool gripPrevious{false};
        bool thumbstickClickPressed{false};
        bool thumbstickClickPrevious{false};
        bool menuPressed{false};
        bool menuPrevious{false};
        bool aPressed{false};
        bool aPrevious{false};
        bool bPressed{false};
        bool bPrevious{false};
        bool xPressed{false};
        bool xPrevious{false};
        bool yPressed{false};
        bool yPrevious{false};
        XrVector2f thumbstick{};
        XrVector2f thumbstickPrevious{};
        bool thumbstickActive{false};
        bool thumbstickActivePrevious{false};
        bool posePrevious{false};

        Ray currentRay;
        Vec3 visualRayOrigin{Vec3::zero};
        Vec3 visualRayStart{Vec3::zero};
        Vec3 rayHitPoint{Vec3::zero};
        bool visualRayOriginValid{false};
        bool visualRayStartValid{false};
        bool rayHitValid{false};
        bool poseValid{false};
        intptr_t pointerId{-1};
        std::string interactionProfile;
    };

    struct AcquiredSwapchain
    {
        XrSwapchain handle{XR_NULL_HANDLE};
        uint32_t index{0};
        uint32_t width{0};
        uint32_t height{0};
        rhi::Texture* texture{nullptr};
        rhi::RenderTarget* renderTarget{nullptr};
    };

    OpenXRContext();
    ~OpenXRContext();

    void onRenderViewChanged(RenderViewCore* rv);
    /** Polls OpenXR session events, waits the next frame, and syncs frame-bound input. */
    void pollEvents();

    bool beginRenderFrame();
    bool locateViews(uint32_t& viewCountOutput);
    bool acquireSwapchains(std::vector<AcquiredSwapchain>& acquired);
    void releaseSwapchains(const std::vector<AcquiredSwapchain>& acquired);
    void endFrameEmpty();
    void endFrameWithProjectionLayer(const std::vector<AcquiredSwapchain>& acquired, uint32_t viewCountOutput);

    bool isInitialized() const { return _initialized; }
    bool isSessionRunning() const { return _sessionRunning; }
    XrInstance getXrInstance() const { return _xrInstance; }
    XrSession getXrSession() const { return _xrSession; }
    XrSpace getLocalSpace() const { return _localSpace; }
    const XrFrameState& getFrameState() const { return _frameState; }
    const std::vector<XrView>& getViews() const { return _views; }
    const ControllerState* getControllers() const { return _controllers; }
    Camera* ensurePointerRayCamera(float nearZ, float farZ);
    Camera* getPointerRayCamera() const { return _pointerRayCamera.get(); }
    void setXrToSceneScale(float scale) { _xrToSceneScale = scale > 0.0f ? scale : 1.0f; }
    float getXrToSceneScale() const { return _xrToSceneScale; }

    static Mat4 xrPoseToMat4(const XrPosef& pose);
    static Mat4 xrFovToProjection(const XrFovf& fov, float nearZ, float farZ);

private:
    bool initXrInstance();
    bool initXrSystem();
    bool initXrSession();
    bool initXrSwapchains();
    bool initXrSpaces();
    void shutdownXr();

    bool createSwapchain(uint32_t width,
                         uint32_t height,
                         int64_t format,
                         XrSwapchain* outSwapchain,
                         uint32_t* outWidth,
                         uint32_t* outHeight);
    void destroySwapchains();

    rhi::Texture* createDepthTexture(uint32_t width, uint32_t height);
    const void* createGraphicsBinding();
    bool checkGraphicsRequirements();
    bool checkVulkanGraphicsDevice();

    bool initXrActions();
    void pollXrActions(XrTime predictedDisplayTime);
    void syncPointerRayCamera();
    void updatePointerViewTransform(uint32_t viewCount);
    void shutdownXrActions();
    bool xrPollEvents();
    void logXrInteractionProfiles();

    static Ray xrPoseToRay(const XrPosef& pose);
    static Vec2 xrToVec2(const XrVector2f& v);

    struct SwapchainData
    {
        XrSwapchain handle{XR_NULL_HANDLE};
        uint32_t width{0};
        uint32_t height{0};
        std::vector<rhi::Texture*> textures;
        std::vector<rhi::RenderTarget*> renderTargets;
    };

    Director* _director{nullptr};

    XrInstance _xrInstance{XR_NULL_HANDLE};
    XrSession _xrSession{XR_NULL_HANDLE};
    XrSpace _localSpace{XR_NULL_HANDLE};
    XrSystemId _xrSystem{XR_NULL_SYSTEM_ID};

    XrViewConfigurationType _viewConfigType{XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO};
    uint32_t _viewCount{2};
    std::vector<XrViewConfigurationView> _viewConfigViews;
    std::vector<XrView> _views;
    uint32_t _locatedViewCount{0};
    bool _viewsLocated{false};
    bool _locatedViewsPoseValid{false};
    int64_t _colorFormat{0};

    std::vector<SwapchainData> _colorSwapchains;
    std::vector<rhi::RenderTarget*> _renderTargets;
    std::vector<rhi::Texture*> _depthTextures;

    XrActionSet _inputActionSet{XR_NULL_HANDLE};
    XrAction _triggerAction{XR_NULL_HANDLE};
    XrAction _triggerValueAction{XR_NULL_HANDLE};
    XrAction _gripAction{XR_NULL_HANDLE};
    XrAction _gripValueAction{XR_NULL_HANDLE};
    XrAction _thumbstickAction{XR_NULL_HANDLE};
    XrAction _thumbstickClickAction{XR_NULL_HANDLE};
    XrAction _menuAction{XR_NULL_HANDLE};
    XrAction _aAction{XR_NULL_HANDLE};
    XrAction _bAction{XR_NULL_HANDLE};
    XrAction _xAction{XR_NULL_HANDLE};
    XrAction _yAction{XR_NULL_HANDLE};
    XrAction _aimPoseAction{XR_NULL_HANDLE};
    XrAction _gripPoseAction{XR_NULL_HANDLE};
    ControllerState _controllers[2];
    RefPtr<Camera> _pointerRayCamera;
    float _pointerRayNearZ{0.1f};
    float _pointerRayFarZ{1000.0f};
    Mat4 _pointerViewTransform{Mat4::identity};
    bool _pointerViewTransformValid{false};
    float _xrToSceneScale{1.0f};

    void* _graphicsBindingStorage{nullptr};

    XrSessionState _sessionState{XR_SESSION_STATE_UNKNOWN};
    XrFrameState _frameState{XR_TYPE_FRAME_STATE};
    bool _sessionRunning{false};
    bool _initialized{false};
    bool _frameReady{false};
    bool _inFrame{false};
};

}  // namespace experimental
}  // namespace ax
