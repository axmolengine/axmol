/* Copyright (c) 2012 Scott Lembcke and Howling Moon Software
 * Copyright (c) 2012 cocos2d-x.org
 * Copyright (c) 2013-2016 Chukong Technologies Inc.
 * Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 * Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "2d/DrawNode.h"
#include <stddef.h>
#include "base/Types.h"
#include "base/EventType.h"
#include "base/Configuration.h"
#include "renderer/Renderer.h"
#include "base/Director.h"
#include "base/EventListenerCustom.h"
#include "base/EventDispatcher.h"
#include "2d/ActionCatmullRom.h"
#include "base/Utils.h"
#include "renderer/Shaders.h"
#include "renderer/backend/ProgramState.h"
#include "poly2tri/poly2tri.h"

namespace ax
{

#if defined(_WIN32)
#    pragma push_macro("TRANSPARENT")
#    undef TRANSPARENT
#endif

/** Is a polygon convex?
 * @param verts A pointer to point coordinates.
 * @param count The number of verts measured in points.
 */
static bool isConvex(const Vec2* verts, int count)
{
    bool isPositive = false, isNegative = false;
    for (unsigned int i = 0; i < count; i++)
    {
        auto& A = verts[i];
        auto& B = verts[(i + 1) % count];
        auto& C = verts[(i + 2) % count];

        double crossProduct = (B.x - A.x) * (C.y - B.y) - (B.y - A.y) * (C.x - B.x);

        if (crossProduct > 0)
            isPositive = true;
        else if (crossProduct < 0)
            isNegative = true;

        if (isPositive && isNegative)
            return false;  // is concave
    }
    return true;  // is convex
}

static V2F_C4B_T2F* expandBufferAndGetPointer(axstd::pod_vector<V2F_C4B_T2F>& buffer, size_t count)
{
    size_t oldSize = buffer.size();
    buffer.expand(count);
    return buffer.data() + oldSize;
}

DrawNode::DrawNode()
{
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;

    properties.setDefaultValues();

#if AX_ENABLE_CACHE_TEXTURE_DATA
    // TODO new-renderer: interface setupBuffer removal

    // Need to listen the event only when not use batchnode, because it will use VBO
    //    auto listener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom* event){
    //        /** listen the event that renderer was recreated on Android/WP8 */
    //        this->setupBuffer();
    //    });

    //    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
#endif
}

DrawNode::~DrawNode()
{
    freeShaderInternal(_customCommandTriangle);
    freeShaderInternal(_customCommandPoint);
    freeShaderInternal(_customCommandLine);
}

DrawNode* DrawNode::create()
{
    DrawNode* ret = new DrawNode();
    if (ret->init())
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

bool DrawNode::init()
{
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
    updateShader();

    _trianglesDirty = true;
    _pointsDirty = true;
    _linesDirty  = true;

    return true;
}

void DrawNode::updateShader()
{
    updateShaderInternal(_customCommandTriangle, backend::ProgramType::POSITION_COLOR_LENGTH_TEXTURE,
                         CustomCommand::DrawType::ARRAY, CustomCommand::PrimitiveType::TRIANGLE);

    updateShaderInternal(_customCommandPoint, backend::ProgramType::POSITION_COLOR_TEXTURE_AS_POINTSIZE,
                         CustomCommand::DrawType::ARRAY, CustomCommand::PrimitiveType::POINT);

    updateShaderInternal(_customCommandLine, backend::ProgramType::POSITION_COLOR_LENGTH_TEXTURE,
                         CustomCommand::DrawType::ARRAY, CustomCommand::PrimitiveType::LINE);
}

void DrawNode::updateShaderInternal(CustomCommand& cmd,
                                    uint32_t programType,
                                    CustomCommand::DrawType drawType,
                                    CustomCommand::PrimitiveType primitiveType)
{
    auto& pipelinePS = cmd.getPipelineDescriptor().programState;
    AX_SAFE_RELEASE(pipelinePS);

    auto program = backend::Program::getBuiltinProgram(programType);
    pipelinePS   = new backend::ProgramState(program);
    setVertexLayout(cmd);
    cmd.setPrimitiveType(primitiveType);
    cmd.setDrawType(drawType);
}

void DrawNode::setVertexLayout(CustomCommand& cmd)
{
    auto* programState = cmd.getPipelineDescriptor().programState;
    programState->validateSharedVertexLayout(backend::VertexLayoutType::DrawNode);
}

void DrawNode::freeShaderInternal(CustomCommand& cmd)
{
    auto& pipelinePS = cmd.getPipelineDescriptor().programState;
    AX_SAFE_RELEASE_NULL(pipelinePS);
}

void DrawNode::updateBlendState(CustomCommand& cmd)
{
    backend::BlendDescriptor& blendDescriptor = cmd.getPipelineDescriptor().blendDescriptor;
    blendDescriptor.blendEnabled              = true;
    if (_blendFunc == BlendFunc::ALPHA_NON_PREMULTIPLIED)
    {
        blendDescriptor.sourceRGBBlendFactor        = backend::BlendFactor::SRC_ALPHA;
        blendDescriptor.destinationRGBBlendFactor   = backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
        blendDescriptor.sourceAlphaBlendFactor      = backend::BlendFactor::SRC_ALPHA;
        blendDescriptor.destinationAlphaBlendFactor = backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
        setOpacityModifyRGB(false);
    }
    else
    {
        blendDescriptor.sourceRGBBlendFactor        = backend::BlendFactor::ONE;
        blendDescriptor.destinationRGBBlendFactor   = backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
        blendDescriptor.sourceAlphaBlendFactor      = backend::BlendFactor::ONE;
        blendDescriptor.destinationAlphaBlendFactor = backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
        setOpacityModifyRGB(true);
    }
}

void DrawNode::updateUniforms(const Mat4& transform, CustomCommand& cmd)
{
    auto& pipelineDescriptor = cmd.getPipelineDescriptor();
    const auto& matrixP      = _director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    Mat4 matrixMVP           = matrixP * transform;
    auto mvpLocation         = pipelineDescriptor.programState->getUniformLocation("u_MVPMatrix");
    pipelineDescriptor.programState->setUniform(mvpLocation, matrixMVP.m, sizeof(matrixMVP.m));

    float alpha               = _displayedOpacity / 255.0f;
    auto alphaUniformLocation = pipelineDescriptor.programState->getUniformLocation("u_alpha");
    pipelineDescriptor.programState->setUniform(alphaUniformLocation, &alpha, sizeof(alpha));
}

void DrawNode::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    if (_trianglesDirty || _pointsDirty || _linesDirty)
        updateBuffers();

    if (_customCommandTriangle.getVertexDrawCount() > 0)
    {
        updateBlendState(_customCommandTriangle);
        updateUniforms(transform, _customCommandTriangle);
        _customCommandTriangle.init(_globalZOrder);
        renderer->addCommand(&_customCommandTriangle);
    }

    if (_customCommandPoint.getVertexDrawCount() > 0)
    {
        updateBlendState(_customCommandPoint);
        updateUniforms(transform, _customCommandPoint);
        _customCommandPoint.init(_globalZOrder);
        renderer->addCommand(&_customCommandPoint);
    }

    if (_customCommandLine.getVertexDrawCount() > 0)
    {
        updateBlendState(_customCommandLine);
        updateUniforms(transform, _customCommandLine);
        _customCommandLine.init(_globalZOrder);
        renderer->addCommand(&_customCommandLine);
    }
}

