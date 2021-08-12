/*
* Copyright (c) 2019 Erin Catto
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

#ifndef __PHYSICSNODES_DEBUGNODE_BOX2D_H__
#define __PHYSICSNODES_DEBUGNODE_BOX2D_H__


#define GLFW_INCLUDE_NONE


#include "extensions/ExtensionMacros.h"
#include "2d/CCDrawNode.h"
#include "extensions/ExtensionExport.h"
#include "box2d/box2d.h"
#include "cocos2d.h"


//struct b2AABB;


// This class implements debug drawing callbacks that are invoked
// inside b2World::Step.

//PhysicsDebugNode : public DrawNode
class CC_EX_DLL DebugDraw : public b2Draw
{
public:
    void initShader( void );

    DebugDraw();
    ~DebugDraw();

    void Create();
    void Destroy();

    void DrawPolygon(const b2Vec2* vertices, int vertexCount, const b2Color& color) override;

    void DrawSolidPolygon(const b2Vec2* vertices, int vertexCount, const b2Color& color) override;

    void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override;

    void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override;

    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;

    void DrawTransform(const b2Transform& xf) override;

    void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override;

    void DrawString(int x, int y, const char* string, ...); 

    void DrawString(const b2Vec2& p, const char* string, ...);

    void DrawAABB(b2AABB* aabb, const b2Color& color);

    void Flush();

    cocos2d::DrawNode* GetDrawNode();
    void SetDrawNode(cocos2d::DrawNode* drawNode);

    float mRatio;
    cocos2d::DrawNode* drawBP = NULL;

    cocos2d::Vec2 debugNodeOffset = { 250, 70 };
};

#endif //__PHYSICSNODES_DEBUGNODE_BOX2D_H__
