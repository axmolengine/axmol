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
#include "navmesh/NavMeshDebugDraw.h"
#if defined(AX_ENABLE_NAVMESH)
#    include <stddef.h>  // offsetof
#    include "base/Types.h"
#    include "renderer/backend/ProgramState.h"
#    include "renderer/backend/DriverBase.h"
#    include "renderer/Renderer.h"
#    include "renderer/RenderState.h"
#    include "renderer/Shaders.h"
#    include "base/Director.h"
#    include "base/Macros.h"

namespace ax
{

NavMeshDebugDraw::NavMeshDebugDraw()
{
    auto* program = backend::Program::getBuiltinProgram(backend::ProgramType::POSITION_COLOR);
    _programState = new backend::ProgramState(program);
    _locMVP       = _programState->getUniformLocation("u_MVPMatrix");

    // the POSITION_COLOR default vertex layout is: V3F_C4B, so we need modify it
    auto vertexLayout = _programState->getMutableVertexLayout();
    vertexLayout->setAttrib("a_position",
                                _programState->getAttributeLocation(backend::Attribute::POSITION),
                                backend::VertexFormat::FLOAT3,
                                offsetof(V3F_C4F, position),
                                false);
    vertexLayout->setAttrib("a_color",
                                _programState->getAttributeLocation(backend::Attribute::COLOR),
                                backend::VertexFormat::FLOAT4,
                                offsetof(V3F_C4F, color),
                                false);
    vertexLayout->setStride(sizeof(V3F_C4F));
}

void NavMeshDebugDraw::initCustomCommand(CustomCommand& command)
{
    command.set3D(true);
    command.setTransparent(true);
    command.init(0, Mat4::IDENTITY, Node::FLAGS_RENDER_AS_3D);
    command.setDrawType(CustomCommand::DrawType::ARRAY);
    auto& pipelineDescriptor        = command.getPipelineDescriptor();
    pipelineDescriptor.programState = _programState;

    auto& blend                = pipelineDescriptor.blendDescriptor;
    blend.blendEnabled         = true;
    blend.sourceRGBBlendFactor = blend.sourceAlphaBlendFactor = BlendFunc::ALPHA_NON_PREMULTIPLIED.src;
    blend.destinationRGBBlendFactor = blend.destinationAlphaBlendFactor = BlendFunc::ALPHA_NON_PREMULTIPLIED.dst;
}

void NavMeshDebugDraw::vertex(const float /*x*/,
                              const float /*y*/,
                              const float /*z*/,
                              unsigned int /*color*/,
                              const float /*u*/,
                              const float /*v*/)
{}

void NavMeshDebugDraw::vertex(const float* pos, unsigned int color, const float* uv)
{
    vertex(pos[0], pos[1], pos[2], color, uv[0], uv[1]);
}

void NavMeshDebugDraw::vertex(const float x, const float y, const float z, unsigned int color)
{
    if (!_currentPrimitive)
        return;
    V3F_C4F vertex = {Vec3(x, y, z), getColor(color)};
    _vertices.emplace_back(vertex);
    _dirtyBuffer = true;
}

void NavMeshDebugDraw::vertex(const float* pos, unsigned int color)
{
    vertex(pos[0], pos[1], pos[2], color);
}

NavMeshDebugDraw::~NavMeshDebugDraw()
{
    for (auto&& iter : _primitiveList)
    {
        delete iter;
    }
    AX_SAFE_RELEASE_NULL(_programState);
    AX_SAFE_RELEASE_NULL(_vertexBuffer);
}

void NavMeshDebugDraw::depthMask(bool state)
{
    _currentDepthMask = state;
}

void NavMeshDebugDraw::begin(duDebugDrawPrimitives prim, float size /*= 1.0f*/)
{
    if (_currentPrimitive)
        return;
    _currentPrimitive            = new Primitive;
    _currentPrimitive->type      = getPrimitiveType(prim);
    _currentPrimitive->depthMask = _currentDepthMask;
    _currentPrimitive->start     = _vertices.size();
    _currentPrimitive->size      = size;
}

void NavMeshDebugDraw::end()
{
    if (!_currentPrimitive)
        return;
    _currentPrimitive->end = _vertices.size();
    _primitiveList.emplace_back(_currentPrimitive);
    _currentPrimitive = nullptr;
}

Vec4 NavMeshDebugDraw::getColor(unsigned int col)
{
    const unsigned int r = col & 0xff;
    const unsigned int g = (col >> 8) & 0xff;
    const unsigned int b = (col >> 16) & 0xff;
    const unsigned int a = (col >> 24) & 0xff;

    float factor = 1.0f / 255.0f;
    return Vec4(r, g, b, a) * factor;
}

backend::PrimitiveType NavMeshDebugDraw::getPrimitiveType(duDebugDrawPrimitives prim)
{
    switch (prim)
    {
    case DU_DRAW_POINTS:
        return backend::PrimitiveType::POINT;
    case DU_DRAW_LINES:
        return backend::PrimitiveType::LINE;
    case DU_DRAW_TRIS:
        return backend::PrimitiveType::TRIANGLE;
    default:
        return backend::PrimitiveType::POINT;
    }
}

void NavMeshDebugDraw::draw(Renderer* renderer)
{
    auto& transform = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    auto beforeCommand = renderer->nextCallbackCommand();
    auto afterCommand  = renderer->nextCallbackCommand();

    beforeCommand->init(0, Mat4::IDENTITY, Node::FLAGS_RENDER_AS_3D);
    afterCommand->init(0, Mat4::IDENTITY, Node::FLAGS_RENDER_AS_3D);

    beforeCommand->func  = AX_CALLBACK_0(NavMeshDebugDraw::onBeforeVisitCmd, this);
    afterCommand->func  = AX_CALLBACK_0(NavMeshDebugDraw::onAfterVisitCmd, this);

    beforeCommand->set3D(true);
    beforeCommand->setTransparent(true);
    afterCommand->set3D(true);
    afterCommand->setTransparent(true);

    _programState->setUniform(_locMVP, transform.m, sizeof(transform.m));

    renderer->addCommand(beforeCommand);

    if (!_vertexBuffer || _vertexBuffer->getSize() < _vertices.size() * sizeof(_vertices[0]))
    {
        _vertexBuffer = backend::DriverBase::getInstance()->newBuffer(
            _vertices.size() * sizeof(_vertices[0]), backend::BufferType::VERTEX, backend::BufferUsage::STATIC);
        _dirtyBuffer = true;
    }

    if (_dirtyBuffer)
    {
        _vertexBuffer->updateData(_vertices.data(), sizeof(_vertices[0]) * _vertices.size());
        _dirtyBuffer = false;
    }
    int idx = 0;
    if (_commands.size() < _primitiveList.size())
    {
        _commands.resize(_primitiveList.size());
    }
    for (auto&& iter : _primitiveList)
    {
        if (iter->type == backend::PrimitiveType::POINT)
            continue;
        if (iter->end - iter->start <= 0)
            continue;

        auto& command = _commands[idx];

        initCustomCommand(command);
        command.setBeforeCallback(AX_CALLBACK_0(NavMeshDebugDraw::onBeforeEachCommand, this, iter->depthMask));

        command.setVertexBuffer(_vertexBuffer);
        command.setPrimitiveType(iter->type);
        command.setVertexDrawInfo(iter->start, iter->end - iter->start);

        renderer->addCommand(&command);

        AX_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, iter->end - iter->start);
        idx++;
    }

