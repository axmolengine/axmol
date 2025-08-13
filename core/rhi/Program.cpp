/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
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

#include "Program.h"
#include "VertexLayout.h"

namespace ax::rhi
{

/*
 * shader vertex layout define functions
 */
struct VertexLayoutHelper
{
    static void defineDummy(Program*) {}
    static void defineTexture(Program* program)
    {
        auto vertexLayout = program->getVertexLayout();

        /// a_position
        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_POSITION,
                                program->getVertexInputDesc(rhi::VertexInputKind::POSITION),
                                rhi::VertexFormat::FLOAT2, 0, false);
        /// a_texCoord
        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_TEXCOORD,
                                program->getVertexInputDesc(rhi::VertexInputKind::TEXCOORD),
                                rhi::VertexFormat::FLOAT2, 2 * sizeof(float), false);

        vertexLayout->setStride(4 * sizeof(float));
    }

    static void definePosUvColor(Program* program)
    {
        auto vertexLayout = program->getVertexLayout();

        /// a_position
        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_POSITION,
                                program->getVertexInputDesc(rhi::VertexInputKind::POSITION),
                                rhi::VertexFormat::FLOAT3, 0, false);
        /// a_texCoord
        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_TEXCOORD,
                                program->getVertexInputDesc(rhi::VertexInputKind::TEXCOORD),
                                rhi::VertexFormat::FLOAT2, offsetof(V3F_T2F_C4F, texCoord), false);

        /// a_color
        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_COLOR,
                                program->getVertexInputDesc(rhi::VertexInputKind::COLOR),
                                rhi::VertexFormat::FLOAT4, offsetof(V3F_T2F_C4F, color), false);
        vertexLayout->setStride(sizeof(V3F_T2F_C4F));
    }

    static void defineSprite(Program* program)
    {
        auto vertexLayout = program->getVertexLayout();

        /// a_position
        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_POSITION,
                                program->getVertexInputDesc(rhi::VertexInputKind::POSITION),
                                rhi::VertexFormat::FLOAT3, 0, false);
        /// a_texCoord
        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_TEXCOORD,
                                program->getVertexInputDesc(rhi::VertexInputKind::TEXCOORD),
                                rhi::VertexFormat::FLOAT2, offsetof(V3F_T2F_C4B, texCoord), false);

        /// a_color
        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_COLOR,
                                program->getVertexInputDesc(rhi::VertexInputKind::COLOR),
                                rhi::VertexFormat::UBYTE4, offsetof(V3F_T2F_C4B, color), true);
        vertexLayout->setStride(sizeof(V3F_T2F_C4B));
    }

    static void defineSprite2D(Program* program)
    {
        auto vertexLayout = program->getVertexLayout();

        /// a_position
        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_POSITION,
                                program->getVertexInputDesc(rhi::VertexInputKind::POSITION),
                                rhi::VertexFormat::FLOAT2, 0, false);
        /// a_texCoord
        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_TEXCOORD,
                                program->getVertexInputDesc(rhi::VertexInputKind::TEXCOORD),
                                rhi::VertexFormat::FLOAT2, offsetof(V2F_T2F_C4B, texCoord), false);

        /// a_color
        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_COLOR,
                                program->getVertexInputDesc(rhi::VertexInputKind::COLOR),
                                rhi::VertexFormat::UBYTE4, offsetof(V2F_T2F_C4B, color), true);
        vertexLayout->setStride(sizeof(V2F_T2F_C4B));
    }

    static void defineDrawNode(Program* program)
    {
        auto vertexLayout = program->getVertexLayout();

        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_POSITION,
                                program->getVertexInputDesc(rhi::VertexInputKind::POSITION),
                                rhi::VertexFormat::FLOAT2, 0, false);

        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_TEXCOORD,
                                program->getVertexInputDesc(rhi::VertexInputKind::TEXCOORD),
                                rhi::VertexFormat::FLOAT2, offsetof(V2F_T2F_C4F, texCoord), false);

        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_COLOR,
                                program->getVertexInputDesc(rhi::VertexInputKind::COLOR),
                                rhi::VertexFormat::FLOAT4, offsetof(V2F_T2F_C4F, color), true);

        vertexLayout->setStride(sizeof(V2F_T2F_C4F));
    }

    static void defineDrawNode3D(Program* program)
    {
        auto vertexLayout = program->getVertexLayout();

        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_POSITION,
                                program->getVertexInputDesc(rhi::VertexInputKind::POSITION),
                                rhi::VertexFormat::FLOAT3, 0, false);

        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_COLOR,
                                program->getVertexInputDesc(rhi::VertexInputKind::COLOR),
                                rhi::VertexFormat::FLOAT4, offsetof(V3F_C4F, color), true);

        vertexLayout->setStride(sizeof(V3F_C4F));
    }

    static void defineSkyBox(Program* program)
    {
        auto vertexLayout = program->getVertexLayout();
        auto attrNameLoc  = program->getVertexInputDesc(rhi::VERTEX_INPUT_NAME_POSITION);
        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_POSITION, attrNameLoc, rhi::VertexFormat::FLOAT3, 0, false);
        vertexLayout->setStride(sizeof(Vec3));
    }

    static void definePos(Program* program)
    {
        auto vertexLayout = program->getVertexLayout();
        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_POSITION,
                                program->getVertexInputDesc(rhi::VertexInputKind::POSITION),
                                rhi::VertexFormat::FLOAT3, 0, false);
        vertexLayout->setStride(sizeof(Vec3));
    }

    static void definePosColor(Program* program)
    {
        auto vertexLayout = program->getVertexLayout();
        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_POSITION,
                                program->getVertexInputDesc(rhi::VertexInputKind::POSITION),
                                rhi::VertexFormat::FLOAT3, 0, false);
        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_COLOR,
                                program->getVertexInputDesc(rhi::VertexInputKind::COLOR),
                                rhi::VertexFormat::FLOAT4, offsetof(V3F_C4F, color), false);
        vertexLayout->setStride(sizeof(V3F_C4F));
    }

    static void defineTerrain3D(Program* program)
    {
        auto vertexLayout = program->getVertexLayout();
        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_POSITION,
                                program->getVertexInputDesc(rhi::VertexInputKind::POSITION),
                                rhi::VertexFormat::FLOAT3, 0, false);
        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_TEXCOORD,
                                program->getVertexInputDesc(rhi::VertexInputKind::TEXCOORD),
                                rhi::VertexFormat::FLOAT2, offsetof(V3F_T2F_N3F, texcoord), false);
        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_NORMAL,
                                program->getVertexInputDesc(rhi::VertexInputKind::NORMAL),
                                rhi::VertexFormat::FLOAT3, offsetof(V3F_T2F_N3F, normal), false);
        vertexLayout->setStride(sizeof(V3F_T2F_N3F));
    }

    static void defineInstanced(Program* program)
    {
        auto vertexLayout = program->getVertexLayout();
        vertexLayout->setAttrib(rhi::VERTEX_INPUT_NAME_INSTANCE,
                                program->getVertexInputDesc(rhi::VertexInputKind::INSTANCE),
                                rhi::VertexFormat::MAT4, 0, false, 1);
        vertexLayout->setInstanceStride(sizeof(Mat4));
    }
};
std::function<void(Program*)> Program::s_vertexLayoutDefineList[static_cast<int>(VertexLayoutType::Count)] = {
    VertexLayoutHelper::defineDummy,      VertexLayoutHelper::definePos,        VertexLayoutHelper::defineTexture,
    VertexLayoutHelper::definePosUvColor, VertexLayoutHelper::defineSprite,     VertexLayoutHelper::defineSprite2D,
    VertexLayoutHelper::defineDrawNode,   VertexLayoutHelper::defineDrawNode3D, VertexLayoutHelper::defineSkyBox,
    VertexLayoutHelper::definePosColor,   VertexLayoutHelper::defineTerrain3D,  VertexLayoutHelper::defineInstanced};

Program::Program(std::string_view vs, std::string_view fs) : _vsSource(vs), _fsSource(fs)
{
    auto driver   = DriverBase::getInstance();
    _vertexLayout = driver->createVertexLayout();
}

Program::~Program()
{
    delete _vertexLayout;
}

void Program::defineVertexLayout(VertexLayoutType vlt)
{
    if (vlt < VertexLayoutType::Count)
        s_vertexLayoutDefineList[static_cast<int>(vlt)](this);
}

void Program::setProgramIds(uint32_t progType, uint64_t progId)
{
    _programType = progType;
    _programId   = progId;
}
}  // namespace ax::rhi