static void udpateCommand(CustomCommand& cmd, const axstd::pod_vector<V2F_C4B_T2F>& buffer)
{
    if (buffer.empty())
    {
        cmd.setVertexBuffer(nullptr);
    }
    else
    {
        cmd.createVertexBuffer(sizeof(V2F_C4B_T2F), buffer.size(), CustomCommand::BufferUsage::STATIC);
        cmd.updateVertexBuffer(buffer.data(), buffer.size() * sizeof(V2F_C4B_T2F));
    }

    cmd.setVertexDrawInfo(0, buffer.size());
}

void DrawNode::updateBuffers()
{
    if (_trianglesDirty)
    {
        _trianglesDirty = false;
        udpateCommand(_customCommandTriangle, _triangles);
    }

    if (_pointsDirty)
    {
        _pointsDirty = false;
        udpateCommand(_customCommandPoint, _points);
    }

    if (_linesDirty)
    {
        _linesDirty = false;
        udpateCommand(_customCommandLine, _lines);
    }
}

void DrawNode::drawPoint(const Vec2& position,
                         const float pointSize,
                         const Color4B& color,
                         const DrawNode::PointType pointType)
{
    if (pointSize <= 0.0f)
        return;

    _drawPoint(position, pointSize, color, pointType);
}

void DrawNode::drawPoints(const Vec2* position,
                          unsigned int numberOfPoints,
                          const Color4B& color,
                          const DrawNode::PointType pointType)
{
    _drawPoints(position, numberOfPoints, 1.0f, color, pointType);
}

void DrawNode::drawPoints(const Vec2* position,
                          unsigned int numberOfPoints,
                          const float pointSize,
                          const Color4B& color,
                          const DrawNode::PointType pointType)
{
    if (pointSize <= 0.0f)
        return;
    _drawPoints(position, numberOfPoints, pointSize, color, pointType);
}

void DrawNode::drawLine(const Vec2& origin,
                        const Vec2& destination,
                        const Color4B& color,
                        float thickness,
                        DrawNode::EndType etStart,
                        DrawNode::EndType etEnd)
{
    if (thickness <= 0.0f)
    {
        AXLOGW("{}: thickness <= 0", __FUNCTION__);
        return;
    }

    _drawSegment(origin, destination, color, thickness, etStart, etEnd);
}

void DrawNode::drawPoly(const Vec2* poli,
                        unsigned int numberOfPoints,
                        bool closedPolygon,
                        const Color4B& color,
                        float thickness)
{
    if (thickness <= 0.0f)
    {
        AXLOGW("{}: thickness <= 0", __FUNCTION__);
        return;
    }
    _drawPoly(poli, numberOfPoints, closedPolygon, color, thickness);
}

void DrawNode::drawCircle(const Vec2& center,
                          float radius,
                          float angle,
                          unsigned int segments,
                          bool drawLineToCenter,
                          float scaleX,
                          float scaleY,
                          const Color4B& color,
                          float thickness)
{
    if (thickness <= 0.0f)
    {
        AXLOGW("{}: thickness <= 0", __FUNCTION__);
        return;
    }
    if (radius == 0.0f)
    {
        AXLOGW("{}: radius == 0", __FUNCTION__);
        return;
    }

    _drawCircle(center, radius, angle, segments, drawLineToCenter, scaleX, scaleY, color, Color4B(), false, thickness);
}

void DrawNode::drawCircle(const Vec2& center,
                          float radius,
                          float angle,
                          unsigned int segments,
                          bool drawLineToCenter,
                          const Color4B& color,
                          float thickness)
{
    if (thickness <= 0.0f)
    {
        AXLOGW("{}: thickness <= 0", __FUNCTION__);
        return;
    }
    if (radius == 0.0f)
    {
        AXLOGW("{}: radius == 0", __FUNCTION__);
        return;
    }

    _drawCircle(center, radius, angle, segments, drawLineToCenter, 1.0f, 1.0f, color, color, false, thickness);
}

void DrawNode::drawStar(const Vec2& center,
                        float radiusI,
                        float radiusO,
                        unsigned int segments,
                        const Color4B& color,
                        float thickness)
{
    if (thickness <= 0.0f)
    {
        AXLOGW("{}: thickness <= 0", __FUNCTION__);
        return;
    }
    _drawAStar(center, radiusI, radiusO, segments, color, color, thickness, false);
}

void DrawNode::drawSolidStar(const Vec2& center,
                             float radiusI,  // inner
                             float radiusO,  // outer
                             unsigned int segments,
                             const Color4B& color,
                             const Color4B& filledColor,
                             float thickness)
{
    if (thickness < 0.0f)
    {
        AXLOGW("{}: thickness < 0, changed to 0", __FUNCTION__);
        thickness = 0.0f;
    }
    _drawAStar(center, radiusI, radiusO, segments, color, filledColor, thickness, true);
}

