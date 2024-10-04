/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2021 @aismann; Peter Eismann, Germany; dreifrankensoft

 https://axmol.dev/

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

using namespace ax;
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

    scheduleUpdate();

    return true;
}

Box2DTest::Box2DTest() : _spriteTexture(nullptr) {}

Box2DTest::~Box2DTest()
{
    b2DestroyWorld(world);
}

void Box2DTest::toggleDebugCallback(Object* sender)
{
    showDebugDraw = !showDebugDraw;
    _debugDrawNode->setVisible(showDebugDraw);
}

b2BodyId Box2DTest::createRigibody(b2BodyDef* def)
{
    auto bodyId = b2CreateBody(world, def);
    return bodyId;
}

void Box2DTest::initPhysics()
{
    b2Vec2 gravity      = {0.0f, -10.0f};
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity    = gravity;
    world               = b2CreateWorld(&worldDef);

    // debug draw node for world
    _debugDrawNode = utils::createInstance<PhysicsDebugNode>(&PhysicsDebugNode::initWithWorld, world);
    addChild(_debugDrawNode, 100);
    _debugDrawNode->setOpacity(150);

    // Do we want to let bodies sleep?
    b2World_EnableSleeping(world, true);
    b2World_EnableContinuous(world, true);

    // The segment ground
    b2BodyDef bodyDef = b2DefaultBodyDef();
    b2BodyId groundId = createRigibody(&bodyDef);

    b2ShapeDef shapeDef = b2DefaultShapeDef();

    auto conerOffset  = 1.0f / PTM_RATIO;
    auto viewRect     = VisibleRect::getVisibleRect();
    b2Segment segment = {{conerOffset, conerOffset}, {viewRect.size.width / PTM_RATIO - conerOffset, conerOffset}};
    b2CreateSegmentShape(groundId, &shapeDef, &segment);  // bottom

    segment = {{conerOffset, viewRect.size.height / PTM_RATIO - conerOffset},
               {viewRect.size.width / PTM_RATIO - conerOffset, viewRect.size.height / PTM_RATIO - conerOffset}};
    b2CreateSegmentShape(groundId, &shapeDef, &segment);  // top

    segment = {{conerOffset, conerOffset}, {conerOffset, viewRect.size.height / PTM_RATIO - conerOffset}};
    b2CreateSegmentShape(groundId, &shapeDef, &segment);  // left

    segment = {{viewRect.size.width / PTM_RATIO - conerOffset, conerOffset},
               {viewRect.size.width / PTM_RATIO - conerOffset, viewRect.size.height / PTM_RATIO - conerOffset}};
    b2CreateSegmentShape(groundId, &shapeDef, &segment);  // right

    // Small triangle
    b2Vec2 vertices[3];
    vertices[0]       = b2Vec2{-1.0f, 0.0f};
    vertices[1]       = b2Vec2{1.0f, 0.0f};
    vertices[2]       = b2Vec2{0.0f, 2.0f};
    b2Hull hull       = b2ComputeHull(vertices, 3);
    b2Polygon polygon = b2MakePolygon(&hull, 0.0f);

    b2BodyDef triangleBodyDef   = b2DefaultBodyDef();
    triangleBodyDef.type        = b2_dynamicBody;
    triangleBodyDef.position    = b2Vec2{static_cast<float>(rand() % 13 + 3), 4.0f};  // bottom-left corner
    auto body1                  = createRigibody(&triangleBodyDef);
    b2ShapeDef triangleShapeDef = b2DefaultShapeDef();
    triangleShapeDef.density    = 1.0f;
    b2CreatePolygonShape(body1, &triangleShapeDef, &polygon);

    // Large triangle (recycle definitions)
    vertices[0] *= 2.0f;
    vertices[1] *= 2.0f;
    vertices[2] *= 2.0f;

    triangleBodyDef.position = b2Vec2{static_cast<float>(rand() % 13 + 3), 4.0f};
    auto body2               = createRigibody(&triangleBodyDef);
    hull                     = b2ComputeHull(vertices, 3);
    polygon                  = b2MakePolygon(&hull, 0.0f);
    b2CreatePolygonShape(body2, &triangleShapeDef, &polygon);

    // Small box
    polygon = b2MakeBox(1.0f, 0.5f);  // .SetAsBox(1.0f, 0.5f);

    auto boxShapeDef    = b2DefaultShapeDef();
    boxShapeDef.density = 1.0f;

    b2BodyDef boxBodyDef = b2DefaultBodyDef();
    boxBodyDef.type      = b2_dynamicBody;
    boxBodyDef.position  = b2Vec2{static_cast<float>(rand() % 13 + 3), 4.0f};

    auto body3 = createRigibody(&boxBodyDef);
    b2CreatePolygonShape(body3, &boxShapeDef, &polygon);

    // Large box (recycle definitions)
    polygon             = b2MakeBox(2.0f, 1.0f);  // .SetAsBox(2.0f, 1.0f);
    boxBodyDef.position = b2Vec2{static_cast<float>(rand() % 13 + 3), 4.0f};

    auto body4 = createRigibody(&boxBodyDef);
    b2CreatePolygonShape(body4, &boxShapeDef, &polygon);

    // Small circle
    auto circleShapeDef    = b2DefaultShapeDef();
    circleShapeDef.density = 1.0f;

    b2Circle circle{{0.0f, 0.0f}, 1.0f};

    b2BodyDef circleBodyDef = b2DefaultBodyDef();
    circleBodyDef.type      = b2_dynamicBody;
    circleBodyDef.position  = b2Vec2{static_cast<float>(rand() % 13 + 3), 4.0f};

    auto body5 = createRigibody(&circleBodyDef);
    b2CreateCircleShape(body5, &circleShapeDef, &circle);

    // Large circle
    circle.radius *= 2.0f;
    circleBodyDef.position = b2Vec2{static_cast<float>(rand() % 13 + 3), 4.0f};

    auto body6 = createRigibody(&circleBodyDef);
    b2CreateCircleShape(body6, &circleShapeDef, &circle);

    _debugDrawNode->setPTMRatio(PTM_RATIO);
    auto& settings      = _debugDrawNode->getB2DebugDraw();
    settings.drawShapes = true;
    settings.drawJoints = true;
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
    AXLOGD("Add sprite x: {:.2} y: {:.2}", p.x, p.y);

    // Define the dynamic body.
    // Set up a 1m squared box in the physics world
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type      = b2_dynamicBody;
    bodyDef.position  = b2Vec2{p.x / PTM_RATIO, p.y / PTM_RATIO};

    AXLOGD("Add PTM_RATIO sprite x: {:.2} y: {:.2}", p.x / PTM_RATIO, p.y / PTM_RATIO);

    auto body = createRigibody(&bodyDef);

    // Define another box shape for our dynamic body.
    auto dynamicBox = b2MakeBox(.5f, .5f);  // These are mid points for our 1m box

    // Define the dynamic body fixture.
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density    = 1.0f;
    shapeDef.friction   = 0.3f;
    b2CreatePolygonShape(body, &shapeDef, &dynamicBox);

    auto parent = this->getChildByTag(kTagParentNode);

    // We have a 64x64 sprite sheet with 4 different 32x32 images.  The following code is
    // just randomly picking one of the images
    int idx     = (AXRANDOM_0_1() > .5 ? 0 : 1);
    int idy     = (AXRANDOM_0_1() > .5 ? 0 : 1);
    auto sprite = PhysicsSprite::createWithTexture(_spriteTexture, Rect(32 * idx, 32 * idy, 32, 32));
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

    // The number of sub-steps, increasing the sub-step count can increase accuracy. Typically 4.
    constexpr int subStepCount = 4;
    b2World_Step(world, _director->getAnimationInterval(), subStepCount);
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
