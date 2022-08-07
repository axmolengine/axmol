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

#include "CCPhysicsDebugNodeBox2D.h"

NS_AX_EXT_BEGIN

PhysicsDebugNodeBox2D::PhysicsDebugNodeBox2D()
{
    drawBP          = DrawNode::create();
    debugNodeOffset = {40.0f, 0.0f};
    mRatio          = 1.0f;
}

PhysicsDebugNodeBox2D::~PhysicsDebugNodeBox2D() {}

ax::DrawNode* PhysicsDebugNodeBox2D::GetDrawNode()
{
    return drawBP;
}

void PhysicsDebugNodeBox2D::SetDrawNode(ax::DrawNode* drawNode)
{
    drawBP = drawNode;
}

ax::Vec2& PhysicsDebugNodeBox2D::GetDebugNodeOffset()
{
    return debugNodeOffset;
}

void PhysicsDebugNodeBox2D::DrawPolygon(const b2Vec2* verts, int vertexCount, const b2Color& color)
{
    Vec2* vec = new Vec2[vertexCount];
    for (size_t i = 0; i < vertexCount; i++)
    {
        vec[i] = Vec2(verts[i].x * mRatio, verts[i].y * mRatio) + debugNodeOffset;
    }
    drawBP->drawPolygon(vec, vertexCount, Color4F::BLACK, 0.4f, Color4F(color.r, color.g, color.b, color.a));
}

void PhysicsDebugNodeBox2D::DrawSolidPolygon(const b2Vec2* verts, int vertexCount, const b2Color& color)
{
    Vec2* vec = new Vec2[vertexCount];
    for (size_t i = 0; i < vertexCount; i++)
    {
        vec[i] = Vec2(verts[i].x * mRatio, verts[i].y * mRatio) + debugNodeOffset;
    }
    drawBP->drawPolygon(vec, vertexCount, Color4F(color.r / 2, color.g / 2, color.b / 2, color.a), 0.4f,
                        Color4F(color.r, color.g, color.b, color.a));
}

void PhysicsDebugNodeBox2D::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
{
    drawBP->drawCircle(Vec2(center.x * mRatio, center.y * mRatio) + debugNodeOffset, radius * mRatio,
                       AX_DEGREES_TO_RADIANS(0), 30, true, 1.0f, 1.0f, Color4F(color.r, color.g, color.b, color.a));
}

void PhysicsDebugNodeBox2D::DrawSolidCircle(const b2Vec2& center,
                                            float radius,
                                            const b2Vec2& axis,
                                            const b2Color& color)
{
    Vec2 c = {Vec2(center.x * mRatio, center.y * mRatio) + debugNodeOffset};
    drawBP->drawSolidCircle(c, radius * mRatio, AX_DEGREES_TO_RADIANS(0), 20, 1.0f, 1.0f,
                            Color4F(color.r / 2, color.g / 2, color.b / 2, color.a), 0.4f,
                            Color4F(color.r, color.g, color.b, color.a));

    // Draw a line fixed in the circle to animate rotation.
    b2Vec2 pp = {(center + radius * axis)};
    Vec2 cp   = {Vec2(pp.x * mRatio, pp.y * mRatio) + debugNodeOffset};
    drawBP->drawLine(c, cp, Color4F(color.r, color.g, color.b, color.a));
}

void PhysicsDebugNodeBox2D::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
    drawBP->drawLine(Vec2(p1.x * mRatio, p1.y * mRatio) + debugNodeOffset,
                     Vec2(p2.x * mRatio, p2.y * mRatio) + debugNodeOffset, Color4F(color.r, color.g, color.b, color.a));
}

void PhysicsDebugNodeBox2D::DrawTransform(const b2Transform& xf)
{
    b2Vec2 p1               = xf.p, p2;
    const float k_axisScale = 0.4f;
    p2                      = p1 + k_axisScale * xf.q.GetXAxis();
    DrawSegment(p1, p2, b2Color(1.0f, 0.0f, 0.0f));

    p2 = p1 + k_axisScale * xf.q.GetYAxis();
    DrawSegment(p1, p2, b2Color(0.0f, 1.0f, 0.0f));
}

void PhysicsDebugNodeBox2D::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
{
    drawBP->drawPoint(Vec2(p.x * mRatio, p.y * mRatio) + debugNodeOffset, size,
                      Color4F(color.r, color.g, color.b, color.a));
}

NS_AX_EXT_END