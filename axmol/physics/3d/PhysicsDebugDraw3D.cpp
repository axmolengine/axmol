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

#include "axmol/physics/3d/PhysicsDebugDraw3D.h"
#include "axmol/physics/3d/PhysicsUtility3D.h"

#if defined(AX_ENABLE_PHYSICS_3D)

namespace ax
{

struct TriangleBatch : public JPH::RefTargetVirtual
{
    TriangleBatch() = default;

    void AddRef() { ++nrefs; }

    /// Virtual release reference
    void Release()
    {
        if (--nrefs == 0)
            delete this;
    }

    std::atomic<uint32_t> nrefs{1};

    std::vector<V3F_C4F> vertices;
    std::vector<uint32_t> indices;
};

PhysicsDebugDraw3D::PhysicsDebugDraw3D()
{
    auto program = axpm->getBuiltinProgram(rhi::ProgramType::POSITION_COLOR);
    _locMVP      = program->getUniformLocation("u_MVPMatrix");

    _lineBuffer.reserve(1024);
    _triBuffer.reserve(2048);

    _lineCommand.setPrimitiveType(CustomCommand::PrimitiveType::LINE);
    _lineCommand.setDrawType(CustomCommand::DrawType::ARRAY);
    _lineCommand.setBeforeCallback(AX_CALLBACK_0(PhysicsDebugDraw3D::onBeforeDraw, this));
    _lineCommand.setAfterCallback(AX_CALLBACK_0(PhysicsDebugDraw3D::onAfterDraw, this));
    _lineCommand.setOwnPSVL(new rhi::ProgramState(program), program->getVertexLayout(), RenderCommand::ADOPT_FLAG_PS);

    _triCommand.setPrimitiveType(CustomCommand::PrimitiveType::TRIANGLE);
    _triCommand.setDrawType(CustomCommand::DrawType::ARRAY);
    _triCommand.setBeforeCallback(AX_CALLBACK_0(PhysicsDebugDraw3D::onBeforeDraw, this));
    _triCommand.setAfterCallback(AX_CALLBACK_0(PhysicsDebugDraw3D::onAfterDraw, this));
    _triCommand.setOwnPSVL(new rhi::ProgramState(program), program->getVertexLayout(), RenderCommand::ADOPT_FLAG_PS);

    Initialize();
}

PhysicsDebugDraw3D::~PhysicsDebugDraw3D()
{
    _lineCommand.releasePSVL();
    _triCommand.releasePSVL();
}

void PhysicsDebugDraw3D::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor)
{
    ax::Color color = jphutil::cast(inColor);

    V3F_C4F a, b;
    a.position = jphutil::cast(inFrom);
    a.color    = color;
    b.position = jphutil::cast(inTo);
    b.color    = color;

    _lineBuffer.emplace_back(a);
    _lineBuffer.emplace_back(b);

    _dirtyLines = true;
}

void PhysicsDebugDraw3D::DrawTriangle(JPH::RVec3Arg inV1,
                                      JPH::RVec3Arg inV2,
                                      JPH::RVec3Arg inV3,
                                      JPH::ColorArg inColor,
                                      ECastShadow)
{
    ax::Color color = jphutil::cast(inColor);

    V3F_C4F a, b, c;
    a.position = jphutil::cast(inV1);
    b.position = jphutil::cast(inV2);
    c.position = jphutil::cast(inV3);
    a.color = b.color = c.color = color;

    _triBuffer.emplace_back(a);
    _triBuffer.emplace_back(b);
    _triBuffer.emplace_back(c);

    _dirtyTris = true;
}

PhysicsDebugDraw3D::Batch PhysicsDebugDraw3D::CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount)
{
    auto batch = new TriangleBatch();
    batch->vertices.reserve(inTriangleCount * 3);
    batch->indices.reserve(inTriangleCount * 3);

    for (int i = 0; i < inTriangleCount; ++i)
    {
        const Triangle& tri = inTriangles[i];
        for (int v = 0; v < 3; ++v)
        {
            V3F_C4F vert;
            vert.position = jphutil::cast(tri.mV[v].mPosition);
            vert.color    = ax::Color(tri.mV[v].mColor);
            batch->vertices.push_back(vert);
            batch->indices.push_back((uint32_t)batch->indices.size());
        }
    }
    return Batch(batch);
}

PhysicsDebugDraw3D::Batch PhysicsDebugDraw3D::CreateTriangleBatch(const Vertex* inVertices,
                                                                  int inVertexCount,
                                                                  const uint32_t* inIndices,
                                                                  int inIndexCount)
{
    auto batch = new TriangleBatch();
    batch->vertices.reserve(inVertexCount);
    batch->indices.reserve(inIndexCount);

    for (int i = 0; i < inVertexCount; ++i)
    {
        V3F_C4F vert;
        vert.position = jphutil::cast(inVertices[i].mPosition);
        vert.color    = ax::Color(inVertices[i].mColor);
        batch->vertices.push_back(vert);
    }
    batch->indices.assign(inIndices, inIndices + inIndexCount);
    return Batch(batch);
}