void DrawNode::drawQuadBezier(const Vec2& origin,
                              const Vec2& control,
                              const Vec2& destination,
                              unsigned int segments,
                              const Color4B& color,
                              float thickness)
{
    if (thickness <= 0.0f)
    {
        AXLOGW("{}: thickness <= 0", __FUNCTION__);
        return;
    }

    axstd::pod_vector<Vec2> _vertices{
        static_cast<size_t>(segments + 1)};  // Vec2* _vertices = _abuf.get<Vec2>(segments + 1);

    float t = 0.0f;
    for (unsigned int i = 0; i < segments; i++)
    {
        _vertices[i].x = powf(1.0f - t, 2.0f) * origin.x + 2.0f * (1.0f - t) * t * control.x + t * t * destination.x;
        _vertices[i].y = powf(1.0f - t, 2.0f) * origin.y + 2.0f * (1.0f - t) * t * control.y + t * t * destination.y;
        t += 1.0f / segments;
    }
    _vertices[segments].x = destination.x;
    _vertices[segments].y = destination.y;

    _drawPoly(_vertices.data(), segments + 1, false, color, thickness, false);
}

void DrawNode::drawCubicBezier(const Vec2& origin,
                               const Vec2& control1,
                               const Vec2& control2,
                               const Vec2& destination,
                               unsigned int segments,
                               const Color4B& color,
                               float thickness)
{
    if (thickness <= 0.0f)
    {
        AXLOGW("{}: thickness <= 0", __FUNCTION__);
        return;
    }

    axstd::pod_vector<Vec2> _vertices{static_cast<size_t>(segments + 1)};

    float t = 0.0f;
    for (unsigned int i = 0; i < segments; i++)
    {
        _vertices[i].x = powf(1.0f - t, 3.0f) * origin.x + 3.0f * powf(1.0f - t, 2.0f) * t * control1.x +
                         3.0f * (1 - t) * t * t * control2.x + t * t * t * destination.x;
        _vertices[i].y = powf(1.0f - t, 3.0f) * origin.y + 3.0f * powf(1.0f - t, 2.0f) * t * control1.y +
                         3.0f * (1 - t) * t * t * control2.y + t * t * t * destination.y;
        t += 1.0f / segments;
    }
    _vertices[segments].x = destination.x;
    _vertices[segments].y = destination.y;

    _drawPoly(_vertices.data(), segments + 1, false, color, thickness, true);
}

void DrawNode::drawCardinalSpline(PointArray* config,
                                  float tension,
                                  unsigned int segments,
                                  const Color4B& color,
                                  float thickness)
{
    if (thickness <= 0.0f)
    {
        AXLOGW("{}: thickness <= 0", __FUNCTION__);
        return;
    }

    axstd::pod_vector<Vec2> _vertices{static_cast<size_t>(segments)};

    ssize_t p;
    float lt;
    float deltaT = 1.0f / config->count();

    for (unsigned int i = 0; i < segments; i++)
    {
        float dt = (float)i / segments;

        // border
        if (dt == 1)
        {
            p  = config->count() - 1;
            lt = 1;
        }
        else
        {
            p  = static_cast<ssize_t>(dt / deltaT);
            lt = (dt - deltaT * (float)p) / deltaT;
        }

        // Interpolate
        Vec2 pp0 = config->getControlPointAtIndex(p - 1);
        Vec2 pp1 = config->getControlPointAtIndex(p + 0);
        Vec2 pp2 = config->getControlPointAtIndex(p + 1);
        Vec2 pp3 = config->getControlPointAtIndex(p + 2);

        Vec2 newPos    = ccCardinalSplineAt(pp0, pp1, pp2, pp3, tension, lt);
        _vertices[i].x = newPos.x;
        _vertices[i].y = newPos.y;
        if (newPos == config->getControlPointAtIndex(config->count() - 1) && i > 0)
        {
            segments = i + 1;
            break;
        }
    }

    _drawPoly(_vertices.data(), segments, false, color, thickness, true);
}

void DrawNode::drawCatmullRom(PointArray* points, unsigned int segments, const Color4B& color, float thickness)
{
    if (thickness <= 0.0f)
    {
        AXLOGW("{}: thickness <= 0", __FUNCTION__);
        return;
    }
    drawCardinalSpline(points, 0.5f, segments, color, thickness);
}

void DrawNode::drawDot(const Vec2& pos, float radius, const Color4B& color)
{
    if (radius <= 0.0f)
    {
        AXLOGW("{}: radius <= 0", __FUNCTION__);
        return;
    }
    _drawDot(pos, radius, color);
}

void DrawNode::drawRect(const Vec2& p1,
                        const Vec2& p2,
                        const Vec2& p3,
                        const Vec2& p4,
                        const Color4B& color,
                        float thickness)
{
    if (thickness <= 0.0f)
    {
        AXLOGW("{}: thickness <= 0", __FUNCTION__);
        return;
    }

    Vec2 line[5] = {p1, p2, p3, p4, p1};
    _drawPoly(line, 5, false, color, thickness, true);
}

void DrawNode::drawRect(const Vec2& origin, const Vec2& destination, const Color4B& color, float thickness)
{
    if (thickness <= 0.0f)
    {
        AXLOGW("{}: thickness <= 0", __FUNCTION__);
        return;
    }

    Vec2 line[5] = {origin, Vec2(destination.x, origin.y), destination, Vec2(origin.x, destination.y), origin};
    _drawPoly(line, 5, false, color, thickness, true);
}

void DrawNode::drawSegment(const Vec2& from,
                           const Vec2& to,
                           float thickness,
                           const Color4B& color,
                           DrawNode::EndType etStart,
                           DrawNode::EndType etEnd)
{
    if (thickness <= 0.0f)
    {
        AXLOGW("{}: thickness <= 0", __FUNCTION__);
        return;
    }
    _drawSegment(from, to, color, thickness, etStart, etEnd);
}

