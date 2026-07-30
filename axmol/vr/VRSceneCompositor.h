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

#include "axmol/renderer/CustomCommand.h"
#include "axmol/scene/SceneCompositor.h"
#include "axmol/vr/OpenXRDriver.h"

namespace ax
{

class Camera;
struct SceneViewData;

inline namespace experimental
{

struct VRScissorTransform
{
    float sx{1.0f};
    float sy{1.0f};
    float ox{0.0f};
    float oy{0.0f};
};

enum class ControllerRayVisualStyle
{
    Straight,
    SubtleCurve,
};

/**
 * @brief Product VR scene compositor backed by OpenXR.
 *
 * VRSceneCompositor bridges Director's SceneCompositor lifecycle to OpenXRDriver.
 * It polls OpenXR frame-bound state before scheduler update, then renders the
 * scene per eye, draws controller rays, and submits the projection layer for the
 * current XR frame.
 */
class AX_DLL VRSceneCompositor : public SceneCompositor
{
public:
    VRSceneCompositor();
    ~VRSceneCompositor() override;

    void setNearZ(float nearZ) { _nearZ = nearZ; }
    float getNearZ() const { return _nearZ; }

    void setFarZ(float farZ) { _farZ = farZ; }
    float getFarZ() const { return _farZ; }

    bool isInitialized() const;

    void setControllerRayVisible(bool visible) { _controllerRayVisible = visible; }
    void setControllerRayLength(float length) { _controllerRayLength = length; }
    void setControllerRayVisualStyle(ControllerRayVisualStyle style) { _controllerRayVisualStyle = style; }
    ControllerRayVisualStyle getControllerRayVisualStyle() const { return _controllerRayVisualStyle; }
    void setControllerRayColors(const Color& idle, const Color& pressed, const Color& hit);
    void setXrToSceneScale(float scale);
    float getXrToSceneScale() const { return _xrToSceneScale; }

    XrInstance getXrInstance() const;
    XrSession getXrSession() const;

    void setXrDriver(OpenXRDriver* xrDriver);
    void pollEvents() override;

protected:
    bool isVRActive() const override;
    void onRenderViewChanged(RenderViewCore* rv) override;
    void renderScene(Renderer* renderer, Scene* scene) override;
    void setScissorRect(float x, float y, float w, float h) override;
    const ScissorRect& getScissorRect() const override;

private:
    void resolveXrFrameInput(Scene* scene);
    void ensureControllerRayResources();
    void shutdownControllerRayResources();
    Camera* resolveXrViewCamera(Scene* scene) const;
    void drawControllerRays(Renderer* renderer, const SceneViewData& view);
    void onBeforeControllerRayDraw();
    void onAfterControllerRayDraw();

    OpenXRDriver* _xrDriver{nullptr};

    CustomCommand _controllerRayCommands[2];
    rhi::UniformLocation _controllerRayMVPLocation;
    bool _controllerRayResourcesInitialized{false};
    bool _controllerRayVisible{true};
    ControllerRayVisualStyle _controllerRayVisualStyle{ControllerRayVisualStyle::SubtleCurve};
    float _controllerRayLength{50.0f};
    Color _controllerRayIdleColor{0.25f, 0.75f, 1.0f, 0.9f};
    Color _controllerRayPressedColor{1.0f, 1.0f, 1.0f, 1.0f};
    Color _controllerRayHitColor{0.2f, 1.0f, 0.6f, 1.0f};
    bool _controllerRayOldDepthTest{false};
    bool _controllerRayOldDepthWrite{false};

    float _nearZ{0.1f};
    float _farZ{1000.0f};
    float _xrToSceneScale{1.0f};
    Mat4 _frameTrackingToWorld{Mat4::identity};
    bool _frameTrackingToWorldValid{false};

    RefPtr<RenderTexturePass> _rtPass;

    ScissorRect _sourceScissorRect;
    LinearStack<VRScissorTransform> _scissorTransformStack;
};

}  // namespace experimental
}  // namespace ax
