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

#include "DrawNodeEx/DrawNodeEx.h"
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


#if defined(_WIN32)
#    pragma push_macro("TRANSPARENT")
#    undef TRANSPARENT
#endif

NS_AX_EXT_BEGIN

/** Is a polygon convex?
* @param verts A pointer to point coordinates.
* @param count The number of verts measured in points.
*/
static bool isConvex(const Vec2* verts, int count)
{
    bool isPositive = false, isNegative = false;
    for (unsigned int i = 0; i < count; i++)
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

Vec2* DrawNodeEx::transform(const Vec2* vertices, unsigned int count)
{
    Vec2* vert = _abuf.get<Vec2>(count);
    if (_dnTransform == false)
    {
        memcpy(vert, vertices, count);
        return vert;
    }

    const float s = sin(_dnRotation);
    const float c = cos(_dnRotation);

    for (unsigned int i = 0; i < count; i++)
    {
        if (_dnRotation == 0.0f)
        {
            vert[i].x = vertices[i].x * _dnScale.x + _dnPosition.x;
            vert[i].y = vertices[i].y * _dnScale.y + _dnPosition.y;
        }
        else  // https://stackoverflow.com/questions/2259476/rotating-a-point-about-another-point-2d
        {

            // translate point back to origin:
            vert[i].x = vertices[i].x - _dnCenter.x;
            vert[i].y = vertices[i].y - _dnCenter.y;

            // rotate point
            float xnew = vert[i].x * c - vert[i].y * s;
            float ynew = vert[i].x * s + vert[i].y * c;

            // translate point back:
            vert[i].x = xnew + _dnCenter.x;
            vert[i].y = ynew + _dnCenter.y;

            // scale and position
            vert[i].x = vert[i].x * _dnScale.x + _dnPosition.x;
            vert[i].y = vert[i].y * _dnScale.y + _dnPosition.y;
        }
    }
    return vert;
}

// implementation of DrawNodeEx
DrawNodeEx::DrawNodeEx(float lineWidth)
    : _lineWidth(lineWidth)
    , _defaultLineWidth(lineWidth)
    , _isConvex(false)
    , _dnPosition(Vec2::ZERO)
    , _dnRotation(0.0f)
    , _dnScale(Vec2::ONE)
    , _dnTransform(true)
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

DrawNodeEx::~DrawNodeEx()
{
    AX_SAFE_FREE(_bufferTriangle);
    AX_SAFE_FREE(_bufferPoint);
    AX_SAFE_FREE(_bufferLine);

    freeShaderInternal(_customCommandTriangle);
    freeShaderInternal(_customCommandPoint);
    freeShaderInternal(_customCommandLine);
}

DrawNodeEx* DrawNodeEx::create(float defaultLineWidth)
{
    DrawNodeEx* ret = new DrawNodeEx(defaultLineWidth);
    if (ret->init())
    {
        ret->autorelease();
        ret->_dnVersion = Version::v2;
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

void DrawNodeEx::ensureCapacityTriangle(int count)
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

void DrawNodeEx::ensureCapacityPoint(int count)
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

void DrawNodeEx::ensureCapacityLine(int count)
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

bool DrawNodeEx::init()
{
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
    updateShader();
    ensureCapacityTriangle(512);
    ensureCapacityPoint(64);
    ensureCapacityLine(256);

    _dirtyTriangle = true;
    _dirtyLine = true;
    _dirtyPoint = true;

    return true;
}

void DrawNodeEx::updateShader()
{
    updateShaderInternal(_customCommandTriangle, backend::ProgramType::POSITION_COLOR_LENGTH_TEXTURE,
        CustomCommand::DrawType::ARRAY, CustomCommand::PrimitiveType::TRIANGLE);

    updateShaderInternal(_customCommandPoint, backend::ProgramType::POSITION_COLOR_TEXTURE_AS_POINTSIZE,
        CustomCommand::DrawType::ARRAY, CustomCommand::PrimitiveType::POINT);

    updateShaderInternal(_customCommandLine, backend::ProgramType::POSITION_COLOR_LENGTH_TEXTURE,
        CustomCommand::DrawType::ARRAY, CustomCommand::PrimitiveType::LINE);
}

void DrawNodeEx::updateShaderInternal(CustomCommand& cmd,
    uint32_t programType,
    CustomCommand::DrawType drawType,
    CustomCommand::PrimitiveType primitiveType)
{
    auto& pipelinePS = cmd.getPipelineDescriptor().programState;
    AX_SAFE_RELEASE(pipelinePS);

    auto program = backend::Program::getBuiltinProgram(programType);
    pipelinePS = new backend::ProgramState(program);
    setVertexLayout(cmd);
    cmd.setPrimitiveType(primitiveType);
    cmd.setDrawType(drawType);
}

void DrawNodeEx::setVertexLayout(CustomCommand& cmd)
{
    auto* programState = cmd.getPipelineDescriptor().programState;
    programState->validateSharedVertexLayout(backend::VertexLayoutType::DrawNode);
}

void DrawNodeEx::freeShaderInternal(CustomCommand& cmd)
{
    auto& pipelinePS = cmd.getPipelineDescriptor().programState;
    AX_SAFE_RELEASE_NULL(pipelinePS);
}

void DrawNodeEx::updateBlendState(CustomCommand& cmd)
{
    backend::BlendDescriptor& blendDescriptor = cmd.getPipelineDescriptor().blendDescriptor;
    blendDescriptor.blendEnabled = true;
    if (_blendFunc == BlendFunc::ALPHA_NON_PREMULTIPLIED)
    {
        blendDescriptor.sourceRGBBlendFactor = backend::BlendFactor::SRC_ALPHA;
        blendDescriptor.destinationRGBBlendFactor = backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
        blendDescriptor.sourceAlphaBlendFactor = backend::BlendFactor::SRC_ALPHA;
        blendDescriptor.destinationAlphaBlendFactor = backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
        setOpacityModifyRGB(false);
    }
    else
    {
        blendDescriptor.sourceRGBBlendFactor = backend::BlendFactor::ONE;
        blendDescriptor.destinationRGBBlendFactor = backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
        blendDescriptor.sourceAlphaBlendFactor = backend::BlendFactor::ONE;
        blendDescriptor.destinationAlphaBlendFactor = backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
        setOpacityModifyRGB(true);
    }
}

void DrawNodeEx::updateUniforms(const Mat4& transform, CustomCommand& cmd)
{
    auto& pipelineDescriptor = cmd.getPipelineDescriptor();
    const auto& matrixP = _director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    Mat4 matrixMVP = matrixP * transform;
    auto mvpLocation = pipelineDescriptor.programState->getUniformLocation("u_MVPMatrix");
    pipelineDescriptor.programState->setUniform(mvpLocation, matrixMVP.m, sizeof(matrixMVP.m));

    float alpha = _displayedOpacity / 255.0f;
    auto alphaUniformLocation = pipelineDescriptor.programState->getUniformLocation("u_alpha");
    pipelineDescriptor.programState->setUniform(alphaUniformLocation, &alpha, sizeof(alpha));
}

void DrawNodeEx::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
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

void DrawNodeEx::drawPoint(const Vec2& position, const float pointSize, const Color4B& color)
{
    ensureCapacityPoint(1);

    V2F_C4B_T2F* point = _bufferPoint + _bufferCountPoint;
    *point = { position, color, Tex2F(pointSize, 0) };

    _customCommandPoint.updateVertexBuffer(point, _bufferCountPoint * sizeof(V2F_C4B_T2F), sizeof(V2F_C4B_T2F));
    _bufferCountPoint += 1;
    _dirtyPoint = true;
    _customCommandPoint.setVertexDrawInfo(0, _bufferCountPoint);
}

void DrawNodeEx::drawPoints(const Vec2* position, unsigned int numberOfPoints, const Color4B& color)
{
    drawPoints(position, numberOfPoints, 1.f, color);
}

void DrawNodeEx::drawPoints(const Vec2* position,
    unsigned int numberOfPoints,
    const float pointSize,
    const Color4B& color)
{
    ensureCapacityPoint(numberOfPoints);

    V2F_C4B_T2F* point = _bufferPoint + _bufferCountPoint;
    for (unsigned int i = 0; i < numberOfPoints; i++)
    {
        *(point + i) = { position[i], color, Tex2F(pointSize, 0) };
    }

    _customCommandPoint.updateVertexBuffer(point, _bufferCountPoint * sizeof(V2F_C4B_T2F),
        numberOfPoints * sizeof(V2F_C4B_T2F));
    _bufferCountPoint += numberOfPoints;
    _dirtyPoint = true;
    _customCommandPoint.setVertexDrawInfo(0, _bufferCountPoint);
}

void DrawNodeEx::drawLine(const Vec2& origin, const Vec2& destination, const Color4B& color, float thickness)
{
    if (thickness != 1.0f)
    {
        drawSegment(origin, destination, thickness, color);
        return;
    }
    else
    {
        Vec2 line[2] = { origin, destination };
        Vec2* _vertices = transform(line, 2);

        ensureCapacityLine(2);

        V2F_C4B_T2F* point = _bufferLine + _bufferCountLine;

        *point = { _vertices[0], color, Tex2F::ZERO };
        *(point + 1) = { _vertices[1], color, Tex2F::ZERO };

        _customCommandLine.updateVertexBuffer(point, _bufferCountLine * sizeof(V2F_C4B_T2F), 2 * sizeof(V2F_C4B_T2F));
        _bufferCountLine += 2;
        _dirtyLine = true;
        _customCommandLine.setVertexDrawInfo(0, _bufferCountLine);
    }
}

void DrawNodeEx::drawRect(const Vec2& origin, const Vec2& destination, const Color4B& color, float thickness)
{
    if (thickness != 1.0f)
    {
        Vec2 line[4] = { {origin}, {Vec2(destination.x, origin.y)}, {destination}, {Vec2(origin.x, destination.y)} };

        swapIsConvex(true);
        _drawPoly(line, 4, true, color, thickness);
        _isConvex = _isConvexTmp;
        return;
    }
    else
    {
        drawLine(origin, Vec2(destination.x, origin.y), color);
        drawLine(Vec2(destination.x, origin.y), destination, color);
        drawLine(destination, Vec2(origin.x, destination.y), color);
        drawLine(Vec2(origin.x, destination.y), origin, color);
    }
}

void DrawNodeEx::drawPoly(const Vec2* poli,
    unsigned int numberOfPoints,
    bool closePolygon,
    const Color4B& color,
    float thickness)
{
    if (thickness != 1.0f)
    {
        _drawPolygon(poli, numberOfPoints, Color4B::TRANSPARENT, thickness, color, closePolygon);
        return;
    }
    else
    {
        Vec2* _vertices = transform(poli, numberOfPoints);

        unsigned int vertex_count;
        if (closePolygon)
        {
            vertex_count = 2 * numberOfPoints;
        }
        else
        {
            vertex_count = 2 * (numberOfPoints - 1);
        }
        ensureCapacityLine(vertex_count);

        V2F_C4B_T2F* point = _bufferLine + _bufferCountLine;
        V2F_C4B_T2F* cursor = point;

        unsigned int i = 0;
        for (; i < numberOfPoints - 1; i++)
        {
            *point = { poli[i], color, Tex2F::ZERO };
            *(point + 1) = { poli[i + 1], color, Tex2F::ZERO };
            point += 2;
        }
        if (closePolygon)
        {
            *point = { poli[i], color, Tex2F::ZERO };
            *(point + 1) = { poli[0], color, Tex2F::ZERO };
        }

        _customCommandLine.updateVertexBuffer(cursor, _bufferCountLine * sizeof(V2F_C4B_T2F),
            vertex_count * sizeof(V2F_C4B_T2F));
        _bufferCountLine += vertex_count;
        _customCommandLine.setVertexDrawInfo(0, _bufferCountLine);
    }
}

void DrawNodeEx::drawCircle(const Vec2& center,
    float radius,
    float angle,
    unsigned int segments,
    bool drawLineToCenter,
    float scaleX,
    float scaleY,
    const Color4B& color,
    float thickness)
{
    const float coef = 2.0f * (float)M_PI / segments;

    auto vertices = _abuf.get<Vec2>(segments + 2);

    for (unsigned int i = 0; i <= segments; i++)
    {
        float rads = i * coef;
        vertices[i].x = radius * cosf(rads + angle) * scaleX + center.x;
        vertices[i].y = radius * sinf(rads + angle) * scaleY + center.y;
    }
    swapIsConvex(false);
    if (thickness != 1.0f)
    {
        _drawPolygon(vertices, segments, Color4B::TRANSPARENT, thickness, color, true);
    }
    else
    {
        if (drawLineToCenter)
        {
            vertices[segments + 1] = center;
            _drawPoly(vertices, segments + 2, true, color, 1.0f);
        }
        else
            _drawPoly(vertices, segments + 1, true, color, thickness);
    }
    _isConvex = _isConvexTmp;
}

void DrawNodeEx::drawCircle(const Vec2& center,
    float radius,
    float angle,
    unsigned int segments,
    bool drawLineToCenter,
    const Color4B& color,
    float thickness)
{
    drawCircle(center, radius, angle, segments, drawLineToCenter, 1.0f, 1.0f, color, thickness);
}

void DrawNodeEx::drawStar(const Vec2& center,
    float radius1,
    float radius2,
    unsigned int segments,
    const Color4B& color,
    float thickness)
{
    const float coef = 2.0f * (float)M_PI / segments;

    float halfAngle = coef / 2.0f;

    auto vertices = _abuf.get<Vec2>(segments * 2);
    int i = 0;
    for (unsigned int a = 0; a < segments; a++)
    {
        float rads = a * coef;
        vertices[i] = Vec2(center.x + cos(rads) * radius2, center.y + sin(rads) * radius2);
        vertices[i + 1] = Vec2(center.x + cos(rads + halfAngle) * radius1, center.y + sin(rads + halfAngle) * radius1);
        i += 2;
    }
    _drawPolygon(vertices, segments * 2, Color4B::TRANSPARENT, thickness, color, true);
}

void DrawNodeEx::drawSolidStar(const Vec2& center,
    float radiusI, // inner 
    float radiusO, // outer
    unsigned int segments,
    const Color4B& color,
    const Color4B& filledColor,
    float thickness)
{
    const float coef = 2.0f * (float)M_PI / segments;

    float halfAngle = coef / 2.0f;

    auto vertices = _abuf.get<Vec2>(segments * 2);
    int i = 0;
    for (unsigned int a = 0; a < segments; a++)
    {
        float rads = a * coef;
        vertices[i] = Vec2(center.x + cos(rads) * radiusO, center.y + sin(rads) * radiusO);
        vertices[i + 1] = Vec2(center.x + cos(rads + halfAngle) * radiusI, center.y + sin(rads + halfAngle) * radiusI);
        i += 2;
    }
    swapIsConvex(false);
    _drawPolygon(vertices, segments * 2, filledColor, thickness, color, true);
    _isConvex = _isConvexTmp;
}

void DrawNodeEx::drawQuadBezier(const Vec2& origin,
    const Vec2& control,
    const Vec2& destination,
    unsigned int segments,
    const Color4B& color,
    float thickness)
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
    swapIsConvex(false);
    _drawPolygon(vertices, segments, Color4B::TRANSPARENT, thickness, color, false);
    _isConvex = _isConvexTmp;
}

void DrawNodeEx::drawCubicBezier(const Vec2& origin,
    const Vec2& control1,
    const Vec2& control2,
    const Vec2& destination,
    unsigned int segments,
    const Color4B& color,
    float thickness)
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
    swapIsConvex(true);
    _drawPolygon(vertices, segments, Color4B::TRANSPARENT, thickness, color, false);
    _isConvex = _isConvexTmp;
}

void DrawNodeEx::drawCardinalSpline(ax::PointArray* config,
    float tension,
    unsigned int segments,
    const Color4B& color,
    float thickness)
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
            p = config->count() - 1;
            lt = 1;
        }
        else
        {
            p = static_cast<ssize_t>(dt / deltaT);
            lt = (dt - deltaT * (float)p) / deltaT;
        }

        // Interpolate
        Vec2 pp0 = config->getControlPointAtIndex(p - 1);
        Vec2 pp1 = config->getControlPointAtIndex(p + 0);
        Vec2 pp2 = config->getControlPointAtIndex(p + 1);
        Vec2 pp3 = config->getControlPointAtIndex(p + 2);

        Vec2 newPos = ccCardinalSplineAt(pp0, pp1, pp2, pp3, tension, lt);
        vertices[i].x = newPos.x;
        vertices[i].y = newPos.y;
    }
    swapIsConvex(true);
    _drawPolygon(vertices, segments, Color4B::TRANSPARENT, thickness, color, false);
    _isConvex = _isConvexTmp;
}

