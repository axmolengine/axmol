/****************************************************************************
 Copyright (c) 2015-2016 Chukong Technologies Inc.
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

#include "axmol/math/Math.h"
#include "axmol/base/Object.h"
#include "axmol/base/Types.h"
#include "axmol/base/Config.h"
#include "axmol/renderer/CustomCommand.h"
#include "axmol/rhi/ProgramState.h"

#if defined(AX_ENABLE_PHYSICS_3D)
#    include <Jolt/Renderer/DebugRenderer.h>
#    include <Jolt/Core/Reference.h>

namespace ax
{

/**
 * @addtogroup _3d
 * @{
 */

class Renderer;

/** @brief PhysicsDebugDraw3D: debug draw the physics object, used by PhysicsWorld3D */
class PhysicsDebugDraw3D : public JPH::DebugRenderer
{
public:
    PhysicsDebugDraw3D();
    virtual ~PhysicsDebugDraw3D();

    // Jolt DebugRenderer overrides
    void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
    void DrawTriangle(JPH::RVec3Arg inV1,
                      JPH::RVec3Arg inV2,
                      JPH::RVec3Arg inV3,
                      JPH::ColorArg inColor,
                      ECastShadow inCastShadow = ECastShadow::Off) override;
    Batch CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount) override;
    Batch CreateTriangleBatch(const Vertex* inVertices,
                              int inVertexCount,
                              const uint32_t* inIndices,
                              int inIndexCount) override;
    void DrawGeometry(JPH::RMat44Arg inModelMatrix,
                      const JPH::AABox& inWorldSpaceBounds,
                      float inLODScaleSq,
                      JPH::ColorArg inModelColor,
                      const GeometryRef& inGeometry,
                      ECullMode inCullMode     = ECullMode::CullBackFace,
                      ECastShadow inCastShadow = ECastShadow::On,
                      EDrawMode inDrawMode     = EDrawMode::Solid) override;
    void DrawText3D(JPH::RVec3Arg inPosition,
                    const std::string_view& inString,
                    JPH::ColorArg inColor = JPH::Color::sWhite,
                    float inHeight        = 0.5f) override;

    // Render entry point
    void draw(ax::Renderer* renderer);
    void clear();

protected:
    void onBeforeDraw();
    void onAfterDraw();

private:
    std::vector<V3F_C4F> _lineBuffer;
    std::vector<V3F_C4F> _triBuffer;

    ax::rhi::UniformLocation _locMVP;
    ax::BlendFunc _blendFunc = BlendFunc::DISABLE;
    ax::CustomCommand _lineCommand;
    ax::CustomCommand _triCommand;

    bool _dirtyLines          = true;
    bool _dirtyTris           = true;
    bool _oldDepthTestEnabled = false;
};
// end of 3d group
/// @}

}  // namespace ax

#endif  // defined(AX_ENABLE_PHYSICS_3D)
