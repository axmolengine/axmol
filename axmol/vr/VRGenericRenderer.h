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

#include "axmol/vr/VRBase.h"
#include "axmol/renderer/CustomCommand.h"

namespace ax
{

class Camera;
class Sprite;
class RenderTexture;

namespace experimental
{
class DistortionMesh;
class Distortion;
class VRGenericHeadTracker;

struct AX_DLL VREye
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

class AX_DLL VRGenericRenderer : public IVRRenderer
{
public:
    VRGenericRenderer();
    virtual ~VRGenericRenderer();

    virtual void setup(RenderView* renderView) override;
    virtual void cleanup() override;
    virtual void render(Scene* scene, Renderer* renderer) override;
    virtual IVRHeadTracker* getHeadTracker() override;

protected:
    void setupProgram();
    DistortionMesh* createDistortionMesh(VREye::EyeType eyeType);

    void pushRasterTransform(Renderer* renderer);
    void popRasterTransform(Renderer* renderer);

    RenderTexture* _renderTexture{nullptr};
    rhi::ProgramState* _programState{nullptr};
    rhi::VertexLayout* _vertexLayout{nullptr};
    Size _winSize;  // The window logic size
    Size _rtSize;   // The render texture (render target) size
    VREye _leftEye;
    VREye _rightEye;
    DistortionMesh* _leftDistortionMesh{nullptr};
    DistortionMesh* _rightDistortionMesh{nullptr};
    Distortion* _distortion{nullptr};
    bool _vignetteEnabled{false};

    CustomCommand _leftEyeCmd;
    CustomCommand _rightEyeCmd;
    VRGenericHeadTracker* _headTracker{nullptr};
};
}  // namespace experimental
}  // namespace ax
