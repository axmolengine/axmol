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
#include "ProgramManager.h"
#include "VertexLayout.h"

NS_AX_BACKEND_BEGIN

/*
 * shader vertex layout setup functions
 */
struct VertexLayoutHelper
{
    static void setupDummy(Program*) {}
    static void setupTexture(Program* program)
    {
        auto vertexLayout = program->getVertexLayout();

        /// a_position
        vertexLayout->setAttrib(backend::ATTRIBUTE_NAME_POSITION,
                                program->getAttributeLocation(backend::Attribute::POSITION),
                                backend::VertexFormat::FLOAT2, 0, false);
        /// a_texCoord
        vertexLayout->setAttrib(backend::ATTRIBUTE_NAME_TEXCOORD,
                                program->getAttributeLocation(backend::Attribute::TEXCOORD),
                                backend::VertexFormat::FLOAT2, 2 * sizeof(float), false);

        vertexLayout->setStride(4 * sizeof(float));
    }

    static void setupSprite(Program* program)
    {
        auto vertexLayout = program->getVertexLayout();

        /// a_position
        vertexLayout->setAttrib(backend::ATTRIBUTE_NAME_POSITION,
                                program->getAttributeLocation(backend::Attribute::POSITION),
                                backend::VertexFormat::FLOAT3, 0, false);
        /// a_texCoord
        vertexLayout->setAttrib(backend::ATTRIBUTE_NAME_TEXCOORD,
                                program->getAttributeLocation(backend::Attribute::TEXCOORD),
                                backend::VertexFormat::FLOAT2, offsetof(V3F_C4B_T2F, texCoords), false);

        /// a_color
        vertexLayout->setAttrib(backend::ATTRIBUTE_NAME_COLOR, program->getAttributeLocation(backend::Attribute::COLOR),
                                backend::VertexFormat::UBYTE4, offsetof(V3F_C4B_T2F, colors), true);
        vertexLayout->setStride(sizeof(V3F_C4B_T2F));
    }

    static void setupDrawNode(Program* program)
    {
        auto vertexLayout = program->getVertexLayout();

        vertexLayout->setAttrib(backend::ATTRIBUTE_NAME_POSITION,
                                program->getAttributeLocation(backend::Attribute::POSITION),
                                backend::VertexFormat::FLOAT2, 0, false);

        vertexLayout->setAttrib(backend::ATTRIBUTE_NAME_TEXCOORD,
                                program->getAttributeLocation(backend::Attribute::TEXCOORD),
                                backend::VertexFormat::FLOAT2, offsetof(V2F_C4B_T2F, texCoords), false);

        vertexLayout->setAttrib(backend::ATTRIBUTE_NAME_COLOR, program->getAttributeLocation(backend::Attribute::COLOR),
                                backend::VertexFormat::UBYTE4, offsetof(V2F_C4B_T2F, colors), true);

        vertexLayout->setStride(sizeof(V2F_C4B_T2F));
    }

    static void setupDrawNode3D(Program* program)
    {
        auto vertexLayout = program->getVertexLayout();

        vertexLayout->setAttrib(backend::ATTRIBUTE_NAME_POSITION,
                                program->getAttributeLocation(backend::Attribute::POSITION),
                                backend::VertexFormat::FLOAT3, 0, false);

        vertexLayout->setAttrib(backend::ATTRIBUTE_NAME_COLOR, program->getAttributeLocation(backend::Attribute::COLOR),
                                backend::VertexFormat::UBYTE4, offsetof(V3F_C4B, colors), true);

        vertexLayout->setStride(sizeof(V3F_C4B));
    }

    static void setupSkyBox(Program* program)
    {
        auto vertexLayout = program->getVertexLayout();
        auto attrNameLoc  = program->getAttributeLocation(backend::ATTRIBUTE_NAME_POSITION);
        vertexLayout->setAttrib(backend::ATTRIBUTE_NAME_POSITION, attrNameLoc,
                                backend::VertexFormat::FLOAT3, 0, false);
        vertexLayout->setStride(sizeof(Vec3));
    }

