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

#ifndef __DRAW_NODE_H__
#define __DRAW_NODE_H__

#include "2d/Node.h"
#include "base/axstd.h"
#include "base/Types.h"
#include "renderer/CustomCommand.h"
#include "math/Math.h"

namespace ax
{

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

    enum PointType
    {
        Circle,
        Rect,
    };

    // See also example on https://www.angusj.com/clipper2/Docs/Units/Clipper/Types/EndType.htm
    enum EndType
    {
        Square,
        Round,
        Butt,
    };

    /** creates and initialize a DrawNode node.
     *
     * @return Return an autorelease object.
     */
    static DrawNode* create(void);
    // DrawNode();

    /** Draw a point.
     *
     * @param point A Vec2 used to point.
     * @param pointSize The point size.
     * @param color The point color.
     * @js NA
     */
    void drawPoint(const Vec2& point,
                   const float pointSize,
                   const Color4B& color,
                   DrawNode::PointType pointType = DrawNode::PointType::Rect);

    /** Draw a group point.
     *
     * @param position A Vec2 pointer.
     * @param numberOfPoints The number of points.
     * @param color The point color.
     * @js NA
     */
    void drawPoints(const Vec2* position,
                    unsigned int numberOfPoints,
                    const Color4B& color,
                    DrawNode::PointType pointType = DrawNode::PointType::Rect);

    /** Draw a group point.
     *
     * @param position A Vec2 pointer.
     * @param numberOfPoints The number of points.
     * @param pointSize The point size.
     * @param color The point color.
     * @js NA
     */
    void drawPoints(const Vec2* position,
                    unsigned int numberOfPoints,
                    const float pointSize,
                    const Color4B& color,
                    DrawNode::PointType pointType = DrawNode::PointType::Rect);

    /** Draw an line from origin to destination with color.
     *
     * @param origin The line origin.
     * @param destination The line destination.
     * @param color The line color.
     * @js NA
     */
    void drawLine(const Vec2& origin,
                  const Vec2& destination,
                  const Color4B& color,
                  float thickness           = 1.0f,
                  DrawNode::EndType etStart = DrawNode::EndType::Round,
                  DrawNode::EndType etEnd   = DrawNode::EndType::Round);

    /** Draws a rectangle given the origin and destination point measured in points.
     * The origin and the destination can not have the same x and y coordinate.
     *
     * @param origin The rectangle origin.
     * @param destination The rectangle destination.
     * @param color The rectangle color.
     */
    void drawRect(const Vec2& origin, const Vec2& destination, const Color4B& color, float thickness = 1.0f);

    /** Draws a polygon given a pointer to point coordinates and the number of vertices measured in points.
     * The polygon can be closed or open.
     *
     * @param poli A pointer to point coordinates.
     * @param numberOfPoints The number of vertices measured in points.
     * @param closePolygon The polygon can be closed or open.
     * @param color The polygon color.
     */
    void drawPoly(const Vec2* poli,
                  unsigned int numberOfPoints,
                  bool closedPolygon,
                  const Color4B& color,
                  float thickness = 1.0f);

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
     * @param thickness  (default 1.0f)
     */
    void drawCircle(const Vec2& center,
                    float radius,
                    float angle,
                    unsigned int segments,
                    bool drawLineToCenter,
                    float scaleX,
                    float scaleY,
                    const Color4B& color,
                    float thickness = 1.0f);

    /** Draws a circle given the center, radius and number of segments.
     *
     * @param center The circle center point.
     * @param radius The circle rotate of radius.
     * @param angle  The circle angle.
     * @param segments The number of segments.
     * @param drawLineToCenter Whether or not draw the line from the origin to center.
     * @param color Set the circle color.
     * @param thickness  (default 1.0f)
     */
    void drawCircle(const Vec2& center,
                    float radius,
                    float angle,
                    unsigned int segments,
                    bool drawLineToCenter,
                    const Color4B& color,
                    float thickness = 1.0f);

    /** Draws a star given the center, radiusI, radiusO and number of segments.
     *
     * @param center The circle center point.
     * @param radiusI The inner radius.
     * @param radiusO The outer radius.
     * @param segments The number of segments.
     * @param color Set the star color.
     * @param thickness  (default = 1.0f)
     */
    void drawStar(const Vec2& center,
                  float radiusI,
                  float radiusO,
                  unsigned int segments,
                  const Color4B& color,
                  float thickness = 1.0f);