void DrawNode::drawPolygon(Vec2* verts,
                           int count,
                           const Color4B& fillColor,
                           float thickness,
                           const Color4B& borderColor,
                           bool isconvex)
{
    if (thickness < 0.0f)
    {
        AXLOGW("{}: thickness < 0, changed to 0", __FUNCTION__);
        thickness = 0.0f;
    }
    _drawPolygon(verts, count, fillColor, borderColor, true, thickness, isconvex);
}

void DrawNode::drawPolygon(Vec2* verts, int count, float thickness, const Color4B& borderColor, bool isconvex)
{
    if (thickness < 0.0f)
    {
        AXLOGW("{}: thickness < 0, changed to 0", __FUNCTION__);
        thickness = 0.0f;
    }
    _drawPolygon(verts, count, Color4B::TRANSPARENT, borderColor, true, thickness, isconvex);
}

void DrawNode::drawSolidPolygon(Vec2* verts,
                                int count,
                                const Color4B& fillColor,
                                float thickness,
                                const Color4B& borderColor,
                                bool isconvex)
{
    if (thickness < 0.0f)
    {
        AXLOGW("{}: thickness < 0, changed to 0", __FUNCTION__);
        thickness = 0.0f;
    }
    _drawPolygon(verts, count, fillColor, borderColor, true, thickness, isconvex);
}

void DrawNode::drawSolidRect(const Vec2& origin,
                             const Vec2& destination,
                             const Color4B& fillColor,
                             float thickness,
                             const Color4B& borderColor)
{
    if (thickness < 0.0f)
    {
        AXLOGW("{}: thickness < 0, changed to 0", __FUNCTION__);
        thickness = 0.0f;
    }
    Vec2 _vertices[] = {origin, Vec2(destination.x, origin.y), destination, Vec2(origin.x, destination.y), origin};
    _drawPolygon(_vertices, 5, fillColor, borderColor, true, thickness, true);
}

void DrawNode::drawSolidPoly(const Vec2* poli,
                             unsigned int numberOfPoints,
                             const Color4B& color,
                             float thickness,
                             const Color4B& borderColor,
                             bool isconvex)
{
    if (thickness < 0.0f)
    {
        AXLOGW("{}: thickness < 0, changed to 0", __FUNCTION__);
        thickness = 0.0f;
    }
    _drawPolygon(poli, numberOfPoints, color, borderColor, true, thickness, isconvex);
}

void DrawNode::drawPie(const Vec2& center,
                       float radius,
                       float rotation,
                       int startAngle,
                       int endAngle,
                       float scaleX,
                       float scaleY,
                       const Color4B& fillColor,
                       const Color4B& borderColor,
                       DrawMode drawMode,
                       float thickness)
{
    if (thickness < 0.0f)
    {
        AXLOGW("{}: thickness < 0, changed to 0", __FUNCTION__);
        thickness = 0.0f;
    }
    _drawPie(center, radius, rotation, startAngle, endAngle, scaleX, scaleY, fillColor, borderColor, drawMode,
             thickness);
}

void DrawNode::drawPie(const Vec2& center,
                       float radius,
                       float angle,
                       int startAngle,
                       int endAngle,
                       float scaleX,
                       float scaleY,
                       const Color4B& color,
                       DrawMode drawMode)
{
    _drawPie(center, radius, angle, startAngle, endAngle, scaleX, scaleY, Color4B::TRANSPARENT, color, drawMode, 1.0f);
}

void DrawNode::drawSolidCircle(const Vec2& center,
                               float radius,
                               float angle,
                               unsigned int segments,
                               float scaleX,
                               float scaleY,
                               const Color4B& fillColor,
                               float thickness,
                               const Color4B& borderColor,
                               bool drawLineToCenter)
{
    if (thickness < 0.0f)
    {
        AXLOGW("{}: thickness < 0, changed to 0", __FUNCTION__);
        thickness = 0.0f;
    }
    _drawCircle(center, radius, angle, segments, drawLineToCenter, scaleX, scaleY, borderColor, fillColor, true,
                thickness);
}

void DrawNode::drawSolidCircle(const Vec2& center,
                               float radius,
                               float angle,
                               unsigned int segments,
                               float scaleX,
                               float scaleY,
                               const Color4B& color)
{
    if (radius < 0.0f)
    {
        AXLOGW("{}: radius < 0, changed to 0", __FUNCTION__);
        radius = 0.0f;
    }
    _drawCircle(center, radius, angle, segments, false, scaleX, scaleY, Color4B(), color, true);
}

void DrawNode::drawSolidCircle(const Vec2& center,
                               float radius,
                               float angle,
                               unsigned int segments,
                               const Color4B& color)
{
    if (radius < 0.0f)
    {
        AXLOGW("{}: radius < 0, changed to 0", __FUNCTION__);
        radius = 0.0f;
    }
    _drawCircle(center, radius, angle, segments, false, 1.0f, 1.0f, Color4B(), color, true);
}

void DrawNode::drawTriangle(const Vec2* vertices3, const Color4B& color)
{
    Vec2 vertices[3] = {vertices3[0], vertices3[1], vertices3[2]};
    _drawTriangle(vertices, Color4B::TRANSPARENT, color, false, 0.0f);
}

void DrawNode::drawTriangle(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Color4B& color)
{
    Vec2 vertices[3] = {p1, p2, p3};
    _drawTriangle(vertices, Color4B::TRANSPARENT, color, false, 0.0f);
}

void DrawNode::drawSolidTriangle(const Vec2* vertices3,
                                 const Color4B& fillColor,
                                 const Color4B& borderColor,
                                 float thickness)
{
    if (thickness < 0.0f)
    {
        AXLOGW("{}: thickness < 0, changed to 0", __FUNCTION__);
        thickness = 0.0f;
    }
    Vec2 vertices[3] = {vertices3[0], vertices3[1], vertices3[2]};
    _drawTriangle(vertices, fillColor, borderColor, true, thickness);
}

