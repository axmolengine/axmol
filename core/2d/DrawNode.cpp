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
#include <stddef.h>  // offsetof
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

NS_AX_BEGIN

static inline Tex2F v2ToTex2F(const Vec2& v)
{
    return {v.x, v.y};
}

/** Is a polygon convex?
 * @param verts A pointer to point coordinates.
 * @param count The number of verts measured in points.
 */
inline bool isConvex(const Vec2* verts, int count)
{
    bool isPositive = false, isNegative = false;
    for (int i = 0; i < count; i++)
    {
        Vec2 A = verts[i];
        Vec2 B = verts[(i + 1) % count];
        Vec2 C = verts[(i + 2) % count];

        double crossProduct = (B.x - A.x) * (C.y - B.y) - (B.y - A.y) * (C.x - B.x);

        if (crossProduct > 0)
        {
            isPositive = true;
        }
        else if (crossProduct < 0)
        {
            isNegative = true;
        }

        if (isPositive && isNegative)
            return false;  // Polygon is concave
    }

    return true;  // Polygon is convex
}

// implementation of DrawNode

DrawNode::DrawNode(float lineWidth) : _lineWidth(lineWidth), _defaultLineWidth(lineWidth), _isConvex(false)
{
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
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
    AX_SAFE_FREE(_bufferTriangle);
    AX_SAFE_FREE(_bufferPoint);
    AX_SAFE_FREE(_bufferLine);

    freeShaderInternal(_customCommandTriangle);
    freeShaderInternal(_customCommandPoint);
    freeShaderInternal(_customCommandLine);
}

