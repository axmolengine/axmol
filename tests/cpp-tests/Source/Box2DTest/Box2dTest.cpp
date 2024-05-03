/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2021 @aismann; Peter Eismann, Germany; dreifrankensoft

 https://axmolengine.github.io/

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
#include "extensions/axmol-ext.h"
#include "renderer/Renderer.h"
#include "renderer/CustomCommand.h"

USING_NS_AX;
USING_NS_AX_EXT;

#define PTM_RATIO 32

enum
{
    kTagParentNode = 1,
};

Box2DTests::Box2DTests()
{
    ADD_TEST_CASE(Box2DTest);
}

std::string Box2DTest::title() const
{
    return "Box2D - Basic";
}

bool Box2DTest::init()
{
    if (!TestCase::init())
    {
        return false;
    }
    auto dispatcher = Director::getInstance()->getEventDispatcher();

    auto touchListener            = EventListenerTouchAllAtOnce::create();
    touchListener->onTouchesEnded = AX_CALLBACK_2(Box2DTest::onTouchesEnded, this);
    dispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    // init physics
    this->initPhysics();
    // create reset button
    this->createResetButton();

    // Set up sprite
#if 1
    // Use batch node. Faster
    auto parent    = SpriteBatchNode::create("Images/blocks.png", 100);
    _spriteTexture = parent->getTexture();
#else
    // doesn't use batch node. Slower
    _spriteTexture = Director::getInstance()->getTextureCache()->addImage("Images/blocks.png");
    auto parent    = Node::create();
#endif
    addChild(parent, 0, kTagParentNode);

    addNewSpriteAtPosition(VisibleRect::center());

    auto label = Label::createWithTTF("Tap screen add boxes.\nSome objects be only visible with debug on.",
                                      "fonts/Marker Felt.ttf", 12.0f);
    addChild(label, 0);
    label->setColor(Color3B(0, 0, 255));
    label->setPosition(VisibleRect::center().x - 50, VisibleRect::top().y - 60);

    // menu for debug layer
    MenuItemFont::setFontSize(18);
    auto item = MenuItemFont::create("Toggle debug", AX_CALLBACK_1(Box2DTest::toggleDebugCallback, this));

    auto menu = Menu::create(item, nullptr);
    this->addChild(menu);
    menu->setPosition(VisibleRect::right().x - 100, VisibleRect::top().y - 60);

    drawBox2D = g_debugDraw.GetDrawNode();
    addChild(drawBox2D, 100);
    drawBox2D->setOpacity(150);

    scheduleUpdate();

    return true;
}

Box2DTest::Box2DTest() : _spriteTexture(nullptr), world(nullptr) {}

Box2DTest::~Box2DTest()
{
    AX_SAFE_DELETE(world);
}