void DrawNodeEx::drawCatmullRom(ax::PointArray* points, unsigned int segments, const Color4B& color, float thickness)
{
    drawCardinalSpline(points, 0.5f, segments, color, thickness);
}

void DrawNodeEx::drawDot(const Vec2& pos, float radius, const Color4B& color)
{
    unsigned int vertex_count = 2 * 3;
    ensureCapacityTriangle(vertex_count);

    V2F_C4B_T2F a = { Vec2(pos.x - radius, pos.y - radius), color, Tex2F(-1.0f, -1.0f) };
    V2F_C4B_T2F b = { Vec2(pos.x - radius, pos.y + radius), color, Tex2F(-1.0f, 1.0f) };
    V2F_C4B_T2F c = { Vec2(pos.x + radius, pos.y + radius), color, Tex2F(1.0f, 1.0f) };
    V2F_C4B_T2F d = { Vec2(pos.x + radius, pos.y - radius), color, Tex2F(1.0f, -1.0f) };

    V2F_C4B_T2F_Triangle* triangles = (V2F_C4B_T2F_Triangle*)(_bufferTriangle + _bufferCountTriangle);
    triangles[0] = { a, b, c };
    triangles[1] = { a, c, d };

    _customCommandTriangle.updateVertexBuffer(triangles, _bufferCountTriangle * sizeof(V2F_C4B_T2F),
        vertex_count * sizeof(V2F_C4B_T2F));
    _bufferCountTriangle += vertex_count;
    _dirtyTriangle = true;
    _customCommandTriangle.setVertexDrawInfo(0, _bufferCountTriangle);
}

