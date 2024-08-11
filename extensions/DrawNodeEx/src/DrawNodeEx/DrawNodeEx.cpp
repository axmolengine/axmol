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

        if (crossProduct > 0) isPositive = true;
        else if (crossProduct < 0) isNegative = true;

        if (isPositive && isNegative)
            return false;  // Polygon is concave
    }
    return true;  // Polygon is convex
}

Vec2* DrawNodeEx::transform(const Vec2* vertices, unsigned int& count, bool closedPolygon)
{
    Vec2* vert = nullptr;
    Vec2 vert0 = vertices[0];
    int closedCounter = 0;

    if (closedPolygon && vertices[0] != vertices[count - 1])
    {
        closedCounter = 1;
    }

    vert = new Vec2[count + closedCounter];
    AXASSERT(vert != nullptr, "NO memory");

    if (_dnTransform == false)
    {
        memcpy(vert, vertices, (count) * sizeof(Vec2));
        if (closedCounter)
        {
            vert[count] = vert0;
            count++;
        }
        return vert;
    }

    const float sinRot = sin(_dnRotation);
    const float cosRot = cos(_dnRotation);

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
            float xnew = vert[i].x * cosRot - vert[i].y * sinRot;
            float ynew = vert[i].x * sinRot + vert[i].y * cosRot;

            // translate point back:
            vert[i].x = xnew + _dnCenter.x;
            vert[i].y = ynew + _dnCenter.y;

            // scale and position
            vert[i].x = vert[i].x * _dnScale.x + _dnPosition.x;
            vert[i].y = vert[i].y * _dnScale.y + _dnPosition.y;
        }
    }

    if (closedCounter)
    {
        vert[count] = vert[0];
        count++;
    }

    return vert;
}

DrawNodeEx::DrawNodeEx(float lineWidth)
    : _lineWidth(lineWidth)
    , _defaultLineWidth(lineWidth)
    , _isConvex(false)
    , _dnPosition(Vec2::ZERO)
    , _dnRotation(0.0f)
    , _dnScale(Vec2::ONE)
    , _dnTransform(true)  // check true/false
    , _drawOrder(false)     // check true/false
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
    freeShaderInternal(_customCommandTriangle);

#if defined(AX_ENABLE_DRAWNODE_DRAW_LINE_POINT)
    AX_SAFE_FREE(_bufferPoint);
    AX_SAFE_FREE(_bufferLine);

    freeShaderInternal(_customCommandPoint);
    freeShaderInternal(_customCommandLine);
#endif
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

#if defined(AX_ENABLE_DRAWNODE_DRAW_LINE_POINT)
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
#endif

bool DrawNodeEx::init()
{
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
    updateShader();
    ensureCapacityTriangle(512);
    _dirtyTriangle = true;

#if defined(AX_ENABLE_DRAWNODE_DRAW_LINE_POINT)
    ensureCapacityPoint(64);
    ensureCapacityLine(256);
    _dirtyLine = true;
    _dirtyPoint = true;
#endif

    return true;
}

void DrawNodeEx::updateShader()
{
    updateShaderInternal(_customCommandTriangle, backend::ProgramType::POSITION_COLOR_LENGTH_TEXTURE,
        CustomCommand::DrawType::ARRAY, CustomCommand::PrimitiveType::TRIANGLE);
#if defined(AX_ENABLE_DRAWNODE_DRAW_LINE_POINT)
    updateShaderInternal(_customCommandPoint, backend::ProgramType::POSITION_COLOR_TEXTURE_AS_POINTSIZE,
        CustomCommand::DrawType::ARRAY, CustomCommand::PrimitiveType::POINT);

    updateShaderInternal(_customCommandLine, backend::ProgramType::POSITION_COLOR_LENGTH_TEXTURE,
        CustomCommand::DrawType::ARRAY, CustomCommand::PrimitiveType::LINE);
#endif
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
#if defined(AX_ENABLE_DRAWNODE_DRAW_LINE_POINT)
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
#endif
}

