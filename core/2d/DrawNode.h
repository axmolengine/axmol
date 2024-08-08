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

#ifndef __CCDRAWNODES_CCDRAW_NODE_H__
#define __CCDRAWNODES_CCDRAW_NODE_H__

#include "2d/Node.h"
#include "base/Types.h"
#include "renderer/CustomCommand.h"
#include "math/Math.h"
#include "base/any_buffer.h"

NS_AX_BEGIN

static const int DEFAULT_LINE_WIDTH = 2;

class PointArray;
/**
 * @addtogroup _2d
 * @{
 */

/** @class DrawNode
 * @brief Node that draws dots, segments and polygons.
 * Faster than the "drawing primitives" since they draws everything in one single batch.
 * @since v2.1
 */
class AX_DLL DrawNode : public Node
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

    /** creates and initialize a DrawNode node.
     *
     * @return Return an autorelease object.
     */
    static DrawNode* create(float defaultLineWidth = DEFAULT_LINE_WIDTH);

    /** Draw a point.
     *
     * @param point A Vec2 used to point.
     * @param pointSize The point size.
     * @param color The point color.
     * @js NA
     */
    void drawPoint(const Vec2& point, const float pointSize, const Color4B& color);

    /** Draw a group point.
     *
     * @param position A Vec2 pointer.
     * @param numberOfPoints The number of points.
     * @param color The point color.
     * @js NA
     */
    void drawPoints(const Vec2* position, unsigned int numberOfPoints, const Color4B& color);

    /** Draw a group point.
     *
     * @param position A Vec2 pointer.
     * @param numberOfPoints The number of points.
     * @param pointSize The point size.
     * @param color The point color.
     * @js NA
     */
    void drawPoints(const Vec2* position, unsigned int numberOfPoints, const float pointSize, const Color4B& color);

    /** Draw an line from origin to destination with color.
     *
     * @param origin The line origin.
     * @param destination The line destination.
     * @param color The line color.
     * @js NA
     */
    void drawLine(const Vec2& origin, const Vec2& destination, const Color4B& color);

    /** Draws a rectangle given the origin and destination point measured in points.
     * The origin and the destination can not have the same x and y coordinate.
     *
     * @param origin The rectangle origin.
     * @param destination The rectangle destination.
     * @param color The rectangle color.
     */
    void drawRect(const Vec2& origin, const Vec2& destination, const Color4B& color);

    /** Draws a polygon given a pointer to point coordinates and the number of vertices measured in points.
     * The polygon can be closed or open.
     *
     * @param poli A pointer to point coordinates.
     * @param numberOfPoints The number of vertices measured in points.
     * @param closePolygon The polygon can be closed or open.
     * @param color The polygon color.
     */
    void drawPoly(const Vec2* poli, unsigned int numberOfPoints, bool closePolygon, const Color4B& color);

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
    void drawCircle(const Vec2& center,
                    float radius,
                    float angle,
                    unsigned int segments,
                    bool drawLineToCenter,
                    float scaleX,
                    float scaleY,
                    const Color4B& color,
                    float threshold = 500);  // 500 should "simulate/save" the backwards compatibility  

    /** Draws a circle given the center, radius and number of segments.
     *
     * @param center The circle center point.
     * @param radius The circle rotate of radius.
     * @param angle  The circle angle.
     * @param segments The number of segments.
     * @param drawLineToCenter Whether or not draw the line from the origin to center.
     * @param color Set the circle color.
     * @param threshold  (optional) Set the threshold which will be draws a better rendered polygon.
     */
    void drawCircle(const Vec2& center,
                    float radius,
                    float angle,
                    unsigned int segments,
                    bool drawLineToCenter,
                    const Color4B& color,
                    float threshold = 500);  // 500 should "simulate/save" the backwards compatibility

    /** Draws a quad bezier path.
     *
     * @param origin The origin of the bezier path.
     * @param control The control of the bezier path.
     * @param destination The destination of the bezier path.
     * @param segments The number of segments.
     * @param color Set the quad bezier color.
     */
    void drawQuadBezier(const Vec2& origin,
                        const Vec2& control,
                        const Vec2& destination,
                        unsigned int segments,
                        const Color4B& color);

    /** Draw a cubic bezier curve with color and number of segments
     *
     * @param origin The origin of the bezier path.
     * @param control1 The first control of the bezier path.
     * @param control2 The second control of the bezier path.
     * @param destination The destination of the bezier path.
     * @param segments The number of segments.
     * @param color Set the cubic bezier color.
     */
    void drawCubicBezier(const Vec2& origin,
                         const Vec2& control1,
                         const Vec2& control2,
                         const Vec2& destination,
                         unsigned int segments,
                         const Color4B& color);

    /** Draws a Cardinal Spline path.
     *
     * @param config A array point.
     * @param tension The tension of the spline.
     * @param segments The number of segments.
     * @param color Set the Spline color.
     */
    void drawCardinalSpline(PointArray* config, float tension, unsigned int segments, const Color4B& color);

    /** Draws a Catmull Rom path.
     *
     * @param points A point array  of control point.
     * @param segments The number of segments.
     * @param color The Catmull Rom color.
     */
    void drawCatmullRom(PointArray* points, unsigned int segments, const Color4B& color);

    /** draw a dot at a position, with a given radius and color.
     *
     * @param pos The dot center.
     * @param radius The dot radius.
     * @param color The dot color.
     */
    void drawDot(const Vec2& pos, float radius, const Color4B& color);

    /** Draws a rectangle with 4 points.
     *
     * @param p1 The rectangle vertex point.
     * @param p2 The rectangle vertex point.
     * @param p3 The rectangle vertex point.
     * @param p4 The rectangle vertex point.
     * @param color The rectangle color.
     */
    void drawRect(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec2& p4, const Color4B& color);

    /** Draws a solid rectangle given the origin and destination point measured in points.
     * The origin and the destination can not have the same x and y coordinate.
     *
     * @param origin The rectangle origin.
     * @param destination The rectangle destination.
     * @param color The rectangle color.
     * @js NA
     */
    void drawSolidRect(const Vec2& origin, const Vec2& destination, const Color4B& color);

    /** Draws a solid polygon given a pointer to CGPoint coordinates, the number of vertices measured in points, and a
     * color.
     *
     * @param poli A solid polygon given a pointer to CGPoint coordinates.
     * @param numberOfPoints The number of vertices measured in points.
     * @param color The solid polygon color.
     * @js NA
     */
    void drawSolidPoly(const Vec2* poli, unsigned int numberOfPoints, const Color4B& color);

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
    void drawSolidCircle(const Vec2& center,
                         float radius,
                         float angle,
                         unsigned int segments,
                         float scaleX,
                         float scaleY,
                         const Color4B& fillColor,
                         float borderWidth,
                         const Color4B& borderColor);

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
    void drawSolidCircle(const Vec2& center,
                         float radius,
                         float angle,
                         unsigned int segments,
                         float scaleX,
                         float scaleY,
                         const Color4B& color);

    /** Draws a solid circle given the center, radius and number of segments.
     * @param center The circle center point.
     * @param radius The circle rotate of radius.
     * @param angle  The circle angle.
     * @param segments The number of segments.
     * @param color The solid circle color.
     * @js NA
     */
    void drawSolidCircle(const Vec2& center,
                 float radius,
                 float angle,
                 unsigned int segments,
                 const Color4B& color);

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
    void drawPie(const Vec2& center,
                 float radius,
                 float angle,
                 int startAngle,
                 int endAngle,
                 float scaleX,
                 float scaleY,
                 const Color4B& color,
                 DrawMode drawMode);

    /** draw a segment with a radius and color.
     *
     * @param from The segment origin.
     * @param to The segment destination.
     * @param radius The segment radius.
     * @param color The segment color.
     */
    void drawSegment(const Vec2& from, const Vec2& to, float radius, const Color4B& color);

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
    void drawPolygon(const Vec2* verts,
                     int count,
                     const Color4B& fillColor,
                     float borderWidth,
                     const Color4B& borderColor);

    /** draw a triangle with color.
     *
     * @param p1 The triangle vertex point.
     * @param p2 The triangle vertex point.
     * @param p3 The triangle vertex point.
     * @param color The triangle color.
     * @js NA
     */

    void drawTriangle(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Color4B& color);

    /** Clear the geometry in the node's buffer. */
    void clear();
    /** Get the color mixed mode.
     * @lua NA
     */
    const BlendFunc& getBlendFunc() const;
    /** Set the color mixed mode.
     * @code
     * When this function bound into js or lua,the parameter will be changed
     * In js: var setBlendFunc(var src, var dst)
     * @endcode
     * @lua NA
     */
    void setBlendFunc(const BlendFunc& blendFunc);

    // Overrides
    virtual void draw(Renderer* renderer, const Mat4& transform, uint32_t flags) override;

    virtual void visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override;

    void setLineWidth(float lineWidth);

    // Get CocosStudio guide lines width.
    float getLineWidth();

    void setIsConvex(bool isConvex) { _isConvex = isConvex; }; // Set backwards compatible with axmol 2.0

    /**
     * When isolated is set, the position of the node is no longer affected by parent nodes.
     * Which means it will be drawn just like a root node.
     */
    void setIsolated(bool isolated) { _isolated = isolated; }

    bool isIsolated() const { return _isolated; }

    DrawNode(float lineWidth = DEFAULT_LINE_WIDTH);
    virtual ~DrawNode();
    virtual bool init() override;

