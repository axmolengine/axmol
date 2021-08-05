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

#include "Box2dTest.h"
#include "../testResource.h"
#include "extensions/cocos-ext.h"
#include "renderer/CCRenderer.h"
#include "renderer/CCCustomCommand.h"



USING_NS_CC;
USING_NS_CC_EXT;

#define PTM_RATIO 32

#define DRAW_STRING_NEW_LINE 25

enum {
    kTagParentNode = 1,
};

Settingss settings;

extern cocos2d::DrawNode* drawBox2D;
extern cocos2d::Vec2 debugNodeOffset;

Box2DTests::Box2DTests()
{
    ADD_TEST_CASE(Box2DTest);
}

bool Box2DTest::init()
{
    if (!TestCase::init())
    {
        return false;
    }
    auto dispatcher = Director::getInstance()->getEventDispatcher();

    auto touchListener = EventListenerTouchAllAtOnce::create();
    touchListener->onTouchesEnded = CC_CALLBACK_2(Box2DTest::onTouchesEnded, this);
    dispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    // init physics
    this->initPhysics();
    // create reset button
    this->createResetButton();

    //Set up sprite
#if 1
    // Use batch node. Faster
    auto parent = SpriteBatchNode::create("Images/blocks.png", 100);
    _spriteTexture = parent->getTexture();
#else
    // doesn't use batch node. Slower
    _spriteTexture = Director::getInstance()->getTextureCache()->addImage("Images/blocks.png");
    auto parent = Node::create();
#endif
    addChild(parent, 0, kTagParentNode);


    addNewSpriteAtPosition(VisibleRect::center());

    auto label = Label::createWithTTF("Tap screen", "fonts/Marker Felt.ttf", 32.0f);
    addChild(label, 0);
    label->setColor(Color3B(0, 0, 255));
    label->setPosition(VisibleRect::center().x, VisibleRect::top().y - 50);

    // menu for debug layer
    MenuItemFont::setFontSize(18);
    auto item = MenuItemFont::create("Toggle debug", CC_CALLBACK_1(Box2DTest::toggleDebugCallback, this));

    auto menu = Menu::create(item, nullptr);
    this->addChild(menu);
    menu->setPosition(VisibleRect::right().x - 100, VisibleRect::top().y - 60);

    addChild(drawBox2D, 100);
    drawBox2D->setOpacity(150);

    scheduleUpdate();

    return true;
}

Box2DTest::Box2DTest()
    : _spriteTexture(nullptr)
    , world(nullptr)
{

}

Box2DTest::~Box2DTest()
{
    CC_SAFE_DELETE(world);
}


void Box2DTest::toggleDebugCallback(Ref* sender)
{
    static bool show = true;
    if (show)
    {
        world->SetDebugDraw(nullptr);
        show = 0;
    }
    else
    {
        world->SetDebugDraw(&g_debugDraw);
        show = true;
    }
}


void Box2DTest::initPhysics()
{
    b2Vec2 gravity;
    gravity.Set(0.0f, -10.0f);
    world = new b2World(gravity);

    // Do we want to let bodies sleep?
    world->SetAllowSleeping(true);

    world->SetContinuousPhysics(true);

 

    // Define the ground body.
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0, 0); // bottom-left corner

    // Call the body factory which allocates memory for the ground body
    // from a pool and creates the ground box shape (also from a pool).
    // The body is also added to the world.
    b2Body* groundBody = world->CreateBody(&groundBodyDef);

    // Define the ground box shape.
    b2EdgeShape groundBox;

    // bottom
    groundBox.SetTwoSided(b2Vec2(VisibleRect::leftBottom().x / PTM_RATIO, VisibleRect::leftBottom().y / PTM_RATIO), b2Vec2(VisibleRect::rightBottom().x / PTM_RATIO, VisibleRect::rightBottom().y / PTM_RATIO));
    groundBody->CreateFixture(&groundBox, 0);

    // top
    groundBox.SetTwoSided(b2Vec2(VisibleRect::leftTop().x / PTM_RATIO, VisibleRect::leftTop().y / PTM_RATIO), b2Vec2(VisibleRect::rightTop().x / PTM_RATIO, VisibleRect::rightTop().y / PTM_RATIO));
    groundBody->CreateFixture(&groundBox, 0);

    // left
    groundBox.SetTwoSided(b2Vec2(VisibleRect::leftTop().x / PTM_RATIO, VisibleRect::leftTop().y / PTM_RATIO), b2Vec2(VisibleRect::leftBottom().x / PTM_RATIO, VisibleRect::leftBottom().y / PTM_RATIO));
    groundBody->CreateFixture(&groundBox, 0);

    // right
    groundBox.SetTwoSided(b2Vec2(VisibleRect::rightBottom().x / PTM_RATIO, VisibleRect::rightBottom().y / PTM_RATIO), b2Vec2(VisibleRect::rightTop().x / PTM_RATIO, VisibleRect::rightTop().y / PTM_RATIO));
    groundBody->CreateFixture(&groundBox, 0);


    g_debugDraw.mRatio = 32;
    debugNodeOffset = { 0, 0 };
    world->SetDebugDraw(&g_debugDraw);

}