    static void setupPU3D(Program* program)
    {
        auto vertexLayout = program->getVertexLayout();

        vertexLayout->setAttrib(backend::ATTRIBUTE_NAME_POSITION,
                                program->getAttributeLocation(backend::Attribute::POSITION),
                                backend::VertexFormat::FLOAT3, offsetof(V3F_T2F_C4F, position), false);

        vertexLayout->setAttrib(backend::ATTRIBUTE_NAME_TEXCOORD,
                                program->getAttributeLocation(backend::Attribute::TEXCOORD),
                                backend::VertexFormat::FLOAT2, offsetof(V3F_T2F_C4F, uv), false);

        vertexLayout->setAttrib(backend::ATTRIBUTE_NAME_COLOR, program->getAttributeLocation(backend::Attribute::COLOR),
                                backend::VertexFormat::FLOAT4, offsetof(V3F_T2F_C4F, color), false);

        vertexLayout->setStride(sizeof(V3F_T2F_C4F));
    }

    static void setupPos(Program* program)
    {
        auto vertexLayout = program->getVertexLayout();
        vertexLayout->setAttrib(backend::ATTRIBUTE_NAME_POSITION,
                                program->getAttributeLocation(backend::Attribute::POSITION),
                                backend::VertexFormat::FLOAT2, 0, false);
        vertexLayout->setStride(sizeof(Vec2));
    }

    static void setupPosColor(Program* program)
    {
        auto vertexLayout = program->getVertexLayout();
        vertexLayout->setAttrib(backend::ATTRIBUTE_NAME_POSITION,
                                program->getAttributeLocation(backend::Attribute::POSITION),
                                backend::VertexFormat::FLOAT3, 0, false);
        vertexLayout->setAttrib(backend::ATTRIBUTE_NAME_COLOR, program->getAttributeLocation(backend::Attribute::COLOR),
                                backend::VertexFormat::FLOAT4, offsetof(V3F_C4F, colors), false);
        vertexLayout->setStride(sizeof(V3F_C4F));
    }

    static void setupTerrain3D(Program* program)
    {
        auto vertexLayout = program->getVertexLayout();
        vertexLayout->setAttrib(backend::ATTRIBUTE_NAME_POSITION,
                                program->getAttributeLocation(backend::Attribute::POSITION),
                                backend::VertexFormat::FLOAT3, 0, false);
        vertexLayout->setAttrib(backend::ATTRIBUTE_NAME_TEXCOORD,
                                program->getAttributeLocation(backend::Attribute::TEXCOORD),
                                backend::VertexFormat::FLOAT2, offsetof(V3F_T2F_N3F, texcoord), false);
        vertexLayout->setAttrib(backend::ATTRIBUTE_NAME_NORMAL,
                                program->getAttributeLocation(backend::Attribute::NORMAL),
                                backend::VertexFormat::FLOAT3, offsetof(V3F_T2F_N3F, normal), false);
        vertexLayout->setStride(sizeof(V3F_T2F_N3F));
    }
};
std::function<void(Program*)> Program::s_vertexLayoutSetupList[static_cast<int>(VertexLayoutType::Count)] = {
    VertexLayoutHelper::setupDummy,    VertexLayoutHelper::setupPos,      VertexLayoutHelper::setupTexture,
    VertexLayoutHelper::setupSprite,   VertexLayoutHelper::setupDrawNode, VertexLayoutHelper::setupDrawNode3D,
    VertexLayoutHelper::setupSkyBox,   VertexLayoutHelper::setupPU3D,     VertexLayoutHelper::setupPosColor,
    VertexLayoutHelper::setupTerrain3D};

Program::Program(std::string_view vs, std::string_view fs)
    : _vertexShader(vs), _fragmentShader(fs), _vertexLayout(new VertexLayout())
{}

Program::~Program()
{
    delete _vertexLayout;
}

void Program::setupVertexLayout(VertexLayoutType vlt)
{
    if (vlt < VertexLayoutType::Count)
        s_vertexLayoutSetupList[static_cast<int>(vlt)](this);
}

Program* Program::getBuiltinProgram(uint32_t type)
{
    return ProgramManager::getInstance()->getBuiltinProgram(type);
}

void Program::setProgramIds(uint32_t progType, uint64_t progId)
{
    _programType  = progType;
    _programId    = progId;
}
NS_AX_BACKEND_END