void DrawNodeEx::drawRect(const Vec2& p1,
    const Vec2& p2,
    const Vec2& p3,
    const Vec2& p4,
    const Color4B& color,
    float thickness)
{
    if (thickness != 1.0f)
    {
        Vec2 line[4] = { {p1}, {p2}, {p3}, {p4} };
        swapIsConvex(true);
        _drawPoly(line, 4, true, color, thickness);
        _isConvex = _isConvexTmp;
    }
    else
    {
        drawLine(p1, p2, color);
        drawLine(p2, p3, color);
        drawLine(p3, p4, color);
        drawLine(p4, p1, color);
    }
}

void DrawNodeEx::drawSegment(const Vec2& from, const Vec2& to, float radius, const Color4B& color, DrawNodeEx::EndType endType)
{
    Vec2 line[2] = { from, to };
    Vec2* _vertices = transform(line, 2);

    Vec2 a = _vertices[0];
    Vec2 b = _vertices[1];
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

    int ii = 0;
    unsigned int vertex_count = 6 * 3;

    ensureCapacityTriangle(vertex_count);
    V2F_C4B_T2F_Triangle* triangles = (V2F_C4B_T2F_Triangle*)(_bufferTriangle + _bufferCountTriangle);

    // 
    Color4B col = color;
    switch (endType)
    {

    case DrawNodeEx::EndType::Butt:
        break;

    case DrawNodeEx::EndType::Square:
        triangles[ii++] = {
            {v0, col, Tex2F::ZERO},
            {v1, col, Tex2F(-n)},
            {v2, col, Tex2F(n)},
        };

        triangles[ii++] = {
            {v3, col, Tex2F(n)},
            {v1, col, Tex2F::ZERO},
            {v2, col, Tex2F(-n)},
        };

        break;
    case DrawNodeEx::EndType::Round:
        triangles[ii++] = {
            {v0, col, Tex2F(-(n + t))},
            {v1, col, Tex2F(n - t)},
            {v2, col, Tex2F(-n)},
        };

        triangles[ii++] = {
            {v3, col, Tex2F(n)},
            {v1, col, Tex2F(n - t)},
            {v2, col, Tex2F(-n)},
        };
        break;

    default:
        break;
    }

    // BODY
    triangles[ii++] = {
        {v3, col, Tex2F(n)},
        {v4, col, Tex2F(-n)},
        {v2, col, Tex2F(-n)},
    };

    triangles[ii++] = {
        {v3, col, Tex2F(n)},
        {v4, col, Tex2F(-n)},
        {v5, col, Tex2F(n)},
    };

    // End
    switch (endType)
    {
    case DrawNodeEx::EndType::Butt:
        break;

    case DrawNodeEx::EndType::Square:
        triangles[ii++] = {
            {v6, col, Tex2F::ZERO},
            {v4, col, Tex2F(-n)},
            {v5, col, Tex2F(n)},
        };

        triangles[ii++] = {
            {v6, col, Tex2F(-n)},
            {v7, col, Tex2F::ZERO},
            {v5, col, Tex2F(n)},
        };
        break;

    case DrawNodeEx::EndType::Round:
        triangles[ii++] = {
            {v6, col, Tex2F(t - n)},
            {v4, col, Tex2F(-n)},
            {v5, col, Tex2F(n)},
        };

        triangles[ii++] = {
            {v6, col, Tex2F(t - n)},
            {v7, col, Tex2F(t + n)},
            {v5, col, Tex2F(n)},
        };
        break;

    default:
        break;
    }


    _customCommandTriangle.updateVertexBuffer(triangles, _bufferCountTriangle * sizeof(V2F_C4B_T2F),
        vertex_count * sizeof(V2F_C4B_T2F));
    _bufferCountTriangle += ii * 3; //vertex_count;
    _dirtyTriangle = true;
    _customCommandTriangle.setVertexDrawInfo(0, _bufferCountTriangle);
}