void DrawNode::drawSolidTriangle(const Vec2& p1,
                                 const Vec2& p2,
                                 const Vec2& p3,
                                 const Color4B& fillColor,
                                 const Color4B& borderColor,
                                 float thickness)
{
    if (thickness < 0.0f)
    {
        AXLOGW("{}: thickness < 0, changed to 0", __FUNCTION__);
        thickness = 0.0f;
    }
    Vec2 vertices[3] = {p1, p2, p3};
    _drawTriangle(vertices, fillColor, borderColor, false, thickness);
}

void DrawNode::clear()
{
    _trianglesDirty = true;
    _pointsDirty    = true;
    _linesDirty     = true;

    _triangles.clear();
    _points.clear();
    _lines.clear();
}

const BlendFunc& DrawNode::getBlendFunc() const
{
    return _blendFunc;
}

void DrawNode::setBlendFunc(const BlendFunc& blendFunc)
{
    _blendFunc = blendFunc;
}



void DrawNode::visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    if (_isolated)
    {
        // ignore `parentTransform` from parent
        Node::visit(renderer, Mat4::IDENTITY, parentFlags);
    }
    else
    {
        Node::visit(renderer, parentTransform, parentFlags);
    }
}

void DrawNode::_drawPolygon(const Vec2* verts,
                            unsigned int count,
                            const Color4B& fillColor,
                            const Color4B& borderColor,
                            bool closedPolygon,
                            float thickness,
                            bool isconvex)
{
    AXASSERT(count >= 0, "invalid count value");

    bool outline = (thickness != 0.0f);

    auto _vertices = _transform(verts, count, closedPolygon);

    std::vector<V2F_C4B_T2F_Triangle> triangleList;

    int vertex_count = 0;

    // calculate the memory (important for correct drawing stuff)
    if (closedPolygon && !isconvex && fillColor.a > 0.0f && !isConvex(_vertices.data(), count) && count >= 3)
    {
        std::vector<p2t::Point> p2pointsStorage;
        p2pointsStorage.reserve(count);
        std::vector<p2t::Point*> p2points;
        p2points.reserve(count);

        for (unsigned int i = 0; i < count - 1;
             i++)  // count-1 is needed because of: _vertices[0] = _vertices[i < count]
        {
            p2points.emplace_back(&p2pointsStorage.emplace_back((float)_vertices[i].x, (float)_vertices[i].y));
        }
        p2t::CDT cdt(p2points);
        cdt.Triangulate();
        std::vector<p2t::Triangle*> tris = cdt.GetTriangles();

        vertex_count += tris.size();
        for (auto&& t : tris)  // use it later; only one calculate!!!
        {
            p2t::Point* vec1 = t->GetPoint(0);
            p2t::Point* vec2 = t->GetPoint(1);
            p2t::Point* vec3 = t->GetPoint(2);

            V2F_C4B_T2F_Triangle triangle = {
                {Vec2(vec1->x, vec1->y), fillColor, Vec2::ZERO},
                {Vec2(vec2->x, vec2->y), fillColor, Vec2::ZERO},
                {Vec2(vec3->x, vec3->y), fillColor, Vec2::ZERO},
            };
            triangleList.emplace_back(triangle);  // use it for drawing later
        }
    }
    else if (fillColor.a > 0.0f)
    {
        vertex_count += count - 2;
    }

    if (outline)
    {
        if (thickness != 1.0f || properties.drawOrder)
        {
            vertex_count += 6 * (count - 1);
        }
        else
        {
            vertex_count += 2 * count;
        }
    }

    vertex_count *= 3;

    auto triangles = reinterpret_cast<V2F_C4B_T2F_Triangle*>(expandBufferAndGetPointer(_triangles, vertex_count));
    _trianglesDirty = true;

    // start drawing...
    int ii = 0;
    if (closedPolygon && !isconvex && fillColor.a > 0.0f && !isConvex(_vertices.data(), count) && count >= 3)
    {
        for (auto&& t : triangleList)
        {
            triangles[ii++] = t;
        }
    }
    else if (fillColor.a > 0.0f)
    {
        for (unsigned int i = 0; i < count - 2; i++)
        {
            triangles[ii++] = {
                {_vertices[0], fillColor, Vec2::ZERO},
                {_vertices[i + 1], fillColor, Vec2::ZERO},
                {_vertices[i + 2], fillColor, Vec2::ZERO},
            };
        }
    }
    if (outline)
    {
        if (thickness != 1.0f || properties.drawOrder)
        {
            thickness *= properties.factor;

            for (unsigned int i = 1; i < (count); i++)
            {
                Vec2 a  = _vertices[i - 1];
                Vec2 b  = _vertices[i];
                Vec2 n  = ((b - a).getPerp()).getNormalized();
                Vec2 t  = n.getPerp();
                Vec2 nw = n * thickness;
                Vec2 tw = t * thickness;
                Vec2 v0 = b - (nw + tw);
                Vec2 v1 = b + (nw - tw);
                Vec2 v2 = b - nw;
                Vec2 v3 = b + nw;
                Vec2 v4 = a - nw;
                Vec2 v5 = a + nw;
                Vec2 v6 = a - (nw - tw);
                Vec2 v7 = a + (nw + tw);

                {
                    triangles[ii++] = {
                        {v0, borderColor, -(n + t)},
                        {v1, borderColor, n - t},
                        {v2, borderColor, -n},
                    };
                    triangles[ii++] = {
                        {v3, borderColor, n},
                        {v1, borderColor, n - t},
                        {v2, borderColor, -n},
                    };
                }

                triangles[ii++] = {
                    {v3, borderColor, n},
                    {v4, borderColor, -n},
                    {v2, borderColor, -n},
                };
                triangles[ii++] = {
                    {v3, borderColor, n},
                    {v4, borderColor, -n},
                    {v5, borderColor, n},
                };

                {
                    triangles[ii++] = {
                        {v6, borderColor, t - n},
                        {v4, borderColor, -n},
                        {v5, borderColor, n},
                    };
                    triangles[ii++] = {
                        {v6, borderColor, t - n},
                        {v7, borderColor, t + n},
                        {v5, borderColor, n},
                    };
                }
            }
        }
        else
        {
            struct ExtrudeVerts
            {
                Vec2 offset, n;
            };

            axstd::pod_vector<ExtrudeVerts> extrude{static_cast<size_t>(sizeof(struct ExtrudeVerts) * count)};

            for (unsigned int i = 0; i < count; i++)
            {
                Vec2 v0 = _vertices[(i - 1 + count) % count];
                Vec2 v1 = _vertices[i];
                Vec2 v2 = _vertices[(i + 1) % count];

                Vec2 n1 = ((v1 - v0).getPerp()).getNormalized();
                Vec2 n2 = ((v2 - v1).getPerp()).getNormalized();

                Vec2 offset = (n1 + n2) * (1.0f / (Vec2::dot(n1, n2) + 1.0f));
                extrude[i]  = {offset, n2};
            }

            for (unsigned int i = 0; i < count-1; i++)
            {
                int j   = (i + 1) % count;
                Vec2 v0 = _vertices[i];
                Vec2 v1 = _vertices[j];

                Vec2 n0 = extrude[i].n;

                Vec2 offset0 = extrude[i].offset;
                Vec2 offset1 = extrude[j].offset;

                Vec2 inner0 = v0 - offset0 * thickness;
                Vec2 inner1 = v1 - offset1 * thickness;
                Vec2 outer0 = v0 + offset0 * thickness;
                Vec2 outer1 = v1 + offset1 * thickness;

                triangles[ii++] = {{inner0, borderColor, -n0}, {inner1, borderColor, -n0}, {outer1, borderColor, n0}};

                triangles[ii++] = {{inner0, borderColor, -n0}, {outer0, borderColor, n0}, {outer1, borderColor, n0}};
            }
        }
    }
}