void DrawNodeEx::drawPoint(const Vec2& position, const float pointSize, const Color4B& color,
    const DrawNodeEx::PointType pointType)
{
    if (pointSize == 0) return;

#if defined(AX_ENABLE_DRAWNODE_DRAW_LINE_POINT)
    if (_drawOrder == true)
    {
#endif
        float pointSize4 = pointSize / 4;
        drawSolidRect(position - Vec2(pointSize4, pointSize4), position + Vec2(pointSize4, pointSize4), color);
#if defined(AX_ENABLE_DRAWNODE_DRAW_LINE_POINT)
    }
    else
    {
        ensureCapacityPoint(1);

        V2F_C4B_T2F* point = _bufferPoint + _bufferCountPoint;
        *point = { position, color, Tex2F(pointSize, 0.0f) };

        _customCommandPoint.updateVertexBuffer(point, _bufferCountPoint * sizeof(V2F_C4B_T2F), sizeof(V2F_C4B_T2F));
        _bufferCountPoint += 1;
        _dirtyPoint = true;
        _customCommandPoint.setVertexDrawInfo(0, _bufferCountPoint);
    }
#endif
}

void DrawNodeEx::drawPoints(const Vec2* position, unsigned int numberOfPoints, const Color4B& color,
    const DrawNodeEx::PointType pointType)
{
    drawPoints(position, numberOfPoints, 1.f, color);
}

void DrawNodeEx::drawPoints(const Vec2* position,
    unsigned int numberOfPoints,
    const float pointSize,
    const Color4B& color,
    const DrawNodeEx::PointType pointType)
{
    if (pointSize == 0) return;

#if defined(AX_ENABLE_DRAWNODE_DRAW_LINE_POINT)
    if (_drawOrder == true)
    {
#endif
        Vec2 pointSize4 = Vec2(pointSize / 4, pointSize / 4);
        for (unsigned int i = 0; i < numberOfPoints; i++)
        {
            switch (pointType)
            {
            case PointType::Circle:
                drawSolidCircle(position[i], pointSize, 90, 8, color);
                break;
            case PointType::Rect:
                drawSolidRect(position[i] - pointSize4, position[i] + pointSize4, color);
                break;
            case PointType::Triangle:
             //   drawTriangle(position[i] - pointSize4, position[i] + pointSize4, color);
                break;

            default:
                break;
            }
        }
#if defined(AX_ENABLE_DRAWNODE_DRAW_LINE_POINT)
        return;
    }
    ensureCapacityPoint(numberOfPoints);

    V2F_C4B_T2F* point = _bufferPoint + _bufferCountPoint;
    for (unsigned int i = 0; i < numberOfPoints; i++)
    {
        *(point + i) = { position[i], color, Tex2F(pointSize, 0.0f) };
    }

    _customCommandPoint.updateVertexBuffer(point, _bufferCountPoint * sizeof(V2F_C4B_T2F),
        numberOfPoints * sizeof(V2F_C4B_T2F));
    _bufferCountPoint += numberOfPoints;
    _dirtyPoint = true;
    _customCommandPoint.setVertexDrawInfo(0, _bufferCountPoint);
#endif
}

void DrawNodeEx::drawLine(const Vec2& origin, const Vec2& destination, const Color4B& color, float thickness, DrawNodeEx::EndType etStart, DrawNodeEx::EndType etEnd)
{
    if (thickness == 0) return;

    if (thickness == 1.0f && !_drawOrder)  // usefull for a DrawNode::drawline thickness = 1.0 only ?
    {
        unsigned int count = 2;
        Vec2 line[] = { origin, destination };
        Vec2* _vertices = transform(line, count, false);

        ensureCapacityLine(count);

        V2F_C4B_T2F* point = _bufferLine + _bufferCountLine;

        *point = { _vertices[0], color, Tex2F::ZERO };
        *(point + 1) = { _vertices[1], color, Tex2F::ZERO };

        _customCommandLine.updateVertexBuffer(point, _bufferCountLine * sizeof(V2F_C4B_T2F), 2 * sizeof(V2F_C4B_T2F));
        _bufferCountLine += 2;
        _dirtyLine = true;
        _customCommandLine.setVertexDrawInfo(0, _bufferCountLine);
        AX_SAFE_DELETE_ARRAY(_vertices);
        return;
    }
    else
    {
        //if (_drawOrder) thickness /= _scaleFactor;

        drawSegment(origin, destination, thickness, color);
        return;
    }
}