void DrawNodeEx::drawPolygon(const Vec2* verts,
    int count,
    const Color4B& fillColor,
    float borderWidth,
    const Color4B& borderColor)
{
    _drawPolygon(verts, count, fillColor, borderWidth, borderColor, true);
}

void DrawNodeEx::drawPolygon(const Vec2* verts,
    int count,
    float borderWidth,
    const Color4B& borderColor)
{
    _drawPolygon(verts, count, Color4B::TRANSPARENT, borderWidth, borderColor, true);
}

void DrawNodeEx::drawSolidPolygon(const Vec2* verts,
    int count,
    const Color4B& fillColor,
    float borderWidth,
    const Color4B& borderColor)
{
    _drawPolygon(verts, count, fillColor, borderWidth, borderColor, true);
}

void DrawNodeEx::drawSolidRect(const Vec2& origin, const Vec2& destination, const Color4B& color)
{
    Vec2 vertices[] = { origin, Vec2(destination.x, origin.y), destination, Vec2(origin.x, destination.y) };
    swapIsConvex(true);
    _drawPolygon(vertices, 4, color, 0.0f, Color4B(), true);
    _isConvex = _isConvexTmp;
}

void DrawNodeEx::drawSolidPoly(const Vec2* poli, unsigned int numberOfPoints, const Color4B& color)
{
    _drawPolygon(poli, numberOfPoints, color, 0.0f, Color4B(), true);
}