void DrawNode::_drawPoly(const Vec2* verts,
                         unsigned int count,
                         bool closedPolygon,
                         const Color4B& color,
                         float thickness,
                         bool isconvex)
{
    if (thickness == 1.0f && !properties.drawOrder)
    {
        auto _vertices = _transform(verts, count, closedPolygon);

        unsigned int vertex_count = (closedPolygon) ? 2 * count : 2 * (count - 1);

        auto line = expandBufferAndGetPointer(_lines, vertex_count);
        _linesDirty = true;

        int ii = 0;
        for (unsigned int i = 0; i < count - 1; i++)
        {
            line[ii++] = {_vertices[i], color, Vec2::ZERO};
            line[ii++] = {_vertices[i + 1], color, Vec2::ZERO};
        }
        if (closedPolygon)
        {
            line[ii++] = {_vertices[count - 1], color, Vec2::ZERO};
            line[ii++] = line[0];
        }
    }
    else
    {
        _drawPolygon(verts, count, Color4B::TRANSPARENT, color, closedPolygon, thickness, isconvex);
    }
}

void DrawNode::_drawSegment(const Vec2& from,
                            const Vec2& to,
                            const Color4B& color,
                            float thickness,
                            DrawNode::EndType etStart,
                            DrawNode::EndType etEnd)
{
    Vec2 vertices[2] = {from, to};
    applyTransform(vertices, vertices, 2);

    if (thickness == 1.0f && !properties.drawOrder)
    {
        auto line = expandBufferAndGetPointer(_lines, 2);
        _linesDirty = true;

        line[0] = {vertices[0], color, Vec2::ZERO};
        line[1] = {vertices[1], color, Vec2::ZERO};
    }
    else
    {
        Vec2 a  = vertices[0];
        Vec2 b  = vertices[1];
        Vec2 n  = ((b - a).getPerp()).getNormalized();
        Vec2 t  = n.getPerp();
        Vec2 nw = n * thickness;
        Vec2 tw = t * thickness;
        Vec2 v0 = b - (nw + tw);
        Vec2 v1 = b + (nw - tw);
        Vec2 v2 = b - nw;
        Vec2 v3 = b + nw;
        Vec2 v4 = a - nw;
        Vec2 v5 = a + nw;
        Vec2 v6 = a - (nw - tw);
        Vec2 v7 = a + (nw + tw);

        unsigned int vertex_count = 3 * ((etStart != DrawNode::EndType::Butt) ? 2 : 0) + 3 * 2 +
                                    3 * ((etEnd != DrawNode::EndType::Butt) ? 2 : 0);
        auto triangles = reinterpret_cast<V2F_C4B_T2F_Triangle*>(expandBufferAndGetPointer(_triangles, vertex_count));
        _trianglesDirty = true;

        int ii = 0;
        switch (etEnd)
        {
        case DrawNode::EndType::Butt:
            break;

        case DrawNode::EndType::Square:
            triangles[ii++] = {
                {v0, color, Vec2::ZERO},
                {v1, color, -n},
                {v2, color, n},
            };

            triangles[ii++] = {
                {v3, color, n},
                {v1, color, Vec2::ZERO},
                {v2, color, -n},
            };

            break;
        case DrawNode::EndType::Round:
            triangles[ii++] = {
                {v0, color, -(n + t)},
                {v1, color, n - t},
                {v2, color, -n},
            };

            triangles[ii++] = {
                {v3, color, n},
                {v1, color, n - t},
                {v2, color, -n},
            };
            break;

        default:
            break;
        }

        // BODY
        triangles[ii++] = {
            {v3, color, n},
            {v4, color, -n},
            {v2, color, -n},
        };

        triangles[ii++] = {
            {v3, color, n},
            {v4, color, -n},
            {v5, color, n},
        };

        switch (etStart)
        {
        case DrawNode::EndType::Butt:
            break;

        case DrawNode::EndType::Square:
            triangles[ii++] = {
                {v6, color, Vec2::ZERO},
                {v4, color, -n},
                {v5, color, n},
            };

            triangles[ii++] = {
                {v6, color, -n},
                {v7, color, Vec2::ZERO},
                {v5, color, n},
            };
            break;

        case DrawNode::EndType::Round:
            triangles[ii++] = {
                {v6, color, t - n},
                {v4, color, -n},
                {v5, color, n},
            };

            triangles[ii++] = {
                {v6, color, t - n},
                {v7, color, t + n},
                {v5, color, n},
            };
            break;

        default:
            break;
        }
    }
}