DrawNode* DrawNode::create(float defaultLineWidth)
{
    DrawNode* ret = new DrawNode(defaultLineWidth);
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

void DrawNode::ensureCapacity(int count)
{
    AXASSERT(count >= 0, "capacity must be >= 0");

    if (_bufferCountTriangle + count > _bufferCapacityTriangle)
    {
        _bufferCapacityTriangle += MAX(_bufferCapacityTriangle, count);
        _bufferTriangle = (V2F_C4B_T2F*)realloc(_bufferTriangle, _bufferCapacityTriangle * sizeof(V2F_C4B_T2F));

        _customCommandTriangle.createVertexBuffer(sizeof(V2F_C4B_T2F), _bufferCapacityTriangle,
                                                  CustomCommand::BufferUsage::STATIC);
        _customCommandTriangle.updateVertexBuffer(_bufferTriangle, _bufferCapacityTriangle * sizeof(V2F_C4B_T2F));
    }
}

void DrawNode::ensureCapacityGLPoint(int count)
{
    AXASSERT(count >= 0, "capacity must be >= 0");

    if (_bufferCountPoint + count > _bufferCapacityPoint)
    {
        _bufferCapacityPoint += MAX(_bufferCapacityPoint, count);
        _bufferPoint = (V2F_C4B_T2F*)realloc(_bufferPoint, _bufferCapacityPoint * sizeof(V2F_C4B_T2F));

        _customCommandPoint.createVertexBuffer(sizeof(V2F_C4B_T2F), _bufferCapacityPoint,
                                               CustomCommand::BufferUsage::STATIC);
        _customCommandPoint.updateVertexBuffer(_bufferPoint, _bufferCapacityPoint * sizeof(V2F_C4B_T2F));
    }
}

void DrawNode::ensureCapacityGLLine(int count)
{
    AXASSERT(count >= 0, "capacity must be >= 0");

    if (_bufferCountLine + count > _bufferCapacityLine)
    {
        _bufferCapacityLine += MAX(_bufferCapacityLine, count);
        _bufferLine = (V2F_C4B_T2F*)realloc(_bufferLine, _bufferCapacityLine * sizeof(V2F_C4B_T2F));

        _customCommandLine.createVertexBuffer(sizeof(V2F_C4B_T2F), _bufferCapacityLine,
                                              CustomCommand::BufferUsage::STATIC);
        _customCommandLine.updateVertexBuffer(_bufferLine, _bufferCapacityLine * sizeof(V2F_C4B_T2F));
    }
}

bool DrawNode::init()
{
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
    updateShader();
    ensureCapacity(512);
    ensureCapacityGLPoint(64);
    ensureCapacityGLLine(256);

    _dirtyTriangle = true;
    _dirtyLine     = true;
    _dirtyPoint    = true;

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
    if (_bufferCountTriangle)
    {
        updateBlendState(_customCommandTriangle);
        updateUniforms(transform, _customCommandTriangle);
        _customCommandTriangle.init(_globalZOrder);
        renderer->addCommand(&_customCommandTriangle);
    }

    if (_bufferCountPoint)
    {
        updateBlendState(_customCommandPoint);
        updateUniforms(transform, _customCommandPoint);
        _customCommandPoint.init(_globalZOrder);
        renderer->addCommand(&_customCommandPoint);
    }

    if (_bufferCountLine)
    {
        updateBlendState(_customCommandLine);
        updateUniforms(transform, _customCommandLine);
        _customCommandLine.init(_globalZOrder);
        renderer->addCommand(&_customCommandLine);
    }
}

void DrawNode::drawPoint(const Vec2& position, const float pointSize, const Color4B& color)
{
    ensureCapacityGLPoint(1);

    V2F_C4B_T2F* point = _bufferPoint + _bufferCountPoint;
    *point             = {position, color, Tex2F(pointSize, 0)};

    _customCommandPoint.updateVertexBuffer(point, _bufferCountPoint * sizeof(V2F_C4B_T2F), sizeof(V2F_C4B_T2F));
    _bufferCountPoint += 1;
    _dirtyPoint = true;
    _customCommandPoint.setVertexDrawInfo(0, _bufferCountPoint);
}

void DrawNode::drawPoints(const Vec2* position, unsigned int numberOfPoints, const Color4B& color)
{
    drawPoints(position, numberOfPoints, 1.0, color);
}

void DrawNode::drawPoints(const Vec2* position,
                          unsigned int numberOfPoints,
                          const float pointSize,
                          const Color4B& color)
{
    ensureCapacityGLPoint(numberOfPoints);

    V2F_C4B_T2F* point = _bufferPoint + _bufferCountPoint;
    for (unsigned int i = 0; i < numberOfPoints; i++)
    {
        *(point + i) = {position[i], color, Tex2F(pointSize, 0)};
    }

    _customCommandPoint.updateVertexBuffer(point, _bufferCountPoint * sizeof(V2F_C4B_T2F),
                                           numberOfPoints * sizeof(V2F_C4B_T2F));
    _bufferCountPoint += numberOfPoints;
    _dirtyPoint = true;
    _customCommandPoint.setVertexDrawInfo(0, _bufferCountPoint);
}

void DrawNode::drawLine(const Vec2& origin, const Vec2& destination, const Color4B& color)
{
    ensureCapacityGLLine(2);

    V2F_C4B_T2F* point = _bufferLine + _bufferCountLine;

    *point       = {origin, color, Tex2F(0.0, 0.0)};
    *(point + 1) = {destination, color, Tex2F(0.0, 0.0)};

    _customCommandLine.updateVertexBuffer(point, _bufferCountLine * sizeof(V2F_C4B_T2F), 2 * sizeof(V2F_C4B_T2F));
    _bufferCountLine += 2;
    _dirtyLine = true;
    _customCommandLine.setVertexDrawInfo(0, _bufferCountLine);
}

void DrawNode::drawRect(const Vec2& origin, const Vec2& destination, const Color4B& color)
{
    drawLine(origin, Vec2(destination.x, origin.y), color);
    drawLine(Vec2(destination.x, origin.y), destination, color);
    drawLine(destination, Vec2(origin.x, destination.y), color);
    drawLine(Vec2(origin.x, destination.y), origin, color);
}

void DrawNode::drawPoly(const Vec2* poli, unsigned int numberOfPoints, bool closePolygon, const Color4B& color)
{
    unsigned int vertex_count;
    if (closePolygon)
    {
        vertex_count = 2 * numberOfPoints;
        ensureCapacityGLLine(vertex_count);
    }
    else
    {
        vertex_count = 2 * (numberOfPoints - 1);
        ensureCapacityGLLine(vertex_count);
    }

    V2F_C4B_T2F* point  = _bufferLine + _bufferCountLine;
    V2F_C4B_T2F* cursor = point;

    unsigned int i = 0;
    for (; i < numberOfPoints - 1; i++)
    {
        *point       = {poli[i], color, Tex2F(0.0, 0.0)};
        *(point + 1) = {poli[i + 1], color, Tex2F(0.0, 0.0)};
        point += 2;
    }
    if (closePolygon)
    {
        *point       = {poli[i], color, Tex2F(0.0, 0.0)};
        *(point + 1) = {poli[0], color, Tex2F(0.0, 0.0)};
    }

    _customCommandLine.updateVertexBuffer(cursor, _bufferCountLine * sizeof(V2F_C4B_T2F),
                                          vertex_count * sizeof(V2F_C4B_T2F));
    _bufferCountLine += vertex_count;
    _customCommandLine.setVertexDrawInfo(0, _bufferCountLine);
}

void DrawNode::drawCircle(const Vec2& center,
                          float radius,
                          float angle,
                          unsigned int segments,
                          bool drawLineToCenter,
                          float scaleX,
                          float scaleY,
                          const Color4B& color,
                          float threshold)
{
    const float coef = 2.0f * (float)M_PI / segments;

    auto vertices = _abuf.get<Vec2>(segments + 2);

    for (unsigned int i = 0; i <= segments; i++)
    {
        float rads    = i * coef;
        vertices[i].x = radius * cosf(rads + angle) * scaleX + center.x;
        vertices[i].y = radius * sinf(rads + angle) * scaleY + center.y;
    }
    if (_lineWidth > threshold)
    {
        _isConvex = true;
        drawPolygon(vertices, segments, Color4B(1.0f, 0.0f, 0.0f, 1.0f), _lineWidth/4, color);
        _isConvex = false;
    }
    else
    {
        if (drawLineToCenter)
        {
            vertices[segments + 1] = center;
            drawPoly(vertices, segments + 2, true, color);
        }
        else
            drawPoly(vertices, segments + 1, true, color);
    }
}

void DrawNode::drawCircle(const Vec2& center,
                          float radius,
                          float angle,
                          unsigned int segments,
                          bool drawLineToCenter,
                          const Color4B& color,
                          float threshold)
{
    drawCircle(center, radius, angle, segments, drawLineToCenter, 1.0f, 1.0f, color, threshold);
}

void DrawNode::drawQuadBezier(const Vec2& origin,
                              const Vec2& control,
                              const Vec2& destination,
                              unsigned int segments,
                              const Color4B& color)
{
    Vec2* vertices = _abuf.get<Vec2>(segments + 1);

    float t = 0.0f;
    for (unsigned int i = 0; i < segments; i++)
    {
        vertices[i].x = powf(1 - t, 2) * origin.x + 2.0f * (1 - t) * t * control.x + t * t * destination.x;
        vertices[i].y = powf(1 - t, 2) * origin.y + 2.0f * (1 - t) * t * control.y + t * t * destination.y;
        t += 1.0f / segments;
    }
    vertices[segments].x = destination.x;
    vertices[segments].y = destination.y;

    drawPoly(vertices, segments + 1, false, color);
}

void DrawNode::drawCubicBezier(const Vec2& origin,
                               const Vec2& control1,
                               const Vec2& control2,
                               const Vec2& destination,
                               unsigned int segments,
                               const Color4B& color)
{
    Vec2* vertices = _abuf.get<Vec2>(segments + 1);

    float t = 0;
    for (unsigned int i = 0; i < segments; i++)
    {
        vertices[i].x = powf(1 - t, 3) * origin.x + 3.0f * powf(1 - t, 2) * t * control1.x +
                        3.0f * (1 - t) * t * t * control2.x + t * t * t * destination.x;
        vertices[i].y = powf(1 - t, 3) * origin.y + 3.0f * powf(1 - t, 2) * t * control1.y +
                        3.0f * (1 - t) * t * t * control2.y + t * t * t * destination.y;
        t += 1.0f / segments;
    }
    vertices[segments].x = destination.x;
    vertices[segments].y = destination.y;

    drawPoly(vertices, segments + 1, false, color);
}

void DrawNode::drawCardinalSpline(PointArray* config, float tension, unsigned int segments, const Color4B& color)
{
    Vec2* vertices = _abuf.get<Vec2>(segments + 1);

    ssize_t p;
    float lt;
    float deltaT = 1.0f / config->count();

    for (unsigned int i = 0; i < segments + 1; i++)
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

        Vec2 newPos   = ccCardinalSplineAt(pp0, pp1, pp2, pp3, tension, lt);
        vertices[i].x = newPos.x;
        vertices[i].y = newPos.y;
    }

    drawPoly(vertices, segments + 1, false, color);
}