void DrawNodeEx::drawPie(const Vec2& center,
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
    // not a real line!
    if (startAngle == endAngle)
        return;

#define DEGREES 360

    const float coef = 2.0f * (float)M_PI / DEGREES;
    Vec2* vertices = _abuf.get<Vec2>(DEGREES + 2);

    int n = 0;
    float rads = 0;
    float _angle = AX_DEGREES_TO_RADIANS(rotation);

    if (startAngle > endAngle)
    {
        int tmp = endAngle;
        endAngle = startAngle;
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
        _drawPolygon(vertices, n, fillColor, thickness, borderColor, true);
        break;
    case DrawMode::Outline:
        vertices[n++] = center;
        _drawPolygon(vertices, n, Color4B::TRANSPARENT, thickness, borderColor, true);
        break;
    case DrawMode::Line:
        _drawPolygon(vertices, n, Color4B::TRANSPARENT, thickness, borderColor, false);
        break;
    case DrawMode::Semi:
        _drawPolygon(vertices, n, fillColor, thickness, borderColor, true);
        break;

    default:
        break;
    }
}

void DrawNodeEx::drawSolidCircle(const Vec2& center,
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
        float j = radius * cosf(rads + angle) * scaleX + center.x;
        float k = radius * sinf(rads + angle) * scaleY + center.y;

        vertices[i].x = j;
        vertices[i].y = k;
    }
    swapIsConvex(true);
    _drawPolygon(vertices, segments, fillColor, borderWidth, borderColor);
    _isConvex = _isConvexTmp;
}