    /** Draws a solid star given the center, radiusI, radiusO and number of segments.
     *
     * @param center The circle center point.
     * @param radiusI The inner radius.
     * @param radiusO The outer radius.
     * @param segments The number of segments.
     * @param color Set the star color.
     * @param thickness  (default = 1.0f)
     */
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
    void drawQuadBezier(const Vec2& origin,
                        const Vec2& control,
                        const Vec2& destination,
                        unsigned int segments,
                        const Color4B& color,
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
    void drawCubicBezier(const Vec2& origin,
                         const Vec2& control1,
                         const Vec2& control2,
                         const Vec2& destination,
                         unsigned int segments,
                         const Color4B& color,
                         float thickness = 1.0f);

    /** Draws a Cardinal Spline path.
     *
     * @param config A array point.
     * @param tension The tension of the spline.
     * @param segments The number of segments.
     * @param color Set the Spline color.
     */
    void drawCardinalSpline(PointArray* config,
                            float tension,
                            unsigned int segments,
                            const Color4B& color,
                            float thickness = 1.0f);

    /** Draws a Catmull Rom path.
     *
     * @param points A point array  of control point.
     * @param segments The number of segments.
     * @param color The Catmull Rom color.
     */
    void drawCatmullRom(PointArray* points, unsigned int segments, const Color4B& color, float thickness = 1.0f);

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
    void drawRect(const Vec2& p1,
                  const Vec2& p2,
                  const Vec2& p3,
                  const Vec2& p4,
                  const Color4B& color,
                  float thickness = 1.0f);

    /** Draws a solid rectangle given the origin and destination point measured in points.
     * The origin and the destination can not have the same x and y coordinate.
     *
     * @param origin The rectangle origin.
     * @param destination The rectangle destination.
     * @param color The rectangle color.
     * @js NA
     */
    void drawSolidRect(const Vec2& origin,
                       const Vec2& destination,
                       const Color4B& color,
                       float thickness            = 0,
                       const Color4B& borderColor = Color4B(0, 0, 0, 0));

    /** Draws a solid polygon given a pointer to CGPoint coordinates, the number of vertices measured in points, and a
     * color.
     *
     * @param poli A solid polygon given a pointer to CGPoint coordinates.
     * @param numberOfPoints The number of vertices measured in points.
     * @param color The solid polygon color.
     * @js NA
     */
    void drawSolidPoly(const Vec2* poli,
                       unsigned int numberOfPoints,
                       const Color4B& color,
                       float thickness            = 0,
                       const Color4B& borderColor = Color4B(0, 0, 0, 0),
                       bool isconvex              = false);

    /** Draws a solid circle given the center, radius and number of segments.
     * @param center The circle center point.
     * @param radius The circle rotate of radius.
     * @param angle  The circle angle.
     * @param segments The number of segments.
     * @param scaleX The scale value in x.
     * @param scaleY The scale value in y.
     * @param fillColor The color will fill in polygon.
     * @param thickness The border of line width.
     * @param borderColor The border of line color.
     * @param drawLineToCenter Whether or not draw the line from the origin to center.
     * @js NA
     */
    void drawSolidCircle(const Vec2& center,
                         float radius,
                         float angle,
                         unsigned int segments,
                         float scaleX,
                         float scaleY,
                         const Color4B& fillColor,
                         float thickness,
                         const Color4B& borderColor,
                         bool drawLineToCenter = false);

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
    void drawSolidCircle(const Vec2& center, float radius, float angle, unsigned int segments, const Color4B& color);

    /** Draws a pie given the center, radius, angle, start angle, end angle  and number of segments.
     * @param center The circle center point.
     * @param radius The circle rotate of radius.
     * @param angle  The circle angle.
     * @param startAngle The start angle.
     * @param endAngle The end angle.
     * @param scaleX The scale value in x.
     * @param scaleY The scale value in y.
     * @param fillColor The solid circle color.
     * @param borderColor The borderColor.
     * @param DrawMode The draw mode
     * @js NA
     */
    void drawPie(const Vec2& center,
                 float radius,
                 float rotation,
                 int startAngle,
                 int endAngle,
                 float scaleX,
                 float scaleY,
                 const Color4B& fillColor,
                 const Color4B& borderColor,
                 DrawMode drawMode = DrawMode::Outline,
                 float thickness = 1.0f);

    // Cocos2dx/Axmol 1.0 API backwards compatibhility
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
                 DrawMode drawMode = DrawMode::Outline);

    void setIsConvex(bool isConvex)
    {
        AXLOGW("'setIsConvex()' No longer supported. Use the new drawPolygon API.");
    };