void DrawNode::drawCatmullRom(PointArray* points, unsigned int segments, const Color4B& color)
{
    drawCardinalSpline(points, 0.5f, segments, color);
}

void DrawNode::drawDot(const Vec2& pos, float radius, const Color4B& color)
{
    unsigned int vertex_count = 2 * 3;
    ensureCapacity(vertex_count);

    V2F_C4B_T2F a = {Vec2(pos.x - radius, pos.y - radius), color, Tex2F(-1.0, -1.0)};
    V2F_C4B_T2F b = {Vec2(pos.x - radius, pos.y + radius), color, Tex2F(-1.0, 1.0)};
    V2F_C4B_T2F c = {Vec2(pos.x + radius, pos.y + radius), color, Tex2F(1.0, 1.0)};
    V2F_C4B_T2F d = {Vec2(pos.x + radius, pos.y - radius), color, Tex2F(1.0, -1.0)};

    V2F_C4B_T2F_Triangle* triangles = (V2F_C4B_T2F_Triangle*)(_bufferTriangle + _bufferCountTriangle);
    V2F_C4B_T2F_Triangle triangle0  = {a, b, c};
    V2F_C4B_T2F_Triangle triangle1  = {a, c, d};
    triangles[0]                    = triangle0;
    triangles[1]                    = triangle1;

    _customCommandTriangle.updateVertexBuffer(triangles, _bufferCountTriangle * sizeof(V2F_C4B_T2F),
                                              vertex_count * sizeof(V2F_C4B_T2F));
    _bufferCountTriangle += vertex_count;
    _dirtyTriangle = true;
    _customCommandTriangle.setVertexDrawInfo(0, _bufferCountTriangle);
}