void DrawNodeEx::drawPoly(const Vec2* poli,
    unsigned int numberOfPoints,
    bool closedPolygon,
    const Color4B& color,
    float thickness)
{
    if (thickness == 0) return;
    _drawPoly(poli, numberOfPoints, closedPolygon, color, thickness);
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
    if (thickness == 0) return;

    if (segments > 40) segments = 40; // performance issue; 40 is enouggh (I hope)


    const float coef = 2.0f * (float)M_PI / segments;

    Vec2* _vertices = _abuf.get<Vec2>(segments + 2);

    for (unsigned int i = 0; i < segments; i++)
    {
        float rads = i * coef;
        float j = radius * cosf(rads + angle) * scaleX + center.x;
        float k = radius * sinf(rads + angle) * scaleY + center.y;

        _vertices[i].x = j;
        _vertices[i].y = k;
    }
    _vertices[segments] = _vertices[0];

    if (drawLineToCenter)  _vertices[++segments] = center;

    swapIsConvex(true);
    //_drawPolygon(_vertices, segments+1, Color4B::TRANSPARENT, thickness, color, false);
    _drawPoly(_vertices, segments + 1, false, color, thickness);
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
    if (thickness == 0) return;
    drawCircle(center, radius, angle, segments, drawLineToCenter, 1.0f, 1.0f, color, thickness);
}

void DrawNodeEx::drawStar(const Vec2& center,
    float radiusI,
    float radiusO,
    unsigned int segments,
    const Color4B& color,
    float thickness)
{
    if (thickness == 0) return;
    _drawAStar(center, radiusI, radiusO, segments, color, color, thickness, false);
}

void DrawNodeEx::drawSolidStar(const Vec2& center,
    float radiusI, // inner 
    float radiusO, // outer
    unsigned int segments,
    const Color4B& color,
    const Color4B& filledColor,
    float thickness)
{
    _drawAStar(center, radiusI, radiusO, segments, color, filledColor, thickness, true);
}

void DrawNodeEx::_drawAStar(const Vec2& center,
    float radiusI, // inner 
    float radiusO, // outer
    unsigned int segments,
    const Color4B& color,
    const Color4B& filledColor,
    float thickness,
    bool solid)
{
    const float coef = 2.0f * (float)M_PI / segments;
    float halfAngle = coef / 2.0f;

    auto vertices = _abuf.get<Vec2>(segments * 2 + 1);

    int i = 0;
    for (unsigned int a = 0; a < segments; a++)
    {
        float rads = a * coef;
        vertices[i++] = Vec2(center.x + cos(rads) * radiusO, center.y + sin(rads) * radiusO);
        vertices[i++] = Vec2(center.x + cos(rads + halfAngle) * radiusI, center.y + sin(rads + halfAngle) * radiusI);
    }

    if (solid)
    {
        swapIsConvex(false);
        _drawPolygon(vertices, i, filledColor, thickness, color, true);
        _isConvex = _isConvexTmp;
    }
    else
    {
        vertices[i++] = vertices[0];
        _drawPoly(vertices, i, true, color, thickness);
    }
}

void DrawNodeEx::drawQuadBezier(const Vec2& origin,
    const Vec2& control,
    const Vec2& destination,
    unsigned int segments,
    const Color4B& color,
    float thickness)
{
    if (thickness == 0) return;

    Vec2* vertices = _abuf.get<Vec2>(segments + 1);

    float t = 0.0f;
    for (unsigned int i = 0; i < segments; i++)
    {
        vertices[i].x = powf(1.0f - t, 2.0f) * origin.x + 2.0f * (1.0f - t) * t * control.x + t * t * destination.x;
        vertices[i].y = powf(1.0f - t, 2.0f) * origin.y + 2.0f * (1.0f - t) * t * control.y + t * t * destination.y;
        t += 1.0f / segments;
    }
    vertices[segments].x = destination.x;
    vertices[segments].y = destination.y;

    ////  if (_drawOrder == true)
    //{
    //    thickness /= _scaleFactor;
    //}

    swapIsConvex(false);
    // _drawPoly(vertices, segments + 1, false, color, thickness);
    _drawPolygon(vertices, segments + 1, Color4B::TRANSPARENT, thickness, color, false);
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
    if (thickness == 0) return;

    Vec2* vertices = _abuf.get<Vec2>(segments + 1);

    float t = 0.0f;
    for (unsigned int i = 0; i < segments; i++)
    {
        vertices[i].x = powf(1.0f - t, 3.0f) * origin.x + 3.0f * powf(1.0f - t, 2.0f) * t * control1.x +
            3.0f * (1 - t) * t * t * control2.x + t * t * t * destination.x;
        vertices[i].y = powf(1.0f - t, 3.0f) * origin.y + 3.0f * powf(1.0f - t, 2.0f) * t * control1.y +
            3.0f * (1 - t) * t * t * control2.y + t * t * t * destination.y;
        t += 1.0f / segments;
    }
    vertices[segments].x = destination.x;
    vertices[segments].y = destination.y;

    //  if (_drawOrder == true)
    {
        //     thickness /= _scaleFactor;
    }
    swapIsConvex(true);
    //   _drawPoly(vertices, segments + 1, false, color, thickness);
    _drawPolygon(vertices, segments + 1, Color4B::TRANSPARENT, thickness, color, false);
    _isConvex = _isConvexTmp;
}