void DrawNode::_drawDot(const Vec2& pos, float radius, const Color4B& color)
{
    unsigned int vertex_count = 2 * 3;
    auto triangles = reinterpret_cast<V2F_C4B_T2F_Triangle*>(expandBufferAndGetPointer(_triangles, vertex_count));
    _trianglesDirty = true;

    V2F_C4B_T2F a = {Vec2(pos.x - radius, pos.y - radius), color, Vec2(-1.0f, -1.0f)};
    V2F_C4B_T2F b = {Vec2(pos.x - radius, pos.y + radius), color, Vec2(-1.0f, 1.0f)};
    V2F_C4B_T2F c = {Vec2(pos.x + radius, pos.y + radius), color, Vec2(1.0f, 1.0f)};
    V2F_C4B_T2F d = {Vec2(pos.x + radius, pos.y - radius), color, Vec2(1.0f, -1.0f)};

    triangles[0] = {a, b, c};
    triangles[1] = {a, c, d};
}

void DrawNode::_drawCircle(const Vec2& center,
                           float radius,
                           float angle,
                           unsigned int segments,
                           bool drawLineToCenter,
                           float scaleX,
                           float scaleY,
                           const Color4B& borderColor,
                           const Color4B& fillColor,
                           bool solid,
                           float thickness)
{
    const float coef = 2.0f * (float)M_PI / segments;

    int count       = (drawLineToCenter) ? 3 : 2;
    Vec2* _vertices = new Vec2[segments + count];

    float rsX = radius * scaleX;
    float rsY = radius * scaleY;
    for (unsigned int i = 0; i < segments; i++)
    {
        float rads     = i * coef + angle;
        _vertices[i].x = rsX * cosf(rads) + center.x;
        _vertices[i].y = rsY * sinf(rads) + center.y;
    }
    _vertices[segments] = _vertices[0];

    if (drawLineToCenter)
        _vertices[++segments] = center;

    if (solid)
        _drawPolygon(_vertices, segments + 1, fillColor, borderColor, false, thickness, true);
    else
        _drawPoly(_vertices, segments + 1, false, borderColor, thickness, true);

    AX_SAFE_DELETE_ARRAY(_vertices);
}

void DrawNode::_drawTriangle(Vec2* vertices3,
                             const Color4B& borderColor,
                             const Color4B& fillColor,
                             bool solid,
                             float thickness)
{
    unsigned int vertex_count = 3;

    if (thickness != 0.0f)
    {
        _drawPolygon(vertices3, vertex_count, fillColor, borderColor, true, thickness, true);
    }
    else
    {
        applyTransform(vertices3, vertices3, vertex_count);

        auto triangles = reinterpret_cast<V2F_C4B_T2F_Triangle*>(expandBufferAndGetPointer(_triangles, vertex_count));
        _trianglesDirty = true;

        triangles[0] = {{vertices3[0], fillColor, Vec2::ZERO},
                        {vertices3[1], fillColor, Vec2::ZERO},
                        {vertices3[2], fillColor, Vec2::ZERO}};
    }
}

void DrawNode::_drawAStar(const Vec2& center,
                          float radiusI,  // inner
                          float radiusO,  // outer
                          unsigned int segments,
                          const Color4B& color,
                          const Color4B& filledColor,
                          float thickness,
                          bool solid)
{
    const float coef = 2.0f * (float)M_PI / segments;
    float halfAngle  = coef / 2.0f;

    axstd::pod_vector<Vec2> _vertices(segments * 2 + 1);

    int i = 0;
    for (unsigned int a = 0; a < segments; a++)
    {
        float rads     = a * coef;
        _vertices[i++] = Vec2(center.x + cos(rads) * radiusO, center.y + sin(rads) * radiusO);
        _vertices[i++] = Vec2(center.x + cos(rads + halfAngle) * radiusI, center.y + sin(rads + halfAngle) * radiusI);
    }

    if (solid)
    {
        _drawPolygon(_vertices.data(), i, filledColor, color, true, thickness, false);
    }
    else
    {
        _vertices[i++] = _vertices[0];
        _drawPoly(_vertices.data(), i, true, color, thickness, false);
    }
}

void DrawNode::_drawPoints(const Vec2* position,
                           unsigned int numberOfPoints,
                           const float pointSize,
                           const Color4B& color,
                           const DrawNode::PointType pointType)
{
    if (properties.drawOrder == true)
    {
        float pointSize4 = pointSize * 0.25f;
        Vec2 vec2Size4   = Vec2(pointSize4, pointSize4);
        for (unsigned int i = 0; i < numberOfPoints; i++)
        {
            switch (pointType)
            {
            case PointType::Circle:
            {
                _drawCircle(position[i], pointSize4, 90, 32, false, 1.0f, 1.0f, Color4B(), color, true);
                break;
            }
            case PointType::Rect:
            {
                Vec2 origin      = position[i] - vec2Size4;
                Vec2 destination = position[i] + vec2Size4;
                Vec2 _vertices[] = {origin, Vec2(destination.x, origin.y), destination, Vec2(origin.x, destination.y),
                                    origin};
                _drawPolygon(_vertices, 5, color, color, false, 0.0f, true);
            }
            break;
            default:
                break;
            }
        }
        return;
    }

    auto point = expandBufferAndGetPointer(_points, numberOfPoints);
    _pointsDirty = true;

    for (unsigned int i = 0; i < numberOfPoints; i++)
    {
        *(point + i) = {position[i], color, Vec2(pointSize, 0.0f)};
    }
}