void Box2DTest::createResetButton()
{
    auto reset = MenuItemImage::create("Images/r1.png", "Images/r2.png", [&](Ref* sender) {
        getTestSuite()->restartCurrTest();
        });

    auto menu = Menu::create(reset, nullptr);

    menu->setPosition(VisibleRect::bottom().x, VisibleRect::bottom().y + 30);
    this->addChild(menu, -1);
}

void Box2DTest::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    //
    // IMPORTANT:
    // This is only for debug purposes
    // It is recommend to disable it
    //
    Scene::draw(renderer, transform, flags);

    _customCmd.init(_globalZOrder, transform, flags);
    _customCmd.func = CC_CALLBACK_0(Box2DTest::onDraw, this, transform, flags);
    renderer->addCommand(&_customCmd);
}

void Box2DTest::onDraw(const Mat4& transform, uint32_t flags)
{
    drawBox2D->clear();
    Step(&settings);
}


void Box2DTest::addNewSpriteAtPosition(Vec2 p)
{
    CCLOG("Add sprite %0.2f x %02.f", p.x, p.y);

    // Define the dynamic body.
    //Set up a 1m squared box in the physics world
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(p.x / PTM_RATIO, p.y / PTM_RATIO);

    b2Body* body = world->CreateBody(&bodyDef);

    // Define another box shape for our dynamic body.
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(.5f, .5f);//These are mid points for our 1m box

    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);

    auto parent = this->getChildByTag(kTagParentNode);

    //We have a 64x64 sprite sheet with 4 different 32x32 images.  The following code is
    //just randomly picking one of the images
    int idx = (CCRANDOM_0_1() > .5 ? 0 : 1);
    int idy = (CCRANDOM_0_1() > .5 ? 0 : 1);
    auto sprite = PhysicsSpriteBox2D::createWithTexture(_spriteTexture, Rect(32 * idx, 32 * idy, 32, 32));
    parent->addChild(sprite);
    sprite->setB2Body(body);
    sprite->setPTMRatio(PTM_RATIO);
    sprite->setPosition(cocos2d::Vec2(p.x, p.y));
}

void Box2DTest::update(float dt)
{
    //It is recommended that a fixed time step is used with Box2D for stability
    //of the simulation, however, we are using a variable time step here.
    //You need to make an informed choice, the following URL is useful
    //http://gafferongames.com/game-physics/fix-your-timestep/

    int velocityIterations = 8;
    int positionIterations = 1;

    // Instruct the world to perform a single step of simulation. It is
    // generally best to keep the time step and iterations fixed.
    world->Step(dt, velocityIterations, positionIterations);
//    world->DebugDraw();
}

void Box2DTest::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    //Add a new body/atlas sprite at the touched location

    for (auto& touch : touches)
    {
        if (!touch)
            break;

        auto location = touch->getLocation();

        addNewSpriteAtPosition(location);
    }
}