     /** draw a segment with a radius and color.
     *
     * @param from The segment origin.
     * @param to The segment destination.
     * @param radius The segment radius.
     * @param color The segment color.
     */
    void drawSegment(const Vec2& from,
                     const Vec2& to,
                     float radius,
                     const Color4B& color,
                     DrawNode::EndType etStart = DrawNode::EndType::Round,
                     DrawNode::EndType etEnd   = DrawNode::EndType::Round);

    /** draw a polygon with a fill color and line color
     * @code
     * When this function bound into js or lua,the parameter will be changed
     * In js: var drawPolygon(var Arrayofpoints, var fillColor, var width, var borderColor)
     * In lua:local drawPolygon(local pointTable,local tableCount,local fillColor,local width,local borderColor)
     * @endcode
     * @param verts A pointer to point coordinates.
     * @param count The number of verts measured in points.
     * @param fillColor The color will fill in polygon.
     * @param thickness The border of line width.
     * @param borderColor The border of line color.
     * @js NA
     */
    void drawPolygon(Vec2* verts,
                     int count,
                     const Color4B& fillColor,
                     float thickness,
                     const Color4B& borderColor,
                     bool isconvex = false);

    void drawPolygon(Vec2* verts, int count, float thickness, const Color4B& borderColor, bool isconvex = false);

    void drawSolidPolygon(Vec2* verts,
                          int count,
                          const Color4B& fillColor,
                          float thickness,
                          const Color4B& borderColor,
                          bool isconvex = false);

    /** draw a triangle with color.
     *
     * @param p1 The triangle vertex point.
     * @param p2 The triangle vertex point.
     * @param p3 The triangle vertex point.
     * @param color The triangle color.
     * @js NA
     */

    void drawTriangle(const Vec2* vertices3, const Color4B& color);

    void drawTriangle(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Color4B& color);

    void drawSolidTriangle(const Vec2* vertices3,
                           const Color4B& fillColor,
                           const Color4B& borderColor,
                           float thickness = 1.0f);

    void drawSolidTriangle(const Vec2& p1,
                           const Vec2& p2,
                           const Vec2& p3,
                           const Color4B& fillColor,
                           const Color4B& borderColor,
                           float thickness = 1.0f);

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

    /**
     * When isolated is set, the position of the node is no longer affected by parent nodes.
     * Which means it will be drawn just like a root node.
     */
    void setIsolated(bool isolated) { _isolated = isolated; }

    bool isIsolated() const { return _isolated; }

    DrawNode();
    virtual ~DrawNode();
    virtual bool init() override;

protected:
    void updateBuffers();
    void updateShader();
    void updateShaderInternal(CustomCommand& cmd,
                              uint32_t programType,
                              CustomCommand::DrawType drawType,
                              CustomCommand::PrimitiveType primitiveType);
    void freeShaderInternal(CustomCommand& cmd);

    void setVertexLayout(CustomCommand& cmd);

    void updateBlendState(CustomCommand& cmd);
    void updateUniforms(const Mat4& transform, CustomCommand& cmd);

    bool _trianglesDirty: 1 = false;
    bool _pointsDirty: 1 = false;
    bool _linesDirty: 1 = false;

    bool _isolated: 1 = false;

    BlendFunc _blendFunc;

    CustomCommand _customCommandTriangle;
    CustomCommand _customCommandPoint;
    CustomCommand _customCommandLine;

    axstd::pod_vector<V2F_C4B_T2F> _triangles;
    axstd::pod_vector<V2F_C4B_T2F> _points;
    axstd::pod_vector<V2F_C4B_T2F> _lines;


private:
    // Internal function _drawPoint
    void _drawPoint(const Vec2& position,
                    const float pointSize,
                    const Color4B& color,
                    const DrawNode::PointType pointType);

    // Internal function _drawPoints
    void _drawPoints(const Vec2* position,
                     unsigned int numberOfPoints,
                     const float pointSize,
                     const Color4B& color,
                     const DrawNode::PointType pointType);

    // Internal function _drawDot
    void _drawDot(const Vec2& pos, float radius, const Color4B& color);

    // Internal function _drawTriangle
    // Note: modifies supplied vertex array
    void _drawTriangle(Vec2* vertices3,
                       const Color4B& borderColor,
                       const Color4B& fillColor,
                       bool solid      = true,
                       float thickness = 0.0f);

    // Internal function _drawAStar
    void _drawAStar(const Vec2& center,
                    float radiusI,
                    float radiusO,
                    unsigned int segments,
                    const Color4B& color,
                    const Color4B& filledColor,
                    float thickness = 1.0f,
                    bool solid      = false);