void PhysicsDebugDraw3D::DrawGeometry(JPH::RMat44Arg inModelMatrix,
                                      const JPH::AABox& inWorldSpaceBounds,
                                      float inLODScaleSq,
                                      JPH::ColorArg inModelColor,
                                      const GeometryRef& inGeometry,
                                      ECullMode,
                                      ECastShadow,
                                      EDrawMode inDrawMode)
{
    const auto& lod = inGeometry->GetLOD(JPH::Vec3::sZero(), inWorldSpaceBounds, inLODScaleSq);
    auto batch      = static_cast<TriangleBatch*>(lod.mTriangleBatch.GetPtr());
    if (!batch || batch->indices.empty())
        return;

    ax::Color color   = jphutil::cast(inModelColor);
    ax::Mat4 modelMat = jphutil::cast(inModelMatrix);

    for (size_t i = 0; i + 2 < batch->indices.size(); i += 3)
    {
        auto v1 = batch->vertices[batch->indices[i]];
        auto v2 = batch->vertices[batch->indices[i + 1]];
        auto v3 = batch->vertices[batch->indices[i + 2]];

        modelMat.transformPoint(v1.position, &v1.position);
        modelMat.transformPoint(v2.position, &v2.position);
        modelMat.transformPoint(v3.position, &v3.position);

        v1.color = v2.color = v3.color = color;

        if (inDrawMode == EDrawMode::Wireframe)
        {
            _lineBuffer.push_back(v1);
            _lineBuffer.push_back(v2);
            _lineBuffer.push_back(v2);
            _lineBuffer.push_back(v3);
            _lineBuffer.push_back(v3);
            _lineBuffer.push_back(v1);
        }
        else
        {
            _triBuffer.push_back(v1);
            _triBuffer.push_back(v2);
            _triBuffer.push_back(v3);
        }
    }

    _dirtyTris = true;
}

void PhysicsDebugDraw3D::DrawText3D(JPH::RVec3Arg inPosition,
                                    const std::string_view& inString,
                                    JPH::ColorArg inColor,
                                    float inHeight)
{
    // For now, just log text. Could be extended to render 3D text.
    // AXLOGD("PhysicsDebugDraw3D::DrawText3D at ({},{},{}): {}", (float)inPosition.GetX(), (float)inPosition.GetY(),
    //        (float)inPosition.GetZ(), inString);
}

void PhysicsDebugDraw3D::draw(ax::Renderer* renderer)
{
    auto& transform = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);

    auto& blend                  = _lineCommand.blendDesc();
    blend.blendEnabled           = true;
    blend.sourceAlphaBlendFactor = blend.sourceRGBBlendFactor = _blendFunc.src;
    blend.destinationAlphaBlendFactor = blend.destinationRGBBlendFactor = _blendFunc.dst;

    // Upload line buffer
    if (_dirtyLines && !_lineBuffer.empty())
    {
        _lineCommand.unsafePS()->setUniform(_locMVP, transform.m, sizeof(transform.m));
        _lineCommand.init(0, Mat4::identity, 0);

        _lineCommand.setPrimitiveType(CustomCommand::PrimitiveType::LINE);
        _lineCommand.createVertexBuffer(sizeof(_lineBuffer[0]), _lineBuffer.size(),
                                        CustomCommand::BufferUsage::DYNAMIC);
        _lineCommand.updateVertexBuffer(_lineBuffer.data(), _lineBuffer.size() * sizeof(_lineBuffer[0]));
        _lineCommand.setVertexDrawInfo(0, _lineBuffer.size());
        renderer->addCommand(&_lineCommand);
        _dirtyLines = false;
    }

    // Upload triangle buffer
    if (_dirtyTris && !_triBuffer.empty())
    {
        _triCommand.unsafePS()->setUniform(_locMVP, transform.m, sizeof(transform.m));
        _triCommand.init(0, Mat4::identity, 0);

        _triCommand.setPrimitiveType(CustomCommand::PrimitiveType::TRIANGLE);
        _triCommand.createVertexBuffer(sizeof(_triBuffer[0]), _triBuffer.size(), CustomCommand::BufferUsage::DYNAMIC);
        _triCommand.updateVertexBuffer(_triBuffer.data(), _triBuffer.size() * sizeof(_triBuffer[0]));
        _triCommand.setVertexDrawInfo(0, _triBuffer.size());
        renderer->addCommand(&_triCommand);
        _dirtyTris = false;
    }

    AX_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, _lineBuffer.size() + _triBuffer.size());
}

void PhysicsDebugDraw3D::clear()
{
    _lineBuffer.clear();
    _triBuffer.clear();
    _dirtyLines = _dirtyTris = true;
}

void PhysicsDebugDraw3D::onBeforeDraw()
{
    auto* renderer       = Director::getInstance()->getRenderer();
    _oldDepthTestEnabled = renderer->getDepthTest();
    renderer->setDepthTest(true);
}

void PhysicsDebugDraw3D::onAfterDraw()
{
    auto* renderer = Director::getInstance()->getRenderer();
    renderer->setDepthTest(_oldDepthTestEnabled);
}

}  // namespace ax

#endif
