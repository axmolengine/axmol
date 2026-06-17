/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

#include "PhysicsTest.h"

#if defined(AX_ENABLE_PHYSICS_2D)

#    include <cmath>
#    include "axmol/ui/axmol-ui.h"
#    include "../testResource.h"

#    include "physics-nodes/PhysicsDebugNode.h"

using namespace ax;

PhysicsTests::PhysicsTests()
{
    ADD_TEST_CASE(PhysicsDemoLogoSmash);
    ADD_TEST_CASE(PhysicsDemoPyramidStack);
    ADD_TEST_CASE(PhysicsDemoClickAdd);
    ADD_TEST_CASE(PhysicsDemoRayCast);
    ADD_TEST_CASE(PhysicsDemoActions);
    ADD_TEST_CASE(PhysicsDemoJoints);
    ADD_TEST_CASE(PhysicsDemoPump);
    ADD_TEST_CASE(PhysicsDemoOneWayPlatform);
    ADD_TEST_CASE(PhysicsDemoSlice);
    ADD_TEST_CASE(PhysicsDemoBug3988);
    ADD_TEST_CASE(PhysicsContactTest);
    ADD_TEST_CASE(PhysicsPositionRotationTest);
    ADD_TEST_CASE(PhysicsSetGravityEnableTest);
    ADD_TEST_CASE(PhysicsDemoBug5482);
    ADD_TEST_CASE(PhysicsFixedUpdate);
    ADD_TEST_CASE(PhysicsTransformTest);
    ADD_TEST_CASE(PhysicsIssue9959);
    ADD_TEST_CASE(PhysicsIssue15932);
    ADD_TEST_CASE(PhysicsDemoPyramidStackFixedUpdate);
}

namespace
{
Color STATIC_COLOR(1.0f, 0.0f, 0.0f, 1.0f);
constexpr auto DRAG_BODYS_BITS = 1u << 7;
}  // namespace

void PhysicsDemo::toggleDebug()
{
#    if defined(AX_ENABLE_PHYSICS_2D)
    _debugDraw = !_debugDraw;
    if (_debugDrawNode)
        _debugDrawNode->setVisible(_debugDraw);
#    endif
}

PhysicsDemo::PhysicsDemo() : _spriteTexture(nullptr), _ball(nullptr), _debugDraw(false) {}

bool PhysicsDemo::init()
{
    TestCase::init();
    setFixedUpdateEnabled(false);
    return initWithPhysics();
}

PhysicsDemo::~PhysicsDemo()
{
    if (_pointerListener)
        _eventDispatcher->removeEventListener(_pointerListener);
}

std::string PhysicsDemo::title() const
{
    return "PhysicsComponentTest";
}

void PhysicsDemo::onEnter()
{
    TestCase::onEnter();

    _pointerListener = PointerEventListener::create();

    _pointerListener->onPointerDown = AX_CALLBACK_1(PhysicsDemo::onPointerDown, this);
    _pointerListener->onPointerMove = AX_CALLBACK_1(PhysicsDemo::onPointerMove, this);
    _pointerListener->onPointerUp   = AX_CALLBACK_1(PhysicsDemo::onPointerUp, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_pointerListener, this);

    // create debug draw node
    auto debugDrawNode = utils::createInstance<extension::PhysicsDebugNode>(&extension::PhysicsDebugNode::initWithWorld,
                                                                            getPhysicsWorld2D()->internalHandle());
    addChild(debugDrawNode, 100);
    debugDrawNode->setOpacity(150);
    auto& settings      = debugDrawNode->getB2DebugDraw();
    settings.drawShapes = true;
    settings.drawJoints = true;
    settings.drawBounds = false;
    debugDrawNode->setVisible(_debugDraw);
    _debugDrawNode = debugDrawNode;

    // batch sprite
    _spriteTexture = SpriteBatchNode::create("Images/grossini_dance_atlas.png", 100)->getTexture();

    // menu for debug layer
    MenuItemFont::setFontSize(18);
    auto item = MenuItemFont::create("Toggle debug", AX_CALLBACK_1(PhysicsDemo::toggleDebugCallback, this));

    auto menu = Menu::create(item, nullptr);
    this->addChild(menu);
    menu->setPosition(Vec2(VisibleRect::right().x - item->getContentSize().width / 2 - 10,
                           VisibleRect::top().y - item->getContentSize().height / 2 - 10));
}

Sprite* PhysicsDemo::addGrossiniAtPosition(Vec2 p, float scale /* = 1.0*/, bool allowDrag /* = true */)
{
    AXLOGD("Add sprite x: {:.2} y: {:.2}", p.x, p.y);

    int posx, posy;

    posx = AXRANDOM_0_1() * 200.0f;
    posy = AXRANDOM_0_1() * 200.0f;

    posx = (posx % 4) * 85;
    posy = (posy % 3) * 121;

    auto sp = Sprite::createWithTexture(_spriteTexture, Rect(posx, posy, 85, 121));

    sp->setScale(scale);
    auto rigidbody = Rigidbody2D::createBox(Size(48.0f, 108.0f));
    if (allowDrag)
        rigidbody->setTag(DRAG_BODYS_BITS);
    sp->addComponent(rigidbody);
    sp->setPosition(p);
    this->addChild(sp);

    return sp;
}

void PhysicsDemo::toggleDebugCallback(Object* /*sender*/)
{
    toggleDebug();
}

namespace
{
const int LOGO_WIDTH_COLORED     = 146;
const int LOGO_HEIGHT_COLORED    = 38;
const float LOGO_IMAGE_COLORED[] = {
    -37.0f, 2.5f,  -143.0f, 2.5f,  -143.0f, 2.5f,  -2.0f,   2.5f,  -139.0f, 2.5f,  -2.0f,   2.5f,  -140.0f, 1.5f,
    -2.0f,  2.5f,  -2.0f,   2.5f,  -133.0f, 3.2f,  -3.0f,   2.5f,  -2.0f,   2.5f,  -132.0f, 7.2f,  -1.0f,   1.5f,
    -2.0f,  2.5f,  -132.0f, 3.2f,  -3.0f,   3.2f,  -2.0f,   2.5f,  -8.0f,   12.1f, -67.0f,  11.1f, -35.0f,  2.2f,
    -5.0f,  2.2f,  -2.0f,   1.5f,  -8.0f,   16.1f, -48.0f,  8.1f,  -7.0f,   15.1f, -31.0f,  2.2f,  -7.0f,   2.2f,
    -10.0f, 17.1f, -45.0f,  11.1f, -5.0f,   17.1f, -30.0f,  2.2f,  -7.0f,   2.2f,  -10.0f,  18.1f, -43.0f,  13.1f,
    -4.0f,  18.1f, -29.0f,  2.2f,  -7.0f,   2.2f,  -10.0f,  19.1f, -41.0f,  15.1f, -3.0f,   19.1f, -29.0f,  2.2f,
    -5.0f,  2.2f,  -11.0f,  20.1f, -40.0f,  16.1f, -2.0f,   20.1f, -28.0f,  3.2f,  -3.0f,   3.2f,  -11.0f,  20.1f,
    -8.0f,  6.1f,  -8.0f,   5.1f,  -6.0f,   5.1f,  -1.0f,   17.1f, -2.0f,   21.1f, -13.0f,  13.4f, -2.0f,   7.2f,
    -12.0f, 7.1f,  -6.0f,   7.1f,  -6.0f,   10.1f, -5.0f,   7.1f,  -4.0f,   14.1f, -3.0f,   8.1f,  -1.0f,   8.1f,
    -5.0f,  8.1f,  -13.0f,  13.4f, -4.0f,   3.2f,  -14.0f,  7.1f,  -6.0f,   7.1f,  -5.0f,   12.1f, -4.0f,   8.1f,
    -2.0f,  15.1f, -4.0f,   7.1f,  -1.0f,   8.1f,  -6.0f,   7.1f,  -13.0f,  13.4f, -21.0f,  7.1f,  -5.0f,   8.1f,
    -4.0f,  14.1f, -4.0f,   7.1f,  -1.0f,   8.1f,  -1.0f,   7.1f,  -4.0f,   7.1f,  -1.0f,   8.1f,  -6.0f,   7.1f,
    -13.0f, 3.4f,  -7.0f,   3.4f,  -21.0f,  20.1f, -3.0f,   16.1f, -3.0f,   15.1f, -3.0f,   5.1f,  -5.0f,   7.1f,
    -1.0f,  8.1f,  -6.0f,   7.1f,  -13.0f,  3.4f,  -7.0f,   3.4f,  -21.0f,  19.1f, -4.0f,   17.1f, -3.0f,   14.1f,
    -12.0f, 7.1f,  -2.0f,   8.1f,  -7.0f,   6.1f,  -13.0f,  3.4f,  -7.0f,   3.4f,  -21.0f,  18.1f, -4.0f,   8.1f,
    -2.0f,  8.1f,  -4.0f,   12.1f, -12.0f,  8.1f,  -2.0f,   8.1f,  -7.0f,   6.1f,  -13.0f,  3.4f,  -7.0f,   3.4f,
    -21.0f, 19.1f, -3.0f,   7.1f,  -4.0f,   8.1f,  -3.0f,   11.1f, -12.0f,  8.1f,  -3.0f,   8.1f,  -7.0f,   6.1f,
    -13.0f, 3.4f,  -7.0f,   3.4f,  -21.0f,  20.1f, -1.0f,   8.1f,  -5.0f,   7.1f,  -4.0f,   10.1f, -11.0f,  8.1f,
    -4.0f,  8.1f,  -7.0f,   6.1f,  -13.0f,  3.4f,  -7.0f,   3.4f,  -21.0f,  20.1f, -1.0f,   8.1f,  -5.0f,   7.1f,
    -4.0f,  9.1f,  -10.0f,  9.1f,  -5.0f,   8.1f,  -7.0f,   6.1f,  -13.0f,  3.4f,  -7.0f,   3.4f,  -21.0f,  7.1f,
    -6.0f,  15.1f, -6.0f,   7.1f,  -5.0f,   8.1f,  -9.0f,   9.1f,  -6.0f,   8.1f,  -6.0f,   7.1f,  -8.0f,   23.4f,
    -16.0f, 7.1f,  -7.0f,   14.1f, -6.0f,   7.1f,  -4.0f,   9.1f,  -8.0f,   9.1f,  -7.0f,   8.1f,  -6.0f,   7.1f,
    -8.0f,  23.4f, -16.0f,  7.1f,  -7.0f,   15.1f, -5.0f,   7.1f,  -4.0f,   10.1f, -6.0f,   9.1f,  -8.0f,   8.1f,
    -6.0f,  7.1f,  -8.0f,   23.4f, -16.0f,  7.1f,  -6.0f,   8.1f,  -1.0f,   7.1f,  -4.0f,   8.1f,  -3.0f,   12.1f,
    -4.0f,  8.1f,  -10.0f,  8.1f,  -4.0f,   9.1f,  -8.0f,   3.4f,  -7.0f,   3.4f,  -7.0f,   3.4f,  -16.0f,  21.1f,
    -1.0f,  8.1f,  -3.0f,   7.1f,  -3.0f,   13.1f, -4.0f,   16.1f, -2.0f,   20.1f, -9.0f,   3.4f,  -7.0f,   3.4f,
    -7.0f,  3.4f,  -16.0f,  20.1f, -2.0f,   18.1f, -3.0f,   14.1f, -2.0f,   18.1f, -1.0f,   20.1f, -9.0f,   3.4f,
    -7.0f,  3.4f,  -7.0f,   3.4f,  -16.0f,  20.1f, -3.0f,   16.1f, -3.0f,   16.1f, -1.0f,   18.1f, -1.0f,   19.1f,
    -10.0f, 3.4f,  -7.0f,   3.4f,  -7.0f,   3.4f,  -16.0f,  19.1f, -5.0f,   15.1f, -2.0f,   8.1f,  -2.0f,   7.1f,
    -1.0f,  18.1f, -1.0f,   18.1f, -11.0f,  3.4f,  -7.0f,   3.4f,  -7.0f,   3.4f,  -16.0f,  18.1f, -7.0f,   13.1f,
    -3.0f,  7.1f,  -3.0f,   26.1f, -1.0f,   17.1f, -12.0f,  3.4f,  -7.0f,   3.4f,  -7.0f,   3.4f,  -17.0f,  15.1f,
    -10.0f, 10.1f, -6.0f,   5.1f,  -5.0f,   6.1f,  -2.0f,   16.1f, -3.0f,   14.1f, -14.0f,  3.4f,  -7.0f,   3.4f,
    -7.0f,  3.4f,  -44.0f,  6.1f,  -73.0f,  23.4f, -115.0f, 40.3f, -106.0f, 40.3f, -106.0f, 40.3f, -106.0f};

float frand()
{
    return rand() / RAND_MAX;
}
}  // namespace

