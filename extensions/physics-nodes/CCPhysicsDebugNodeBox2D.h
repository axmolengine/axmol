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


struct b2AABB;

extern cocos2d::DrawNode* drawBox2D;

/// Test settings. Some can be controlled in the GUI.
struct Settings
{
    Settings()
    {
        viewCenter.Set(0.0f, 0.0f);
        hz = 60.0f;
        velocityIterations = 8;
        positionIterations = 3;
        drawShapes = 1;
        drawJoints = 1;
        drawAABBs = 0;
        drawContactPoints = 0;
        drawContactNormals = 0;
        drawContactImpulse = 0;
        drawFrictionImpulse = 0;
        drawCOMs = 0;
        drawStats = 0;
        drawProfile = 0;
        enableWarmStarting = 1;
        enableContinuous = 1;
        enableSubStepping = 0;
        enableSleep = 1;
        pause = 0;
        singleStep = 0;
    }

    b2Vec2 viewCenter;
    float hz;
    int32 velocityIterations;
    int32 positionIterations;
    int32 drawShapes;
    int32 drawJoints;
    int32 drawAABBs;
    int32 drawContactPoints;
    int32 drawContactNormals;
    int32 drawContactImpulse;
    int32 drawFrictionImpulse;
    int32 drawCOMs;
    int32 drawStats;
    int32 drawProfile;
    int32 enableWarmStarting;
    int32 enableContinuous;
    int32 enableSubStepping;
    int32 enableSleep;
    int32 pause;
    int32 singleStep;
};

struct ContactPoint
{
    b2Fixture* fixtureA;
    b2Fixture* fixtureB;
    b2Vec2 normal;
    b2Vec2 position;
    b2PointState state;
    float normalImpulse;
    float tangentImpulse;
    float separation;
};

//struct Camera
//{
//    Camera()
//    {
//        m_center.Set(0.0f, 20.0f);
//        m_zoom = 1.0f;
//        m_width = 1280;
//        m_height = 800;
//    }
//
//    b2Vec2 ConvertScreenToWorld(const b2Vec2& screenPoint);
//    b2Vec2 ConvertWorldToScreen(const b2Vec2& worldPoint);
//    void BuildProjectionMatrix(float* m, float zBias);
//
//    b2Vec2 m_center;
//    float m_zoom;
//    int32 m_width;
//    int32 m_height;
//};

// This class implements debug drawing callbacks that are invoked
// inside b2World::Step.

//PhysicsDebugNode : public DrawNode
class CC_EX_DLL DebugDraw : public b2Draw
{

 //   cocos2d::g* mShaderProgram;
   // GLint        mColorLocation;

    cocos2d::DrawNode* drawBP = NULL;

    void initShader( void );
public:
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

    float mRatio;
};

#endif //__PHYSICSNODES_DEBUGNODE_BOX2D_H__