void DrawNode::drawRect(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec2& p4, const Color4B& color)
{
    drawLine(p1, p2, color);
    drawLine(p2, p3, color);
    drawLine(p3, p4, color);
    drawLine(p4, p1, color);
}

void DrawNode::drawSegment(const Vec2& from, const Vec2& to, float radius, const Color4B& color)
{
    unsigned int vertex_count = 6 * 3;
    ensureCapacity(vertex_count);

    Vec2 a = from;
    Vec2 b = to;

    Vec2 n = ((b - a).getPerp()).getNormalized();
    Vec2 t = n.getPerp();

    Vec2 nw = n * radius;
    Vec2 tw = t * radius;
    Vec2 v0 = b - (nw + tw);
    Vec2 v1 = b + (nw - tw);
    Vec2 v2 = b - nw;
    Vec2 v3 = b + nw;
    Vec2 v4 = a - nw;
    Vec2 v5 = a + nw;
    Vec2 v6 = a - (nw - tw);
    Vec2 v7 = a + (nw + tw);

    V2F_C4B_T2F_Triangle* triangles = (V2F_C4B_T2F_Triangle*)(_bufferTriangle + _bufferCountTriangle);

    V2F_C4B_T2F_Triangle triangles0 = {
        {v0, color, v2ToTex2F(-(n + t))},
        {v1, color, v2ToTex2F(n - t)},
        {v2, color, v2ToTex2F(-n)},
    };
    triangles[0] = triangles0;

    V2F_C4B_T2F_Triangle triangles1 = {
        {v3, color, v2ToTex2F(n)},
        {v1, color, v2ToTex2F(n - t)},
        {v2, color, v2ToTex2F(-n)},
    };
    triangles[1] = triangles1;

    V2F_C4B_T2F_Triangle triangles2 = {
        {v3, color, v2ToTex2F(n)},
        {v4, color, v2ToTex2F(-n)},
        {v2, color, v2ToTex2F(-n)},
    };
    triangles[2] = triangles2;

    V2F_C4B_T2F_Triangle triangles3 = {
        {v3, color, v2ToTex2F(n)},
        {v4, color, v2ToTex2F(-n)},
        {v5, color, v2ToTex2F(n)},
    };
    triangles[3] = triangles3;

    V2F_C4B_T2F_Triangle triangles4 = {
        {v6, color, v2ToTex2F(t - n)},
        {v4, color, v2ToTex2F(-n)},
        {v5, color, v2ToTex2F(n)},
    };
    triangles[4] = triangles4;

    V2F_C4B_T2F_Triangle triangles5 = {
        {v6, color, v2ToTex2F(t - n)},
        {v7, color, v2ToTex2F(t + n)},
        {v5, color, v2ToTex2F(n)},
    };
    triangles[5] = triangles5;

    _customCommandTriangle.updateVertexBuffer(triangles, _bufferCountTriangle * sizeof(V2F_C4B_T2F),
                                              vertex_count * sizeof(V2F_C4B_T2F));
    _bufferCountTriangle += vertex_count;
    _dirtyTriangle = true;
    _customCommandTriangle.setVertexDrawInfo(0, _bufferCountTriangle);
}

