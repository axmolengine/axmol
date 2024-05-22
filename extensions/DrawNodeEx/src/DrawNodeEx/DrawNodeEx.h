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

/*
* Code copied & pasted from SpacePatrol game https://github.com/slembcke/SpacePatrol
*
* Renamed and added some changes for cocos2d
*
*/

#ifndef __DRAW_NODE_EX_H__
#define __DRAW_NODE_EX_H__

#include "2d/Node.h"
#include "base/Types.h"
#include "renderer/CustomCommand.h"
#include "math/Math.h"
#include "base/any_buffer.h"

#include "extensions/ExtensionMacros.h"
#include "extensions/ExtensionExport.h"
#include "axmol.h"

NS_AX_EXT_BEGIN

#define DRAWNODE_DRAW_LINE_POINT

static const int DEFAULT_LINEWIDTH = 2;


/** @class DrawNodeExt
* @brief Node that draws dots, segments and polygons.
* Faster than the "drawing primitives" since they draws everything in one single batch.
* @since v2.1
*/
class AX_EX_DLL DrawNodeEx : public ax::Node
{
public:
    /** Different draw modus types.
    *
    *.
    */
    enum DrawMode
    {
        Fill,
        Outline,
        Line,
        Semi,
    };


    // see https://www.angusj.com/clipper2/Docs/Units/Clipper/Types/EndType.htm
    enum EndType
    {
        Square,
        Round,
        Butt,
    };

    enum class Version
    {
        v0,  // DrawNode cocos2dx/axmol 1.0
        v1,  // DrawNode 1.0
        v2,  
        v3,
        v4,
    };

    // DrawNodeExt stuff
    Version _dnVersion = Version::v1;
    ax::Vec2 _dnScale;
    ax::Vec2 _dnCenter;
    float _dnRotation = 0.0f;
    ax::Vec2 _dnPosition;
    float _dnLineWidth = 0.0f;
    float _dnDefaultLineWidth = 0.0f;
    bool _isConvex = true;
    bool _isConvexTmp = _isConvex;
    Version _dnVersionTmp = _dnVersion;
    ax::Vec2 _dnScaleTmp = _dnScale;
    ax::Vec2 _dnCenterTmp = _dnCenter;
    float _dnRotationTmp = _dnRotation;
    ax::Vec2 _dnPositionTmp = _dnPosition;
    float _dnLineWidthTmp = _dnLineWidth;
    bool  _dnTransform = false;

#if defined(DRAWNODE_DRAW_LINE_POINT)
    bool _drawOrder = true;
#endif

    bool swapIsConvex(bool isConvex) {
        _isConvexTmp = _isConvex; _isConvex = isConvex; return _isConvexTmp;
    };

    ax::extension::DrawNodeEx::Version getDNVersion() {
        return _dnVersion;
    };
    void setDNScale(ax::Vec2 scale) {
        _dnScale = scale;
    };
    void setDNScaleX(float scaleX) {
        _dnScale.x = scaleX;
    };
    void setDNScaleY(float scaleY) {
        _dnScale.y = scaleY;
    };
    void setDNRotation(float rotation) {
        _dnRotation = rotation;
    };
    float getDNRotation() {
        return _dnRotation;
    };
    void setDNCenter(ax::Vec2 center) {
        _dnCenter = center;
    };
    ax::Vec2 getDNCenter() {
        return _dnCenter;
    };
    void setDNPosition(ax::Vec2 position) {
        _dnPosition = position;
    };
    ax::Vec2 getDNPosition() {
        return _dnPosition;
    };
    void setDNLineWidth(float lineWidth) {
        _dnLineWidth = lineWidth;
    };
    float getDNLineWidth() {
        return _dnLineWidth;
    };
    ax::Vec2* transform(const ax::Vec2* vertices, unsigned int count);
    void resetDNValues()
    {
        _dnVersion = Version::v2;
        _dnScale = Vec2(1.f, 1.f);
        _dnCenter = Vec2(0.f, 0.f);
        _dnRotation = 0.0f;
        _dnPosition = Vec2(0.f, 0.f);
        _dnLineWidth = 1.0f;
    };
    void ensureDNValues()
    {
        _dnVersionTmp = _dnVersion;
        _dnScaleTmp = _dnScale;
        _dnCenterTmp = _dnCenter;
        _dnRotationTmp = _dnRotation;
        _dnPositionTmp = _dnPosition;
        _dnLineWidthTmp = _dnLineWidth;
    };
    void restoreDNValues()
    {
        _dnVersion = _dnVersionTmp;
        _dnScale = _dnScaleTmp;
        _dnCenter = _dnCenterTmp;
        _dnRotation = _dnRotationTmp;
        _dnPosition = _dnPositionTmp;
        _dnLineWidth = _dnLineWidthTmp;
    };