protected:
    void ensureCapacity(int count);
    void ensureCapacityGLPoint(int count);
    void ensureCapacityGLLine(int count);

    void updateShader();
    void updateShaderInternal(CustomCommand& cmd,
                              uint32_t programType,
                              CustomCommand::DrawType drawType,
                              CustomCommand::PrimitiveType primitiveType);
    void freeShaderInternal(CustomCommand& cmd);

    void setVertexLayout(CustomCommand& cmd);

    void updateBlendState(CustomCommand& cmd);
    void updateUniforms(const Mat4& transform, CustomCommand& cmd);

    int _bufferCapacityTriangle  = 0;
    int _bufferCountTriangle     = 0;
    V2F_C4B_T2F* _bufferTriangle = nullptr;

    int _bufferCapacityPoint  = 0;
    int _bufferCountPoint     = 0;
    V2F_C4B_T2F* _bufferPoint = nullptr;
    Color4F _pointColor;
    int _pointSize = 0;

    int _bufferCapacityLine  = 0;
    int _bufferCountLine     = 0;
    V2F_C4B_T2F* _bufferLine = nullptr;

    BlendFunc _blendFunc;

    CustomCommand _customCommandTriangle;
    CustomCommand _customCommandPoint;
    CustomCommand _customCommandLine;

    bool _dirtyTriangle     = false;
    bool _dirtyPoint        = false;
    bool _dirtyLine         = false;
    bool _isolated          = false;
    float _lineWidth        = 0.0f;
    float _defaultLineWidth = 0.0f;

    ax::any_buffer _abuf;

    bool _isConvex = true;

private:
    AX_DISALLOW_COPY_AND_ASSIGN(DrawNode);
};
/** @} */
NS_AX_END

#endif  // __CCDRAWNODES_CCDRAW_NODE_H__