    // Internal function _drawPoly
    void _drawPoly(const Vec2* poli,
                   unsigned int numberOfPoints,
                   bool closedPolygon,
                   const Color4B& color,
                   float thickness = 1.0f,
                   bool isconvex   = true);

    // Internal function _drawPolygon
    void _drawPolygon(const Vec2* verts,
                      unsigned int count,
                      const Color4B& fillColor,
                      const Color4B& borderColor,
                      bool closedPolygon = true,
                      float thickness    = 1.0f,
                      bool isconvex      = true);

    // Internal function _drawSegment
    void _drawSegment(const Vec2& origin,
                      const Vec2& destination,
                      const Color4B& color,
                      float thickness           = 1.0f,
                      DrawNode::EndType etStart = DrawNode::EndType::Square,
                      DrawNode::EndType etEnd   = DrawNode::EndType::Square);

    // Internal function _drawCircle
    void _drawCircle(const Vec2& center,
                     float radius,
                     float angle,
                     unsigned int segments,
                     bool drawLineToCenter,
                     float scaleX,
                     float scaleY,
                     const Color4B& borderColor,
                     const Color4B& fillColor,
                     bool solid,
                     float thickness = 1.0f);

    // Internal function _drawPie
    void _drawPie(const Vec2& center,
                  float radius,
                  float rotation,
                  int startAngle,
                  int endAngle,
                  float scaleX,
                  float scaleY,
                  const Color4B& fillColor,
                  const Color4B& borderColor,
                  DrawMode drawMode,
                  float thickness = 1.0f);

    /* Internal function _transform
     * @param vertices A Vec2 vertices list.
     * @param count The number of vertices.
     * @param closedPolygon The closedPolygon flag.
     * @js NA
     */
    axstd::pod_vector<Vec2> _transform(const Vec2* vertices, unsigned int& count, bool closedPolygon = false);

    void applyTransform(const Vec2* from, Vec2* to, unsigned int count);

private:
    AX_DISALLOW_COPY_AND_ASSIGN(DrawNode);

public:
    class Properties
    {
    public:
        float factor = 0.5f;  /// thickness factor

        // transforming stuff
        Vec2 scale;
        Vec2 center;
        float rotation;
        Vec2 position;


        // Drawing flags
        bool transform = false;
        bool drawOrder = false;

        /** Set the DrawNode drawOrder
        *
        * @param drawOrder. true/false = On/Off
        * Its for performance there
        * false = cocos2dx behaviour => faster but works only on 1.0f thickness

        * @js NA
        */
        void setDrawOrder(bool dO) { drawOrder = dO; };

        /** Get the DrawNode drawOrder
         *
         * @js NA
         */
        bool getDrawOrder(void) { return drawOrder; };

        /** Set the DrawNode transform
         *
         * @param transform. true/false = On/Off
         *
         * @js NA
         */
        void setTransform(bool t) { transform = t; };

        /** Get the DrawNode transform
         *
         * @js NA
         */
        bool getTransform(void) { return transform; };

        /** Set the DrawNode scale for each drawing primitive after this.

        * @js NA
        */
        void setScale(Vec2 s) { scale = s; };

        /** Set the DrawNode rotation for each drawing primitive after this.

        * @js NA
        */
        void setRotation(float r) { rotation = r; };

        /** Get the DrawNode rotation for each drawing primitive after this.

        * @js NA
        */
        float getRotation() { return rotation; };

        /** Set the DrawNode center of rotation for each drawing primitive after this.

        * @js NA
        */
        void setCenter(Vec2 c) { center = c; };

        /** Get the DrawNode center of rotation for each drawing primitive after this.

        * @js NA
        */
        Vec2 getCenter() { return center; };

        /** Set the DrawNode position for each drawing primitive after this.

        * @js NA
        */
        void setPosition(Vec2 p) { position = p; };

        /** Get the DrawNode position for drawing primitive.

        * @js NA
        */
        Vec2 getPosition() { return position; };

        /** Set all default DrawNode properties.

        * @js NA
        */
        void setDefaultValues()
        {
            scale     = Vec2(1.0f, 1.0f);
            center    = Vec2(0.0f, 0.0f);
            rotation  = 0.0f;
            position  = Vec2(0.0f, 0.0f);
            drawOrder = false;
        };
    } properties;
};

/** @} */
}  // namespace ax

#endif  // __DRAW_NODE_EX_H__