    /** creates and initialize a DrawNodeExt node.
    *
    * @return Return an autorelease object.
    */
    static DrawNodeEx* create(float defaultLineWidth = DEFAULT_LINEWIDTH);

    /** Draw a point.
    *
    * @param point A Vec2 used to point.
    * @param pointSize The point size.
    * @param color The point color.
    * @js NA
    */
    void drawPoint(const ax::Vec2& point, const float pointSize, const ax::Color4B& color);

    /** Draw a group point.
    *
    * @param position A Vec2 pointer.
    * @param numberOfPoints The number of points.
    * @param color The point color.
    * @js NA
    */
    void drawPoints(const ax::Vec2* position, unsigned int numberOfPoints, const ax::Color4B& color);

    /** Draw a group point.
    *
    * @param position A Vec2 pointer.
    * @param numberOfPoints The number of points.
    * @param pointSize The point size.
    * @param color The point color.
    * @js NA
    */
    void drawPoints(const ax::Vec2* position,
        unsigned int numberOfPoints,
        const float pointSize,
        const ax::Color4B& color);

    /** Draw an line from origin to destination with color.
    *
    * @param origin The line origin.
    * @param destination The line destination.
    * @param color The line color.
    * @js NA
    */
    void drawLine(const ax::Vec2& origin,
        const ax::Vec2& destination,
        const ax::Color4B& color,
        float thickness = 1.0f);

    /** Draws a rectangle given the origin and destination point measured in points.
    * The origin and the destination can not have the same x and y coordinate.
    *
    * @param origin The rectangle origin.
    * @param destination The rectangle destination.
    * @param color The rectangle color.
    */
    void drawRect(const ax::Vec2& origin, const ax::Vec2& destination, const ax::Color4B& color, float thickness = 1);

    /** Draws a polygon given a pointer to point coordinates and the number of vertices measured in points.
    * The polygon can be closed or open.
    *
    * @param poli A pointer to point coordinates.
    * @param numberOfPoints The number of vertices measured in points.
    * @param closePolygon The polygon can be closed or open.
    * @param color The polygon color.
    */
    void drawPoly(const ax::Vec2* poli,
        unsigned int numberOfPoints,
        bool closePolygon,
        const ax::Color4B& color,
        float thickness = 1);
    void _drawPoly(const ax::Vec2* poli,
        unsigned int numberOfPoints,
        bool closePolygon,
        const ax::Color4B& color,
        float thickness = 1);

    /** Draws a circle given the center, radius and number of segments.
    *
    * @param center The circle center point.
    * @param radius The circle rotate of radius.
    * @param angle  The circle angle.
    * @param segments The number of segments.
    * @param drawLineToCenter Whether or not draw the line from the origin to center.
    * @param scaleX The scale value in x.
    * @param scaleY The scale value in y.
    * @param color Set the circle color.
    * @param threshold (optional) Set the threshold which will be draws a better rendered polygon.
    */
    void drawCircle(const ax::Vec2& center,
        float radius,
        float angle,
        unsigned int segments,
        bool drawLineToCenter,
        float scaleX,
        float scaleY,
        const ax::Color4B& color,
        float thickness = 1.0f);  // 500 should "simulate/save" the backwards compatibility

