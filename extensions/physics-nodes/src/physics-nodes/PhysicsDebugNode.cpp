/*
 * Copyright (c) 2021 @aismann; Peter Eismann, Germany; dreifrankensoft
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "PhysicsDebugNode.h"
#include "axmol//physics/2d/PhysicsUtility2D.h"

NS_AX_EXT_BEGIN

/// Draw a closed polygon provided in CCW order.
static void b2DrawPolygon(b2WorldTransform t, const b2Vec2* verts, int vertexCount, b2HexColor color, void* context)
{
    auto dn = static_cast<PhysicsDebugNode*>(context);
    tlx::pod_vector<Vec2> vec(vertexCount);
    for (size_t i = 0; i < vertexCount; i++)
    {
        auto pt = b2TransformWorldPoint(t, verts[i]);
        vec[i]  = Vec2(pt.x * dn->getPTMRatio(), pt.y * dn->getPTMRatio()) + dn->getWorldOffset();
    }
    dn->drawPolygon(vec.data(), vertexCount, ax::Color::black, 0.4f, b2util::cast(color));
}

/// Draw a solid closed polygon provided in CCW order.
static void b2DrawSolidPolygon(b2WorldTransform t,
                               const b2Vec2* verts,
                               int vertexCount,
                               float radius,
                               b2HexColor color,
                               void* context)
{
    auto dn = static_cast<PhysicsDebugNode*>(context);
    tlx::pod_vector<Vec2> vec(vertexCount);
    for (size_t i = 0; i < vertexCount; i++)
    {
        auto pt = b2TransformWorldPoint(t, verts[i]);
        vec[i]  = Vec2(pt.x * dn->getPTMRatio(), pt.y * dn->getPTMRatio()) + dn->getWorldOffset();
    }
    auto color4f = b2util::cast(color);
    dn->drawPolygon(vec.data(), vertexCount, ax::Color(color4f.r / 2, color4f.g / 2, color4f.b / 2, color4f.a), 0.5f,
                    color4f);
}

/// Draw a circle.
static void b2DrawCircle(b2Pos center, float radius, b2HexColor color, void* context)
{
    auto dn = static_cast<PhysicsDebugNode*>(context);
    dn->drawDot(Vec2(center.x * dn->getPTMRatio(), center.y * dn->getPTMRatio()) + dn->getWorldOffset(),
                radius * dn->getPTMRatio(), b2util::cast(color));
}

/// Draw a solid circle.
static void b2DrawSolidCircle(b2WorldTransform t, b2Vec2 localCenter, float radius, b2HexColor color, void* context)
{
    auto dn      = static_cast<PhysicsDebugNode*>(context);
    auto center  = b2TransformWorldPoint(t, localCenter);
    Vec2 c       = {Vec2(center.x * dn->getPTMRatio(), center.y * dn->getPTMRatio()) + dn->getWorldOffset()};
    auto color4f = b2util::cast(color);

#if AX_DRAWNODE_FAST_LINE2CENTER
    dn->drawSolidCircle(c, radius * dn->getPTMRatio(), color4f,
                        ax::Color(color4f.r / 4, color4f.g / 4, color4f.b / 4, color4f.a),
                        AX_RADIANS_TO_DEGREES(b2Rot_GetAngle(t.q)));
#else
    dn->drawDot(Vec2(center.x * dn->getPTMRatio(), center.y * dn->getPTMRatio()) + dn->getWorldOffset(),
                radius * dn->getPTMRatio(), color4f);
    dn->drawDot(Vec2(center.x * dn->getPTMRatio(), center.y * dn->getPTMRatio()) + dn->getWorldOffset(),
                radius * dn->getPTMRatio() - 0.5f, ax::Color(color4f.r / 4, color4f.g / 4, color4f.b / 4, color4f.a));

    // Draw a line fixed in the circle to animate rotation.
    auto pp   = center + radius * b2Rot_GetXAxis(t.q);
    Vec2 cp   = {Vec2(pp.x * dn->getPTMRatio(), pp.y * dn->getPTMRatio()) + dn->getWorldOffset()};
    dn->drawLine(c, cp, color4f);
#endif  // AX_DRAWNODE_FAST_LINE2CENTER
}

/// Draw a line segment.
static void b2DrawLine(b2Pos p1, b2Pos p2, b2HexColor color, void* context)
{
    auto dn = static_cast<PhysicsDebugNode*>(context);
    dn->drawLine(Vec2(p1.x * dn->getPTMRatio(), p1.y * dn->getPTMRatio()) + dn->getWorldOffset(),
                 Vec2(p2.x * dn->getPTMRatio(), p2.y * dn->getPTMRatio()) + dn->getWorldOffset(), b2util::cast(color));
}

/// Draw a transform. Choose your own length scale.
static void b2DrawTransform(b2WorldTransform t, void* context)
{
    b2Pos p1               = t.p, p2;
    const float k_axisScale = 0.4f;

    p2 = p1 + k_axisScale * b2Rot_GetXAxis(t.q);
    b2DrawLine(p1, p2, b2HexColor::b2_colorRed, context);

    p2 = p1 + k_axisScale * b2Rot_GetYAxis(t.q);
    b2DrawLine(p1, p2, b2HexColor::b2_colorGreen, context);
}

/// Draw a point.
static void b2DrawPoint(b2Pos p, float size, b2HexColor color, void* context)
{
    auto dn = static_cast<PhysicsDebugNode*>(context);
    dn->drawPoint(Vec2(p.x * dn->getPTMRatio(), p.y * dn->getPTMRatio()) + dn->getWorldOffset(), size,
                  b2util::cast(color));
}

bool PhysicsDebugNode::initWithWorld(b2WorldId worldId)
{
    bool ret = DrawNode::init();

    _world = worldId;
    return ret;
}

void PhysicsDebugNode::draw(const SceneRenderState& state, const Mat4& transform, uint32_t flags)
{
    if (!b2World_IsValid(_world))
    {
        return;
    }

    if (_autoDraw)
    {
        // clear the shapes information before draw current shapes.
        clear();
        b2World_Draw(_world, _debugDraw);
    }

    DrawNode::draw(state, transform, flags);
}

PhysicsDebugNode::PhysicsDebugNode() : _ownDebugDraw(true)
{
    _debugDraw = new b2DebugDraw(b2DefaultDebugDraw());
    setBuiltinDrawFuncs();
}

PhysicsDebugNode::PhysicsDebugNode(b2DebugDraw* externalDebugDraw) : _ownDebugDraw(false)
{
    _debugDraw = externalDebugDraw;
    setBuiltinDrawFuncs();
}

PhysicsDebugNode::~PhysicsDebugNode()
{
    if (_ownDebugDraw)
        AX_SAFE_DELETE(_debugDraw);
}

void PhysicsDebugNode::setBuiltinDrawFuncs()
{
    _debugDraw->context             = this;
    _debugDraw->DrawPolygonFcn      = b2DrawPolygon;
    _debugDraw->DrawSolidPolygonFcn = b2DrawSolidPolygon;
    _debugDraw->DrawCircleFcn       = b2DrawCircle;
    _debugDraw->DrawSolidCircleFcn  = b2DrawSolidCircle;
    _debugDraw->DrawLineFcn         = b2DrawLine;
    _debugDraw->DrawTransformFcn    = b2DrawTransform;
    _debugDraw->DrawPointFcn        = b2DrawPoint;
}

NS_AX_EXT_END