Sprite* PhysicsDemo::makeBall(Vec2 point, float radius, const ax::PhysicsMaterial2D& material)
{
    Sprite* ball = nullptr;
    if (_ball != nullptr)
        ball = Sprite::createWithTexture(_ball->getTexture());
    else
        ball = Sprite::create("Images/ball.png");

    ball->setScale(0.13f * radius);

    auto rigidbody = Rigidbody2D::createCircle(ball->getContentSize().width / 2, material);
    rigidbody->setTag(DRAG_BODYS_BITS);
    ball->addComponent(rigidbody);
    ball->setPosition(Vec2(point.x, point.y));

    return ball;
}

Sprite* PhysicsDemo::makeBox(Vec2 point, Size size, int color, const ax::PhysicsMaterial2D& material)
{
    bool yellow = false;
    if (color == 0)
    {
        yellow = AXRANDOM_0_1() > 0.5f;
    }
    else
    {
        yellow = color == 1;
    }

    auto box = yellow ? Sprite::create("Images/YellowSquare.png") : Sprite::create("Images/CyanSquare.png");
    box->setScaleX(size.width / 100.0f);
    box->setScaleY(size.height / 100.0f);

    auto rigidbody = Rigidbody2D::createBox(box->getContentSize(), material);
    rigidbody->setTag(DRAG_BODYS_BITS);
    box->addComponent(rigidbody);

    box->setPosition(Vec2(point.x, point.y));

    return box;
}

Sprite* PhysicsDemo::makeTriangle(Vec2 point, Size size, int color, const ax::PhysicsMaterial2D& material)
{
    bool yellow = false;
    if (color == 0)
    {
        yellow = AXRANDOM_0_1() > 0.5f;
    }
    else
    {
        yellow = color == 1;
    }

    auto triangle = yellow ? Sprite::create("Images/YellowTriangle.png") : Sprite::create("Images/CyanTriangle.png");

    if (size.height == 0)
    {
        triangle->setScale(size.width / 100.0f);
    }
    else
    {
        triangle->setScaleX(size.width / 50.0f);
        triangle->setScaleY(size.height / 43.5f);
    }

    Vec2 vers[] = {Vec2(0.0f, triangle->getContentSize().height / 2),
                   Vec2(triangle->getContentSize().width / 2, -triangle->getContentSize().height / 2),
                   Vec2(-triangle->getContentSize().width / 2, -triangle->getContentSize().height / 2)};

    triangle->addComponent(Rigidbody2D::createPolygon(vers, material));
    triangle->setPosition(Vec2(point.x, point.y));

    return triangle;
}

bool PhysicsDemo::onPointerDown(PointerEvent* event)
{
    _isPressed = true;

    auto location = event->getLocation();
    auto collider = _physicsWorld2D->overlapPoint(location);

    if (!collider)
        return _isPressed;

    auto body = collider->getAttachedBody();
    if ((body->getTag() & DRAG_BODYS_BITS) != 0)
    {
        Node* mouse   = Node::create();
        auto moseBody = Rigidbody2D::create();
        moseBody->setKinematic();
        mouse->addComponent(moseBody);
        this->addChild(mouse);
        auto joint = TargetJoint2D::create(location, body);
        joint->setMaxForceScale(100.0f);
        mouse->addComponent(joint);
        mouse->setPosition(location);
        _draggers.insert(std::make_pair(event->getPointerId(), mouse));

        return _isPressed;
    }

    return _isPressed;
}

void PhysicsDemo::onPointerMove(PointerEvent* event)
{
    auto it = _draggers.find(event->getPointerId());

    if (it != _draggers.end())
    {
        auto mouseNode = it->second;
        mouseNode->setPosition(event->getLocation());

        return;
    }
}

void PhysicsDemo::onPointerUp(PointerEvent* event)
{
    _isPressed = false;

    auto it = _draggers.find(event->getPointerId());

    if (it != _draggers.end())
    {
        this->removeChild(it->second);
        _draggers.erase(it);
    }
}

// Implementation of PhysicsComponentDemoLogoSmash

void PhysicsDemoLogoSmash::onEnter()
{
    PhysicsDemo::onEnter();

    Color32 colorList[] = {Color32::white, Color32::gray,        Color32::red,
                           Color32::green, Color32(0, 178, 255), Color32(255, 174, 201)};

    _physicsWorld2D->setGravity(Vec2(0.0f, 0.0f));

    _ball = SpriteBatchNode::create(
        "Images/ball.png", LOGO_WIDTH_COLORED * LOGO_HEIGHT_COLORED + LOGO_WIDTH_COLORED + LOGO_HEIGHT_COLORED);
    addChild(_ball);

    int x     = 0;
    int y     = 0;
    int index = 0;
    while (index < (sizeof(LOGO_IMAGE_COLORED) / sizeof(LOGO_IMAGE_COLORED[0])))
    {
        int colPos  = 0;
        int step    = 0;
        float value = LOGO_IMAGE_COLORED[index];

        if (value < 0)
            step = abs(value);
        else
        {
            step = int(value);
            value += 0.12f;  // (int) rounding "fix" for next line
            colPos = int((value - step) * 10 - 1);
        }
        while (step > 0)
        {
            if (colorList[colPos] != Color32::white)
            {
                float xJitter = 0.15 * frand();
                float yJitter = 0.15 * frand();

                Node* ball = makeBall(
                    Vec2(2 * (x + xJitter) + VisibleRect::getVisibleRect().size.width / 4,
                         (2 * (-y + yJitter) + VisibleRect::getVisibleRect().size.height / 2 + LOGO_HEIGHT_COLORED)),
                    0.95f, PhysicsMaterial2D(0.01f, 0.0f, 0.0f));
                ball->setColor(colorList[colPos]);
                auto Rigidbody2D = ball->getRigidbody2D();

                _ball->addChild(ball);
            }
            x++;
            step--;
            if (x >= LOGO_WIDTH_COLORED)
            {
                y++;
                x = 0;
            }
        }
        index++;
    }
    auto bullet = makeBall(Vec2(0.0f, 0.0f), 10, PhysicsMaterial2D(phconsts::UpperBound, 0, 0));
    bullet->getRigidbody2D()->setVelocity(Vec2(200.0f, 0.0f));
    bullet->setPosition(Vec2(-100.0f, VisibleRect::getVisibleRect().size.height / 2));
    _ball->addChild(bullet);
}

std::string PhysicsDemoLogoSmash::title() const
{
    return "Logo Smash";
}

// Implementation of PhysicsComponentDemoClickAdd

PhysicsDemoClickAdd::~PhysicsDemoClickAdd()
{
    Device::setPreferredOrientation(Device::Orientation::SensorLandscape);
    Device::setAccelerometerEnabled(false);
}

void PhysicsDemoClickAdd::onEnter()
{
    PhysicsDemo::onEnter();

    Device::setAccelerometerEnabled(true);
    auto accListener = AccelerationEventListener::create(AX_CALLBACK_1(PhysicsDemoClickAdd::onAcceleration, this));
    _eventDispatcher->addEventListenerWithSceneGraphPriority(accListener, this);

    auto node = Node::create();
    node->addComponent(Rigidbody2D::createEdgeBox(VisibleRect::getVisibleRect().size * 0.95));
    node->setPosition(VisibleRect::center());
    this->addChild(node);

    addGrossiniAtPosition(VisibleRect::center());

    Device::setPreferredOrientation(Device::Orientation::Landscape);
}

std::string PhysicsDemoClickAdd::subtitle() const
{
    return "multi touch to add grossini";
}

bool PhysicsDemoClickAdd::onPointerDown(ax::PointerEvent* event)
{
    auto ret = PhysicsDemo::onPointerDown(event);

    return ret || _isPressed;
}

void PhysicsDemoClickAdd::onPointerUp(PointerEvent* event)
{
    // Add a new body/atlas sprite at the touched location

    PhysicsDemo::onPointerUp(event);

    auto location = event->getLocation();

    addGrossiniAtPosition(location);
}