    /** Draws a circle given the center, radius and number of segments.
    *
    * @param center The circle center point.
    * @param radius The circle rotate of radius.
    * @param angle  The circle angle.
    * @param segments The number of segments.
    * @param drawLineToCenter Whether or not draw the line from the origin to center.
    * @param color Set the circle color.
    * @param thickness  (optional) Set the threshold which will be draws a better rendered polygon.
    */
    void drawCircle(const Vec2& center,
        float radius,
        float angle,
        unsigned int segments,
        bool drawLineToCenter,
        const Color4B& color,
        float thickness = 1.0f);  // 500 should "simulate/save" the backwards compatibility

    /** Draws a star given the center, radius1, radius2 and number of segments.
    *
    * @param center The circle center point.
    * @param radiusI The inner radius.
    * @param radiusO The outer radius.
    * @param segments The number of segments.
    * @param color Set the star color.
    * @param thickness  (optional)
    */
    void drawStar(const Vec2& center,
        float radiusI,
        float radiusO,
        unsigned int segments,
        const Color4B& color,
        float thickness = 1.0f);

    void drawSolidStar(const Vec2& center,
        float radiusI,
        float radiusO,
        unsigned int segments,
        const Color4B& color,
        const Color4B& filledColor,
        float thickness = 1.0f);

    /** Draws a quad bezier path.
    *
    * @param origin The origin of the bezier path.
    * @param control The control of the bezier path.
    * @param destination The destination of the bezier path.
    * @param segments The number of segments.
    * @param color Set the quad bezier color.
    */
    void drawQuadBezier(const ax::Vec2& origin,
        const ax::Vec2& control,
        const ax::Vec2& destination,
        unsigned int segments,
        const ax::Color4B& color,
        float thickness = 1.0f);

    /** Draw a cubic bezier curve with color and number of segments
    *
    * @param origin The origin of the bezier path.
    * @param control1 The first control of the bezier path.
    * @param control2 The second control of the bezier path.
    * @param destination The destination of the bezier path.
    * @param segments The number of segments.
    * @param color Set the cubic bezier color.
    */
    void drawCubicBezier(const ax::Vec2& origin,
        const ax::Vec2& control1,
        const ax::Vec2& control2,
        const ax::Vec2& destination,
        unsigned int segments,
        const ax::Color4B& color,
        float thickness = 1.0f);

    /** Draws a Cardinal Spline path.
    *
    * @param config A array point.
    * @param tension The tension of the spline.
    * @param segments The number of segments.
    * @param color Set the Spline color.
    */
    void drawCardinalSpline(ax::PointArray* config,
        float tension,
        unsigned int segments,
        const ax::Color4B& color,
        float thickness = 1.0f);

    /** Draws a Catmull Rom path.
    *
    * @param points A point array  of control point.
    * @param segments The number of segments.
    * @param color The Catmull Rom color.
    */
    void drawCatmullRom(ax::PointArray* points,
        unsigned int segments,
        const ax::Color4B& color,
        float thickness = 1.0f);

    /** draw a dot at a position, with a given radius and color.
    *
    * @param pos The dot center.
    * @param radius The dot radius.
    * @param color The dot color.
    */
    void drawDot(const ax::Vec2& pos, float radius, const ax::Color4B& color);

    /** Draws a rectangle with 4 points.
    *
    * @param p1 The rectangle vertex point.
    * @param p2 The rectangle vertex point.
    * @param p3 The rectangle vertex point.
    * @param p4 The rectangle vertex point.
    * @param color The rectangle color.
    */
    void drawRect(const ax::Vec2& p1,
        const ax::Vec2& p2,
        const ax::Vec2& p3,
        const ax::Vec2& p4,
        const ax::Color4B& color,
        float thickness = 1);

    /** Draws a solid rectangle given the origin and destination point measured in points.
    * The origin and the destination can not have the same x and y coordinate.
    *
    * @param origin The rectangle origin.
    * @param destination The rectangle destination.
    * @param color The rectangle color.
    * @js NA
    */
    void drawSolidRect(const ax::Vec2& origin, const ax::Vec2& destination, const ax::Color4B& color);