void DrawNodeEx::drawCardinalSpline(ax::PointArray* config,
    float tension,
    unsigned int segments,
    const Color4B& color,
    float thickness)
{
    if (thickness == 0.0f) return;

    Vec2* vertices = _abuf.get<Vec2>(segments + 1);

    if (!vertices)
        return;

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
    _drawPoly(vertices, segments + 1, false, color, thickness);
    _isConvex = _isConvexTmp;
}

void DrawNodeEx::drawCatmullRom(ax::PointArray* points, unsigned int segments, const Color4B& color, float thickness)
{
    if (thickness == 0.0f) return;
    drawCardinalSpline(points, 0.5f, segments, color, thickness);
}


void DrawNodeEx::drawDot(const Vec2& pos, float radius, const Color4B& color)
{
    if (radius == 0.0f) return;

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
    if (thickness == 0.0f) return;

    ////  if (thickness == 1.0f)  // usefull for a DrawNode::drawRect thickness = 1.0 only ?
    //{
    //    if (_drawOrder)
    //        thickness /= 1.33;//(_scaleFactor/3.5);  //  ???   its works but .... 

    //    drawLine(p1, p2, color, thickness);
    //    drawLine(p2, p3, color, thickness);
    //    drawLine(p3, p4, color, thickness);
    //    drawLine(p4, p1, color, thickness);
    //}
    //else
    {
        Vec2 line[5] = { {p1}, {p2}, {p3}, {p4}, {p1} };
        swapIsConvex(true);
        _drawPoly(line, 5, false, color, thickness);
        _isConvex = _isConvexTmp;
    }
}

void DrawNodeEx::drawRect(const Vec2& origin, const Vec2& destination, const Color4B& color, float thickness)
{
    if (thickness == 0.0f) return;

    drawRect(origin, Vec2(destination.x, origin.y), destination, Vec2(origin.x, destination.y), color, thickness);
}