void PhysicsDemoClickAdd::onAcceleration(AccelerationEvent* event)
{
    static float prevX = 0, prevY = 0;

#    define FILTER_FACTOR 0.05f

    auto& acc = event->getAcceleration();

    float accelX = (float)acc.x * FILTER_FACTOR + (1 - FILTER_FACTOR) * prevX;
    float accelY = (float)acc.y * FILTER_FACTOR + (1 - FILTER_FACTOR) * prevY;

    prevX = accelX;
    prevY = accelY;

    auto v = Vec2(accelX, accelY);
    v      = v * 200;

    _physicsWorld2D->setGravity(v);
}

void PhysicsDemoPyramidStack::onEnter()
{
    PhysicsDemo::onEnter();

    auto node = Node::create();
    node->addComponent(Rigidbody2D::createEdgeSegment(VisibleRect::leftBottom() + Vec2(0.0f, 50.0f),
                                                      VisibleRect::rightBottom() + Vec2(0.0f, 50.0f)));
    this->addChild(node);

    auto ball = Sprite::create("Images/ball.png");
    ball->setScale(1);
    ball->setTag(100);
    auto body = Rigidbody2D::createCircle(10);
    body->setTag(DRAG_BODYS_BITS);
    ball->addComponent(body);
    ball->setPosition(VisibleRect::bottom() + Vec2(0.0f, 60.0f));
    this->addChild(ball);

    scheduleOnce(AX_SCHEDULE_SELECTOR(PhysicsDemoPyramidStack::updateOnce), 3.0);

    for (int i = 0; i < 14; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            addGrossiniAtPosition(VisibleRect::bottom() + Vec2((i / 2 - j) * 11, (14 - i) * 23 + 100), 0.2f);
        }
    }
}

void PhysicsDemoPyramidStack::updateOnce(float /*delta*/)
{
    auto ball = getChildByTag(100);
    if (ball)
        ball->setScale(ball->getScale() * 3);
}

std::string PhysicsDemoPyramidStack::title() const
{
    return "Pyramid Stack";
}

PhysicsDemoRayCast::PhysicsDemoRayCast() : _angle(0.0f), _node(nullptr), _mode(0) {}

void PhysicsDemoRayCast::onEnter()
{
    PhysicsDemo::onEnter();

    _physicsWorld2D->setGravity(Point::zero);

    auto node = DrawNode::create();
    node->addComponent(Rigidbody2D::createEdgeSegment(VisibleRect::leftBottom() + Vec2(0.0f, 50.0f),
                                                      VisibleRect::rightBottom() + Vec2(0.0f, 50.0f)));
    node->drawSegment(VisibleRect::leftBottom() + Vec2(0.0f, 50.0f), VisibleRect::rightBottom() + Vec2(0.0f, 50.0f),
                      0.5f, STATIC_COLOR);
    this->addChild(node);

    MenuItemFont::setFontSize(18);
    auto item = MenuItemFont::create("Change Mode(any)", AX_CALLBACK_1(PhysicsDemoRayCast::changeModeCallback, this));

    auto menu = Menu::create(item, nullptr);
    this->addChild(menu);
    menu->setPosition(Vec2(VisibleRect::left().x + 100, VisibleRect::top().y - 10));

    scheduleUpdate();
}

void PhysicsDemoRayCast::changeModeCallback(Object* sender)
{
    _mode = (_mode + 1) % 3;

    switch (_mode)
    {
    case 0:
        ((MenuItemFont*)sender)->setString("Change Mode(any)");
        break;
    case 1:
        ((MenuItemFont*)sender)->setString("Change Mode(closest)");
        break;
    case 2:
        ((MenuItemFont*)sender)->setString("Change Mode(multiple)");
        break;

    default:
        break;
    }
}

bool PhysicsDemoRayCast::anyRay(PhysicsWorld2D& /*world*/, const RayCastHit2D& hitInfo, void* data)
{
    *((Vec2*)data) = hitInfo.point;
    return false;
}

void PhysicsDemoRayCast::update(float /*delta*/)
{
    float L     = 150.0f;
    Vec2 point1 = VisibleRect::center();
    Vec2 d(L * cosf(_angle), L * sinf(_angle));
    Vec2 point2 = point1 + d;

    removeChild(_node);
    _node = DrawNode::create();
    switch (_mode)
    {
    case 0:  // any
    {
        Vec2 point3 = point2;
        auto func   = AX_CALLBACK_3(PhysicsDemoRayCast::anyRay, this);

        _physicsWorld2D->rayCast(func, Ray2D::fromPoints(point1, point2), &point3);
        _node->drawSegment(point1, point3, 0.5f, STATIC_COLOR);

        if (point2 != point3)
        {
            _node->drawDot(point3, 2, Color(1.0f, 1.0f, 1.0f, 1.0f));
        }
        addChild(_node);

        break;
    }
    case 1:  // closest
    {
        Vec2 point3  = point2;
        auto castRet = _physicsWorld2D->rayCastClosest(Ray2D::fromPoints(point1, point2));
        if (castRet)
            point3 = castRet->point;
        _node->drawSegment(point1, point3, 1, STATIC_COLOR);

        if (point2 != point3)
        {
            _node->drawDot(point3, 2, Color(1.0f, 1.0f, 1.0f, 1.0f));
        }
        addChild(_node);

        break;
    }
    case 2:  // mutliple
    {
#    define MAX_MULTI_RAYCAST_NUM 5
        Vec2 points[MAX_MULTI_RAYCAST_NUM];
        int num = 0;

        RayCastHitCallback2D func = [&points, &num](PhysicsWorld2D& /*world*/, const RayCastHit2D& info,
                                                    void* /*data*/) -> bool {
            if (num < MAX_MULTI_RAYCAST_NUM)
            {
                points[num++] = info.point;
            }

            return true;
        };

        _physicsWorld2D->rayCast(func, Ray2D::fromPoints(point1, point2), nullptr);

        _node->drawSegment(point1, point2, 1, STATIC_COLOR);

        for (int i = 0; i < num; ++i)
        {
            _node->drawDot(points[i], 2, Color(1.0f, 1.0f, 1.0f, 1.0f));
        }

        addChild(_node);

        break;
    }

    default:
        break;
    }

    _angle += 0.25f * (float)M_PI / 180.0f;
}

void PhysicsDemoRayCast::onPointerUp(PointerEvent* event)
{
    // Add a new body/atlas sprite at the touched location
    {
        PhysicsDemo::onPointerUp(event);

        auto location = event->getLocation();

        float r = AXRANDOM_0_1();

        if (r < 1.0f / 3.0f)
        {
            addChild(makeBall(location, 5 + AXRANDOM_0_1() * 10));
        }
        else if (r < 2.0f / 3.0f)
        {
            addChild(makeBox(location, Size(10 + AXRANDOM_0_1() * 15, 10 + AXRANDOM_0_1() * 15)));
        }
        else
        {
            addChild(makeTriangle(location, Size(10 + AXRANDOM_0_1() * 20, 10 + AXRANDOM_0_1() * 20)));
        }
    }
}

std::string PhysicsDemoRayCast::title() const
{
    return "Ray Cast";
}

void PhysicsDemoActions::onEnter()
{
    PhysicsDemo::onEnter();
    _physicsWorld2D->setGravity(Vec2::zero);

    auto node = Node::create();
    node->addComponent(Rigidbody2D::createEdgeBox(VisibleRect::getVisibleRect().size));
    node->setPosition(VisibleRect::center());
    this->addChild(node);

    Sprite* sp1 = addGrossiniAtPosition(VisibleRect::center());
    Sprite* sp2 = addGrossiniAtPosition(VisibleRect::left() + Vec2(50.0f, 0.0f));
    Sprite* sp3 = addGrossiniAtPosition(VisibleRect::right() - Vec2(20.0f, 0.0f));
    Sprite* sp4 = addGrossiniAtPosition(VisibleRect::leftTop() + Vec2(50.0f, -50.0f));
    sp4->getRigidbody2D()->setGravityEnabled(false);

    auto actionTo     = JumpTo::create(2, Vec2(100, 100), 50, 4);
    auto actionBy     = JumpBy::create(2, Vec2(300, 0), 50, 4);
    auto actionUp     = JumpBy::create(2, Vec2(0, 50), 80, 4);
    auto actionByBack = actionBy->reverse();
    auto rotateBy     = RotateBy::create(2, 180);
    auto rotateByBack = RotateBy::create(2, -180);

    sp1->runAction(RepeatForever::create(actionUp));
    sp2->runAction(RepeatForever::create(Sequence::create(actionBy, actionByBack, nullptr)));
    sp3->runAction(actionTo);
    sp4->runAction(RepeatForever::create(Sequence::create(rotateBy, rotateByBack, nullptr)));
}

std::string PhysicsDemoActions::title() const
{
    return "Actions";
}

// implementation of PhysicsDemoJoints

