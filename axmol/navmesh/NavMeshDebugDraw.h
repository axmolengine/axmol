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

#include "axmol/base/Config.h"
#if defined(AX_ENABLE_NAVMESH)

#    include "axmol/renderer/RenderState.h"
#    include "axmol/rhi/ProgramState.h"
#    include "axmol/rhi/Buffer.h"
#    include "axmol/math/Vec3.h"
#    include "recast/DebugDraw.h"
#    include "axmol/renderer/GroupCommand.h"
#    include "axmol/renderer/CallbackCommand.h"
#    include "axmol/renderer/CustomCommand.h"
#    include <string>
#    include <vector>

namespace ax
{

/**
 * @addtogroup 3d
 * @{
 */
class Renderer;
class NavMeshDebugDraw : public duDebugDraw
{
public:
    NavMeshDebugDraw();
    virtual ~NavMeshDebugDraw();

    void depthMask(bool state) override;
    void texture(bool /*state*/) override{};
    void begin(duDebugDrawPrimitives prim, float size = 1.0f) override;

    void vertex(const float* pos, unsigned int color) override;
    void vertex(const float x, const float y, const float z, unsigned int color) override;

    void vertex(const float* pos, unsigned int color, const float* uv) override;
    virtual void vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v)
        override;

    void end() override;

    void draw(Renderer* renderer);

    void clear();

private:
    void initCustomCommand(CustomCommand& command);
    rhi::PrimitiveType getPrimitiveType(duDebugDrawPrimitives prim);
    static Vec4 getColor(unsigned int col);

    void onBeforeVisitCmd();
    void onAfterVisitCmd();
    void onBeforeEachCommand(bool enableDepthWrite);

private:
    struct V3F_C4F
    {
        Vec3 position;
        Vec4 color;
    };

    struct Primitive
    {
        rhi::PrimitiveType type = rhi::PrimitiveType::LINE;
        bool depthMask              = false;
        unsigned short start        = 0;
        unsigned short end          = 0;
        float size                  = 1.0f;
    };

    Primitive* _currentPrimitive         = nullptr;
    rhi::ProgramState* _programState     = nullptr;
    rhi::VertexLayout* _vertexLayout     = nullptr;
    bool _currentDepthMask               = true;
    bool _dirtyBuffer                    = true;
    rhi::Buffer* _vertexBuffer       = nullptr;

    // RenderState::StateBlock     _stateBlock;
    std::vector<V3F_C4F> _vertices;
    std::vector<Primitive*> _primitiveList;
    rhi::UniformLocation _locMVP;
    std::vector<CustomCommand> _commands;

    //CallbackCommand _beforeCommand;
    //CallbackCommand _afterCommand;

    // renderer state cache variables
    bool _rendererDepthTestEnabled                 = true;
    rhi::CompareFunc _rendererDepthCmpFunc = rhi::CompareFunc::LESS;
    rhi::CullMode _rendererCullMode            = rhi::CullMode::BACK;
    rhi::Winding _rendererWinding              = rhi::Winding::COUNTER_CLOCK_WISE;
    bool _rendererDepthWrite                       = false;
};

/** @} */

}

#endif  // AX_ENABLE_NAVMESH