void DrawNodeEx::drawSegment(const Vec2& from, const Vec2& to, float thickness, const Color4B& color, DrawNodeEx::EndType etStart, DrawNodeEx::EndType etEnd)
{
    if (thickness == 0.0f) return;

    unsigned int count = 2;
    Vec2 line[] = { from, to };
    Vec2* _vertices = transform(line, count, false);

    Vec2 a = _vertices[0];
    Vec2 b = _vertices[1];
    Vec2 n = ((b - a).getPerp()).getNormalized();
    Vec2 t = n.getPerp();
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

    //   unsigned int vertex_count = 6 * 3;
    unsigned int vertex_count = 3 * ((etStart != DrawNodeEx::EndType::Butt) ? 2 : 0) + 3 * 2 + 3 * ((etEnd != DrawNodeEx::EndType::Butt) ? 2 : 0);
    ensureCapacityTriangle(vertex_count);
    V2F_C4B_T2F_Triangle* triangles = (V2F_C4B_T2F_Triangle*)(_bufferTriangle + _bufferCountTriangle);

    int ii = 0;
    switch (etEnd)
    {
    case DrawNodeEx::EndType::Butt:
        break;

    case DrawNodeEx::EndType::Square:
        triangles[ii++] = {
            {v0, color, Tex2F::ZERO},
            {v1, color, Tex2F(-n)},
            {v2, color, Tex2F(n)},
        };

        triangles[ii++] = {
            {v3, color, Tex2F(n)},
            {v1, color, Tex2F::ZERO},
            {v2, color, Tex2F(-n)},
        };

        break;
    case DrawNodeEx::EndType::Round:
        triangles[ii++] = {
            {v0, color, Tex2F(-(n + t))},
            {v1, color, Tex2F(n - t)},
            {v2, color, Tex2F(-n)},
        };

        triangles[ii++] = {
            {v3, color, Tex2F(n)},
            {v1, color, Tex2F(n - t)},
            {v2, color, Tex2F(-n)},
        };
        break;

    default:
        break;
    }

    // BODY
    triangles[ii++] = {
        {v3, color, Tex2F(n)},
        {v4, color, Tex2F(-n)},
        {v2, color, Tex2F(-n)},
    };

    triangles[ii++] = {
        {v3, color, Tex2F(n)},
        {v4, color, Tex2F(-n)},
        {v5, color, Tex2F(n)},
    };

    // Start
    switch (etStart)
    {
    case DrawNodeEx::EndType::Butt:
        break;

    case DrawNodeEx::EndType::Square:
        triangles[ii++] = {
            {v6, color, Tex2F::ZERO},
            {v4, color, Tex2F(-n)},
            {v5, color, Tex2F(n)},
        };

        triangles[ii++] = {
            {v6, color, Tex2F(-n)},
            {v7, color, Tex2F::ZERO},
            {v5, color, Tex2F(n)},
        };
        break;

    case DrawNodeEx::EndType::Round:
        triangles[ii++] = {
            {v6, color, Tex2F(t - n)},
            {v4, color, Tex2F(-n)},
            {v5, color, Tex2F(n)},
        };

        triangles[ii++] = {
            {v6, color, Tex2F(t - n)},
            {v7, color, Tex2F(t + n)},
            {v5, color, Tex2F(n)},
        };
        break;

    default:
        break;
    }
    if (ii * 3 != vertex_count)
    {
        //       AXLOGD("==============end  ii {}*3: {}  vertex_count: {}", ii, ii * 3, vertex_count);
    }

    _customCommandTriangle.updateVertexBuffer(triangles, _bufferCountTriangle * sizeof(V2F_C4B_T2F),
        vertex_count * sizeof(V2F_C4B_T2F));
    _bufferCountTriangle += vertex_count; //ii * 3; 
    _dirtyTriangle = true;
    _customCommandTriangle.setVertexDrawInfo(0, _bufferCountTriangle);

    AX_SAFE_DELETE_ARRAY(_vertices);
}

void DrawNodeEx::drawPolygon(Vec2* verts,
    int count,
    const Color4B& fillColor,
    float thickness,
    const Color4B& borderColor)
{
    _drawPolygon(verts, count, fillColor, thickness, borderColor, true);
}

void DrawNodeEx::drawPolygon(Vec2* verts,
    int count,
    float thickness,
    const Color4B& borderColor)
{
    _drawPolygon(verts, count, Color4B::TRANSPARENT, thickness, borderColor, true);
}

void DrawNodeEx::drawSolidPolygon(Vec2* verts,
    int count,
    const Color4B& fillColor,
    float thickness,
    const Color4B& borderColor)
{
    _drawPolygon(verts, count, fillColor, thickness, borderColor, true);
}

void DrawNodeEx::drawSolidRect(const Vec2& origin, const Vec2& destination, const Color4B& fillColor, float thickness,
    const Color4B& borderColor)
{
    //   if (_drawOrder == true) thickness /= _scaleFactor; //2;

    Vec2 vertices[] = { origin, Vec2(destination.x, origin.y), destination, Vec2(origin.x, destination.y) };
    swapIsConvex(true);
    _drawPolygon(vertices, 4, fillColor, thickness, borderColor, false);
    _isConvex = _isConvexTmp;
}