void PhysicsDemoJoints::onEnter()
{
    PhysicsDemo::onEnter();
    toggleDebug();

    float width  = (VisibleRect::getVisibleRect().size.width - 10) / 4;
    float height = (VisibleRect::getVisibleRect().size.height - 50) / 4;

    Node* node            = Node::create();
    Rigidbody2D* worldBox = Rigidbody2D::create();
    node->addComponent(worldBox);

    worldBox->setBodyType(Rigidbody2D::StaticBody);
    node->setPosition(Point::zero);
    this->addChild(node);

    int testidx = 10;

    // order: left -> right, bottom --> top
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if ((i * 4 + j) > testidx)
                continue;
            Vec2 offset(VisibleRect::leftBottom().x + 5 + j * width + width / 2,
                        VisibleRect::leftBottom().y + 50 + i * height + height / 2);
            worldBox->addCollider(EdgeBoxCollider2D::create(Size(width, height), PHYSICS_MATERIAL_2D_DEFAULT, offset));

            switch (i * 4 + j)
            {
            case 0:
            {
                auto sp1            = makeBall(offset - Vec2(30, 0), 10);
                auto sp2            = makeBall(offset + Vec2(30, 0), 10);
                auto sp2Rigidbody2D = sp2->getRigidbody2D();

                auto anchor1 = sp1->getRigidbody2D()->world2Local(offset);
                auto anchor2 = sp2Rigidbody2D->world2Local(offset);

                auto joint = PivotJoint2D::create(sp2Rigidbody2D);
                joint->setAnchors(anchor1, anchor2);
                sp1->addComponent(joint);

                this->addChild(sp1);
                this->addChild(sp2);

                break;
            }
            case 1:
            {
                auto sp1            = makeBall(offset - Vec2(30.0f, 0.0f), 10);
                auto sp2            = makeBox(offset + Vec2(30.0f, 0.0f), Size(30.0f, 10.0f));
                auto sp2Rigidbody2D = sp2->getRigidbody2D();

                FixedJoint2D* joint = FixedJoint2D::create(sp2Rigidbody2D);
                auto anchor1        = sp1->getRigidbody2D()->world2Local(offset);
                auto anchor2        = sp2Rigidbody2D->world2Local(offset);
                joint->setAnchors(anchor1, anchor2);
                sp1->addComponent(joint);

                this->addChild(sp1);
                this->addChild(sp2);
                break;
            }
            case 2:
            {
                auto sp1            = makeBall(offset - Vec2(30.0f, 0.0f), 10);
                auto sp2            = makeBox(offset + Vec2(30.0f, 0.0f), Size(30.0f, 10.0f));
                auto sp2Rigidbody2D = sp2->getRigidbody2D();

                auto joint = DistanceJoint2D::create(sp2Rigidbody2D);
                sp1->addComponent(joint);

                this->addChild(sp1);
                this->addChild(sp2);
                break;
            }
            case 3:
            {
                auto sp1            = makeBall(offset - Vec2(30.0f, 0.0f), 10);
                auto sp2            = makeBox(offset + Vec2(30.0f, 0.0f), Size(30.0f, 10.0f));
                auto sp2Rigidbody2D = sp2->getRigidbody2D();

                auto joint = DistanceJoint2D::create(sp2Rigidbody2D);
                joint->setLimits({30.0f, 52.0f});
                sp1->addComponent(joint);

                this->addChild(sp1);
                this->addChild(sp2);
                break;
            }
            case 4:
            {
                auto sp1            = makeBall(offset - Vec2(30.0f, 0.0f), 10);
                auto sp2            = makeBox(offset + Vec2(30.0f, 0.0f), Size(30.0f, 10.0f));
                auto sp2Rigidbody2D = sp2->getRigidbody2D();

                auto joint = SpringJoint2D::create(500.0f, 0.3f, sp2Rigidbody2D);
                sp1->addComponent(joint);

                this->addChild(sp1);
                this->addChild(sp2);
                break;
            }
            case 5:
            {
                auto sp1            = makeBall(offset - Vec2(30.0f, 0.0f), 10);
                auto sp2            = makeBox(offset + Vec2(30.0f, 0.0f), Size(30.0f, 10.0f));
                auto sp2Rigidbody2D = sp2->getRigidbody2D();

                // GrooveJoint2D
                Vec2 sliderOrigin{30.0f, 15.0f};
                Vec2 sliderTarget{30.0f, -15.0f};
                Vec2 connectedAnchor{-30.0f, 0.0f};
                auto joint = SliderJoint2D::create(sp2Rigidbody2D);
                joint->setAnchor(sliderOrigin);
                joint->setConnectedAnchor(connectedAnchor);
                joint->setLimits({0.0f, sliderOrigin.distance(sliderTarget)});
                joint->setAngleInRadians((sliderTarget - sliderOrigin).getAngle());

                sp1->addComponent(joint);

                this->addChild(sp1);
                this->addChild(sp2);
                break;
            }
            case 6:
            {
                auto sp1            = makeBox(offset - Vec2(30.0f, 0.0f), Size(30.0f, 10.0f));
                auto sp1Rigidbody2D = sp1->getRigidbody2D();

                auto sp2            = makeBox(offset + Vec2(30.0f, 0.0f), Size(30.0f, 10.0f));
                auto sp2Rigidbody2D = sp2->getRigidbody2D();

                auto joint = SpringJoint2D::create(3000.0f, 60.0f, sp2Rigidbody2D);
                joint->setRestLength(60);
                sp1->addComponent(joint);

                this->addChild(sp1);
                this->addChild(sp2);
                break;
            }
            case 7:
            {
                auto sp1            = makeBall(offset - Vec2(0.0f, 10.0f), 10.0f);
                auto sp1Rigidbody2D = sp1->getRigidbody2D();

                auto sp2            = makeBox(offset + Vec2(0.0f, 10.0f), Size(30.0f, 10.0f));
                auto sp2Rigidbody2D = sp2->getRigidbody2D();

                auto joint = WheelJoint2D::create(sp2Rigidbody2D);
                joint->setMotor(JointMotor2D{60.0f, phconsts::MaxForce});
                joint->setLimits({-1.0f, 1.0f});
                sp1->addComponent(joint);

                this->addChild(sp1);
                this->addChild(sp2);
                break;
            }
            case 8:
            {
                auto sp1            = makeBox(offset - Vec2(30.0f, 0.0f), Size(30.0f, 10.0f));
                auto sp1Rigidbody2D = sp1->getRigidbody2D();

                auto sp2            = makeBox(offset + Vec2(30.0f, 0.0f), Size(30.0f, 10.0f));
                auto sp2Rigidbody2D = sp2->getRigidbody2D();

                auto joint = FilterJoint2D::create(sp2Rigidbody2D);
                sp1->addComponent(joint);

                this->addChild(sp1);
                this->addChild(sp2);
                break;
            }
            case 9:
            {
                auto sp1            = makeBox(offset - Vec2(30.0f, 0.0f), Size(30.0f, 10.0f));
                auto sp2            = makeBox(offset + Vec2(30.0f, 0.0f), Size(30.0f, 10.0f));
                auto sp2Rigidbody2D = sp2->getRigidbody2D();

                sp1->addComponent(PinJoint2D::create(sp1->getPosition(), worldBox));
                sp2->addComponent(PinJoint2D::create(sp2->getPosition(), worldBox));

                auto joint = MotorJoint2D::create(sp2Rigidbody2D);
                joint->setAngularVelocity(0.0f);
                sp1->addComponent(joint);

                this->addChild(sp1);
                this->addChild(sp2);
                break;
            }
            case 10:
            {
                auto sp1            = makeBox(offset - Vec2(30.0f, 0.0f), Size(30.0f, 10.0f));
                auto sp2            = makeBox(offset + Vec2(30.0f, 0.0f), Size(30.0f, 10.0f));
                auto sp2Rigidbody2D = sp2->getRigidbody2D();

                auto pin1 = PinJoint2D::create(sp1->getPosition(), worldBox);
                auto pin2 = PinJoint2D::create(sp2->getPosition(), worldBox);
                sp1->addComponent(pin1);
                sp2->addComponent(pin2);

                auto joint = MotorJoint2D::create(sp2Rigidbody2D);
                sp1->addComponent(joint);

                this->addChild(sp1);
                this->addChild(sp2);
                break;
            }
            default:
                break;
            }
        }
    }
}

std::string PhysicsDemoJoints::title() const
{
    return "Joints";
}

void PhysicsDemoPump::onEnter()
{
    PhysicsDemo::onEnter();
    toggleDebug();

    _distance  = 0.0f;
    _rotationV = 0.0f;
    scheduleUpdate();

    auto worldBox     = Node::create();
    auto worldBoxBody = Rigidbody2D::create();
    worldBox->addComponent(worldBoxBody);
    worldBoxBody->setBodyType(Rigidbody2D::StaticBody);

    PhysicsMaterial2D staticMaterial(phconsts::UpperBound, 0, 0.5f);
    worldBoxBody->addCollider(EdgeSegmentCollider2D::create(VisibleRect::leftTop() + Vec2(50, 0),
                                                            VisibleRect::leftTop() + Vec2(50, -130), staticMaterial));
    worldBoxBody->addCollider(EdgeSegmentCollider2D::create(VisibleRect::leftTop() + Vec2(190, 0),
                                                            VisibleRect::leftTop() + Vec2(100, -50), staticMaterial));
    worldBoxBody->addCollider(EdgeSegmentCollider2D::create(VisibleRect::leftTop() + Vec2(100, -50),
                                                            VisibleRect::leftTop() + Vec2(100, -90), staticMaterial));
    worldBoxBody->addCollider(EdgeSegmentCollider2D::create(VisibleRect::leftTop() + Vec2(50, -130),
                                                            VisibleRect::leftTop() + Vec2(100, -145), staticMaterial));
    worldBoxBody->addCollider(EdgeSegmentCollider2D::create(VisibleRect::leftTop() + Vec2(100, -145),
                                                            VisibleRect::leftBottom() + Vec2(100, 80), staticMaterial));
    worldBoxBody->addCollider(EdgeSegmentCollider2D::create(VisibleRect::leftTop() + Vec2(150, -80),
                                                            VisibleRect::leftBottom() + Vec2(150, 80), staticMaterial));
    worldBoxBody->addCollider(EdgeSegmentCollider2D::create(
        VisibleRect::leftTop() + Vec2(150, -80), VisibleRect::rightTop() + Vec2(-100, -150), staticMaterial));

    worldBoxBody->setCategoryBits(0x01);
    worldBox->setPosition(Vec2::zero);
    this->addChild(worldBox);
    worldBox->setName("world-box");

    // balls
    for (int i = 0; i < 6; ++i)
    {
        auto ball = makeBall(VisibleRect::leftTop() + Vec2(75 + AXRANDOM_0_1() * 90, 0.0f), 22,
                             PhysicsMaterial2D(0.05f, 0.0f, 0.1f));
        ball->setName("ball");
        addChild(ball);
    }

    Vec2 vec[4] = {VisibleRect::leftTop() + Vec2(102, -148), VisibleRect::leftTop() + Vec2(148, -161),
                   VisibleRect::leftBottom() + Vec2(148, 20), VisibleRect::leftBottom() + Vec2(102, 20)};

    // pump
    auto pump   = Node::create();
    auto center = Collider2D::getPolygonCenter(vec);
    pump->setPosition(center);
    auto pumpBody = Rigidbody2D::createPolygon(vec, PHYSICS_MATERIAL_2D_DEFAULT, -center);
    pump->addComponent(pumpBody);
    this->addChild(pump);
    pumpBody->setCategoryBits(0x02);
    pumpBody->setGravityEnabled(false);
    pump->setName("pump");

    // small gear
    auto sgearBody = Rigidbody2D::createCircle(44);
    sgearBody->setCategoryBits(0x04);
    sgearBody->setCollisionMaskBits(0x04);
    auto sgear = Node::create();
    sgear->setTag(1);
    sgear->setName("small-gear");
    sgear->addComponent(sgearBody);
    sgear->setPosition(VisibleRect::leftBottom() + Vec2(125.0f, 0.0f));
    this->addChild(sgear);

    sgear->addComponent(PinJoint2D::create(sgear->getPosition(), worldBoxBody));
    auto distanceJoint = DistanceJoint2D::create(sgearBody);
    distanceJoint->setConnectedAnchor(Vec2{0.0f, -44.0f});
    pump->addComponent(distanceJoint);

    // big gear
    auto bgearBody = Rigidbody2D::createCircle(100);
    bgearBody->setCategoryBits(0x04);
    auto bgear = Node::create();
    bgearBody->setTag(2);
    bgear->setName("big-gear");
    bgear->addComponent(bgearBody);
    bgear->setPosition(VisibleRect::leftBottom() + Vec2(275.0f, 0.0f));
    this->addChild(bgear);

    bgear->addComponent(PinJoint2D::create(bgear->getPosition(), worldBoxBody));
    auto motorJoint = MotorJoint2D::create(bgearBody);
    motorJoint->setAngularVelocity(-90.0f);
    sgear->addComponent(motorJoint);

    // plugger
    Vec2 seg[]     = {VisibleRect::leftTop() + Vec2(75, -120), VisibleRect::leftBottom() + Vec2(75, -100)};
    Vec2 segCenter = (seg[1] + seg[0]) / 2;

    Vec2 p0 = seg[0] - segCenter;
    Vec2 p1 = seg[1] - segCenter;

    float lineWidth = 20.0f;
    Vec2 segDir     = p1 - p0;
    Vec2 perpDir    = segDir.getPerp();
    perpDir.normalize();

    Vec2 offset = perpDir * (lineWidth / 2.0f);

    Vec2 polygonVertices[] = {
        p0 - offset,  // Bottom-left point
        p1 - offset,  // Bottom-right point
        p1 + offset,  // Top-right point
        p0 + offset   // Top-left point
    };

    auto pluggerBody = Rigidbody2D::createPolygon(polygonVertices, PhysicsMaterial2D(0.01f, 0.0f, 0.5f));
    pluggerBody->setDynamic();
    pluggerBody->setAutoMass(false);
    pluggerBody->setMass(30);
    pluggerBody->setMoment(100000);
    pluggerBody->setCategoryBits(0x02);
    auto plugger = Node::create();
    plugger->setName("plugger");
    plugger->addComponent(pluggerBody);
    plugger->setPosition(segCenter);
    this->addChild(plugger);

    sgearBody->setCollisionMaskBits(0x04 | 0x01);

    auto pinJoint = PinJoint2D::create(plugger->getPosition(), worldBoxBody);
    pinJoint->setMotor(JointMotor2D{});
    plugger->addComponent(pinJoint);

    distanceJoint = DistanceJoint2D::create(sgearBody);
    distanceJoint->setConnectedAnchor(Vec2{44.0f, 0.0f});
    plugger->addComponent(distanceJoint);
}

