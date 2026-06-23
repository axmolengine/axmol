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

#pragma once

#include <array>

#include "axmol/vr/VRBase.h"
#include "axmol/scene/SceneRenderer.h"
#include "axmol/renderer/CustomCommand.h"
#include "axmol/renderer/RenderTexturePass.h"

namespace ax
{

class Camera;
class Director;
class RenderView;

inline namespace experimental
{
class DistortionMesh;
class Distortion;
class VRGenericHeadTracker;

struct ScissorTransform
{
    float sx{1}, sy{1};
    float ox{0}, oy{0};
};

struct VREye
{
    enum class EyeType
    {
        MONO,
        LEFT,
        RIGHT,
    };

    EyeType type;
    Viewport viewport;
    ScissorTransform scissorTransform;
};

/**
 * @brief Generic stereo renderer with lens distortion output.
 *
 * VRGenericRenderer renders the current scene into a side-by-side render texture,
 * once for the left eye and once for the right eye, then draws distortion meshes
 * to present the final image on the screen.
 *
 * This renderer is intended as a generic VR rendering foundation. It does not
 * create duplicate scene cameras. Instead, it reuses the scene's existing cameras
 * and applies per-eye view overrides during rendering. This keeps camera ownership
 * in the scene while still allowing stereo rendering behavior.
 *
 * The renderer also remaps scissor rectangles into the active eye viewport so UI
 * clipping components such as ScrollView remain correct during VR rendering.
 *
 * This class is not a full XR runtime backend. Device tracking, per-eye projection,
 * swapchains, and frame timing from runtimes such as OpenXR should be integrated
 * by a dedicated XR backend on top of this rendering foundation.
 */
class AX_DLL VRGenericRenderer : public SceneRenderer
{
public:
    VRGenericRenderer();
    ~VRGenericRenderer() override;

    /**
     * Toggles whether to ignore head tracker rotation during VR rendering.
     * This is primarily intended for debugging on devices without a connected headset,
     * such as Android or iOS emulators.
     *
     * @param debug If true, the head tracker rotation is ignored and an identity transform is used.
     *              If false, the actual head tracker rotation is applied.
     */
    void setDebugIgnoreHeadTracker(bool debug) { _debugIgnoreHeadTracker = debug; }

    /**
     * @brief Sets the distance between the left and right eye views in scene units.
     *
     * The renderer uses half of this value as the per-eye offset when building the
     * left and right eye view transforms. The default value preserves the original
     * generic VR behavior and is mainly intended for non-XR-runtime preview usage.
     *
     * A real XR backend should normally use the per-eye poses supplied by the XR
     * runtime instead of relying on this fixed separation value.
     *
     * @param separation Eye separation in scene units.
     */
    void setEyeSeparation(float separation) { _eyeSeparation = separation; }

    /**
     * @brief Returns the configured distance between the left and right eye views.
     */
    float getEyeSeparation() const { return _eyeSeparation; }

    /**
     * @brief Enables or disables the vignette effect in the distortion mesh.
     *
     * The vignette darkens the outer edge of each eye image and can help hide lens
     * distortion boundaries in generic VR previews.
     *
     * @param enabled True to enable vignette rendering; false to disable it.
     */
    void setVignetteEnabled(bool enabled) { _vignetteEnabled = enabled; }

    /**
     * @brief Returns whether vignette rendering is enabled.
     */
    bool isVignetteEnabled() const { return _vignetteEnabled; }

    /**
     * @brief Returns the head tracker used to produce the generic VR head rotation.
     *
     * The returned tracker belongs to the renderer and should not be released by the caller.
     */
    IVRHeadTracker* getHeadTracker();

    /**
     * @brief Sets the offscreen render scale used by the generic VR render texture.
     *
     * The scale is applied to the final VR output viewport size when creating the
     * side-by-side render texture. A value greater than 1.0 renders the eye views
     * at a higher offscreen resolution before the distortion pass, improving VR
     * sampling quality at the cost of GPU memory and fill rate.
     *
     * @param renderScale Offscreen render scale. Values less than or equal to zero are invalid.
     */
    void setRenderScale(float renderScale);

    /**
     * @brief Returns the offscreen render scale used by the generic VR render texture.
     */
    float getRenderScale() const { return _renderScale; }

protected:
    enum EyeIndex
    {
        LeftEyeIndex = 0,
        RightEyeIndex,
        EyeCount,
    };

    void onRenderViewChanged(RenderViewCore* rv) override;
    void renderScene(Renderer* renderer, Scene* scene) override;
    void setScissorRect(float x, float y, float w, float h) override;
    const ScissorRect& getScissorRect() const override;

    void reset(RenderViewCore* rv);
    void setupDistortionProgram();

    void prepareEyesViewport(const Vec2& sourceSize, const Vec2& renderSize);
    DistortionMesh* createDistortionMesh(VREye::EyeType eyeType, const Size& screenSize);

    void clearEyeRenderTexture(Renderer* renderer, const Color& clearColor);
    void renderSceneToEye(Renderer* renderer, Scene* scene, EyeIndex eyeIndex, const Mat4& eyeTransform);
    void renderDistortionPass(Renderer* renderer);

    Director* _director{nullptr};

    ScissorRect _sourceScissorRect;

    VRGenericHeadTracker* _headTracker{nullptr};
    rhi::ProgramState* _programState{nullptr};
    rhi::VertexLayout* _vertexLayout{nullptr};

    // runtime resources, rebuild when screen size changed
    RenderTexture* _renderTexture{nullptr};
    RefPtr<RenderTexturePass> _rtPass;
    DistortionMesh* _leftDistortionMesh{nullptr};
    DistortionMesh* _rightDistortionMesh{nullptr};
    Distortion* _distortion{nullptr};

    std::array<VREye, EyeCount> _eyes;

    float _eyeSeparation{1.0f};
    float _renderScale{2.0f};

    bool _vignetteEnabled{true};
    bool _debugIgnoreHeadTracker{false};

    CustomCommand _leftEyeCmd;
    CustomCommand _rightEyeCmd;

    // Runtime stack used to transform scissor rectangles from the normal screen
    // viewport into the active eye viewport while queued UI clipping commands execute,
    // ensuring clipped components such as ScrollView remain correct in VR rendering.
    LinearStack<ScissorTransform> _scissorTransformStack;
};
}  // namespace experimental
}  // namespace ax