void Box2DTest::toggleDebugCallback(Object* sender)
{
    showDebugDraw = !showDebugDraw;
    drawBox2D->clear();
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
    groundBodyDef.position.Set(0, 0);  // bottom-left corner

    // Call the body factory which allocates memory for the ground body
    // from a pool and creates the ground box shape (also from a pool).
    // The body is also added to the world.
    b2Body* groundBody = world->CreateBody(&groundBodyDef);

    // Define the ground box shape.
    b2EdgeShape groundBox;

    // bottom
    groundBox.SetTwoSided(b2Vec2(VisibleRect::leftBottom().x / PTM_RATIO, VisibleRect::leftBottom().y / PTM_RATIO),
                          b2Vec2(VisibleRect::rightBottom().x / PTM_RATIO, VisibleRect::rightBottom().y / PTM_RATIO));
    groundBody->CreateFixture(&groundBox, 0);

    // top
    groundBox.SetTwoSided(b2Vec2(VisibleRect::leftTop().x / PTM_RATIO, VisibleRect::leftTop().y / PTM_RATIO),
                          b2Vec2(VisibleRect::rightTop().x / PTM_RATIO, VisibleRect::rightTop().y / PTM_RATIO));
    groundBody->CreateFixture(&groundBox, 0);

    // left
    groundBox.SetTwoSided(b2Vec2(VisibleRect::leftTop().x / PTM_RATIO, VisibleRect::leftTop().y / PTM_RATIO),
                          b2Vec2(VisibleRect::leftBottom().x / PTM_RATIO, VisibleRect::leftBottom().y / PTM_RATIO));
    groundBody->CreateFixture(&groundBox, 0);

    // right
    groundBox.SetTwoSided(b2Vec2(VisibleRect::rightBottom().x / PTM_RATIO, VisibleRect::rightBottom().y / PTM_RATIO),
                          b2Vec2(VisibleRect::rightTop().x / PTM_RATIO, VisibleRect::rightTop().y / PTM_RATIO));
    groundBody->CreateFixture(&groundBox, 0);

    // Small triangle
    b2Vec2 vertices[3];
    vertices[0].Set(-1.0f, 0.0f);
    vertices[1].Set(1.0f, 0.0f);
    vertices[2].Set(0.0f, 2.0f);

    b2PolygonShape polygon;
    polygon.Set(vertices, 3);

    b2FixtureDef triangleShapeDef;
    triangleShapeDef.shape   = &polygon;
    triangleShapeDef.density = 1.0f;

    b2BodyDef triangleBodyDef;
    triangleBodyDef.type = b2_dynamicBody;
    triangleBodyDef.position.Set(rand() % 13 + 3, 4);

    b2Body* body1 = world->CreateBody(&triangleBodyDef);
    body1->CreateFixture(&triangleShapeDef);

    // Large triangle (recycle definitions)
    vertices[0] *= 2.0f;
    vertices[1] *= 2.0f;
    vertices[2] *= 2.0f;
    polygon.Set(vertices, 3);

    triangleBodyDef.position.Set(rand() % 13 + 3, 4);

    b2Body* body2 = world->CreateBody(&triangleBodyDef);
    body2->CreateFixture(&triangleShapeDef);

    // Small box
    polygon.SetAsBox(1.0f, 0.5f);

    b2FixtureDef boxShapeDef;
    boxShapeDef.shape   = &polygon;
    boxShapeDef.density = 1.0f;

    b2BodyDef boxBodyDef;
    boxBodyDef.type = b2_dynamicBody;
    boxBodyDef.position.Set(rand() % 13 + 3, 4);

    b2Body* body3 = world->CreateBody(&boxBodyDef);
    body3->CreateFixture(&boxShapeDef);

    // Large box (recycle definitions)
    polygon.SetAsBox(2.0f, 1.0f);
    boxBodyDef.position.Set(rand() % 13 + 3, 4);

    b2Body* body4 = world->CreateBody(&boxBodyDef);
    body4->CreateFixture(&boxShapeDef);

    // Small circle
    b2CircleShape circle;
    circle.m_radius = 1.0f;

    b2FixtureDef circleShapeDef;
    circleShapeDef.shape   = &circle;
    circleShapeDef.density = 1.0f;

    b2BodyDef circleBodyDef;
    circleBodyDef.type = b2_dynamicBody;
    circleBodyDef.position.Set(rand() % 13 + 3, 4);

    b2Body* body5 = world->CreateBody(&circleBodyDef);
    body5->CreateFixture(&circleShapeDef);

    // Large circle
    circle.m_radius *= 2.0f;
    circleBodyDef.position.Set(rand() % 13 + 3, 4);

    b2Body* body6 = world->CreateBody(&circleBodyDef);
    body6->CreateFixture(&circleShapeDef);

    uint32 flags = 0;
    flags += 1 * b2Draw::e_shapeBit;
    flags += 1 * b2Draw::e_jointBit;
    flags += 0 * b2Draw::e_aabbBit;
    flags += 0 * b2Draw::e_centerOfMassBit;
    g_debugDraw.SetFlags(flags);
    g_debugDraw.mRatio          = PTM_RATIO;
    g_debugDraw.debugNodeOffset = {0, 0};
    world->SetDebugDraw(&g_debugDraw);
}

void Box2DTest::createResetButton()
{
    auto reset = MenuItemImage::create("Images/r1.png", "Images/r2.png",
                                       [&](Object* sender) { getTestSuite()->restartCurrTest(); });

    auto menu = Menu::create(reset, nullptr);

    menu->setPosition(VisibleRect::bottom().x, VisibleRect::bottom().y + 30);
    this->addChild(menu, -1);
}

void Box2DTest::addNewSpriteAtPosition(Vec2 p)
{
    AXLOG("Add sprite %0.2f x %02.f", p.x, p.y);

    // Define the dynamic body.
    // Set up a 1m squared box in the physics world
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(p.x / PTM_RATIO, p.y / PTM_RATIO);

    AXLOG("Add PTM_RATIO sprite %0.2f x %0.2f", p.x / PTM_RATIO, p.y / PTM_RATIO);

    b2Body* body = world->CreateBody(&bodyDef);

    // Define another box shape for our dynamic body.
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(.5f, .5f);  // These are mid points for our 1m box

    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape    = &dynamicBox;
    fixtureDef.density  = 1.0f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);

    auto parent = this->getChildByTag(kTagParentNode);

    // We have a 64x64 sprite sheet with 4 different 32x32 images.  The following code is
    // just randomly picking one of the images
    int idx     = (AXRANDOM_0_1() > .5 ? 0 : 1);
    int idy     = (AXRANDOM_0_1() > .5 ? 0 : 1);
    auto sprite = PhysicsSpriteBox2D::createWithTexture(_spriteTexture, Rect(32 * idx, 32 * idy, 32, 32));
    parent->addChild(sprite);
    sprite->setB2Body(body);
    sprite->setPTMRatio(PTM_RATIO);
    sprite->setPosition(ax::Vec2(p.x, p.y));
}

void Box2DTest::update(float dt)
{
    // It is recommended that a fixed time step is used with Box2D for stability
    // of the simulation, however, we are using a variable time step here.
    // You need to make an informed choice, the following URL is useful
    // http://gafferongames.com/game-physics/fix-your-timestep/

    int velocityIterations = 8;
    int positionIterations = 1;

    // Instruct the world to perform a single step of simulation. It is
    // generally best to keep the time step and iterations fixed.
    world->Step(dt, velocityIterations, positionIterations);

    // Debug draw
    if (showDebugDraw)
    {
        drawBox2D->clear();
        world->DebugDraw();
    }
}

void Box2DTest::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    // Add a new body/atlas sprite at the touched location

    for (auto& touch : touches)
    {
        if (!touch)
            break;

        auto location = touch->getLocation();

        addNewSpriteAtPosition(location);
    }
}