void DrawNode::_drawPoint(const Vec2& position,
                          const float pointSize,
                          const Color4B& color,
                          const DrawNode::PointType pointType)
{
    if (properties.drawOrder == true)
    {
        float pointSize4 = pointSize * 0.25f;
        Vec2 vec2Size4   = Vec2(pointSize4, pointSize4);

        switch (pointType)
        {
        case PointType::Circle:
        {
            _drawCircle(position, pointSize4, 90, 32, false, 1.0f, 1.0f, Color4B(), color, true);
            break;
        }
        case PointType::Rect:
        {
            Vec2 origin      = position - vec2Size4;
            Vec2 destination = position + vec2Size4;
            Vec2 _vertices[] = {origin, Vec2(destination.x, origin.y), destination, Vec2(origin.x, destination.y),
                                origin};
            _drawPolygon(_vertices, 5, color, color, false, 0.0f, true);
        }
        break;
        default:
            break;
        }

        return;
    }

    if (properties.drawOrder == true)
    {
        float pointSize4 = pointSize * 0.25f;
        Vec2 origin      = position - Vec2(pointSize4, pointSize4);
        Vec2 destination = position + Vec2(pointSize4, pointSize4);
        Vec2 _vertices[] = {origin, Vec2(destination.x, origin.y), destination, Vec2(origin.x, destination.y)};
        _drawPolygon(_vertices, 4, color, color, false, 0.0f, true);
    }
    else
    {
        auto point = expandBufferAndGetPointer(_points, 1);
        _pointsDirty = true;

        *point             = {position, color, Vec2(pointSize, 0.0f)};
    }
}

void DrawNode::_drawPie(const Vec2& center,
                        float radius,
                        float rotation,
                        int startAngle,
                        int endAngle,
                        float scaleX,
                        float scaleY,
                        const Color4B& fillColor,
                        const Color4B& borderColor,
                        DrawMode drawMode,
                        float thickness)
{
#define DEGREES 360

    // Not a real line!
    if (startAngle == endAngle)
        return;

    // Its a circle?
    if (MAX((startAngle - endAngle), (endAngle - startAngle)) == DEGREES)
    {
        switch (drawMode)
        {
        case DrawMode::Fill:
            _drawCircle(center, radius, 0.0f, 360, false, scaleX, scaleY, borderColor, fillColor, true, thickness);
            break;
        case DrawMode::Outline:
            _drawCircle(center, radius, 0.0f, 360, false, scaleX, scaleY, borderColor, Color4B::TRANSPARENT, true,
                        thickness);
            break;
        case DrawMode::Line:
            _drawCircle(center, radius, 0.0f, 360, false, scaleX, scaleY, borderColor, Color4B::TRANSPARENT, true,
                        thickness);
            break;
        case DrawMode::Semi:
            _drawCircle(center, radius, 0.0f, 360, false, scaleX, scaleY, borderColor, fillColor, true, thickness);
            break;
        default:
            break;
        }
        return;
    }
    else
    {
        const float coef = 2.0f * (float)M_PI / DEGREES;

        axstd::pod_vector<Vec2> _vertices(DEGREES + 2);

        int n        = 0;
        float rads   = 0.0f;
        float _angle = AX_DEGREES_TO_RADIANS(rotation);

        if (startAngle > endAngle)
        {
            std::swap(endAngle, startAngle);
        }

        for (int i = 0; i <= DEGREES; i++)
        {
            if (startAngle <= i && endAngle >= i)
            {
                rads = i * coef;

                float j = radius * cosf(rads + _angle) * scaleX + center.x;
                float k = radius * sinf(rads + _angle) * scaleY + center.y;

                _vertices[n].x = j;
                _vertices[n].y = k;
                n++;
            }
        }
        switch (drawMode)
        {
        case DrawMode::Fill:
            _vertices[n++] = center;
            _vertices[n++] = _vertices[0];
            _drawPolygon(_vertices.data(), n, fillColor, borderColor, true, thickness, false);
            break;
        case DrawMode::Outline:
            _vertices[n++] = center;
            _vertices[n++] = _vertices[0];
            _drawPolygon(_vertices.data(), n, Color4B::TRANSPARENT, borderColor, false, thickness, false);
            break;
        case DrawMode::Line:
            _drawPolygon(_vertices.data(), n - 1, Color4B::TRANSPARENT, borderColor, false, thickness, false);
            break;
        case DrawMode::Semi:
            _drawPolygon(_vertices.data(), n - 1, fillColor, borderColor, true, thickness, false);
            break;
        default:
            break;
        }
    }
}

axstd::pod_vector<Vec2> DrawNode::_transform(const Vec2* _vertices, unsigned int& count, bool closedPolygon)
{
    Vec2 vert0        = _vertices[0];
    int closedCounter = 0;

    if (closedPolygon && _vertices[0] != _vertices[count - 1])
    {
        closedCounter = 1;
    }

    axstd::pod_vector<Vec2> vert(count + closedCounter);
    if (properties.transform == false)
    {
        memcpy(vert.data(), _vertices, count * sizeof(Vec2));
        if (closedCounter)
        {
            vert[count++] = vert0;
        }
        return vert;
    }

    applyTransform(_vertices, vert.data(), count);

    if (closedCounter)
    {
        vert[count++] = vert[0];
    }

    return vert;
}

void DrawNode::applyTransform(const Vec2* from, Vec2* to, unsigned int count)
{
    if (properties.transform == false)
        return;

    auto scale = properties.scale;
    auto position = properties.position;

    if (properties.rotation == 0.0f)
    {
        for (unsigned int i = 0; i < count; i++)
        {
            to[i].x = from[i].x * scale.x + position.x;
            to[i].y = from[i].y * scale.y + position.y;
        }
    }
    else
    {
        const float sinRot = sin(properties.rotation);
        const float cosRot = cos(properties.rotation);
        auto center = properties.center;

        // https://stackoverflow.com/questions/2259476/rotating-a-point-about-another-point-2d
        for (unsigned int i = 0; i < count; i++)
        {
            // translate point to origin
            float x = from[i].x - center.x;
            float y = from[i].y - center.y;

            // rotate point
            float rx = x * cosRot - y * sinRot;
            float ry = x * sinRot + y * cosRot;

            // translate point back
            x = rx + center.x;
            y = ry + center.y;

            // scale and position
            to[i].x = x * scale.x + position.x;
            to[i].y = y * scale.y + position.y;
        }
    }
}

#if defined(_WIN32)
#    pragma pop_macro("TRANSPARENT")
#endif
}  // namespace ax