    /** Draws a solid polygon given a pointer to CGPoint coordinates, the number of vertices measured in points, and a
    * color.
    *
    * @param poli A solid polygon given a pointer to CGPoint coordinates.
    * @param numberOfPoints The number of vertices measured in points.
    * @param color The solid polygon color.
    * @js NA
    */
    void drawSolidPoly(const ax::Vec2* poli, unsigned int numberOfPoints, const ax::Color4B& color);

    /** Draws a solid circle given the center, radius and number of segments.
    * @param center The circle center point.
    * @param radius The circle rotate of radius.
    * @param angle  The circle angle.
    * @param segments The number of segments.
    * @param scaleX The scale value in x.
    * @param scaleY The scale value in y.
    * @param fillColor The color will fill in polygon.
    * @param borderWidth The border of line width.
    * @param borderColor The border of line color.
    * @js NA
    */
    void drawSolidCircle(const ax::Vec2& center,
        float radius,
        float angle,
        unsigned int segments,
        float scaleX,
        float scaleY,
        const ax::Color4B& fillColor,
        float borderWidth,
        const ax::Color4B& borderColor);

    /** Draws a solid circle given the center, radius and number of segments.
    * @param center The circle center point.
    * @param radius The circle rotate of radius.
    * @param angle  The circle angle.
    * @param segments The number of segments.
    * @param scaleX The scale value in x.
    * @param scaleY The scale value in y.
    * @param color The solid circle color.
    * @js NA
    */
    void drawSolidCircle(const ax::Vec2& center,
        float radius,
        float angle,
        unsigned int segments,
        float scaleX,
        float scaleY,
        const ax::Color4B& color);

    /** Draws a solid circle given the center, radius and number of segments.
    * @param center The circle center point.
    * @param radius The circle rotate of radius.
    * @param angle  The circle angle.
    * @param segments The number of segments.
    * @param color The solid circle color.
    * @js NA
    */
    void drawSolidCircle(const ax::Vec2& center,
        float radius,
        float angle,
        unsigned int segments,
        const ax::Color4B& color);

    /** Draws a pie given the center, radius, angle, start angle, end angle  and number of segments.
    * @param center The circle center point.
    * @param radius The circle rotate of radius.
    * @param angle  The circle angle.
    * @param startAngle The start angle.
    * @param endAngle The end angle.
    * @param scaleX The scale value in x.
    * @param scaleY The scale value in y.
    * @param color The solid circle color.
    * @param DrawMode The draw mode
    * @js NA
    */
    void drawPie(const ax::Vec2& center,
        float radius,
        float rotation,
        int startAngle,
        int endAngle,
        float scaleX,
        float scaleY,
        const ax::Color4B& fillColor,
        const ax::Color4B& borderColor,
        DrawMode drawMode,
        float thickness = 1.0f);

    /** draw a segment with a radius and color.
    *
    * @param from The segment origin.
    * @param to The segment destination.
    * @param radius The segment radius.
    * @param color The segment color.
    * @param endType The segment DrawNodeEx::EndType.
    */
    void drawSegment(const ax::Vec2& from, const ax::Vec2& to, float radius, const ax::Color4B& color, DrawNodeEx::EndType endType = DrawNodeEx::EndType::Round);

    /** draw a polygon with a fill color and line color
    * @code
    * When this function bound into js or lua,the parameter will be changed
    * In js: var drawPolygon(var Arrayofpoints, var fillColor, var width, var borderColor)
    * In lua:local drawPolygon(local pointTable,local tableCount,local fillColor,local width,local borderColor)
    * @endcode
    * @param verts A pointer to point coordinates.
    * @param count The number of verts measured in points.
    * @param fillColor The color will fill in polygon.
    * @param borderWidth The border of line width.
    * @param borderColor The border of line color.
    * @js NA
    */
    void drawPolygon(const ax::Vec2* verts,
        int count,
        const ax::Color4B& fillColor,
        float borderWidth,
        const ax::Color4B& borderColor);

    void drawPolygon(const ax::Vec2* verts, int count, float borderWidth, const ax::Color4B& borderColor);
    void drawSolidPolygon(const ax::Vec2* verts,
        int count,
        const ax::Color4B& fillColor,
        float borderWidth,
        const ax::Color4B& borderColor);