void DrawNode::drawPolygon(const Vec2* verts,
                           int count,
                           const Color4B& fillColor,
                           float borderWidth,
                           const Color4B& borderColor)
{
    AXASSERT(count >= 0, "invalid count value");

    bool outline = (borderColor.a > 0.0f && borderWidth > 0.0f);

    auto triangle_count = outline ? (3 * count - 2) : (count - 2);
    auto vertex_count   = 3 * triangle_count;
    ensureCapacity(vertex_count);

    V2F_C4B_T2F_Triangle* triangles = (V2F_C4B_T2F_Triangle*)(_bufferTriangle + _bufferCountTriangle);
    V2F_C4B_T2F_Triangle* cursor    = triangles;

    if (!_isConvex && count >= 3 && !isConvex(verts, count))
    {
        std::vector<p2t::Point> p2pointsStorage;
        p2pointsStorage.reserve(count);
        std::vector<p2t::Point*> p2points;
        p2points.reserve(count);

        for (int i = 0; i < count; ++i)
        {
            p2points.emplace_back(&p2pointsStorage.emplace_back((double)verts[i].x, (double)verts[i].y));
        }
        p2t::CDT cdt(p2points);
        cdt.Triangulate();
        std::vector<p2t::Triangle*> tris = cdt.GetTriangles();

        if ((tris.size() * 3) > vertex_count)
        {
            ensureCapacity(static_cast<int>(tris.size() * 3));
            triangles = (V2F_C4B_T2F_Triangle*)(_bufferTriangle + _bufferCountTriangle);
            cursor    = triangles;
        }

        for (auto&& t : tris)
        {
            p2t::Point* vec1 = t->GetPoint(0);
            p2t::Point* vec2 = t->GetPoint(1);
            p2t::Point* vec3 = t->GetPoint(2);

            V2F_C4B_T2F_Triangle tmp = {
                {Vec2(vec1->x, vec1->y), fillColor, Tex2F(0.0, 0.0)},
                {Vec2(vec2->x, vec2->y), fillColor, Tex2F(0.0, 0.0)},
                {Vec2(vec3->x, vec3->y), fillColor, Tex2F(0.0, 0.0)},
            };
            *cursor++ = tmp;
        }
    }
    else
    {
        for (int i = 0; i < count - 2; i++)
        {
            V2F_C4B_T2F_Triangle tmp = {
                {verts[0], fillColor, v2ToTex2F(Vec2::ZERO)},
                {verts[i + 1], fillColor, v2ToTex2F(Vec2::ZERO)},
                {verts[i + 2], fillColor, v2ToTex2F(Vec2::ZERO)},
            };

            *cursor++ = tmp;
        }
    }
    if (outline)
    {
        struct ExtrudeVerts
        {
            Vec2 offset, n;
        };
        struct ExtrudeVerts* extrude = (struct ExtrudeVerts*)malloc(sizeof(struct ExtrudeVerts) * count);

        for (int i = 0; i < count; i++)
        {
            Vec2 v0 = verts[(i - 1 + count) % count];
            Vec2 v1 = verts[i];
            Vec2 v2 = verts[(i + 1) % count];

            Vec2 n1 = ((v1 - v0).getPerp()).getNormalized();
            Vec2 n2 = ((v2 - v1).getPerp()).getNormalized();

            Vec2 offset = (n1 + n2) * (1.0f / (Vec2::dot(n1, n2) + 1.0f));
            extrude[i]  = {offset, n2};
        }

        for (int i = 0; i < count; i++)
        {
            int j   = (i + 1) % count;
            Vec2 v0 = verts[i];
            Vec2 v1 = verts[j];

            Vec2 n0 = extrude[i].n;

            Vec2 offset0 = extrude[i].offset;
            Vec2 offset1 = extrude[j].offset;

            Vec2 inner0 = v0 - offset0 * borderWidth;
            Vec2 inner1 = v1 - offset1 * borderWidth;
            Vec2 outer0 = v0 + offset0 * borderWidth;
            Vec2 outer1 = v1 + offset1 * borderWidth;

            V2F_C4B_T2F_Triangle tmp1 = {{inner0, borderColor, v2ToTex2F(-n0)},
                                         {inner1, borderColor, v2ToTex2F(-n0)},
                                         {outer1, borderColor, v2ToTex2F(n0)}};
            *cursor++                 = tmp1;

            V2F_C4B_T2F_Triangle tmp2 = {{inner0, borderColor, v2ToTex2F(-n0)},
                                         {outer0, borderColor, v2ToTex2F(n0)},
                                         {outer1, borderColor, v2ToTex2F(n0)}};
            *cursor++                 = tmp2;
        }

        free(extrude);
    }

    _customCommandTriangle.updateVertexBuffer(triangles, _bufferCountTriangle * sizeof(V2F_C4B_T2F),
                                              vertex_count * sizeof(V2F_C4B_T2F));
    _bufferCountTriangle += vertex_count;
    _customCommandTriangle.setVertexDrawInfo(0, _bufferCountTriangle);
    _dirtyTriangle = true;
}