void PhysicsDemoPump::update(float delta)
{
    for (const auto& child : getChildren())
    {
        auto rigidbody = child->getRigidbody2D();
        if (!rigidbody)
            continue;
        if ((rigidbody->getTag() & DRAG_BODYS_BITS) && child->getPosition().y < 0.0f)
        {
            child->setPosition(VisibleRect::leftTop() + Vec2(75 + AXRANDOM_0_1() * 90, 0.0f));
            auto rigidbody = child->getRigidbody2D();
            if (rigidbody)
                rigidbody->setVelocity(Vec2(0.0f, 0.0f));
        }
    }

    auto gear = getChildByTag(1);
    if (!gear)
        return;

    Rigidbody2D* gearBody = gear->getRigidbody2D();
    if (gearBody)
    {
        if (_distance != 0.0f)
        {
            _rotationV += _distance / 2500.0f;

            if (_rotationV > 30)
                _rotationV = 30.0f;
            if (_rotationV < -30)
                _rotationV = -30.0f;
        }

        gearBody->setAngularVelocity(_rotationV);
        _rotationV *= 0.995f;
    }
}

bool PhysicsDemoPump::onPointerDown(PointerEvent* event)
{
    PhysicsDemo::onPointerDown(event);

    _distance = event->getLocation().x - VisibleRect::center().x;

    return true;
}

void PhysicsDemoPump::onPointerMove(PointerEvent* event)
{
    PhysicsDemo::onPointerMove(event);

    _distance = event->getLocation().x - VisibleRect::center().x;
}

void PhysicsDemoPump::onPointerUp(PointerEvent* event)
{
    PhysicsDemo::onPointerUp(event);

    _distance = 0;
}

std::string PhysicsDemoPump::title() const
{
    return "Pump";
}

std::string PhysicsDemoPump::subtitle() const
{
    return "touch screen on left or right";
}