void DrawNodeEx::drawSolidCircle(const Vec2& center,
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
        float j = radius * cosf(rads + angle) * scaleX + center.x;
        float k = radius * sinf(rads + angle) * scaleY + center.y;

        vertices[i].x = j;
        vertices[i].y = k;
    }
    swapIsConvex(true);
    _drawPolygon(vertices, segments, color, 0.0f, Color4B(), true);
    _isConvex = _isConvexTmp;
}

void DrawNodeEx::drawSolidCircle(const Vec2& center,
    float radius,
    float angle,
    unsigned int segments,
    const Color4B& color)
{
    drawSolidCircle(center, radius, angle, segments, 1.0f, 1.0f, color);
}

void DrawNodeEx::drawTriangle(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Color4B& color, float thickness)
{
    Vec2 poli[3] = { p1, p2, p3 };
    unsigned int vertex_count = 3;

    if (thickness != 1.0f)
    {
        swapIsConvex(true);
        _drawPolygon(poli, vertex_count, Color4B::TRANSPARENT, thickness, color, true);
        _isConvex = _isConvexTmp;
        return;
    }
    Vec2* _vertices = transform(poli, vertex_count);

    ensureCapacityTriangle(vertex_count);

    V2F_C4B_T2F a = { _vertices[0], color, Tex2F::ZERO };
    V2F_C4B_T2F b = { _vertices[1], color, Tex2F::ZERO };
    V2F_C4B_T2F c = { _vertices[2], color, Tex2F::ZERO };

    V2F_C4B_T2F_Triangle* triangles = (V2F_C4B_T2F_Triangle*)(_bufferTriangle + _bufferCountTriangle);
    triangles[0] = { a, b, c };

    _customCommandTriangle.updateVertexBuffer(triangles, _bufferCountTriangle * sizeof(V2F_C4B_T2F),
        vertex_count * sizeof(V2F_C4B_T2F));
    _bufferCountTriangle += vertex_count;
    _dirtyTriangle = true;
    _customCommandTriangle.setVertexDrawInfo(0, _bufferCountTriangle);
}

