/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef _BOX2D_TEST_H_
#define _BOX2D_TEST_H_

#include "cocos2d.h"
#include "box2d/box2d.h"
#include "../BaseTest.h"
#include "extensions/cocos-ext.h"

DEFINE_TEST_SUITE(Box2DTests);

/// Test settings. Some can be controlled in the GUI.
struct Settingss
{
    Settingss()
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


const int32 k_maxContactPoints = 2048;

class Box2DTest : public TestCase, b2ContactListener
{
public:
    CREATE_FUNC(Box2DTest);

    virtual bool init() override;

    Box2DTest();
    ~Box2DTest();

    void initPhysics();
    void createResetButton();
    virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;

    void addNewSpriteAtPosition(cocos2d::Vec2 p);
    void update(float dt) override;
    void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);

    void toggleDebugCallback(cocos2d::Ref* sender);

    virtual void Step(Settingss* settings);


    // Let derived tests know that a joint was destroyed.
    virtual void JointDestroyed(b2Joint* joint) { B2_NOT_USED(joint); }

    // Callbacks for derived classes.
    virtual void BeginContact(b2Contact* contact)  override { B2_NOT_USED(contact); }
    virtual void EndContact(b2Contact* contact)  override { B2_NOT_USED(contact); }
 //   virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
    virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override
    {
        B2_NOT_USED(contact);
        B2_NOT_USED(impulse);
    }

    void ShiftOrigin(const b2Vec2& newOrigin);

protected:
    cocos2d::Mat4 _modelViewMV;
    void onDraw(const cocos2d::Mat4& transform, uint32_t flags);
    cocos2d::CallbackCommand  _customCmd;
    cocos2d::Texture2D* _spriteTexture;


    DebugDraw g_debugDraw;

    cocos2d::extension::PhysicsDebugNode* _debugLayer; // weak ref

    b2World* world;



    friend class DestructionListener;
    friend class BoundaryListener;
    friend class ContactListener;

    b2Body* m_groundBody;
    b2AABB m_worldAABB;
    ContactPoint m_points[k_maxContactPoints];
    int32 m_pointCount;
 //   DestructionListener m_destructionListener;
    int32 m_textLine;

    b2Body* m_bomb;
    b2MouseJoint* m_mouseJoint;
    b2Vec2 m_bombSpawnPoint;
    bool m_bombSpawning;
    b2Vec2 m_mouseWorld;
    int32 m_stepCount;
    int32 m_textIncrement;
    b2Profile m_maxProfile;
    b2Profile m_totalProfile;
} ;

#endif