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
#include "physics/PhysicsHelper.h"

#if defined(_WIN32)
#    pragma push_macro("TRANSPARENT")
#    undef TRANSPARENT
#endif

NS_AX_EXT_BEGIN

/// Draw a closed polygon provided in CCW order.
// void (*DrawPolygon)(const b2Vec2* vertices, int vertexCount, b2HexColor color, void* context);
static void b2DrawPolygon(const b2Vec2* verts, int vertexCount, b2HexColor color, PhysicsDebugNode* dn)
{
    Vec2* vec = new Vec2[vertexCount];
    for (size_t i = 0; i < vertexCount; i++)
    {
        vec[i] = Vec2(verts[i].x * dn->getPTMRatio(), verts[i].y * dn->getPTMRatio()) + dn->getWorldOffset();
    }
    dn->drawPolygon(vec, vertexCount, ax::Color::BLACK, 0.4f, PhysicsHelper::toColor(color));
}

/// Draw a solid closed polygon provided in CCW order.
// void (*DrawSolidPolygon)(b2Transform,
//                          const b2Vec2* vertices,
//                          int vertexCount,
//                          float radius,
//                          b2HexColor color,
//                          void* context);
static void b2DrawSolidPolygon(b2Transform t,
                               const b2Vec2* verts,
                               int vertexCount,
                               float radius,
                               b2HexColor color,
                               PhysicsDebugNode* dn)
{
    axstd::pod_vector<Vec2> vec(vertexCount);
    for (size_t i = 0; i < vertexCount; i++)
    {
        auto pt = b2TransformPoint(t, verts[i]);
        vec[i]  = Vec2(pt.x * dn->getPTMRatio(), pt.y * dn->getPTMRatio()) + dn->getWorldOffset();
    }
    auto color4f = PhysicsHelper::toColor(color);
    dn->drawPolygon(vec.data(), vertexCount, ax::Color(color4f.r / 2, color4f.g / 2, color4f.b / 2, color4f.a), 0.5f,
                    color4f);
}

/// Draw a circle.
// void (*DrawCircle)(b2Vec2 center, float radius, b2HexColor color, void* context);
static void b2DrawCircle(b2Vec2 center, float radius, b2HexColor color, PhysicsDebugNode* dn)
{
    dn->drawCircle(Vec2(center.x * dn->getPTMRatio(), center.y * dn->getPTMRatio()) + dn->getWorldOffset(),
                   radius * dn->getPTMRatio(), AX_DEGREES_TO_RADIANS(0), 30, true, 1.0f, 1.0f,
                   PhysicsHelper::toColor(color));
}

/// Draw a solid circle.
// void (*DrawSolidCircle)(b2Transform, float radius, b2HexColor color, void* context);
static void b2DrawSolidCircle(b2Transform t, float radius, b2HexColor color, PhysicsDebugNode* dn)
{
    auto center  = b2TransformPoint(t, b2Vec2_zero);
    Vec2 c       = {Vec2(center.x * dn->getPTMRatio(), center.y * dn->getPTMRatio()) + dn->getWorldOffset()};
    auto color4f = PhysicsHelper::toColor(color);

    dn->drawSolidCircle(c, radius * dn->getPTMRatio(), AX_DEGREES_TO_RADIANS(0), 20, 1.0f, 1.0f,
                        ax::Color(color4f.r / 2, color4f.g / 2, color4f.b / 2, color4f.a), 0.4f, color4f);
    // Draw a line fixed in the circle to animate rotation.
    b2Vec2 pp = {(center + radius * b2Rot_GetXAxis(t.q))};
    Vec2 cp   = {Vec2(pp.x * dn->getPTMRatio(), pp.y * dn->getPTMRatio()) + dn->getWorldOffset()};
    dn->drawLine(c, cp, color4f);
}

/// Draw a solid capsule.
// void (*DrawSolidCapsule)(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void* context);

/// Draw a line segment.
// void (*DrawSegment)(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* context);
static void b2DrawSegment(b2Vec2 p1, b2Vec2 p2, b2HexColor color, PhysicsDebugNode* dn)
{
    dn->drawLine(Vec2(p1.x * dn->getPTMRatio(), p1.y * dn->getPTMRatio()) + dn->getWorldOffset(),
                 Vec2(p2.x * dn->getPTMRatio(), p2.y * dn->getPTMRatio()) + dn->getWorldOffset(),
                 PhysicsHelper::toColor(color));
}

/// Draw a transform. Choose your own length scale.
// void (*DrawTransform)(b2Transform transform, void* context);
static void b2DrawTransform(b2Transform t, PhysicsDebugNode* dn)
{
    b2Vec2 p1               = t.p, p2;
    const float k_axisScale = 0.4f;

    p2 = p1 + k_axisScale * b2Rot_GetXAxis(t.q);
    b2DrawSegment(p1, p2, b2HexColor::b2_colorRed, dn);

    p2 = p1 + k_axisScale * b2Rot_GetYAxis(t.q);
    b2DrawSegment(p1, p2, b2HexColor::b2_colorGreen, dn);
}

/// Draw a point.
// void (*DrawPoint)(b2Vec2 p, float size, b2HexColor color, void* context);
static void b2DrawPoint(b2Vec2 p, float size, b2HexColor color, PhysicsDebugNode* dn)
{
    dn->drawPoint(Vec2(p.x * dn->getPTMRatio(), p.y * dn->getPTMRatio()) + dn->getWorldOffset(), size,
                  PhysicsHelper::toColor(color));
}

bool PhysicsDebugNode::initWithWorld(b2WorldId worldId)
{
    bool ret = DrawNode::init();

    _world = worldId;
    return ret;
}

void PhysicsDebugNode::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    if (!b2World_IsValid(_world))
    {
        return;
    }

    if (_autoDraw)
    {
        // clear the shapes information before draw current shapes.
        clear();
        b2World_Draw(_world, &_debugDraw);
    }

    DrawNode::draw(renderer, transform, flags);
}

PhysicsDebugNode::PhysicsDebugNode()
{
    _debugDraw.context = this;
#define __b2_setfun(f) _debugDraw.f = reinterpret_cast<decltype(_debugDraw.f)>(b2##f);
    __b2_setfun(DrawPolygon);
    __b2_setfun(DrawSolidPolygon);
    __b2_setfun(DrawCircle);
    __b2_setfun(DrawSolidCircle);
    __b2_setfun(DrawSegment);
    __b2_setfun(DrawTransform);
    __b2_setfun(DrawPoint);
#undef __b2_setfun
}

NS_AX_EXT_END

#if defined(_WIN32)
#    pragma pop_macro("TRANSPARENT")
#endif
