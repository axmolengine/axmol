/* Copyright (c) 2012 Scott Lembcke and Howling Moon Software
 * Copyright (c) 2012 cocos2d-x.org
 * Copyright (c) 2013-2016 Chukong Technologies Inc.
 * Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

#include "2d/CCDrawNode.h"
#include <stddef.h> // offsetof
#include "base/ccTypes.h"
#include "base/CCEventType.h"
#include "base/CCConfiguration.h"
#include "renderer/CCRenderer.h"
#include "base/CCDirector.h"
#include "base/CCEventListenerCustom.h"
#include "base/CCEventDispatcher.h"
#include "2d/CCActionCatmullRom.h"
#include "base/ccUtils.h"
#include "renderer/ccShaders.h"
#include "renderer/backend/ProgramState.h"

NS_CC_BEGIN

static inline Tex2F v2ToTex2F(const Vec2 &v)
{
    return {v.x, v.y};
}

// implementation of DrawNode

DrawNode::DrawNode(float lineWidth)
: _lineWidth(lineWidth)
, _defaultLineWidth(lineWidth)
{
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
#if CC_ENABLE_CACHE_TEXTURE_DATA
    //TODO new-renderer: interface setupBuffer removal

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
    CC_SAFE_FREE(_bufferTriangle);
    CC_SAFE_FREE(_bufferPoint);
    CC_SAFE_FREE(_bufferLine);
    
    freeShaderInternal(_customCommandTriangle);
    freeShaderInternal(_customCommandPoint);
    freeShaderInternal(_customCommandLine);
}

DrawNode* DrawNode::create(float defaultLineWidth)
{
    DrawNode* ret = new (std::nothrow) DrawNode(defaultLineWidth);
    if (ret && ret->init())
    {
        ret->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(ret);
    }
    
    return ret;
}

void DrawNode::ensureCapacity(int count)
{
    CCASSERT(count>=0, "capacity must be >= 0");
    
    if(_bufferCountTriangle + count > _bufferCapacityTriangle)
    {
        _bufferCapacityTriangle += MAX(_bufferCapacityTriangle, count);
        _bufferTriangle = (V2F_C4B_T2F*)realloc(_bufferTriangle, _bufferCapacityTriangle * sizeof(V2F_C4B_T2F));
        
        _customCommandTriangle.createVertexBuffer(sizeof(V2F_C4B_T2F), _bufferCapacityTriangle, CustomCommand::BufferUsage::STATIC);
        _customCommandTriangle.updateVertexBuffer(_bufferTriangle, _bufferCapacityTriangle * sizeof(V2F_C4B_T2F));
    }
}

void DrawNode::ensureCapacityGLPoint(int count)
{
    CCASSERT(count>=0, "capacity must be >= 0");
    
    if(_bufferCountPoint + count > _bufferCapacityPoint)
    {
        _bufferCapacityPoint += MAX(_bufferCapacityPoint, count);
        _bufferPoint = (V2F_C4B_T2F*)realloc(_bufferPoint, _bufferCapacityPoint*sizeof(V2F_C4B_T2F));
        
        _customCommandPoint.createVertexBuffer(sizeof(V2F_C4B_T2F), _bufferCapacityPoint, CustomCommand::BufferUsage::STATIC);
        _customCommandPoint.updateVertexBuffer(_bufferPoint, _bufferCapacityPoint*sizeof(V2F_C4B_T2F));
    }
}

void DrawNode::ensureCapacityGLLine(int count)
{
    CCASSERT(count>=0, "capacity must be >= 0");
    
    if(_bufferCountLine + count > _bufferCapacityLine)
    {
        _bufferCapacityLine += MAX(_bufferCapacityLine, count);
        _bufferLine = (V2F_C4B_T2F*)realloc(_bufferLine, _bufferCapacityLine*sizeof(V2F_C4B_T2F));
        
        _customCommandLine.createVertexBuffer(sizeof(V2F_C4B_T2F), _bufferCapacityLine, CustomCommand::BufferUsage::STATIC);
        _customCommandLine.updateVertexBuffer(_bufferLine, _bufferCapacityLine*sizeof(V2F_C4B_T2F));
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
    _dirtyLine = true;
    _dirtyPoint = true;

    return true;
}

void DrawNode::updateShader()
{
    updateShaderInternal(_customCommandTriangle, backend::ProgramType::POSITION_COLOR_LENGTH_TEXTURE, 
        CustomCommand::DrawType::ARRAY, 
        CustomCommand::PrimitiveType::TRIANGLE);

    updateShaderInternal(_customCommandPoint, backend::ProgramType::POSITION_COLOR_TEXTURE_AS_POINTSIZE,
        CustomCommand::DrawType::ARRAY,
        CustomCommand::PrimitiveType::POINT);

    updateShaderInternal(_customCommandLine, backend::ProgramType::POSITION_COLOR_LENGTH_TEXTURE,
        CustomCommand::DrawType::ARRAY,
        CustomCommand::PrimitiveType::LINE);
}

void DrawNode::updateShaderInternal(CustomCommand& cmd, uint32_t programType, CustomCommand::DrawType drawType, CustomCommand::PrimitiveType primitiveType)
{
    auto& pipelinePS = cmd.getPipelineDescriptor().programState;
    CC_SAFE_RELEASE(pipelinePS);

    auto program = backend::Program::getBuiltinProgram(programType);
    pipelinePS = new (std::nothrow) backend::ProgramState(program);
    setVertexLayout(cmd);
    cmd.setPrimitiveType(primitiveType);
    cmd.setDrawType(drawType);
}

void DrawNode::freeShaderInternal(CustomCommand& cmd)
{
    auto& pipelinePS = cmd.getPipelineDescriptor().programState;
    CC_SAFE_RELEASE_NULL(pipelinePS);
}

void DrawNode::setVertexLayout(CustomCommand& cmd)
{
    auto* programState = cmd.getPipelineDescriptor().programState;
    auto layout = programState->getVertexLayout();
    const auto& attributeInfo = programState->getProgram()->getActiveAttributes();
    auto iter = attributeInfo.find("a_position");
    if(iter != attributeInfo.end())
    {
        layout->setAttribute("a_position", iter->second.location, backend::VertexFormat::FLOAT2, 0, false);
    }
    
    iter = attributeInfo.find("a_texCoord");
    if(iter != attributeInfo.end())
    {
        layout->setAttribute("a_texCoord", iter->second.location, backend::VertexFormat::FLOAT2, offsetof(V2F_C4B_T2F, texCoords), false);
    }
    
    iter = attributeInfo.find("a_color");
    if(iter != attributeInfo.end())
    {
        layout->setAttribute("a_color", iter->second.location, backend::VertexFormat::UBYTE4, offsetof(V2F_C4B_T2F, colors), true);
    }
    layout->setLayout(sizeof(V2F_C4B_T2F));
}

void DrawNode::updateBlendState(CustomCommand& cmd)
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

void DrawNode::updateUniforms(const Mat4 &transform, CustomCommand& cmd)
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

void DrawNode::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
    if(_bufferCountTriangle)
    {
        updateBlendState(_customCommandTriangle);
        updateUniforms(transform, _customCommandTriangle);
        _customCommandTriangle.init(_globalZOrder);
        renderer->addCommand(&_customCommandTriangle);
    }
    
    if(_bufferCountPoint)
    {
        updateBlendState(_customCommandPoint);
        updateUniforms(transform, _customCommandPoint);
        _customCommandPoint.init(_globalZOrder);
        renderer->addCommand(&_customCommandPoint);
    }
    
    if(_bufferCountLine)
    {
        updateBlendState(_customCommandLine);
        updateUniforms(transform, _customCommandLine);
        _customCommandLine.setLineWidth(_lineWidth);
        _customCommandLine.init(_globalZOrder);
        renderer->addCommand(&_customCommandLine);
    }
}

void DrawNode::drawPoint(const Vec2& position, const float pointSize, const Color4F &color)
{
    ensureCapacityGLPoint(1);
    
    V2F_C4B_T2F *point = _bufferPoint + _bufferCountPoint;
    *point = {position, Color4B(color), Tex2F(pointSize,0)};
    
    _customCommandPoint.updateVertexBuffer(point, _bufferCountPoint*sizeof(V2F_C4B_T2F), sizeof(V2F_C4B_T2F));
    _bufferCountPoint += 1;
    _dirtyPoint = true;
    _customCommandPoint.setVertexDrawInfo(0, _bufferCountPoint);
}

void DrawNode::drawPoints(const Vec2 *position, unsigned int numberOfPoints, const Color4F &color)
{
    drawPoints(position, numberOfPoints, 1.0, color);
}

void DrawNode::drawPoints(const Vec2 *position, unsigned int numberOfPoints, const float pointSize, const Color4F &color)
{
    ensureCapacityGLPoint(numberOfPoints);
    
    V2F_C4B_T2F *point = _bufferPoint + _bufferCountPoint;
    for(unsigned int i=0; i < numberOfPoints; i++)
    {
        *(point + i) = {position[i], Color4B(color), Tex2F(pointSize,0)};
    }
    
    _customCommandPoint.updateVertexBuffer(point, _bufferCountPoint*sizeof(V2F_C4B_T2F), numberOfPoints*sizeof(V2F_C4B_T2F));
    _bufferCountPoint += numberOfPoints;
    _dirtyPoint = true;
    _customCommandPoint.setVertexDrawInfo(0, _bufferCountPoint);
}

void DrawNode::drawLine(const Vec2 &origin, const Vec2 &destination, const Color4F &color)
{
    ensureCapacityGLLine(2);
    
    V2F_C4B_T2F *point = _bufferLine + _bufferCountLine;
    
    *point = {origin, Color4B(color), Tex2F(0.0, 0.0)};
    *(point+1) = {destination, Color4B(color), Tex2F(0.0, 0.0)};
    
    _customCommandLine.updateVertexBuffer(point, _bufferCountLine*sizeof(V2F_C4B_T2F), 2*sizeof(V2F_C4B_T2F));
    _bufferCountLine += 2;
    _dirtyLine = true;
    _customCommandLine.setVertexDrawInfo(0, _bufferCountLine);
}

void DrawNode::drawRect(const Vec2 &origin, const Vec2 &destination, const Color4F &color)
{
    drawLine(origin, Vec2(destination.x, origin.y), color);
    drawLine(Vec2(destination.x, origin.y), destination, color);
    drawLine(destination, Vec2(origin.x, destination.y), color);
    drawLine(Vec2(origin.x, destination.y), origin, color);
}

void DrawNode::drawPoly(const Vec2 *poli, unsigned int numberOfPoints, bool closePolygon, const Color4F &color)
{
    unsigned int vertex_count;
    if(closePolygon)
    {
        vertex_count = 2 * numberOfPoints;
        ensureCapacityGLLine(vertex_count);
    }
    else
    {
        vertex_count = 2 * (numberOfPoints - 1);
        ensureCapacityGLLine(vertex_count);
    }
    
    V2F_C4B_T2F *point = _bufferLine + _bufferCountLine;
    V2F_C4B_T2F *cursor = point;
    
    unsigned int i = 0;
    for(; i < numberOfPoints - 1; i++)
    {
        *point = {poli[i], Color4B(color), Tex2F(0.0, 0.0)};
        *(point + 1) = {poli[i+1], Color4B(color), Tex2F(0.0, 0.0)};
        point += 2;
    }
    if(closePolygon)
    {
        *point = {poli[i], Color4B(color), Tex2F(0.0, 0.0)};
        *(point + 1) = {poli[0], Color4B(color), Tex2F(0.0, 0.0)};
    }
    
    _customCommandLine.updateVertexBuffer(cursor, _bufferCountLine*sizeof(V2F_C4B_T2F), vertex_count*sizeof(V2F_C4B_T2F));
    _bufferCountLine += vertex_count;
    _customCommandLine.setVertexDrawInfo(0, _bufferCountLine);
}

void DrawNode::drawCircle(const Vec2& center, float radius, float angle, unsigned int segments, bool drawLineToCenter, float scaleX, float scaleY, const Color4F &color)
{
    const float coef = 2.0f * (float)M_PI/segments;
    
    Vec2 *vertices = new (std::nothrow) Vec2[segments+2];
    if( ! vertices )
        return;
    
    for(unsigned int i = 0;i <= segments; i++) {
        float rads = i*coef;
        float j = radius * cosf(rads + angle) * scaleX + center.x;
        float k = radius * sinf(rads + angle) * scaleY + center.y;
        
        vertices[i].x = j;
        vertices[i].y = k;
    }
    if(drawLineToCenter)
    {
        vertices[segments+1].x = center.x;
        vertices[segments+1].y = center.y;
        drawPoly(vertices, segments+2, true, color);
    }
    else
        drawPoly(vertices, segments+1, true, color);
    
    CC_SAFE_DELETE_ARRAY(vertices);
}

void DrawNode::drawCircle(const Vec2 &center, float radius, float angle, unsigned int segments, bool drawLineToCenter, const Color4F &color)
{
    drawCircle(center, radius, angle, segments, drawLineToCenter, 1.0f, 1.0f, color);
}

void DrawNode::drawQuadBezier(const Vec2 &origin, const Vec2 &control, const Vec2 &destination, unsigned int segments, const Color4F &color)
{
    Vec2* vertices = new (std::nothrow) Vec2[segments + 1];
    if( ! vertices )
        return;
    
    float t = 0.0f;
    for(unsigned int i = 0; i < segments; i++)
    {
        vertices[i].x = powf(1 - t, 2) * origin.x + 2.0f * (1 - t) * t * control.x + t * t * destination.x;
        vertices[i].y = powf(1 - t, 2) * origin.y + 2.0f * (1 - t) * t * control.y + t * t * destination.y;
        t += 1.0f / segments;
    }
    vertices[segments].x = destination.x;
    vertices[segments].y = destination.y;
    
    drawPoly(vertices, segments+1, false, color);

    CC_SAFE_DELETE_ARRAY(vertices);
}

void DrawNode::drawCubicBezier(const Vec2 &origin, const Vec2 &control1, const Vec2 &control2, const Vec2 &destination, unsigned int segments, const Color4F &color)
{
    Vec2* vertices = new (std::nothrow) Vec2[segments + 1];
    if( ! vertices )
        return;
    
    float t = 0;
    for (unsigned int i = 0; i < segments; i++)
    {
        vertices[i].x = powf(1 - t, 3) * origin.x + 3.0f * powf(1 - t, 2) * t * control1.x + 3.0f * (1 - t) * t * t * control2.x + t * t * t * destination.x;
        vertices[i].y = powf(1 - t, 3) * origin.y + 3.0f * powf(1 - t, 2) * t * control1.y + 3.0f * (1 - t) * t * t * control2.y + t * t * t * destination.y;
        t += 1.0f / segments;
    }
    vertices[segments].x = destination.x;
    vertices[segments].y = destination.y;
    
    drawPoly(vertices, segments+1, false, color);

    CC_SAFE_DELETE_ARRAY(vertices);
}

void DrawNode::drawCardinalSpline(PointArray *config, float tension,  unsigned int segments, const Color4F &color)
{
    Vec2* vertices = new (std::nothrow) Vec2[segments + 1];
    if( ! vertices )
        return;
    
    ssize_t p;
    float lt;
    float deltaT = 1.0f / config->count();
    
    for( unsigned int i=0; i < segments+1;i++) {
        
        float dt = (float)i / segments;
        
        // border
        if( dt == 1 ) {
            p = config->count() - 1;
            lt = 1;
        } else {
            p = static_cast<ssize_t>(dt / deltaT);
            lt = (dt - deltaT * (float)p) / deltaT;
        }
        
        // Interpolate
        Vec2 pp0 = config->getControlPointAtIndex(p-1);
        Vec2 pp1 = config->getControlPointAtIndex(p+0);
        Vec2 pp2 = config->getControlPointAtIndex(p+1);
        Vec2 pp3 = config->getControlPointAtIndex(p+2);
        
        Vec2 newPos = ccCardinalSplineAt( pp0, pp1, pp2, pp3, tension, lt);
        vertices[i].x = newPos.x;
        vertices[i].y = newPos.y;
    }
    
    drawPoly(vertices, segments+1, false, color);
    
    CC_SAFE_DELETE_ARRAY(vertices);
}

void DrawNode::drawCatmullRom(PointArray *points, unsigned int segments, const Color4F &color)
{
    drawCardinalSpline( points, 0.5f, segments, color);
}

void DrawNode::drawDot(const Vec2 &pos, float radius, const Color4F &color)
{
    unsigned int vertex_count = 2*3;
    ensureCapacity(vertex_count);
    
    V2F_C4B_T2F a = {Vec2(pos.x - radius, pos.y - radius), Color4B(color), Tex2F(-1.0, -1.0) };
    V2F_C4B_T2F b = {Vec2(pos.x - radius, pos.y + radius), Color4B(color), Tex2F(-1.0,  1.0) };
    V2F_C4B_T2F c = {Vec2(pos.x + radius, pos.y + radius), Color4B(color), Tex2F( 1.0,  1.0) };
    V2F_C4B_T2F d = {Vec2(pos.x + radius, pos.y - radius), Color4B(color), Tex2F( 1.0, -1.0) };
    
    V2F_C4B_T2F_Triangle *triangles = (V2F_C4B_T2F_Triangle *)(_bufferTriangle + _bufferCountTriangle);
    V2F_C4B_T2F_Triangle triangle0 = {a, b, c};
    V2F_C4B_T2F_Triangle triangle1 = {a, c, d};
    triangles[0] = triangle0;
    triangles[1] = triangle1;
    
    _customCommandTriangle.updateVertexBuffer(triangles, _bufferCountTriangle * sizeof(V2F_C4B_T2F), vertex_count*sizeof(V2F_C4B_T2F));
    _bufferCountTriangle += vertex_count;
    _dirtyTriangle = true;
    _customCommandTriangle.setVertexDrawInfo(0, _bufferCountTriangle);
}

void DrawNode::drawRect(const Vec2 &p1, const Vec2 &p2, const Vec2 &p3, const Vec2& p4, const Color4F &color)
{
    drawLine(p1, p2, color);
    drawLine(p2, p3, color);
    drawLine(p3, p4, color);
    drawLine(p4, p1, color);
}

void DrawNode::drawSegment(const Vec2 &from, const Vec2 &to, float radius, const Color4F &color)
{
    unsigned int vertex_count = 6*3;
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
    
    
    V2F_C4B_T2F_Triangle *triangles = (V2F_C4B_T2F_Triangle *)(_bufferTriangle + _bufferCountTriangle);
    
    V2F_C4B_T2F_Triangle triangles0 = {
        {v0, Color4B(color), v2ToTex2F(-(n + t))},
        {v1, Color4B(color), v2ToTex2F(n - t)},
        {v2, Color4B(color), v2ToTex2F(-n)},
    };
    triangles[0] = triangles0;

    V2F_C4B_T2F_Triangle triangles1 = {
        {v3, Color4B(color), v2ToTex2F(n)},
        {v1, Color4B(color), v2ToTex2F(n - t)},
        {v2, Color4B(color), v2ToTex2F(-n)},
    };
    triangles[1] = triangles1;

    V2F_C4B_T2F_Triangle triangles2 = {
        {v3, Color4B(color), v2ToTex2F(n)},
        {v4, Color4B(color), v2ToTex2F(-n)},
        {v2, Color4B(color), v2ToTex2F(-n)},
    };
    triangles[2] = triangles2;

    V2F_C4B_T2F_Triangle triangles3 = {
        {v3, Color4B(color), v2ToTex2F(n)},
        {v4, Color4B(color), v2ToTex2F(-n)},
        {v5, Color4B(color), v2ToTex2F(n) },
    };
    triangles[3] = triangles3;

    V2F_C4B_T2F_Triangle triangles4 = {
        {v6, Color4B(color), v2ToTex2F(t - n)},
        {v4, Color4B(color), v2ToTex2F(-n) },
        {v5, Color4B(color), v2ToTex2F(n)},
    };
    triangles[4] = triangles4;

    V2F_C4B_T2F_Triangle triangles5 = {
        {v6, Color4B(color), v2ToTex2F(t - n)},
        {v7, Color4B(color), v2ToTex2F(t + n)},
        {v5, Color4B(color), v2ToTex2F(n)},
    };
    triangles[5] = triangles5;
    
    _customCommandTriangle.updateVertexBuffer(triangles, _bufferCountTriangle * sizeof(V2F_C4B_T2F), vertex_count*sizeof(V2F_C4B_T2F));
    _bufferCountTriangle += vertex_count;
    _dirtyTriangle = true;
    _customCommandTriangle.setVertexDrawInfo(0, _bufferCountTriangle);
}

void DrawNode::drawPolygon(const Vec2 *verts, int count, const Color4F &fillColor, float borderWidth, const Color4F &borderColor)
{
    CCASSERT(count >= 0, "invalid count value");
    
    bool outline = (borderColor.a > 0.0f && borderWidth > 0.0f);
    
    auto  triangle_count = outline ? (3*count - 2) : (count - 2);
    auto vertex_count = 3*triangle_count;
    ensureCapacity(vertex_count);
    
    V2F_C4B_T2F_Triangle *triangles = (V2F_C4B_T2F_Triangle *)(_bufferTriangle + _bufferCountTriangle);
    V2F_C4B_T2F_Triangle *cursor = triangles;
    
    for (int i = 0; i < count-2; i++)
    {
        V2F_C4B_T2F_Triangle tmp = {
            {verts[0], Color4B(fillColor), v2ToTex2F(Vec2::ZERO)},
            {verts[i+1], Color4B(fillColor), v2ToTex2F(Vec2::ZERO)},
            {verts[i+2], Color4B(fillColor), v2ToTex2F(Vec2::ZERO)},
        };
        
        *cursor++ = tmp;
    }
    
    if(outline)
    {
        struct ExtrudeVerts {Vec2 offset, n;};
        struct ExtrudeVerts* extrude = (struct ExtrudeVerts*)malloc(sizeof(struct ExtrudeVerts) * count);
        
        for (int i = 0; i < count; i++)
        {
            Vec2 v0 = verts[(i-1+count)%count];
            Vec2 v1 = verts[i];
            Vec2 v2 = verts[(i+1)%count];
            
            Vec2 n1 = ((v1 - v0).getPerp()).getNormalized();
            Vec2 n2 = ((v2 - v1).getPerp()).getNormalized();
            
            Vec2 offset = (n1 + n2) * (1.0f / (Vec2::dot(n1, n2) + 1.0f));
            extrude[i] = {offset, n2};
        }
        
        for(int i = 0; i < count; i++)
        {
            int j = (i+1)%count;
            Vec2 v0 = verts[i];
            Vec2 v1 = verts[j];
            
            Vec2 n0 = extrude[i].n;
            
            Vec2 offset0 = extrude[i].offset;
            Vec2 offset1 = extrude[j].offset;
            
            Vec2 inner0 = v0 - offset0 * borderWidth;
            Vec2 inner1 = v1 - offset1 * borderWidth;
            Vec2 outer0 = v0 + offset0 * borderWidth;
            Vec2 outer1 = v1 + offset1 * borderWidth;
            
            V2F_C4B_T2F_Triangle tmp1 = {
                {inner0, Color4B(borderColor), v2ToTex2F(-n0)},
                {inner1, Color4B(borderColor), v2ToTex2F(-n0)},
                {outer1, Color4B(borderColor), v2ToTex2F(n0)}
            };
            *cursor++ = tmp1;
            
            V2F_C4B_T2F_Triangle tmp2 = {
                {inner0, Color4B(borderColor), v2ToTex2F(-n0)},
                {outer0, Color4B(borderColor), v2ToTex2F(n0)},
                {outer1, Color4B(borderColor), v2ToTex2F(n0)}
            };
            *cursor++ = tmp2;
        }
        
        free(extrude);
    }
    
    _customCommandTriangle.updateVertexBuffer(triangles, _bufferCountTriangle * sizeof(V2F_C4B_T2F), vertex_count * sizeof(V2F_C4B_T2F));
    _bufferCountTriangle += vertex_count;
    _customCommandTriangle.setVertexDrawInfo(0, _bufferCountTriangle);
    _dirtyTriangle = true;
}

void DrawNode::drawSolidRect(const Vec2 &origin, const Vec2 &destination, const Color4F &color)
{
    Vec2 vertices[] = {
        origin,
        Vec2(destination.x, origin.y),
        destination,
        Vec2(origin.x, destination.y)
    };
    
    drawSolidPoly(vertices, 4, color);
}

void DrawNode::drawSolidPoly(const Vec2 *poli, unsigned int numberOfPoints, const Color4F &color)
{
    drawPolygon(poli, numberOfPoints, color, 0.0, Color4F());
}

void DrawNode::drawSolidCircle(const Vec2& center, float radius, float angle, unsigned int segments, float scaleX, float scaleY, const Color4F &color)
{
    const float coef = 2.0f * (float)M_PI/segments;
    
    Vec2 *vertices = new (std::nothrow) Vec2[segments];
    if( ! vertices )
        return;
    
    for(unsigned int i = 0;i < segments; i++)
    {
        float rads = i*coef;
        float j = radius * cosf(rads + angle) * scaleX + center.x;
        float k = radius * sinf(rads + angle) * scaleY + center.y;
        
        vertices[i].x = j;
        vertices[i].y = k;
    }
    
    drawSolidPoly(vertices, segments, color);
    
    CC_SAFE_DELETE_ARRAY(vertices);
}

void DrawNode::drawSolidCircle( const Vec2& center, float radius, float angle, unsigned int segments, const Color4F& color)
{
    drawSolidCircle(center, radius, angle, segments, 1.0f, 1.0f, color);
}

void DrawNode::drawTriangle(const Vec2 &p1, const Vec2 &p2, const Vec2 &p3, const Color4F &color)
{
    unsigned int vertex_count = 3;
    ensureCapacity(vertex_count);

    Color4B col = Color4B(color);
    V2F_C4B_T2F a = {p1, col, Tex2F(0.0, 0.0) };
    V2F_C4B_T2F b = {p2, col, Tex2F(0.0,  0.0) };
    V2F_C4B_T2F c = {p3, col, Tex2F(0.0,  0.0) };

    V2F_C4B_T2F_Triangle *triangles = (V2F_C4B_T2F_Triangle *)(_bufferTriangle + _bufferCountTriangle);
    V2F_C4B_T2F_Triangle triangle = {a, b, c};
    triangles[0] = triangle;

    _customCommandTriangle.updateVertexBuffer(triangles, _bufferCountTriangle * sizeof(V2F_C4B_T2F), vertex_count*sizeof(V2F_C4B_T2F));
    _bufferCountTriangle += vertex_count;
    _dirtyTriangle = true;
    _customCommandTriangle.setVertexDrawInfo(0, _bufferCountTriangle);
}

void DrawNode::clear()
{
    _bufferCountTriangle = 0;
    _dirtyTriangle = true;
    _bufferCountLine = 0;
    _dirtyLine = true;
    _bufferCountPoint = 0;
    _dirtyPoint = true;
    _lineWidth = _defaultLineWidth;
}

const BlendFunc& DrawNode::getBlendFunc() const
{
    return _blendFunc;
}

void DrawNode::setBlendFunc(const BlendFunc &blendFunc)
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

void DrawNode::visit(Renderer* renderer, const Mat4 &parentTransform, uint32_t parentFlags)
{
    if (_isolated)
    {
        //ignore `parentTransform` from parent
        Node::visit(renderer, Mat4::IDENTITY, parentFlags);
    }
    else
    {
        Node::visit(renderer, parentTransform, parentFlags);
    }
}

NS_CC_END