void DrawNode::drawSolidRect(const Vec2& origin, const Vec2& destination, const Color4B& color)
{
    Vec2 vertices[] = {origin, Vec2(destination.x, origin.y), destination, Vec2(origin.x, destination.y)};
    _isConvex = true;  // Fix issue #1546 of UILayout(#1549)
    drawSolidPoly(vertices, 4, color);
    _isConvex = false;
}

void DrawNode::drawSolidPoly(const Vec2* poli, unsigned int numberOfPoints, const Color4B& color)
{
    drawPolygon(poli, numberOfPoints, color, 0.0, Color4B());
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
    // not a real line!
    if (startAngle == endAngle)
        return;

#define DEGREES 360

    const float coef = 2.0f * (float)M_PI / DEGREES;
    Vec2* vertices   = _abuf.get<Vec2>(DEGREES + 2);

    int n        = 0;
    float rads   = 0;
    float _angle = AX_DEGREES_TO_RADIANS(angle);

    if (startAngle > endAngle)
    {
        int tmp    = endAngle;
        endAngle   = startAngle;
        startAngle = tmp;
    }

    for (int i = 0; i <= DEGREES; i++)
    {
        if (startAngle <= i && endAngle >= i)
        {
            rads = i * coef;

            float j = radius * cosf(rads + _angle) * scaleX + center.x;
            float k = radius * sinf(rads + _angle) * scaleY + center.y;

            vertices[n].x = j;
            vertices[n].y = k;
            n++;
        }
    }
    switch (drawMode)
    {
    case DrawMode::Fill:
        vertices[n++] = center;
        drawSolidPoly(vertices, n, color);
        break;
    case DrawMode::Outline:
        vertices[n++] = center;
        drawPoly(vertices, n, true, color);
        break;
    case DrawMode::Line:
        drawPoly(vertices, n, false, color);
        break;
    case DrawMode::Semi:
        drawPoly(vertices, n, true, color);
        break;

    default:
        break;
    }
}