void Box2DTest::Step(Settingss* settings)
{
    float timeStep = settings->hz > 0.0f ? 1.0f / settings->hz : float(0.0f);

    if (settings->pause)
    {
        if (settings->singleStep)
        {
            settings->singleStep = 0;
        }
        else
        {
            timeStep = 0.0f;
        }

        g_debugDraw.DrawString(5, m_textLine, "****PAUSED****");
        m_textLine += DRAW_STRING_NEW_LINE;
    }

    uint32 flags = 0;
    flags += settings->drawShapes * b2Draw::e_shapeBit;
    flags += settings->drawJoints * b2Draw::e_jointBit;
    flags += settings->drawAABBs * b2Draw::e_aabbBit;
    flags += settings->drawCOMs * b2Draw::e_centerOfMassBit;
    g_debugDraw.SetFlags(flags);

    world->SetAllowSleeping(settings->enableSleep > 0);
    world->SetWarmStarting(settings->enableWarmStarting > 0);
    world->SetContinuousPhysics(settings->enableContinuous > 0);
    world->SetSubStepping(settings->enableSubStepping > 0);

    m_pointCount = 0;

    world->Step(timeStep, settings->velocityIterations, settings->positionIterations);

    world->DebugDraw();

    if (timeStep > 0.0f)
    {
        ++m_stepCount;
    }

    if (settings->drawStats)
    {
        int32 bodyCount = world->GetBodyCount();
        int32 contactCount = world->GetContactCount();
        int32 jointCount = world->GetJointCount();
        g_debugDraw.DrawString(5, m_textLine, "bodies/contacts/joints = %d/%d/%d", bodyCount, contactCount, jointCount);
        m_textLine += DRAW_STRING_NEW_LINE;

        int32 proxyCount = world->GetProxyCount();
        int32 height = world->GetTreeHeight();
        int32 balance = world->GetTreeBalance();
        float quality = world->GetTreeQuality();
        g_debugDraw.DrawString(5, m_textLine, "proxies/height/balance/quality = %d/%d/%d/%g", proxyCount, height, balance, quality);
        m_textLine += DRAW_STRING_NEW_LINE;
    }

    // Track maximum profile times
    {
        const b2Profile& p = world->GetProfile();
        m_maxProfile.step = b2Max(m_maxProfile.step, p.step);
        m_maxProfile.collide = b2Max(m_maxProfile.collide, p.collide);
        m_maxProfile.solve = b2Max(m_maxProfile.solve, p.solve);
        m_maxProfile.solveInit = b2Max(m_maxProfile.solveInit, p.solveInit);
        m_maxProfile.solveVelocity = b2Max(m_maxProfile.solveVelocity, p.solveVelocity);
        m_maxProfile.solvePosition = b2Max(m_maxProfile.solvePosition, p.solvePosition);
        m_maxProfile.solveTOI = b2Max(m_maxProfile.solveTOI, p.solveTOI);
        m_maxProfile.broadphase = b2Max(m_maxProfile.broadphase, p.broadphase);

        m_totalProfile.step += p.step;
        m_totalProfile.collide += p.collide;
        m_totalProfile.solve += p.solve;
        m_totalProfile.solveInit += p.solveInit;
        m_totalProfile.solveVelocity += p.solveVelocity;
        m_totalProfile.solvePosition += p.solvePosition;
        m_totalProfile.solveTOI += p.solveTOI;
        m_totalProfile.broadphase += p.broadphase;
    }

    if (settings->drawProfile)
    {
        const b2Profile& p = world->GetProfile();

        b2Profile aveProfile;
        memset(&aveProfile, 0, sizeof(b2Profile));
        if (m_stepCount > 0)
        {
            float scale = 1.0f / m_stepCount;
            aveProfile.step = scale * m_totalProfile.step;
            aveProfile.collide = scale * m_totalProfile.collide;
            aveProfile.solve = scale * m_totalProfile.solve;
            aveProfile.solveInit = scale * m_totalProfile.solveInit;
            aveProfile.solveVelocity = scale * m_totalProfile.solveVelocity;
            aveProfile.solvePosition = scale * m_totalProfile.solvePosition;
            aveProfile.solveTOI = scale * m_totalProfile.solveTOI;
            aveProfile.broadphase = scale * m_totalProfile.broadphase;
        }

        g_debugDraw.DrawString(5, m_textLine, "step [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.step, aveProfile.step, m_maxProfile.step);
        m_textLine += DRAW_STRING_NEW_LINE;
        g_debugDraw.DrawString(5, m_textLine, "collide [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.collide, aveProfile.collide, m_maxProfile.collide);
        m_textLine += DRAW_STRING_NEW_LINE;
        g_debugDraw.DrawString(5, m_textLine, "solve [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.solve, aveProfile.solve, m_maxProfile.solve);
        m_textLine += DRAW_STRING_NEW_LINE;
        g_debugDraw.DrawString(5, m_textLine, "solve init [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.solveInit, aveProfile.solveInit, m_maxProfile.solveInit);
        m_textLine += DRAW_STRING_NEW_LINE;
        g_debugDraw.DrawString(5, m_textLine, "solve velocity [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.solveVelocity, aveProfile.solveVelocity, m_maxProfile.solveVelocity);
        m_textLine += DRAW_STRING_NEW_LINE;
        g_debugDraw.DrawString(5, m_textLine, "solve position [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.solvePosition, aveProfile.solvePosition, m_maxProfile.solvePosition);
        m_textLine += DRAW_STRING_NEW_LINE;
        g_debugDraw.DrawString(5, m_textLine, "solveTOI [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.solveTOI, aveProfile.solveTOI, m_maxProfile.solveTOI);
        m_textLine += DRAW_STRING_NEW_LINE;
        g_debugDraw.DrawString(5, m_textLine, "broad-phase [ave] (max) = %5.2f [%6.2f] (%6.2f)", p.broadphase, aveProfile.broadphase, m_maxProfile.broadphase);
        m_textLine += DRAW_STRING_NEW_LINE;
    }

    //if (m_mouseJoint)
    //{
    //    b2Vec2 p1 = m_mouseJoint->GetAnchorB();
    //    b2Vec2 p2 = m_mouseJoint->GetTarget();

    //    b2Color c;
    //    c.Set(0.0f, 1.0f, 0.0f);
    //    g_debugDraw.DrawPoint(p1, 4.0f, c);
    //    g_debugDraw.DrawPoint(p2, 4.0f, c);

    //    c.Set(0.8f, 0.8f, 0.8f);
    //    g_debugDraw.DrawSegment(p1, p2, c);
    //}

    if (m_bombSpawning)
    {
        b2Color c;
        c.Set(0.0f, 0.0f, 1.0f);
        g_debugDraw.DrawPoint(m_bombSpawnPoint, 4.0f, c);

        c.Set(0.8f, 0.8f, 0.8f);
        g_debugDraw.DrawSegment(m_mouseWorld, m_bombSpawnPoint, c);
    }

    if (settings->drawContactPoints)
    {
        const float k_impulseScale = 0.1f;
        const float k_axisScale = 0.3f;

        for (int32 i = 0; i < m_pointCount; ++i)
        {
            ContactPoint* point = m_points + i;

            if (point->state == b2_addState)
            {
                // Add
                g_debugDraw.DrawPoint(point->position, 10.0f, b2Color(0.3f, 0.95f, 0.3f));
            }
            else if (point->state == b2_persistState)
            {
                // Persist
                g_debugDraw.DrawPoint(point->position, 5.0f, b2Color(0.3f, 0.3f, 0.95f));
            }

            if (settings->drawContactNormals == 1)
            {
                b2Vec2 p1 = point->position;
                b2Vec2 p2 = p1 + k_axisScale * point->normal;
                g_debugDraw.DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.9f));
            }
            else if (settings->drawContactImpulse == 1)
            {
                b2Vec2 p1 = point->position;
                b2Vec2 p2 = p1 + k_impulseScale * point->normalImpulse * point->normal;
                g_debugDraw.DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.3f));
            }

            if (settings->drawFrictionImpulse == 1)
            {
                b2Vec2 tangent = b2Cross(point->normal, 1.0f);
                b2Vec2 p1 = point->position;
                b2Vec2 p2 = p1 + k_impulseScale * point->tangentImpulse * tangent;
                g_debugDraw.DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.3f));
            }
        }
    }
}