void DrawNodeEx::drawSolidPoly(const Vec2* poli, unsigned int numberOfPoints, const Color4B& color, float thickness,
    const Color4B& borderColor)
{
    //   thickness /= _scaleFactor;
    _drawPolygon(poli, numberOfPoints, color, thickness, borderColor, true);
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
#define DEGREES 360
    bool _circle = false;

    // Not a real line!
    if (startAngle == endAngle)
        return;

    // Its a circle?
    if (MAX((startAngle - endAngle), (endAngle - startAngle)) == DEGREES)
    {
        _circle = true;
    }

    const float coef = 2.0f * (float)M_PI / DEGREES;
    Vec2* vertices = _abuf.get<Vec2>(DEGREES + 2);

    int n = 0;
    float rads = 0.0f;
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

            vertices[n].x = j;
            vertices[n].y = k;
            n++;
        }
    }
    switch (drawMode)
    {
    case DrawMode::Fill:
        vertices[n++] = center;
        vertices[n++] = vertices[0];
        if (_circle)
        {
            drawSolidCircle(center, radius, 0.0f, 36, scaleX, scaleY, fillColor, thickness, borderColor);
            break;
        }
        _drawPolygon(vertices, n, fillColor, thickness, borderColor, true);
        break;
    case DrawMode::Outline:
        vertices[n++] = center;
        vertices[n++] = vertices[0];
        if (_circle)
        {
            drawSolidCircle(center, radius, 0.0f, 36, scaleX, scaleY, Color4B::TRANSPARENT, thickness, borderColor);
            break;
        }
        _drawPolygon(vertices, n, Color4B::TRANSPARENT, thickness, borderColor, false);
        break;
    case DrawMode::Line:
        _drawPolygon(vertices, n, Color4B::TRANSPARENT, thickness, borderColor, false);
        break;
    case DrawMode::Semi:
        _drawPolygon(vertices, n - 1, fillColor, thickness, borderColor, true);
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
    float thickness,
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
    _drawPolygon(vertices, segments, fillColor, thickness, borderColor);
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
    drawSolidCircle(center, radius, angle, segments, scaleX, scaleY, color, 0.0f, Color4B());
}

void DrawNodeEx::drawSolidCircle(const Vec2& center,
    float radius,
    float angle,
    unsigned int segments,
    const Color4B& color)
{
    drawSolidCircle(center, radius, angle, segments, 1.0f, 1.0f, color, 0.0f, Color4B());
}

void DrawNodeEx::drawTriangle(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Color4B& color, float thickness)
{
    if (thickness == 0.0f) return;

    Vec2 poli[3] = { p1, p2, p3 };
    unsigned int vertex_count = 3;

    if (thickness != 1.0f)
    {
        swapIsConvex(true);
        _drawPolygon(poli, vertex_count, Color4B::TRANSPARENT, thickness, color, true);
        _isConvex = _isConvexTmp;
    }
    else   // PeEi Noch checken!!!!!!
    {
        Vec2* _vertices = transform(poli, vertex_count, false);

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

        AX_SAFE_DELETE_ARRAY(_vertices);
    }
}

void DrawNodeEx::clear()
{
    _bufferCountTriangle = 0;
    _dirtyTriangle = true;
#if defined(AX_ENABLE_DRAWNODE_DRAW_LINE_POINT)
    _bufferCountLine = 0;
    _dirtyLine = true;
    _bufferCountPoint = 0;
    _dirtyPoint = true;
#endif
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
    _defaultLineWidth = lineWidth;
}

