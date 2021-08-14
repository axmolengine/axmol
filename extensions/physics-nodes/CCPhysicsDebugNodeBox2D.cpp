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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#include "CCPhysicsDebugNodeBox2D.h"




USING_NS_CC;


#if defined(CC_PLATFORM_PC)
extern cocos2d::Label* labelDebugDraw;
#endif

#define BUFFER_OFFSET(x)  ((const void*) (x))


DebugDraw::DebugDraw()
{
	drawBP = DrawNode::create();
	debugNodeOffset = { 40, 0 };
}

DebugDraw::~DebugDraw()
{
}

void DebugDraw::initShader(void)
{
	// initShader is unsupported
}

cocos2d::DrawNode* DebugDraw::GetDrawNode()
{
	return drawBP;
}

void DebugDraw::SetDrawNode(cocos2d::DrawNode* drawNode)
{
	CCASSERT(!drawBP, "drawBP is not NULL");
	drawBP = drawNode;
}

cocos2d::Vec2& DebugDraw::GetDebugNodeOffset()
{
	return debugNodeOffset;
}


void DebugDraw::DrawPolygon(const b2Vec2* verts, int vertexCount, const b2Color& color)
{
	Vec2* vec = new (std::nothrow) Vec2[vertexCount];
	for (size_t i = 0; i < vertexCount; i++) {
		vec[i] = Vec2(verts[i].x * mRatio, verts[i].y * mRatio) + debugNodeOffset;
	}
	//    drawBP->drawPolygon(vec, vertexCount, Color4F(color.r, color.g, color.b, color.a), 1, Color4F(color.r, color.g, color.b, color.a));
	drawBP->drawPolygon(vec, vertexCount, Color4F::BLACK, 0.4f, Color4F(color.r, color.g, color.b, color.a));
}

void DebugDraw::DrawSolidPolygon(const b2Vec2* verts, int vertexCount, const b2Color& color)
{
	Vec2* vec = new (std::nothrow) Vec2[vertexCount];
	for (size_t i = 0; i < vertexCount; i++) {
		vec[i] = Vec2(verts[i].x * mRatio, verts[i].y * mRatio) + debugNodeOffset;
	}
	drawBP->drawPolygon(vec, vertexCount, Color4F(color.r / 2, color.g / 2, color.b / 2, color.a), 0.4f, Color4F(color.r, color.g, color.b, color.a));
	//drawBP->drawSolidPoly(vec, vertexCount, Color4F(color.r, color.g, color.b, color.a));
}

void DebugDraw::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
{
	drawBP->drawCircle(Vec2(center.x * mRatio, center.y * mRatio) + debugNodeOffset, radius * mRatio, CC_DEGREES_TO_RADIANS(0), 30, true, 1.0f,
		1.0f, Color4F(color.r, color.g, color.b, color.a));
}

void DebugDraw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
{
	// DrawSolidCircle Maybe have to fix later
	Vec2 c = { Vec2(center.x * mRatio, center.y * mRatio) + debugNodeOffset };
	drawBP->drawCircle(c, radius * mRatio, CC_DEGREES_TO_RADIANS(0), 20, false, 1.0f, 1.0f, Color4F(color.r, color.g, color.b, color.a));

	// Draw a line fixed in the circle to animate rotation.
	b2Vec2 pp = { (center + radius * axis) };
	Vec2 cp = { Vec2(pp.x * mRatio, pp.y * mRatio) + debugNodeOffset };
	drawBP->drawLine(c, cp, Color4F(color.r, color.g, color.b, color.a));
}

void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	drawBP->drawLine(Vec2(p1.x * mRatio, p1.y * mRatio) + debugNodeOffset, Vec2(p2.x * mRatio, p2.y * mRatio) + debugNodeOffset, Color4F(color.r, color.g, color.b, color.a));
}

void DebugDraw::DrawTransform(const b2Transform& xf)
{
	b2Vec2 p1 = xf.p, p2;
	const float k_axisScale = 0.4f;
	p2 = p1 + k_axisScale * xf.q.GetXAxis();
	DrawSegment(p1, p2, b2Color(1, 0, 0));

	p2 = p1 + k_axisScale * xf.q.GetYAxis();
	DrawSegment(p1, p2, b2Color(0, 1, 0));
}

void DebugDraw::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
{
	drawBP->drawPoint(Vec2(p.x * mRatio, p.y * mRatio) + debugNodeOffset, size, Color4F(color.r, color.g, color.b, color.a));
}

void DebugDraw::DrawString(int x, int y, const char* fmt, ...)
{
#if defined(CC_PLATFORM_PC)
	debugString.append(std::string(fmt));
	debugString.append("\n");
	labelDebugDraw->setString(debugString);
//	labelDebugDraw->setPosition(x, y);
#endif
}

void DebugDraw::DrawString(const b2Vec2& pw, const char* fmt, ...)
{
#if defined(CC_PLATFORM_PC)
	debugString.append(std::string(fmt));
	debugString.append("\n");
	labelDebugDraw->setString(debugString);
//	labelDebugDraw->setPosition(pw.x, pw.y);
#endif
}



void DebugDraw::DrawAABB(b2AABB* aabb, const b2Color& color)
{
	b2Vec2 p1 = aabb->lowerBound;
	b2Vec2 p2 = b2Vec2(aabb->upperBound.x, aabb->lowerBound.y);
	b2Vec2 p3 = aabb->upperBound;
	b2Vec2 p4 = b2Vec2(aabb->lowerBound.x, aabb->upperBound.y);

	Vec2 verts[] = {
	Vec2(p1.x * mRatio, p1.y * mRatio) + debugNodeOffset ,
	Vec2(p2.x * mRatio, p2.y * mRatio) + debugNodeOffset ,
	Vec2(p3.x * mRatio, p3.y * mRatio) + debugNodeOffset ,
	Vec2(p4.x * mRatio, p4.y * mRatio) + debugNodeOffset ,
	};
	drawBP->drawPolygon(verts, sizeof(verts) / sizeof(verts[0]), Color4F(color.r / 2, color.g / 2, color.b / 2, 0), 0.4f, Color4F(color.r, color.g, color.b, color.a));
}

void DebugDraw::Flush()
{
	// Flush is unsupported
}