void DrawNodeEx::clear()
{
    _bufferCountTriangle = 0;
    _dirtyTriangle = true;
    _bufferCountLine = 0;
    _dirtyLine = true;
    _bufferCountPoint = 0;
    _dirtyPoint = true;
    _lineWidth = _defaultLineWidth;
}

const BlendFunc& DrawNodeEx::getBlendFunc() const
{
    return _blendFunc;
}

void DrawNodeEx::setBlendFunc(const BlendFunc& blendFunc)
{
    _blendFunc = blendFunc;
}

void DrawNodeEx::setLineWidth(float lineWidth)
{
    _lineWidth = lineWidth;
}

float DrawNodeEx::getLineWidth()
{
    return this->_lineWidth;
}

void DrawNodeEx::visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
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

void DrawNodeEx::_drawPolygon(const Vec2* verts,
    unsigned int count,
    const Color4B& fillColor,
    float borderWidth,
    const Color4B& borderColo,
    bool closedPolygon)
{
    AXASSERT(count >= 0, "invalid count value");

    Vec2* _vertices = transform(verts, count);

    bool outline = (borderColo.a > 0.0f && borderWidth > 0.0f);

    Color4B borderColor = borderColo;

    auto triangle_count = outline ? (3 * count - 2) : (count - 2);
    auto vertex_count = 3 * triangle_count;
    ensureCapacityTriangle(vertex_count);

    V2F_C4B_T2F_Triangle* triangles = (V2F_C4B_T2F_Triangle*)(_bufferTriangle + _bufferCountTriangle);
    V2F_C4B_T2F_Triangle* cursor = triangles;

    if (closedPolygon && !_isConvex && count >= 3 && !isConvex(_vertices, count))
    {
        std::vector<p2t::Point> p2pointsStorage;
        p2pointsStorage.reserve(count);
        std::vector<p2t::Point*> p2points;
        p2points.reserve(count);

        for (int i = 0; i < count; ++i)
        {
            p2points.emplace_back(&p2pointsStorage.emplace_back((float)_vertices[i].x, (float)_vertices[i].y));
        }
        p2t::CDT cdt(p2points);
        cdt.Triangulate();
        std::vector<p2t::Triangle*> tris = cdt.GetTriangles();

        if ((tris.size() * 3) > vertex_count)
        {
            ensureCapacityTriangle((tris.size() * 3));
            triangles = (V2F_C4B_T2F_Triangle*)(_bufferTriangle + _bufferCountTriangle);
            cursor = triangles;
        }

        for (auto&& t : tris)
        {
            p2t::Point* vec1 = t->GetPoint(0);
            p2t::Point* vec2 = t->GetPoint(1);
            p2t::Point* vec3 = t->GetPoint(2);

            V2F_C4B_T2F_Triangle tmp = {
                {Vec2(vec1->x, vec1->y), fillColor, Tex2F::ZERO},
                {Vec2(vec2->x, vec2->y), fillColor, Tex2F::ZERO},
                {Vec2(vec3->x, vec3->y), fillColor, Tex2F::ZERO},
            };
            *cursor++ = tmp;
        }
    }
    else
    {
        for (int i = 0; i < count - 2; i++)
        {
            V2F_C4B_T2F_Triangle tmp = {
                {_vertices[0], fillColor, Tex2F::ZERO},
                {_vertices[i + 1], fillColor, Tex2F::ZERO},
                {_vertices[i + 2], fillColor, Tex2F::ZERO},
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

        for (unsigned int i = 0; i < count; i++)
        {
            Vec2 v0 = _vertices[(i - 1 + count) % count];
            Vec2 v1 = _vertices[i];
            Vec2 v2 = _vertices[(i + 1) % count];

            Vec2 n1 = ((v1 - v0).getPerp()).getNormalized();
            Vec2 n2 = ((v2 - v1).getPerp()).getNormalized();

            Vec2 offset = (n1 + n2) * (1.0f / (Vec2::dot(n1, n2) + 1.0f));
            extrude[i] = { offset, n2 };
        }

        for (unsigned int i = 0; i < count; i++)
        {
            int j = (i + 1) % count;
            Vec2 v0 = _vertices[i];
            Vec2 v1 = _vertices[j];

            Vec2 n0 = extrude[i].n;

            Vec2 offset0 = extrude[i].offset;
            Vec2 offset1 = extrude[j].offset;

            Vec2 inner0 = v0 - offset0 * borderWidth;
            Vec2 inner1 = v1 - offset1 * borderWidth;
            Vec2 outer0 = v0 + offset0 * borderWidth;
            Vec2 outer1 = v1 + offset1 * borderWidth;
            borderColor = borderColo;
            if (i >= count - 1 && !closedPolygon)  // /-2  ??
            {
                borderColor = Color4B::TRANSPARENT;
            }

            V2F_C4B_T2F_Triangle tmp1 = { {inner0, borderColor, Tex2F(-n0)},
                {inner1, borderColor, Tex2F(-n0)},
                {outer1, borderColor, Tex2F(n0)} };
            *cursor++ = tmp1;

            V2F_C4B_T2F_Triangle tmp2 = { {inner0, borderColor, Tex2F(-n0)},
                {outer0, borderColor, Tex2F(n0)},
                {outer1, borderColor, Tex2F(n0)} };
            *cursor++ = tmp2;
        }

        free(extrude);
    }

    _customCommandTriangle.updateVertexBuffer(triangles, _bufferCountTriangle * sizeof(V2F_C4B_T2F),
        vertex_count * sizeof(V2F_C4B_T2F));
    _bufferCountTriangle += vertex_count;
    _customCommandTriangle.setVertexDrawInfo(0, _bufferCountTriangle);
    _dirtyTriangle = true;
}

void DrawNodeEx::_drawPoly(const Vec2* poli,
    unsigned int numberOfPoints,
    bool closePolygon,
    const Color4B& color,
    float thickness)
{

    if (thickness != 1.0f)
    {
        if (closePolygon)
        {
            _drawPolygon(poli, numberOfPoints, Color4B::TRANSPARENT, thickness, color, true);
            return;
        }
        else
        {
            _drawPolygon(poli, numberOfPoints, Color4B::TRANSPARENT, thickness, color, false);
            return;
        }
    }
    else
    {
        Vec2* _vertices = transform(poli, numberOfPoints);

        unsigned int vertex_count;
        if (closePolygon)
        {
            vertex_count = 2 * numberOfPoints;
        }
        else
        {
            vertex_count = 2 * (numberOfPoints - 1);
        }
        ensureCapacityLine(vertex_count);

        V2F_C4B_T2F* point = _bufferLine + _bufferCountLine;
        V2F_C4B_T2F* cursor = point;

        unsigned int i = 0;
        for (; i < numberOfPoints - 1; i++)
        {
            *point = { _vertices[i], color, Tex2F::ZERO };
            *(point + 1) = { _vertices[i + 1], color, Tex2F::ZERO };
            point += 2;
        }
        if (closePolygon)
        {
            *point = { _vertices[i], color, Tex2F::ZERO };
            *(point + 1) = { _vertices[0], color, Tex2F::ZERO };
        }

        _customCommandLine.updateVertexBuffer(cursor, _bufferCountLine * sizeof(V2F_C4B_T2F),
            vertex_count * sizeof(V2F_C4B_T2F));
        _bufferCountLine += vertex_count;
        _customCommandLine.setVertexDrawInfo(0, _bufferCountLine);
    }
}
NS_AX_EXT_END

#if defined(_WIN32)
#    pragma pop_macro("TRANSPARENT")
#endif