void DrawNode::drawSolidCircle(const Vec2& center,
                               float radius,
                               float angle,
                               unsigned int segments,
                               float scaleX,
                               float scaleY,
                               const Color4B& fillColor,
                               float borderWidth,
                               const Color4B& borderColor)
{
    const float coef = 2.0f * (float)M_PI / segments;

    Vec2* vertices = _abuf.get<Vec2>(segments);

    for (unsigned int i = 0; i < segments; i++)
    {
        float rads = i * coef;
        float j    = radius * cosf(rads + angle) * scaleX + center.x;
        float k    = radius * sinf(rads + angle) * scaleY + center.y;

        vertices[i].x = j;
        vertices[i].y = k;
    }
    _isConvex = true;
    drawPolygon(vertices, segments, fillColor, borderWidth, borderColor);
    _isConvex = false;
}

void DrawNode::drawSolidCircle(const Vec2& center,
                               float radius,
                               float angle,
                               unsigned int segments,
                               float scaleX,
                               float scaleY,
                               const Color4B& color)
{
    const float coef = 2.0f * (float)M_PI / segments;

    Vec2* vertices = _abuf.get<Vec2>(segments);

    for (unsigned int i = 0; i < segments; i++)
    {
        float rads = i * coef;
        float j    = radius * cosf(rads + angle) * scaleX + center.x;
        float k    = radius * sinf(rads + angle) * scaleY + center.y;

        vertices[i].x = j;
        vertices[i].y = k;
    }

    drawSolidPoly(vertices, segments, color);
}

void DrawNode::drawSolidCircle(const Vec2& center,
                               float radius,
                               float angle,
                               unsigned int segments,
                               const Color4B& color)
{
    drawSolidCircle(center, radius, angle, segments, 1.0f, 1.0f, color);
}

void DrawNode::drawTriangle(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Color4B& color)
{
    unsigned int vertex_count = 3;
    ensureCapacity(vertex_count);

    V2F_C4B_T2F a = {p1, color, Tex2F(0.0, 0.0)};
    V2F_C4B_T2F b = {p2, color, Tex2F(0.0, 0.0)};
    V2F_C4B_T2F c = {p3, color, Tex2F(0.0, 0.0)};

    V2F_C4B_T2F_Triangle* triangles = (V2F_C4B_T2F_Triangle*)(_bufferTriangle + _bufferCountTriangle);
    V2F_C4B_T2F_Triangle triangle   = {a, b, c};
    triangles[0]                    = triangle;

    _customCommandTriangle.updateVertexBuffer(triangles, _bufferCountTriangle * sizeof(V2F_C4B_T2F),
                                              vertex_count * sizeof(V2F_C4B_T2F));
    _bufferCountTriangle += vertex_count;
    _dirtyTriangle = true;
    _customCommandTriangle.setVertexDrawInfo(0, _bufferCountTriangle);
}

void DrawNode::clear()
{
    _bufferCountTriangle = 0;
    _dirtyTriangle       = true;
    _bufferCountLine     = 0;
    _dirtyLine           = true;
    _bufferCountPoint    = 0;
    _dirtyPoint          = true;
    _lineWidth           = _defaultLineWidth;
}

const BlendFunc& DrawNode::getBlendFunc() const
{
    return _blendFunc;
}

void DrawNode::setBlendFunc(const BlendFunc& blendFunc)
{
    _blendFunc = blendFunc;
}

void DrawNode::setLineWidth(float lineWidth)
{
    _lineWidth = lineWidth;
}

float DrawNode::getLineWidth()
{
    return this->_lineWidth;
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

NS_AX_END
