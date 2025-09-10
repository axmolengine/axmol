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

#pragma once

#include <stack>

#include "axmol/vr/VRBase.h"
#include "axmol/renderer/CustomCommand.h"

namespace ax
{

class Camera;
class Sprite;
class RenderTexture;
class Director;
class RenderView;

namespace experimental
{
class DistortionMesh;
class Distortion;
class VRGenericHeadTracker;

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
};

struct RasterTransform
{
    float sx{1}, sy{1};
    float ox{0}, oy{0};
};

class AX_DLL VRGenericRenderer : public IVRRenderer
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

    IVRHeadTracker* getHeadTracker() override;

    void setScissorRect(float x, float y, float w, float h) override;
    const ScissorRect& getScissorRect() const override;

    void onRenderViewResized(RenderView* rv) override;

    void init(RenderView* rv) override;
    void cleanup() override;

    void render(Scene* scene, Renderer* renderer) override;

protected:
    void setupProgram();

    void fillEyeViewports(RenderView* rv, const Vec2& screenSize);

    DistortionMesh* createDistortionMesh(VREye::EyeType eyeType, const Size& screenSize);

    void pushLeftRasterTransform(Renderer* renderer);
    void pushRightRasterTransform(Renderer* renderer);

    void popRasterTransform(Renderer* renderer);

    void pushScissorTransform(const RasterTransform& xf) { _stStack.push(xf); }
    void popScissorTransform() { _stStack.pop(); }

    Director* _director{nullptr};

    ScissorRect _sourceScissorRect;

    // static resources
    VRGenericHeadTracker* _headTracker{nullptr};
    rhi::ProgramState* _programState{nullptr};
    rhi::VertexLayout* _vertexLayout{nullptr};

    // runtime resources, rebuild when screen size changed
    RenderTexture* _renderTexture{nullptr};
    DistortionMesh* _leftDistortionMesh{nullptr};
    DistortionMesh* _rightDistortionMesh{nullptr};
    Distortion* _distortion{nullptr};

    VREye _leftEye;
    VREye _rightEye;
    RasterTransform _xfLeft;
    RasterTransform _xfRight;

    bool _vignetteEnabled{true};

    bool _debugIgnoreHeadTracker{false};

    CustomCommand _leftEyeCmd;
    CustomCommand _rightEyeCmd;

    std::stack<RasterTransform, std::vector<RasterTransform>> _stStack;
};
}  // namespace experimental
}  // namespace ax