    renderer->addCommand(afterCommand);
}

void NavMeshDebugDraw::onBeforeVisitCmd()
{
    auto* renderer = Director::getInstance()->getRenderer();

    _rendererDepthTestEnabled = renderer->getDepthTest();
    _rendererDepthCmpFunc     = renderer->getDepthCompareFunction();
    _rendererCullMode         = renderer->getCullMode();

    _rendererDepthWrite = renderer->getDepthWrite();
    _rendererWinding    = renderer->getWinding();

    renderer->setCullMode(backend::CullMode::NONE);
    renderer->setDepthTest(true);
}

void NavMeshDebugDraw::onAfterVisitCmd()
{
    auto* renderer = Director::getInstance()->getRenderer();
    renderer->setDepthTest(_rendererDepthTestEnabled);
    renderer->setDepthCompareFunction(_rendererDepthCmpFunc);
    renderer->setCullMode(_rendererCullMode);
    renderer->setDepthWrite(_rendererDepthWrite);
    renderer->setWinding(_rendererWinding);
}

void NavMeshDebugDraw::onBeforeEachCommand(bool enableDepthWrite)
{
    auto* renderer = Director::getInstance()->getRenderer();
    renderer->setDepthWrite(enableDepthWrite);
}

void NavMeshDebugDraw::clear()
{
    _vertices.clear();
    for (auto&& iter : _primitiveList)
    {
        delete iter;
    }
    _primitiveList.clear();
}

}

#endif  // AX_ENABLE_NAVMESH