float DrawNodeEx::getLineWidth()
{
    return this->_defaultLineWidth;
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
    float thickness,
    const Color4B& borderColor,
    bool closedPolygon)
{
    AXASSERT(count >= 0, "invalid count value");
    Color4B debugColor;

    bool outline = (thickness != 0.0f);

    if (thickness == 1.0f && !_drawOrder)  // usefull for a DrawNode:::_drawPolygon thickness = 1.0 only ?
    {
    }

    Vec2* _vertices = transform(verts, count, closedPolygon);

    std::vector<V2F_C4B_T2F_Triangle> triangleList;

    // 
    int vertex_count = 0;
    // calculate the memory (important for correct drawing stuff)
    if (closedPolygon && !_isConvex && fillColor.a > 0.0f && !isConvex(_vertices, count) && count >= 3)
    {
        std::vector<p2t::Point> p2pointsStorage;
        p2pointsStorage.reserve(count);
        std::vector<p2t::Point*> p2points;
        p2points.reserve(count);

        for (int i = 0; i < count - 1; i++)  // count-1 is needed because of: _vertices[0] = _vertices[i < count]
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
                {Vec2(vec1->x, vec1->y), fillColor, Tex2F::ZERO},
                {Vec2(vec2->x, vec2->y), fillColor, Tex2F::ZERO},
                {Vec2(vec3->x, vec3->y), fillColor, Tex2F::ZERO},
            };
            triangleList.emplace_back(triangle); // use it for drawing later
        }
    }
    else if (fillColor.a > 0.0f)
    {
        vertex_count += count - 2;
    }

    //std::vector<Vec2> vectorList;
    if (outline)
    {
        if (1/* thickness != 1.0f*/)
        {
            if (1 /*thickness != 1.0f*/)
            {
                vertex_count += 6 * (count - 1);
            }
            else
            {
                vertex_count += 2 * count;
            }
        }
    }

    vertex_count *= 3;
    ensureCapacityTriangle(vertex_count);
    V2F_C4B_T2F_Triangle* triangles = (V2F_C4B_T2F_Triangle*)(_bufferTriangle + _bufferCountTriangle);

    // start drawing...
    int ii = 0;
    if (closedPolygon && !_isConvex && fillColor.a > 0.0f && !isConvex(_vertices, count) && count >= 3)
    {
        for (auto&& t : triangleList)
        {
            triangles[ii++] = t;
        }
    }
    else if (fillColor.a > 0.0f)
    {
        for (int i = 0; i < count - 2; i++)
        {
            triangles[ii++] = {
                {_vertices[0], fillColor, Tex2F::ZERO},
                {_vertices[i + 1], fillColor, Tex2F::ZERO},
                {_vertices[i + 2], fillColor, Tex2F::ZERO},
            };
        }
    }
    if (outline)
    {
        if (1/* thickness != 1.0f*/)
        {
            Vec2 vo0, vo1, vo2, vo3, vo4, vo5, vo6, vo7;

            for (int i = 1; i < (count); i++)
            {
                Vec2 a = _vertices[i - 1];
                Vec2 b = _vertices[i];
                Vec2 n = ((b - a).getPerp()).getNormalized();
                Vec2 t = n.getPerp();
                Vec2 nw = n * thickness;  //  geteilt durch 2 ?
                Vec2 tw = t * thickness;  //  geteilt durch 2 ?
                Vec2 v0 = b - (nw + tw);
                Vec2 v1 = b + (nw - tw);
                Vec2 v2 = b - nw;
                Vec2 v3 = b + nw;
                Vec2 v4 = a - nw;
                Vec2 v5 = a + nw;
                Vec2 v6 = a - (nw - tw);
                Vec2 v7 = a + (nw + tw);

                //   if (i == 1)
                {
                    //  debugColor = Color4B::BLUE;
                    debugColor = borderColor;
                    triangles[ii++] = {
                        {v0, debugColor, Tex2F(-(n + t))},
                        {v1, debugColor, Tex2F(n - t)},
                        {v2, debugColor, Tex2F(-n)},
                    };
                    //          debugColor = Color4B::BLUE;//borderColor;
                    triangles[ii++] = {
                        {v3, debugColor, Tex2F(n)},
                        {v1, debugColor, Tex2F(n - t)},
                        {v2, debugColor, Tex2F(-n)},
                    };
                }

                triangles[ii++] = {
                    {v3, borderColor, Tex2F(n)},
                    {v4, borderColor, Tex2F(-n)},
                    {v2, borderColor, Tex2F(-n)},
                };
                triangles[ii++] = {
                    {v3, borderColor, Tex2F(n)},
                    {v4, borderColor, Tex2F(-n)},
                    {v5, borderColor, Tex2F(n)},
                };

                //         if (i == count - 1)  //only the last 
                {
                    //    debugColor = Color4B::RED; //borderColor; //Color4B::RED;
                    debugColor = borderColor; //Color4B::BLUE;
                    triangles[ii++] = {
                        {v6, debugColor, Tex2F(t - n)},
                        {v4, debugColor, Tex2F(-n)},
                        {v5, debugColor, Tex2F(n)},
                    };
                    //    debugColor = Color4B::RED; //borderColor; //Color4B::RED;
                    triangles[ii++] = {
                        {v6, debugColor, Tex2F(t - n)},
                        {v7, debugColor, Tex2F(t + n)},
                        {v5, debugColor, Tex2F(n)},
                    };
                }

                //if (i == ???)
                //{
                //    vo0 = v0;
                //    vo1 = v1;
                //    vo2 = v2;
                //    vo3 = v3;
                //    vo4 = v4;
                //    vo5 = v5;
                //    vo6 = v6;
                //    vo7 = v7;
                //}
            }
        }
        else
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
                extrude[i] = { offset, n2 };
            }

            for (int i = 0; i < count; i++)
            {
                int j = (i + 1) % count;
                Vec2 v0 = verts[i];
                Vec2 v1 = verts[j];

                Vec2 n0 = extrude[i].n;

                Vec2 offset0 = extrude[i].offset;
                Vec2 offset1 = extrude[j].offset;

                Vec2 inner0 = v0 - offset0 * thickness;
                Vec2 inner1 = v1 - offset1 * thickness;
                Vec2 outer0 = v0 + offset0 * thickness;
                Vec2 outer1 = v1 + offset1 * thickness;

                triangles[ii++] = {
                    {inner0, borderColor, Tex2F(-n0)},
                    {inner1, borderColor, Tex2F(-n0)},
                    {outer1, borderColor, Tex2F(n0)} };

                triangles[ii++] = {
                    {inner0, borderColor, Tex2F(-n0)},
                    {outer0, borderColor, Tex2F(n0)},
                    {outer1, borderColor, Tex2F(n0)} };
            }
            free(extrude);
        }
    }
    if (ii * 3 != vertex_count)
    {
        //       AXLOGD("==============end  ii {}*3: {}  vertex_count: {} tempCount: {}", ii, ii * 3, vertex_count, tempCount);
    }

    _customCommandTriangle.updateVertexBuffer(triangles, _bufferCountTriangle * sizeof(V2F_C4B_T2F), vertex_count * sizeof(V2F_C4B_T2F));
    _bufferCountTriangle += vertex_count;
    _customCommandTriangle.setVertexDrawInfo(0, _bufferCountTriangle);
    _dirtyTriangle = true;

    //  AX_SAFE_DELETE_ARRAY(_vertices);
}