void PhysicsDemoOneWayPlatform::onEnter()
{
    PhysicsDemo::onEnter();

    _physicsWorld2D->setGlobalEventEnabled(
        ContactEventBits::PreSolve | ContactEventBits::Hit | ContactEventBits::Contact, true);
    _physicsWorld2D->setPreSolveCallback(AX_CALLBACK_1(PhysicsDemoOneWayPlatform::onPreSolve, this));

    auto ground = Node::create();
    ground->addComponent(Rigidbody2D::createEdgeSegment(VisibleRect::leftBottom() + Vec2(0.0f, 50.0f),
                                                        VisibleRect::rightBottom() + Vec2(0.0f, 50.0f)));
    this->addChild(ground);

    auto platform     = makeBox(VisibleRect::center(), Size(200.0f, 50.0f));
    auto platformBody = platform->getRigidbody2D();
    platformBody->setBodyType(Rigidbody2D::StaticBody);
    this->addChild(platform);

    auto ball     = makeBall(VisibleRect::center() - Vec2(0.0f, 50.0f), 20);
    auto ballBody = ball->getRigidbody2D();
    ballBody->setVelocity(Vec2(0.0f, 150.0f));
    this->addChild(ball);

    auto contactListener             = ContactEventListener2D::create(platformBody, ballBody);
    contactListener->onContactFilter = [_a = platformBody, _b = ballBody](ContactEvent2D* event) {
        auto colliderA = event->getColliderA();
        auto colliderB = event->getColliderB();
        if ((colliderA->getAttachedBody() == _a && colliderB->getAttachedBody() == _b) ||
            (colliderA->getAttachedBody() == _b && colliderB->getAttachedBody() == _a))
        {
            return true;
        }

        return false;
    };

    contactListener->onContactBegin = [](ContactEvent2D* event) { AXLOGI("onContactBegin"); };
    contactListener->onContactEnd   = [](ContactEvent2D* event) { AXLOGI("onContactEnd"); };
    contactListener->onCollisionHit = AX_CALLBACK_1(PhysicsDemoOneWayPlatform::onCollisionHit, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

bool PhysicsDemoOneWayPlatform::onPreSolve(const ContactInfo2D& info)
{
    // Axmol-3.0 physics2d is built on Box2D v3.
    // In Box2D v3, the contact normal always points from shape A toward shape B.
    // For a ball colliding with a one-way platform, a positive normal.y means
    // the ball is above the platform (the normal points downward).
    // In this case we return true to allow the collision.
    // If normal.y <= 0, the ball is below the platform and the collision is ignored.
    auto normalY = info.normal.y;

    return normalY > 0;
}

void PhysicsDemoOneWayPlatform::onCollisionHit(ax::ContactEvent2D* contact)
{
    auto& contactInfo = contact->getContactInfo();
    if (contactInfo.pointCount > 0)
        AXLOGI("hit point: ({}, {}), normalVelocity: {}", contactInfo.points[0].point.x, contactInfo.points[0].point.y,
               contactInfo.points[0].normalVelocity);
}

std::string PhysicsDemoOneWayPlatform::title() const
{
    return "One Way Platform";
}

void PhysicsDemoSlice::onEnter()
{
    PhysicsDemo::onEnter();
    toggleDebug();

    _sliceTag = 1;

    auto ground = Node::create();
    ground->addComponent(Rigidbody2D::createEdgeSegment(VisibleRect::leftBottom() + Vec2(0, 50),
                                                        VisibleRect::rightBottom() + Vec2(0.0f, 50.0f)));
    this->addChild(ground);

    auto box       = Node::create();
    Vec2 points[4] = {Vec2(-100.0f, -100.0f), Vec2(-100.0f, 100.0f), Vec2(100.0f, 100.0f), Vec2(100.0f, -100.0f)};
    box->addComponent(Rigidbody2D::createPolygon(points));
    box->setPosition(VisibleRect::center());
    box->getRigidbody2D()->setTag(_sliceTag);
    box->setName(fmt::format("slice-{}", ++_sliceId));
    addChild(box);
}

bool PhysicsDemoSlice::slice(PhysicsWorld2D& /*world*/, const RayCastHit2D& hitInfo, void* data)
{
    if (hitInfo.collider->getAttachedBody()->getTag() != _sliceTag)
    {
        return true;
    }

    Ray2D* ray = static_cast<Ray2D*>(data);

    if (!hitInfo.collider->containsPoint(ray->origin) && !hitInfo.collider->containsPoint(ray->endPoint()))
    {
        auto normal = ray->translation.getPerp().getNormalized();
        float dist  = ray->origin.dot(normal);

        clipPoly(dynamic_cast<PolygonCollider2D*>(hitInfo.collider), normal, dist);
        clipPoly(dynamic_cast<PolygonCollider2D*>(hitInfo.collider), -normal, -dist);

        auto rigidbody = hitInfo.collider->getAttachedBody();
        auto owner     = rigidbody->getOwner();
        // remove rigidbody only also works, owner->removeComponent(rigidbody);
        owner->removeFromParent();
    }

    return true;
}

void PhysicsDemoSlice::clipPoly(PolygonCollider2D* collider, Vec2 normal, float distance)
{
    Rigidbody2D* body = collider->getAttachedBody();
    int count         = collider->getPointsCount();
    int pointsCount   = 0;

    std::vector<Vec2> points(count + 1);

    for (int i = 0, j = count - 1; i < count; j = i, ++i)
    {
        Vec2 a      = body->local2World(collider->getPoint(j));
        float aDist = a.dot(normal) - distance;

        if (aDist < 0.0f)
        {
            points[pointsCount] = a;
            ++pointsCount;
        }

        Vec2 b      = body->local2World(collider->getPoint(i));
        float bDist = b.dot(normal) - distance;

        if (aDist * bDist < 0.0f)
        {
            float t             = std::fabs(aDist) / (std::fabs(aDist) + std::fabs(bDist));
            points[pointsCount] = a.lerp(b, t);
            ++pointsCount;
        }
    }
    points.resize(pointsCount);
    Vec2 center          = Collider2D::getPolygonCenter(points);
    Node* node           = Node::create();
    Rigidbody2D* polygon = Rigidbody2D::createPolygon(points, PHYSICS_MATERIAL_2D_DEFAULT, -center);
    node->setPosition(center);
    node->addComponent(polygon);
    polygon->setVelocity(body->getVelocityAtWorldPoint(center));
    polygon->setAngularVelocity(body->getAngularVelocity());
    polygon->setTag(_sliceTag);
    node->setName(fmt::format("slice-{}", ++_sliceId));
    addChild(node);
}

void PhysicsDemoSlice::onPointerUp(PointerEvent* event)
{
    PhysicsDemo::onPointerUp(event);

    auto func = AX_CALLBACK_3(PhysicsDemoSlice::slice, this);

    Ray2D ray = Ray2D::fromPoints(event->getStartLocation(), event->getLocation());
    getPhysicsWorld2D()->rayCast(func, ray, &ray);
}

std::string PhysicsDemoSlice::title() const
{
    return "Slice";
}

std::string PhysicsDemoSlice::subtitle() const
{
    return "click and drag to slice up the block";
}

void PhysicsDemoBug3988::onEnter()
{
    PhysicsDemo::onEnter();
    toggleDebug();
    _physicsWorld2D->setGravity(Vec2::zero);

    auto ball = Sprite::create("Images/YellowSquare.png");
    ball->setPosition(VisibleRect::center() - Vec2(100.0f, 0.0f));
    ball->setRotation(30.0f);
    this->addChild(ball);

    auto physicsBall = makeBox(VisibleRect::center() + Vec2(100.0f, 0.0f), Size(100.0f, 100.0f));
    physicsBall->setRotation(30.0f);
    this->addChild(physicsBall);
}

std::string PhysicsDemoBug3988::title() const
{
    return "Bug3988";
}

std::string PhysicsDemoBug3988::subtitle() const
{
    return "All the Rectangles should have same rotation angle";
}

void PhysicsContactTest::onEnter()
{
    PhysicsDemo::onEnter();

    _physicsWorld2D->setGlobalEventEnabled(ContactEventBits::PreSolve, true);

    _physicsWorld2D->setPreSolveCallback(AX_CALLBACK_1(PhysicsContactTest::onPreSolve, this));

    _physicsWorld2D->setGravity(Vec2::zero);
    auto s = VisibleRect::getVisibleRect().size;

    _yellowBoxNum      = 50;
    _blueBoxNum        = 50;
    _yellowTriangleNum = 50;
    _blueTriangleNum   = 50;

    MenuItemFont::setFontSize(65);
    auto decrease1 = MenuItemFont::create(" - ", AX_CALLBACK_1(PhysicsContactTest::onDecrease, this));
    decrease1->setColor(Color32(0, 200, 20));
    auto increase1 = MenuItemFont::create(" + ", AX_CALLBACK_1(PhysicsContactTest::onIncrease, this));
    increase1->setColor(Color32(0, 200, 20));
    decrease1->setTag(1);
    increase1->setTag(1);

    float prevMenuPos = getSubtitleLabel()->getPosition().y - getSubtitleLabel()->getContentSize().height;
    float menuStep    = (getSubtitleLabel()->getPosition().y - getRestartTestItem()->getPosition().y) * 0.25f;
    auto menu1        = Menu::create(decrease1, increase1, nullptr);
    menu1->alignItemsHorizontally();
    menu1->setPosition(Vec2(s.width / 2, prevMenuPos));
    addChild(menu1, 1);

    auto label = Label::createWithTTF("yellow box", "fonts/arial.ttf", 32);
    addChild(label, 1);
    label->setPosition(Vec2(s.width / 2 - 150, prevMenuPos));

    auto decrease2 = MenuItemFont::create(" - ", AX_CALLBACK_1(PhysicsContactTest::onDecrease, this));
    decrease2->setColor(Color32(0, 200, 20));
    auto increase2 = MenuItemFont::create(" + ", AX_CALLBACK_1(PhysicsContactTest::onIncrease, this));
    increase2->setColor(Color32(0, 200, 20));
    decrease2->setTag(2);
    increase2->setTag(2);

    auto menu2 = Menu::create(decrease2, increase2, nullptr);
    menu2->alignItemsHorizontally();
    menu2->setPosition(Vec2(s.width / 2, prevMenuPos -= menuStep));
    addChild(menu2, 1);

    label = Label::createWithTTF("blue box", "fonts/arial.ttf", 32);
    addChild(label, 1);
    label->setPosition(Vec2(s.width / 2 - 150, prevMenuPos));

    auto decrease3 = MenuItemFont::create(" - ", AX_CALLBACK_1(PhysicsContactTest::onDecrease, this));
    decrease3->setColor(Color32(0, 200, 20));
    auto increase3 = MenuItemFont::create(" + ", AX_CALLBACK_1(PhysicsContactTest::onIncrease, this));
    increase3->setColor(Color32(0, 200, 20));
    decrease3->setTag(3);
    increase3->setTag(3);

    auto menu3 = Menu::create(decrease3, increase3, nullptr);
    menu3->alignItemsHorizontally();
    menu3->setPosition(Vec2(s.width / 2, prevMenuPos -= menuStep));
    addChild(menu3, 1);

    label = Label::createWithTTF("yellow triangle", "fonts/arial.ttf", 32);
    addChild(label, 1);
    label->setPosition(Vec2(s.width / 2 - 150, prevMenuPos));

    auto decrease4 = MenuItemFont::create(" - ", AX_CALLBACK_1(PhysicsContactTest::onDecrease, this));
    decrease4->setColor(Color32(0, 200, 20));
    auto increase4 = MenuItemFont::create(" + ", AX_CALLBACK_1(PhysicsContactTest::onIncrease, this));
    increase4->setColor(Color32(0, 200, 20));
    decrease4->setTag(4);
    increase4->setTag(4);

    auto menu4 = Menu::create(decrease4, increase4, nullptr);
    menu4->alignItemsHorizontally();
    menu4->setPosition(Vec2(s.width / 2, prevMenuPos -= menuStep));
    addChild(menu4, 1);

    label = Label::createWithTTF("blue triangle", "fonts/arial.ttf", 32);
    addChild(label, 1);
    label->setPosition(Vec2(s.width / 2 - 150, prevMenuPos));

    resetTest();
}

void PhysicsContactTest::onDecrease(Object* sender)
{
    switch (dynamic_cast<Node*>(sender)->getTag())
    {
    case 1:
        if (_yellowBoxNum > 0)
            _yellowBoxNum -= 50;
        break;
    case 2:
        if (_blueBoxNum > 0)
            _blueBoxNum -= 50;
        break;
    case 3:
        if (_yellowTriangleNum > 0)
            _yellowTriangleNum -= 50;
        break;
    case 4:
        if (_blueTriangleNum > 0)
            _blueTriangleNum -= 50;
        break;

    default:
        break;
    }

    resetTest();
}

void PhysicsContactTest::onIncrease(Object* sender)
{
    switch (dynamic_cast<Node*>(sender)->getTag())
    {
    case 1:
        _yellowBoxNum += 50;
        break;
    case 2:
        _blueBoxNum += 50;
        break;
    case 3:
        _yellowTriangleNum += 50;
        break;
    case 4:
        _blueTriangleNum += 50;
        break;

    default:
        break;
    }

    resetTest();
}

void PhysicsContactTest::resetTest()
{
    removeChildByTag(10);
    auto root = Node::create();
    root->setTag(10);
    this->addChild(root);

    auto s            = VisibleRect::getVisibleRect().size;
    float prevMenuPos = getSubtitleLabel()->getPosition().y - getSubtitleLabel()->getContentSize().height;
    float menuStep    = (getSubtitleLabel()->getPosition().y - getRestartTestItem()->getPosition().y) * 0.25f;
    std::string strNum;
    char buf[10];

    auto mark  = fmt::format_to_z(buf, "{}", _yellowBoxNum);
    auto label = Label::createWithTTF(mark, "fonts/arial.ttf", 32);
    root->addChild(label, 1);
    label->setPosition(Vec2(s.width / 2, prevMenuPos));

    mark  = fmt::format_to_z(buf, "{}", _blueBoxNum);
    label = Label::createWithTTF(mark, "fonts/arial.ttf", 32);
    root->addChild(label, 1);
    label->setPosition(Vec2(s.width / 2, prevMenuPos -= menuStep));

    mark  = fmt::format_to_z(buf, "{}", _yellowTriangleNum);
    label = Label::createWithTTF(mark, "fonts/arial.ttf", 32);
    root->addChild(label, 1);
    label->setPosition(Vec2(s.width / 2, prevMenuPos -= menuStep));

    mark  = fmt::format_to_z(buf, "{}", _blueTriangleNum);
    label = Label::createWithTTF(mark, "fonts/arial.ttf", 32);
    root->addChild(label, 1);
    label->setPosition(Vec2(s.width / 2, prevMenuPos -= menuStep));

    auto wall     = Node::create();
    auto wallBody = Rigidbody2D::createEdgeBox(VisibleRect::getVisibleRect().size, PhysicsMaterial2D(0.1f, 1, 0.0f));
    wall->addComponent(wallBody);
    wall->setPosition(VisibleRect::center());
    root->addChild(wall);

    // The wall default categoryBits = 1
    // all rigidbody should collide with the wall world box (categoryBits=1)

    // yellow box, will collide with itself and blue box.
    for (int i = 0; i < _yellowBoxNum; ++i)
    {
        Size size(10 + AXRANDOM_0_1() * 10, 10 + AXRANDOM_0_1() * 10);
        Size canvasSize = VisibleRect::getVisibleRect().size;
        Vec2 position   = Vec2(canvasSize.width, canvasSize.height) - Vec2(size.width, size.height);
        position.x      = position.x * AXRANDOM_0_1();
        position.y      = position.y * AXRANDOM_0_1();
        position        = VisibleRect::leftBottom() + position + Vec2(size.width / 2, size.height / 2);
        Vec2 velocity((float)(AXRANDOM_0_1() - 0.5) * 200, (float)(AXRANDOM_0_1() - 0.5) * 200);
        auto box     = makeBox(position, size, 1, PhysicsMaterial2D(0.1f, 1, 0.0f));
        auto boxBody = box->getRigidbody2D();
        boxBody->setVelocity(velocity);
        boxBody->setCategoryBits(2);       // 0b00010
        boxBody->setCollisionMaskBits(7);  // 0b00111
        root->addChild(box);
    }

    // blue box, will collide with blue box.
    for (int i = 0; i < _blueBoxNum; ++i)
    {
        Size size(10 + AXRANDOM_0_1() * 10, 10 + AXRANDOM_0_1() * 10);
        Size canvasSize = VisibleRect::getVisibleRect().size;
        Vec2 position   = Vec2(canvasSize.width, canvasSize.height) - Vec2(size.width, size.height);
        position.x      = position.x * AXRANDOM_0_1();
        position.y      = position.y * AXRANDOM_0_1();
        position        = VisibleRect::leftBottom() + position + Vec2(size.width / 2, size.height / 2);
        Vec2 velocity((float)(AXRANDOM_0_1() - 0.5) * 200, (float)(AXRANDOM_0_1() - 0.5) * 200);
        auto box     = makeBox(position, size, 2, PhysicsMaterial2D(0.1f, 1, 0.0f));
        auto boxBody = box->getRigidbody2D();
        boxBody->setVelocity(velocity);
        boxBody->setCategoryBits(4);       // 0b00100
        boxBody->setCollisionMaskBits(5);  // 0b00101
        root->addChild(box);
    }

    // yellow triangle, will collide with itself and blue box.
    for (int i = 0; i < _yellowTriangleNum; ++i)
    {
        Size size(10 + AXRANDOM_0_1() * 10, 10 + AXRANDOM_0_1() * 10);
        Size canvasSize = VisibleRect::getVisibleRect().size;
        Vec2 position   = Vec2(canvasSize.width, canvasSize.height) - Vec2(size.width, size.height);
        position.x      = position.x * AXRANDOM_0_1();
        position.y      = position.y * AXRANDOM_0_1();
        position        = VisibleRect::leftBottom() + position + Vec2(size.width / 2, size.height / 2);
        Vec2 velocity((float)(AXRANDOM_0_1() - 0.5) * 300, (float)(AXRANDOM_0_1() - 0.5) * 300);
        auto triangle     = makeTriangle(position, size, 1, PhysicsMaterial2D(0.1f, 1, 0.0f));
        auto triangleBody = triangle->getRigidbody2D();
        triangleBody->setVelocity(velocity);
        triangleBody->setCategoryBits(8);        // 0b01000
        triangleBody->setCollisionMaskBits(13);  // 0b01101
        root->addChild(triangle);
    }

    // blue triangle, will collide with yellow box.
    for (int i = 0; i < _blueTriangleNum; ++i)
    {
        Size size(10 + AXRANDOM_0_1() * 10, 10 + AXRANDOM_0_1() * 10);
        Size canvasSize = VisibleRect::getVisibleRect().size;
        Vec2 position   = Vec2(canvasSize.width, canvasSize.height) - Vec2(size.width, size.height);
        position.x      = position.x * AXRANDOM_0_1();
        position.y      = position.y * AXRANDOM_0_1();
        position        = VisibleRect::leftBottom() + position + Vec2(size.width / 2, size.height / 2);
        Vec2 velocity((float)(AXRANDOM_0_1() - 0.5) * 300, (float)(AXRANDOM_0_1() - 0.5) * 300);
        auto triangle     = makeTriangle(position, size, 2, PhysicsMaterial2D(0.1f, 1, 0.0f));
        auto triangleBody = triangle->getRigidbody2D();
        triangleBody->setVelocity(velocity);
        triangleBody->setCategoryBits(16);      // 0b10000
        triangleBody->setCollisionMaskBits(3);  // 0b00011
        root->addChild(triangle);
    }
}

bool PhysicsContactTest::onPreSolve(const ContactInfo2D& info)
{
    auto colA = info.colliderA;
    auto colB = info.colliderB;
    if (!colA || !colB)
        return true;

    auto bodyA = colA->getAttachedBody();
    auto bodyB = colB->getAttachedBody();
    if (!bodyA || !bodyB)
        return true;

    auto catA  = bodyA->getCategoryBits();
    auto catB  = bodyB->getCategoryBits();
    auto maskA = bodyA->getCollisionMaskBits();
    auto maskB = bodyB->getCollisionMaskBits();

    bool a_allowed = (catA & maskB) != 0;
    bool b_allowed = (catB & maskA) != 0;
    AXASSERT(a_allowed && b_allowed, "onPreSolve: unexpected collision - category/mask mismatch");

    return true;
}

std::string PhysicsContactTest::title() const
{
    return "Contact Test";
}

std::string PhysicsContactTest::subtitle() const
{
    return "should not crash";
}

void PhysicsPositionRotationTest::onEnter()
{
    PhysicsDemo::onEnter();
    toggleDebug();
    _physicsWorld2D->setGravity(Point::zero);

    auto wall = Node::create();
    wall->addComponent(Rigidbody2D::createEdgeBox(VisibleRect::getVisibleRect().size));
    wall->setPosition(VisibleRect::center());
    addChild(wall);

    // anchor test
    auto anchorNode = Sprite::create("Images/YellowSquare.png");
    // anchorNode->setAnchorPoint(Vec2(0.1f, 0.9f));
    anchorNode->setPosition(100, 100);
    anchorNode->setScale(0.25);
    anchorNode->addComponent(Rigidbody2D::createBox(anchorNode->getContentSize()));
    anchorNode->getRigidbody2D()->setTag(DRAG_BODYS_BITS);
    addChild(anchorNode);

    anchorNode->getRigidbody2D()->setAngularVelocity(-286.0f);

    // parent test
    auto parent = Sprite::create("Images/YellowSquare.png");
    parent->setPosition(300, 100);
    parent->setScale(0.5);
    parent->addComponent(Rigidbody2D::createBox(parent->getContentSize()));
    parent->getRigidbody2D()->setTag(DRAG_BODYS_BITS);
    addChild(parent);

    auto leftBall = Sprite::create("Images/YellowSquare.png");
    leftBall->setPosition(-50, 0);
    leftBall->Node::setScale(0.5);
    leftBall->addComponent(Rigidbody2D::createBox(leftBall->getContentSize()));
    leftBall->getRigidbody2D()->setTag(DRAG_BODYS_BITS);
    parent->addChild(leftBall);

    parent->getRigidbody2D()->setAngularVelocity(286.0f);

    // offset position rotation test
    auto offsetPosNode = Sprite::create("Images/YellowSquare.png");
    offsetPosNode->setPosition(100, 200);
    auto body = Rigidbody2D::createBox(offsetPosNode->getContentSize() / 2);
    offsetPosNode->addComponent(body);
    body->setPositionOffset(-Vec2(offsetPosNode->getContentSize() / 2));
    body->setRotationOffset(45);
    offsetPosNode->getRigidbody2D()->setTag(DRAG_BODYS_BITS);
    addChild(offsetPosNode);

    offsetPosNode->getRigidbody2D()->setAngularVelocity(286.0f);

    return;
}

std::string PhysicsPositionRotationTest::title() const
{
    return "Position/Rotation Test";
}

void PhysicsSetGravityEnableTest::onEnter()
{
    PhysicsDemo::onEnter();

    // wall
    auto wall = Node::create();
    wall->addComponent(
        Rigidbody2D::createEdgeBox(VisibleRect::getVisibleRect().size, PhysicsMaterial2D(0.1f, 1.0f, 0.0f)));
    wall->setPosition(VisibleRect::center());
    addChild(wall);

    // common box
    auto commonBox = makeBox(Vec2(100, 100), Size(50, 50), 1);
    commonBox->setTag(1);
    auto commonBoxBody = commonBox->getRigidbody2D();
    commonBoxBody->setGravityEnabled(true);
    // Critical fix for Box2D physics behavior difference from Chipmunk in Axmol
    // Box2D uses strict floating-point physics, which causes rectangular boxes to
    // rotate / wobble / bounce sideways when landing vertically due to precision errors.
    // Chipmunk automatically stabilizes rotation internally, but Box2D does not.
    // This line disables rotation entirely to keep the body stable and eliminate side bouncing,
    // making Box2D behave consistently with Chipmunk for stable platformer / object physics.
    commonBoxBody->setRotationEnabled(false);
    addChild(commonBox);

    auto box     = makeBox(Vec2(200, 100), Size(50, 50), 2);
    auto boxBody = box->getRigidbody2D();
    boxBody->setGravityEnabled(false);
    addChild(box);

    auto ball = makeBall(Vec2(200, 200), 50);
    ball->setTag(2);
    auto ballBody = ball->getRigidbody2D();
    ballBody->setGravityEnabled(false);
    addChild(ball);

    scheduleOnce(AX_SCHEDULE_SELECTOR(PhysicsSetGravityEnableTest::onScheduleOnce), 1.0);
}

void PhysicsSetGravityEnableTest::onScheduleOnce(float /*delta*/)
{
    auto ball = getChildByTag(2);

    auto rigidbody = ball->getRigidbody2D();
    if (rigidbody)
    {
        rigidbody->setAutoMass(false);
        rigidbody->setMass(200);
    }

    // Unlike Chipmunk, Axmol v3 physics2d (Box2D-based) uses real-world gravity units MKS.
    _physicsWorld2D->setGravity(Vec2(0, -9.8));
}

std::string PhysicsSetGravityEnableTest::title() const
{
    return "Set Gravity Enable Test";
}

std::string PhysicsSetGravityEnableTest::subtitle() const
{
    return "only yellow box drop down";
}

void PhysicsDemoBug5482::onEnter()
{
    PhysicsDemo::onEnter();

    toggleDebug();

    _bodyInA = false;

    // wall
    auto wall = Node::create();
    wall->addComponent(
        Rigidbody2D::createEdgeBox(VisibleRect::getVisibleRect().size, PhysicsMaterial2D(0.1f, 1.0f, 0.0f)));
    wall->setPosition(VisibleRect::center());
    addChild(wall);

    // button
    MenuItemFont::setFontSize(18);
    _button = MenuItemFont::create("Set Body To A", AX_CALLBACK_1(PhysicsDemoBug5482::changeBodyCallback, this));

    auto menu = Menu::create(_button, nullptr);
    this->addChild(menu);

    _nodeA = Sprite::create("Images/YellowSquare.png");
    _nodeA->setPosition(VisibleRect::center().x - 150, 100);
    _nodeA->setTag(1);
    this->addChild(_nodeA);

    _nodeB = Sprite::create("Images/YellowSquare.png");
    _nodeB->setPosition(VisibleRect::center().x + 150, 100);
    _nodeB->setTag(2);
    this->addChild(_nodeB);

    _body = Rigidbody2D::createBox(_nodeA->getContentSize());
    _body->setTag(DRAG_BODYS_BITS);
    _body->retain();
}

void PhysicsDemoBug5482::onExit()
{
    PhysicsDemo::onExit();
    _body->release();
}

void PhysicsDemoBug5482::changeBodyCallback(Object* /*sender*/)
{
    Sprite* node = _bodyInA ? _nodeB : _nodeA;
    if (_bodyInA)
    {
        _button->setString("Set Body To A");
    }
    else
    {
        _button->setString("Set Body To B");
    }

    if (_body->getOwner())
    {
        _body->getOwner()->removeComponent(_body);
    }

    node->addComponent(_body);
    _bodyInA = !_bodyInA;
}

std::string PhysicsDemoBug5482::title() const
{
    return "bug 5482: setRigidbody2DTest";
}

std::string PhysicsDemoBug5482::subtitle() const
{
    return "change rigid-body to the other.";
}

void PhysicsFixedUpdate::onEnter()
{
    PhysicsDemo::onEnter();

    toggleDebug();
    _physicsWorld2D->setGravity(Point::zero);

    // wall
    auto wall = Node::create();
    wall->addComponent(
        Rigidbody2D::createEdgeBox(VisibleRect::getVisibleRect().size, PhysicsMaterial2D(0.1f, 1, 0.0f)));
    wall->setPosition(VisibleRect::center());
    this->addChild(wall);

    addBall();

    scheduleOnce(AX_SCHEDULE_SELECTOR(PhysicsFixedUpdate::updateStart), 2);
}

void PhysicsFixedUpdate::addBall()
{
    auto ball = Sprite::create("Images/ball.png");
    ball->setPosition(100, 100);
    auto ballBody = Rigidbody2D::createCircle(ball->getContentSize().width / 2, PhysicsMaterial2D(0.1f, 1, 0.0f));
    ball->addComponent(ballBody);
    ballBody->setTag(DRAG_BODYS_BITS);
    ballBody->setVelocity(Point(1000, 20));
    this->addChild(ball);
}

void PhysicsFixedUpdate::updateStart(float /*delta*/)
{
    addBall();

    setFixedUpdateEnabled(true);
    setFixedDeltaTime(1 / 180.0f);
}

std::string PhysicsFixedUpdate::title() const
{
    return "Fixed Update Test";
}

std::string PhysicsFixedUpdate::subtitle() const
{
    return "The second ball should not run across the wall";
}

bool PhysicsTransformTest::onPointerDown(PointerEvent* event)
{
    _parentSprite->setPosition(_rootLayer->convertPointerToNodeSpace(event));
    return false;
}

void PhysicsTransformTest::onEnter()
{
    PhysicsDemo::onEnter();
    toggleDebug();
    _physicsWorld2D->setGravity(Point::zero);

    _rootLayer = Layer::create();
    addChild(_rootLayer);

    auto wall = Node::create();
    wall->addComponent(
        Rigidbody2D::createEdgeBox(VisibleRect::getVisibleRect().size, PhysicsMaterial2D(0.1f, 1.0f, 0.0f)));
    wall->setPosition(VisibleRect::center());
    _rootLayer->addChild(wall);

    // parent test
    _parentSprite = Sprite::create("Images/YellowSquare.png");
    _parentSprite->setPosition(200, 100);
    _parentSprite->setScale(0.25);
    _parentSprite->addComponent(
        Rigidbody2D::createBox(_parentSprite->getContentSize(), PhysicsMaterial2D(0.1f, 1.0f, 0.0f)));
    _parentSprite->setTag(1);
    _parentSprite->getRigidbody2D()->setTag(DRAG_BODYS_BITS);
    _rootLayer->addChild(_parentSprite);

    auto leftBall = Sprite::create("Images/ball.png");
    leftBall->setPosition(-30, 0);
    leftBall->setScale(2);
    leftBall->addComponent(
        Rigidbody2D::createCircle(leftBall->getContentSize().width / 2, PhysicsMaterial2D(0.1f, 1.0f, 0.0f)));
    leftBall->getRigidbody2D()->setTag(DRAG_BODYS_BITS);
    _parentSprite->addChild(leftBall);

    ScaleTo* scaleTo   = ScaleTo::create(2.0, 0.5);
    ScaleTo* scaleBack = ScaleTo::create(2.0, 1.0);
    _parentSprite->runAction(RepeatForever::create(Sequence::create(scaleTo, scaleBack, nullptr)));

    auto normal = Sprite::create("Images/YellowSquare.png");
    normal->setPosition(300, 100);
    normal->setScale(0.25, 0.5);
    normal->addComponent(Rigidbody2D::createBox(normal->getContentSize(), PhysicsMaterial2D(0.1f, 1.0f, 0.0f)));
    normal->getRigidbody2D()->setTag(DRAG_BODYS_BITS);
    _rootLayer->addChild(normal);

    auto bullet = Sprite::create("Images/ball.png");
    bullet->setPosition(200, 200);
    bullet->addComponent(
        Rigidbody2D::createCircle(bullet->getContentSize().width / 2, PhysicsMaterial2D(0.1f, 1.0f, 0.0f)));
    bullet->getRigidbody2D()->setVelocity(Vec2(100.0f, 100.0f));
    _rootLayer->addChild(bullet);

    MoveBy* move    = MoveBy::create(2.0f, Vec2(100.0f, 100.0f));
    MoveBy* move2   = MoveBy::create(2.0f, Vec2(-200.0f, 0.0f));
    MoveBy* move3   = MoveBy::create(2.0f, Vec2(100.0f, -100.0f));
    ScaleTo* scale  = ScaleTo::create(3.0f, 0.3f);
    ScaleTo* scale2 = ScaleTo::create(3.0f, 1.0f);

    RotateBy* rotate = RotateBy::create(6.0f, 360);

    _rootLayer->runAction(RepeatForever::create(Sequence::create(move, move2, move3, nullptr)));
    _rootLayer->runAction(RepeatForever::create(Sequence::create(scale, scale2, nullptr)));
    _rootLayer->runAction(RepeatForever::create(rotate));
}

std::string PhysicsTransformTest::title() const
{
    return "Physics transform test";
}

void PhysicsIssue9959::onEnter()
{
    PhysicsDemo::onEnter();

    auto origin      = Director::getInstance()->getVisibleOrigin();
    auto visibleSize = Director::getInstance()->getVisibleSize();

    auto scale9Sprite1 = ui::Scale9Sprite::create("Images/blocks.png");
    scale9Sprite1->setPosition(origin + visibleSize / 2);
    addChild(scale9Sprite1);
    scale9Sprite1->runAction(RepeatForever::create(Sequence::create(
        MoveBy::create(2.0f, Vec2(100.0f, 0.0f)), MoveBy::create(2.0f, Vec2(-100.0f, 0.0f)), nullptr)));

    auto scale9Sprite2 = ui::Scale9Sprite::create("Images/blocks.png");
    scale9Sprite2->setPosition(origin + visibleSize / 2 + Vec2(0.0f, 50.0f));
    addChild(scale9Sprite2);
    scale9Sprite2->runAction(
        RepeatForever::create(Sequence::create(ScaleTo::create(2.0f, 1.5f), ScaleTo::create(2.0f, 1.0f), nullptr)));

    auto scale9Sprite3 = ui::Scale9Sprite::create("Images/blocks.png");
    scale9Sprite3->setPosition(origin + visibleSize / 2 + Vec2(0.0f, -50.0f));
    addChild(scale9Sprite3);
    scale9Sprite3->runAction(RepeatForever::create(RotateBy::create(2.0f, 360.0f)));
}

std::string PhysicsIssue9959::title() const
{
    return "Reorder issue #9959";
}

std::string PhysicsIssue9959::subtitle() const
{
    return "Test Scale9Sprite run scale/move/rotation action in physics scene";
}

//
void PhysicsIssue15932::onEnter()
{
    PhysicsDemo::onEnter();

    Rigidbody2D* pb = Rigidbody2D::createBox(Size(15, 5), PhysicsMaterial2D(0.1f, 0.0f, 1.0f));
    this->removeComponent(pb);
    this->addComponent(pb);
    this->removeComponent(pb);
}

std::string PhysicsIssue15932::title() const
{
    return "Github issue #15932";
}

std::string PhysicsIssue15932::subtitle() const
{
    return "addComponent()/removeComponent() should not crash";
}

void PhysicsDemoPyramidStackFixedUpdate::onEnter()
{
    PhysicsDemo::onEnter();

    setFixedUpdateEnabled(true);

    auto node = Node::create();
    node->addComponent(Rigidbody2D::createEdgeSegment(VisibleRect::leftBottom() + Vec2(0.0f, 50.0f),
                                                      VisibleRect::rightBottom() + Vec2(0.0f, 50.0f)));
    this->addChild(node);

    auto ball = Sprite::create("Images/ball.png");
    ball->setScale(1);
    auto body = Rigidbody2D::createCircle(10);
    ball->addComponent(body);
    ball->setTag(100);
    body->setTag(DRAG_BODYS_BITS);
    ball->setPosition(VisibleRect::bottom() + Vec2(0.0f, 60.0f));
    this->addChild(ball);

    _delayTime = 0;
    _isAddBall = false;

    int count = 1;
    for (int i = 0; i < 14; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            auto sp = addGrossiniAtPosition(VisibleRect::bottom() + Vec2((i / 2 - j) * 11, (14 - i) * 23 + 100), 0.2f);

            auto label = Label::createWithTTF(fmt::format("{}", count++), "fonts/arial.ttf", 24);
            label->setPosition(sp->getContentSize().width * 0.5f, sp->getContentSize().height * 0.5f);
            sp->addChild(label, 1);
        }
    }

    this->toggleDebug();
}

std::string PhysicsDemoPyramidStackFixedUpdate::title() const
{
    return "Pyramid Stack fixed update";
}

void PhysicsDemoPyramidStackFixedUpdate::fixedUpdate(float delta)
{
    Scene::fixedUpdate(delta);

    _delayTime += delta;
    if (_delayTime >= 3.0f && !_isAddBall)
    {
        _isAddBall = true;
        auto ball  = getChildByTag(100);
        if (ball)
            ball->setScale(ball->getScale() * 3);
    }
}

#endif
