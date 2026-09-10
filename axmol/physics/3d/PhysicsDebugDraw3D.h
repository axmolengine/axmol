/****************************************************************************
 Copyright (c) 2015-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
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
struct SceneRenderState;

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
    void draw(const ax::SceneRenderState& state);
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