    void _drawPolygon(const ax::Vec2* verts,
        unsigned int count,
        const ax::Color4B& fillColor,
        float borderWidth,
        const ax::Color4B& borderColor,
        bool closedPolygon = true);

    /** draw a triangle with color.
    *
    * @param p1 The triangle vertex point.
    * @param p2 The triangle vertex point.
    * @param p3 The triangle vertex point.
    * @param color The triangle color.
    * @js NA
    */

    void drawTriangle(const ax::Vec2& p1,
        const ax::Vec2& p2,
        const ax::Vec2& p3,
        const ax::Color4B& color,
        float thickness = 1.0f);

    /** Clear the geometry in the node's buffer. */
    void clear();
    /** Get the color mixed mode.
    * @lua NA
    */
    const ax::BlendFunc& getBlendFunc() const;
    /** Set the color mixed mode.
    * @code
    * When this function bound into js or lua,the parameter will be changed
    * In js: var setBlendFunc(var src, var dst)
    * @endcode
    * @lua NA
    */
    void setBlendFunc(const ax::BlendFunc& blendFunc);

    // Overrides
    virtual void draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;

    virtual void visit(ax::Renderer* renderer, const ax::Mat4& parentTransform, uint32_t parentFlags) override;

    void setLineWidth(float lineWidth);
    // Get CocosStudio guide lines width.
    float getLineWidth();

    void setIsConvex(bool isConvex) {
        _isConvex = isConvex;
    };  // Set backwards compatible with cocos2dx/axmol 2.0

    /**
    * When isolated is set, the position of the node is no longer affected by parent nodes.
    * Which means it will be drawn just like a root node.
    */
    void setIsolated(bool isolated) {
        _isolated = isolated;
    }

    bool isIsolated() const {
        return _isolated;
    }

    DrawNodeEx(float lineWidth = DEFAULT_LINEWIDTH);
    virtual ~DrawNodeEx();
    virtual bool init() override;

protected:
    void ensureCapacityTriangle(int count);
#if defined(DRAWNODE_DRAW_LINE_POINT)
    void ensureCapacityPoint(int count);
    void ensureCapacityLine(int count);
#endif

    void updateShader();
    void updateShaderInternal(ax::CustomCommand& cmd,
        uint32_t programType,
        ax::CustomCommand::DrawType drawType,
        ax::CustomCommand::PrimitiveType primitiveType);
    void freeShaderInternal(ax::CustomCommand& cmd);

    void setVertexLayout(ax::CustomCommand& cmd);

    void updateBlendState(ax::CustomCommand& cmd);
    void updateUniforms(const ax::Mat4& transform, ax::CustomCommand& cmd);

    int _bufferCapacityTriangle = 0;
    int _bufferCountTriangle = 0;
    ax::V2F_C4B_T2F* _bufferTriangle = nullptr;
    ax::CustomCommand _customCommandTriangle;
    bool _dirtyTriangle = false;

#if defined(DRAWNODE_DRAW_LINE_POINT)
    int _bufferCapacityPoint = 0;
    int _bufferCountPoint = 0;
    ax::V2F_C4B_T2F* _bufferPoint = nullptr;
    ax::Color4F _pointColor;
    int _pointSize = 0;

    int _bufferCapacityLine = 0;
    int _bufferCountLine = 0;
    ax::V2F_C4B_T2F* _bufferLine = nullptr;


    ax::CustomCommand _customCommandPoint;
    ax::CustomCommand _customCommandLine;
    bool _dirtyPoint = false;
    bool _dirtyLine = false;
#endif

    ax::BlendFunc _blendFunc;

    bool _isolated = false;
    float _lineWidth = 0.0f;
    float _defaultLineWidth = 0.0f;

    ax::any_buffer _abuf;

private:
    AX_DISALLOW_COPY_AND_ASSIGN(DrawNodeEx);
};
/** @} */
NS_AX_EXT_END

#endif  // __DRAW_NODE_EX_H__