void DrawNodeEx::_drawPoly(const Vec2* verts,
    unsigned int count,
    bool closedPolygon,
    const Color4B& color,
    float thickness)
{
    if (0 /*thickness == 1.0f && !_drawOrder*/)  // usefull for a DrawNode:::_drawPoly thickness = 1.0 only ?
    {
        Vec2* _vertices = transform(verts, count);

        unsigned int vertex_count = (closedPolygon) ? 2 * count : 2 * (count - 1);

        ensureCapacityLine(vertex_count);

        V2F_C4B_T2F* point = _bufferLine + _bufferCountLine;
        V2F_C4B_T2F* cursor = point;

        unsigned int i = 0;
        for (; i < count - 1; i++)
        {
            *point = { _vertices[i], color, Tex2F::ZERO };
            *(point + 1) = { _vertices[i + 1], color, Tex2F::ZERO };
            point += 2;
        }
        if (closedPolygon)
        {
            *point = { _vertices[i], color, Tex2F::ZERO };
            *(point + 1) = { _vertices[0], color, Tex2F::ZERO };
        }

        _customCommandLine.updateVertexBuffer(cursor, _bufferCountLine * sizeof(V2F_C4B_T2F),
            vertex_count * sizeof(V2F_C4B_T2F));
        _bufferCountLine += vertex_count;
        _customCommandLine.setVertexDrawInfo(0, _bufferCountLine);

        AX_SAFE_DELETE_ARRAY(_vertices);
    }
    else
    {
        //bool _clipper = false;
        //if (_clipper)
        //{
        //    // clipper2 is maybe a good idea here
        //    //Clipper2Lib::Path64 subject, clip, solution;
        //    //subject.push_back(MakePath({100, 50, 10, 79, 65, 2, 65, 98, 10, 21}));
        //    //clip.push_back(MakePath({98, 63, 4, 68, 77, 8, 52, 100, 19, 12}));
        //    //solution = Clipper2Lib::Intersect(subject, clip, Clipper2Lib::FillRule::NonZero);


        //    std::vector<Vec2> points;
        //    for (int i = 0; i < count + 1; ++i)
        //    {
        //        points.emplace_back(verts[i].x, verts[i].y);
        //    }

        //    std::vector<Vec2> res = expand(points, Rect(0, 0, 600, 600), 0.5f);

        //    Vec2* ress = new Vec2[res.size()];
        //    int i = 0;
        //    for (auto&& t : res)  // use it later; only one calculate!!!
        //    {
        //        ress[i++] = t;
        //    }

        //    _drawPolygon(ress, res.size(), Color4B::TRANSPARENT, thickness, color, closedPolygon);
        //}


        //  thickness /= _scaleFactor;
        _drawPolygon(verts, count, Color4B::TRANSPARENT, thickness, color, closedPolygon);
    }
}
NS_AX_EXT_END

#if defined(_WIN32)
#    pragma pop_macro("TRANSPARENT")
#endif
