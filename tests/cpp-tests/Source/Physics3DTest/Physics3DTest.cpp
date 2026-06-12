/****************************************************************************
 Copyright (c) 2012 cocos2d-x.org
 Copyright (c) 2015-2016 Chukong Technologies Inc.
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

#include "Physics3DTest.h"

#if defined(AX_ENABLE_PHYSICS_3D)

#    include "axmol/3d/Terrain.h"
#    include "axmol/3d/Bundle3D.h"

#    include "Particle3D/PU/PUParticleSystem3D.h"
USING_NS_AX_EXT;
using namespace ax;

static const std::string_view boss[] = {"MeshRendererTest/boss.c3b", "MeshRendererTest/boss.obj"};
static const int boss_count          = sizeof(boss) / sizeof(boss[0]);

enum
{
    IDC_NEXT = 100,
    IDC_BACK,
    IDC_RESTART
};

#    define START_POS_X  -0.5
#    define START_POS_Y  -2.5
#    define START_POS_Z  -0.5

#    define ARRAY_SIZE_X 4
#    define ARRAY_SIZE_Y 3
#    define ARRAY_SIZE_Z 4

static ax::Scene* physicsScene = nullptr;

Physics3DTests::Physics3DTests()
{
    ADD_TEST_CASE(BasicPhysics3DDemo);
    ADD_TEST_CASE(Joint3DDemo);
    ADD_TEST_CASE(Physics3DOneWayPlatform);
    ADD_TEST_CASE(Physics3DKinematicDemo);
    ADD_TEST_CASE(Physics3DCollisionCallbackDemo);
    ADD_TEST_CASE(Physics3DColliderDemo);
    ADD_TEST_CASE(Physics3DTerrainDemo);
};

std::string Physics3DTestDemo::title() const
{
    return "Physics3D Test";
}

std::string Physics3DTestDemo::subtitle() const
{
    return "";
}

bool Physics3DTestDemo::init()
{
    if (!TestCase::init())
        return false;

    if (initWithPhysics())
    {
        getPhysicsWorld3D()->setDebugDrawEnabled(false);

        physicsScene = this;
        Size size    = Director::getInstance()->getCanvasSize();
        _camera      = Camera::createPerspective(30.0f, size.width / size.height, 1.0f, 1000.0f);
        _camera->setPosition3D(Vec3(0.0f, 50.0f, 100.0f));
        _camera->lookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
        _camera->setCameraFlag(CameraFlag::USER1);
        this->addChild(_camera);

        auto listener           = PointerEventListener::create();
        listener->onPointerDown = AX_CALLBACK_1(Physics3DTestDemo::onPointerDown, this);
        listener->onPointerMove = AX_CALLBACK_1(Physics3DTestDemo::onPointerMove, this);
        listener->onPointerUp   = AX_CALLBACK_1(Physics3DTestDemo::onPointerUp, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

        TTFConfig ttfConfig("fonts/arial.ttf", 10);
        auto label    = Label::createWithTTF(ttfConfig, "DebugDraw OFF");
        auto menuItem = MenuItemLabel::create(label, [this, label](Object* /*sender*/) {
            if (getPhysicsWorld3D()->isDebugDrawEnabled())
            {
                getPhysicsWorld3D()->setDebugDrawEnabled(false);
                label->setString("DebugDraw OFF");
            }
            else
            {
                getPhysicsWorld3D()->setDebugDrawEnabled(true);
                label->setString("DebugDraw ON");
            }
        });

        auto menu = Menu::create(menuItem, nullptr);
        menu->setPosition(Vec2::ZERO);
        menuItem->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
        menuItem->setPosition(Vec2(VisibleRect::left().x, VisibleRect::top().y - 50));
        this->addChild(menu);

        _angle = 0.0f;
        return true;
    }
    physicsScene = nullptr;
    return false;
}

bool Physics3DTestDemo::onPointerDown(ax::PointerEvent* event)
{
    _needShootBox = true;
    event->stopPropagation();

    return true;
}

void Physics3DTestDemo::onPointerMove(ax::PointerEvent* event)
{
    if (!event->isCaptured())
        return;
    if (_camera)
    {
        auto delta = event->getDelta();

        _angle -= AX_DEGREES_TO_RADIANS(delta.x);
        _camera->setPosition3D(Vec3(100.0f * sinf(_angle), 50.0f, 100.0f * cosf(_angle)));
        _camera->lookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));

        if (delta.lengthSquared() > 16)
        {
            _needShootBox = false;
        }
        event->stopPropagation();

        return;
    }
}

void Physics3DTestDemo::onPointerUp(ax::PointerEvent* event)
{
    if (!_needShootBox)
        return;

    auto ray = _camera->screenToRay(event->getScreenLocation());
    shootBox(_camera->getPosition3D() + ray.direction * 10.0f);
    event->stopPropagation();
}

Physics3DTestDemo::Physics3DTestDemo() {}

void Physics3DTestDemo::update(float /*delta*/) {}

Physics3DTestDemo::~Physics3DTestDemo() {}

void Physics3DTestDemo::shootBox(const ax::Vec3& des)
{
    Vec3 linearVel = des - _camera->getPosition3D();
    linearVel.normalize();
    linearVel *= 100.0f;
    auto mesh = MeshRenderer::create("MeshRendererTest/box.c3t");
    mesh->setTexture("Images/Icon.png");

    this->addChild(mesh);
    mesh->setPosition3D(_camera->getPosition3D());
    mesh->setScale(0.5f);

    // In Bullet, shootBox used a BoxCollider3D with CCD swept sphere radius.
    // Bullet approximates fast-moving boxes as spheres during CCD, which suppresses edge contacts and rolling.
    // Jolt's CCD is shape-based and does not provide a swept sphere radius parameter.
    // To replicate Bullet's behavior (fast-moving object not rolling when hitting the ground),
    // we explicitly use a SphereCollider3D here instead of a BoxCollider3D.
    // auto rigidbody = Rigidbody3D::create(BoxCollider3D::create(Vec3(0.5f, 0.5f, 0.5f)), 1.0f);
    auto rigidbody = Rigidbody3D::create(SphereCollider3D::create(0.4f), 1.0f);
    rigidbody->setLinearVelocity(linearVel);
    mesh->addComponent(rigidbody);
    rigidbody->setCollisionDetectionMode(Rigidbody3D::CollisionDetectionMode::Continuous);
    rigidbody->setAllowSleeping(false);
    // the setCcdSweptSphereRadius was removed since axmol switch to JoltPhysics
    // rigidbody->setCcdSweptSphereRadius(0.4);

    rigidbody->syncNodeToPhysics();

    // optimize, only sync node to physics
    rigidbody->setSyncFlag(Rigidbody3D::SyncFlag::PHYSICS_TO_NODE);  // sync node to physics

    mesh->setCameraMask((unsigned short)CameraFlag::USER1);
}

std::string BasicPhysics3DDemo::subtitle() const
{
    return "Basic Physics3D";
}

bool BasicPhysics3DDemo::init()
{
    if (!Physics3DTestDemo::init())
        return false;

    // create floor
    auto floor         = MeshRenderer::create("MeshRendererTest/box.c3t");
    auto floorCollider = BoxCollider3D::create(Vec3(60.0f, 1.0f, 60.0f));
    auto floorBody     = Rigidbody3D::create(floorCollider);
    floor->setTexture("MeshRendererTest/plane.png");
    floor->setScaleX(60);
    floor->setScaleZ(60);
    this->addChild(floor);
    floor->addComponent(floorCollider);
    floor->setCameraMask((unsigned short)CameraFlag::USER1);
    floorBody->syncNodeToPhysics();
    // static body sync is not needed
    floorBody->setSyncFlag(Rigidbody3D::SyncFlag::NODE_TO_PHYSICS);

    // create several boxes using PhysicsMeshRenderer
    auto collider = BoxCollider3D::create(Vec3(0.8f, 0.8f, 0.8f));
    float start_x = START_POS_X - ARRAY_SIZE_X / 2;
    float start_y = START_POS_Y;
    float start_z = START_POS_Z - ARRAY_SIZE_Z / 2;

    for (int k = 0; k < ARRAY_SIZE_Y; k++)
    {
        for (int i = 0; i < ARRAY_SIZE_X; i++)
        {
            for (int j = 0; j < ARRAY_SIZE_Z; j++)
            {
                float x = 1.0 * i + start_x;
                float y = 5.0 + 1.0 * k + start_y;
                float z = 1.0 * j + start_z;

                auto mesh = MeshRenderer::create("MeshRendererTest/box.c3t");
                mesh->setTexture("Images/CyanSquare.png");
                mesh->setPosition3D(Vec3(x, y, z));
                auto rigidbody = Rigidbody3D::create(collider, 1.0f);
                mesh->addComponent(rigidbody);
                rigidbody->syncNodeToPhysics();
                rigidbody->setSyncFlag(Rigidbody3D::SyncFlag::PHYSICS_TO_NODE);
                mesh->setCameraMask((unsigned short)CameraFlag::USER1);
                mesh->setScale(0.8f);
                this->addChild(mesh);
            }
        }
    }

    setDebugCamera(_camera);

    return true;
}

std::string Physics3DOneWayPlatform::subtitle() const
{
    return "Physics3D One Way Platform";
}

bool Physics3DOneWayPlatform::init()
{
    if (!Physics3DTestDemo::init())
        return false;

    setDebugCamera(_camera);

    // Enable PreSolve events globally to allow contact filtering
    _physicsWorld3D->setGlobalEventEnabled(
        ContactEventBits::PreSolve | ContactEventBits::Contact | ContactEventBits::Hit, true);
    _physicsWorld3D->setPreSolveCallback(AX_CALLBACK_1(Physics3DOneWayPlatform::onPreSolve, this));

    // Large floor (same as BasicDemo)
    auto floor         = MeshRenderer::create("MeshRendererTest/box.c3t");
    auto floorCollider = BoxCollider3D::create(Vec3(60.0f, 1.0f, 60.0f));
    auto floorBody     = Rigidbody3D::create(floorCollider);
    floor->setTexture("MeshRendererTest/plane.png");
    floor->setScaleX(60);
    floor->setScaleZ(60);
    floor->setCameraMask((unsigned short)CameraFlag::USER1);
    this->addChild(floor);
    floor->addComponent(floorBody);
    floorBody->syncNodeToPhysics();
    floorBody->setSyncFlag(Rigidbody3D::SyncFlag::NODE_TO_PHYSICS);

    // Ball falling from above (will stop on the platform)
    // auto ballAbove = MeshRenderer::create("MeshRendererTest/sphere.c3b");
    // ballAbove->setTexture("MeshRendererTest/plane.png");
    // ballAbove->setScale(1.0f / ballAbove->getContentSize().width);  // radius ≈ 1.0
    // auto ballAboveBody = Rigidbody3D::create(SphereCollider3D::create(0.5f, PhysicsMaterial{.restitution =
    // 0.5}), 1.0f); ballAbove->addComponent(ballAboveBody); ballAbove->setPosition3D(Vec3(0.0f, 6.0f, 0.0f));
    // ballAbove->setCameraMask((unsigned short)CameraFlag::USER1);
    // this->addChild(ballAbove);
    // ballAbove->setColor(Color32::RED);

    // Ball launching upward from below (will pass through the platform and land on the floor)
    auto ballBelow = MeshRenderer::create("MeshRendererTest/sphere.c3b");
    ballBelow->setTexture("MeshRendererTest/plane.png");
    ballBelow->setScale(1.0f / ballBelow->getContentSize().width);
    auto ballBelowBody = Rigidbody3D::create(SphereCollider3D::create(0.5f, PhysicsMaterial{.restitution = 0.5}), 0.5f);
    ballBelow->addComponent(ballBelowBody);
    ballBelow->setPosition3D(Vec3(0.0f, -6.0f, 0.0f));
    ballBelow->setCameraMask((unsigned short)CameraFlag::USER1);
    ballBelowBody->setLinearVelocity(Vec3(0, 20.0f, 0));  // upward velocity
    this->addChild(ballBelow);
    ballBelow->setColor(Color32::BLUE);

    // Register the PreSolve callback that decides when to allow collisions through the platform
    auto listener            = ContactEventListener3D::create();
    listener->onContactBegin = [](ContactEvent3D* event) { AXLOGI("onContactBegin"); };
    listener->onContactEnd   = [](ContactEvent3D* event) { AXLOGI("onContactEnd"); };
    listener->onCollisionHit = [](ContactEvent3D* event) { AXLOGI("onCollisionHit"); };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

bool Physics3DOneWayPlatform::onPreSolve(const ax::ContactInfo3D& info)
{
    // Allow only if the other body hits the top side (normal.y < 0)
    return info.normal.y > 0.0f;
}

std::string Joint3DDemo::subtitle() const
{
    return "Joint3D";
}

bool Joint3DDemo::init()
{
    if (!Physics3DTestDemo::init())
        return false;

    setDebugCamera(_camera);

    std::vector<std::function<void()>> subTests;

    auto createBoxWithRigidbody = [&](const Vec3& size, float mass, const Vec3& pos, const std::string& meshFile,
                                      const std::string& texFile, const std::string& name,
                                      bool disableGravity = false) -> MeshRenderer* {
        auto rigidbody = Rigidbody3D::create(BoxCollider3D::create(size), mass);
        auto mesh      = MeshRenderer::create(meshFile);
        if (!texFile.empty())
            mesh->setTexture(texFile);
        mesh->setScale(size.x);
        mesh->setPosition3D(pos);
        mesh->addComponent(rigidbody);
        mesh->setCameraMask((unsigned short)CameraFlag::USER1);
        mesh->setName(name);
        if (disableGravity)
            rigidbody->setGravityScale(0);
        this->addChild(mesh);
        return mesh;
    };

    // --- Pivot joint test
    subTests.emplace_back([&]() {
        auto mesh = MeshRenderer::create("MeshRendererTest/orc.c3b");
        Quaternion quat;
        Quaternion::createFromAxisAngle(Vec3(0.f, 1.f, 0.f), MathUtil::radians(180), &quat);
        auto rigidbody = Rigidbody3D::create(BoxCollider3D::create(Vec3(5.0f, 5.0f, 5.0f)), 10.0f);
        rigidbody->setTransformInPhysics(Vec3(0.f, -3.f, 0.f), quat);
        mesh->addComponent(rigidbody);
        addChild(mesh);
        mesh->setCameraMask((unsigned short)CameraFlag::USER1);
        mesh->setScale(0.4f);
        mesh->setPosition3D(Vec3(-30.f, 5.f, 0.f));
        // rigidbody->syncNodeToPhysics();
        rigidbody->setSyncFlag(Rigidbody3D::SyncFlag::PHYSICS_TO_NODE);

        auto joint = PivotJoint3D::create();
        joint->setAnchorsInWorldSpace(Vec3(-28.5f, 10.5f, -2.5f));
        mesh->addComponent(joint);
        mesh->setName("pivot");
    });

    // --- Fixed joint test
    subTests.emplace_back([&] {
        Vec3 p1(-30.f, -10.f, 0.f);
        Vec3 p2(p1.x, p1.y - 5.f, p1.z);
        Vec3 anchor(p1.x, (p1.y + p2.y) / 2.f, p1.z);

        auto meshA = createBoxWithRigidbody(Vec3(3.f, 3.f, 3.f), 1.0f, p1, "MeshRendererTest/box.c3t",
                                            "MeshRendererTest/plane.png", "fixed-bodyA", true);
        auto meshB = createBoxWithRigidbody(Vec3(3.f, 3.f, 3.f), 1.0f, p2, "MeshRendererTest/box.c3t",
                                            "MeshRendererTest/plane.png", "fixed-bodyB", true);

        auto fixedJoint = FixedJoint3D::create(meshB->getComponent<Rigidbody3D>());
        meshA->addComponent(fixedJoint);
        fixedJoint->setAnchorsInWorldSpace(anchor);
    });

    // --- Distance joint test
    subTests.emplace_back([&] {
        auto meshA =
            createBoxWithRigidbody(Vec3(3.f, 3.f, 3.f), 1.0f, Vec3(-20.f, 5.f, 0.f), "MeshRendererTest/box.c3t",
                                   "MeshRendererTest/plane.png", "fixed-bodyA", true);
        auto meshB =
            createBoxWithRigidbody(Vec3(3.f, 3.f, 3.f), 1.0f, Vec3(-20.f, 0.f, 0.f), "MeshRendererTest/box.c3t",
                                   "MeshRendererTest/plane.png", "fixed-bodyB", true);

        auto distanceJoint = DistanceJoint3D::create(meshB->getComponent<Rigidbody3D>());
        meshA->addComponent(distanceJoint);
        distanceJoint->setLimits({.lower = 5, .upper = 10});
        distanceJoint->setSpring(JointSpring{.frequency = 0.7f, .damping = 0.4f});
    });

    // --- Hinge joint test
    subTests.emplace_back([&]() {
        auto rigidbody = Rigidbody3D::create(BoxCollider3D::create(Vec3(8.0f, 8.0f, 1.f)), 1.0f);
        auto mesh      = MeshRenderer::create("MeshRendererTest/box.c3t");
        mesh->setTexture("MeshRendererTest/plane.png");
        mesh->setScaleX(8.f);
        mesh->setScaleY(8.f);
        mesh->setPosition3D(Vec3(0.f, 0.f, 0.f));
        mesh->addComponent(rigidbody);
        mesh->setCameraMask((unsigned short)CameraFlag::USER1);
        this->addChild(mesh);
        // rigidbody->syncNodeToPhysics();

        auto joint = HingeJoint3D::createInWorldSpace(Vec3(4.f, 4.f, 0.5f), Vec3(0.f, 1.f, 0.f));
        joint->setUseMotor(true);
        joint->setMotor(JointMotor{.targetVelocity = -90, .maxForce = 100});

        mesh->addComponent(joint);
        mesh->setName("hinge");
        mesh->setTag(1129);
    });

    // --- Slider joint test
    subTests.emplace_back([&]() {
        auto meshSmall =
            createBoxWithRigidbody(Vec3(3.f, 1.2f, 3.f), 1.0f, Vec3(30.f, 15.f, 0.f), "MeshRendererTest/box.c3t",
                                   "MeshRendererTest/plane.png", "slide-small");
        auto rbSmall = meshSmall->getComponent<Rigidbody3D>();
        // rbSmall->syncNodeToPhysics();
        rbSmall->setLinearVelocity(Vec3(0, 3, 0));

        auto meshBig = createBoxWithRigidbody(Vec3(3.f, 3.f, 3.f), 0.0f, Vec3(30.f, 5.f, 0.f),
                                              "MeshRendererTest/box.c3t", "MeshRendererTest/plane.png", "slide-big");
        auto rbBig   = meshBig->getComponent<Rigidbody3D>();
        // rbBig->syncNodeToPhysics();

        ax::Vec3 axis    = ax::Vec3(0, 1, 0);
        ax::Vec3 pivotA  = ax::Vec3(0, -5, 0);
        ax::Vec3 pivotB  = ax::Vec3(0, 5, 0);
        auto sliderJoint = SliderJoint3D::create(rbBig);
        sliderJoint->setAnchors(pivotA, pivotB);
        sliderJoint->setAxes(axis, axis);

        meshSmall->addComponent(sliderJoint);
        sliderJoint->setLimits(JointLimits{.lower = -5.f, .upper = 5.f});
    });

    // --- ConeTwist joint test
    subTests.emplace_back([&]() {
        auto mesh = createBoxWithRigidbody(Vec3(3.f, 3.f, 3.f), 1.0f, Vec3(-10.f, 5.f, 0.f), "MeshRendererTest/box.c3t",
                                           "MeshRendererTest/plane.png", "cone-twist");
        auto rb   = mesh->getComponent<Rigidbody3D>();
        // rb->syncNodeToPhysics();

        ax::Vec3 axis   = ax::Vec3(0, 0, 1);
        auto twistJoint = ConeTwistJoint3D::create();
        twistJoint->setAnchor(Vec3{0.f, -10.0f, 0.f});
        twistJoint->setAxes(axis, axis);
        twistJoint->setLimits(TwistLimits{.swingSpan1 = 10, .swingSpan2 = 10, .twistSpan = 40});
        twistJoint->setSwingMotor(JointMotor{.targetVelocity = 45, .maxForce = 1000});
        twistJoint->setTwistMotor(JointMotor{.targetVelocity = 45, .maxForce = 1000});

        mesh->addComponent(twistJoint);
    });

    // --- Six DOF joint test
    subTests.emplace_back([&]() {
        auto mesh = createBoxWithRigidbody(Vec3(3.f, 3.f, 3.f), 1.0f, Vec3(30.f, -5.f, 0.f), "MeshRendererTest/box.c3t",
                                           "MeshRendererTest/plane.png", "six-dof");
        auto rb   = mesh->getComponent<Rigidbody3D>();
        // rb->syncNodeToPhysics();

        auto sixDofJoint = SixDofJoint3D::createInWorldSpace(Vec3(30.f, -5.f, 0.f), Vec3{1.0f, 0.f, 0.f});
        mesh->addComponent(sixDofJoint);
        sixDofJoint->setLinearLimits(SixDofLimits{.lower = Vec3(-10, 0, 0), .upper = Vec3(10, 0, 0)});
        sixDofJoint->setAngularLimits(SixDofLimits{.lower = Vec3(0, 0, 0), .upper = Vec3(0, 0, 0)});
    });

    // Call all joint tests
    for (auto& subTestFunc : subTests)
        subTestFunc();

    return true;
}

bool Joint3DDemo::onPointerDown(ax::PointerEvent* event)
{
    // ray trace
    if (_camera)
    {
        auto location = event->getScreenLocation();
        Vec3 nearP(location.x, location.y, 0.0f), farP(location.x, location.y, 1.0f);

        auto size = Director::getInstance()->getCanvasSize();
        nearP     = _camera->deprojectScreenToWorld(nearP);
        farP      = _camera->deprojectScreenToWorld(farP);

        PhysicsWorld3D::HitResult result;
        bool ret = physicsScene->getPhysicsWorld3D()->rayCast(nearP, farP, &result);
        if (ret && result.hitActor)
        {
            _constraint = PivotJoint3D::create();

            _constraint->setAnchorsInWorldSpace(result.hitPosition);
            result.hitActor->getOwner()->addComponent(_constraint);
            _pickingDistance = (result.hitPosition - nearP).length();
            event->stopPropagation();
            return true;
        }
    }
    Physics3DTestDemo::onPointerDown(event);
    _needShootBox = false;

    return true;
}

void Joint3DDemo::onPointerMove(ax::PointerEvent* event)
{
    if (!event->isCaptured())
        return;
    if (_constraint)
    {
        auto p2pConstraint = ((PivotJoint3D*)_constraint);
        auto ray           = _camera->screenToRay(event->getScreenLocation());
        p2pConstraint->setConnectedAnchor(ray.origin + ray.direction * _pickingDistance);
        event->stopPropagation();
        return;
    }
    Physics3DTestDemo::onPointerMove(event);
}

void Joint3DDemo::onPointerUp(ax::PointerEvent* event)
{
    if (_constraint)
    {
        auto owner = _constraint->getOwner();
        if (owner)
        {
            owner->removeComponent(_constraint);
        }
        _constraint = nullptr;
        event->stopPropagation();
        return;
    }
    Physics3DTestDemo::onPointerUp(event);
}

std::string Physics3DKinematicDemo::subtitle() const
{
    return "Physics3D Kinematic + #879";
}

bool Physics3DKinematicDemo::init()
{
    if (!Physics3DTestDemo::init())
        return false;

    // create floor
    auto floor     = MeshRenderer::create("MeshRendererTest/box.c3t");
    auto floorBody = Rigidbody3D::create(BoxCollider3D::create(Vec3(60.0f, 1.0f, 60.0f)));
    floor->setTexture("MeshRendererTest/plane.png");
    floor->setScaleX(60);
    floor->setScaleZ(60);
    floor->setPosition3D(Vec3(0.f, -1.f, 0.f));
    this->addChild(floor);
    floor->addComponent(floorBody);
    floor->setCameraMask((unsigned short)CameraFlag::USER1);
    floorBody->syncNodeToPhysics();
    // static object sync is not needed
    floorBody->setSyncFlag(Rigidbody3D::SyncFlag::NONE);

    // Issue #879 STARTS BELOW
    {
        std::string tree1 = "MeshRendererTest/tree-model/tree1.obj";

        float scale                     = 12.0f;
        std::vector<Vec3> trianglesList = Bundle3D::getTrianglesList(tree1);
        for (auto& it : trianglesList)
        {
            it *= scale;
        }

        auto sprite   = MeshRenderer::create(tree1);
        auto material = sprite->getSubMeshMaterial(1, 0);
        material->enableTransparentDoubleSided();
        sprite->setCameraMask((unsigned short)CameraFlag::USER1 | (unsigned short)CameraFlag::USER2 |
                              (unsigned short)CameraFlag::USER3);
        sprite->setPosition3D(Vec3(20.0f, 0.0f, 0.0f));
        sprite->setScale(scale);
        this->addChild(sprite);

        auto rigidbody = Rigidbody3D::create(MeshCollider3D::create(trianglesList));
        sprite->addComponent(rigidbody);
    }
    // Issue #879 ENDS HERE

    // create Kinematics
    for (unsigned int i = 0; i < 3; ++i)
    {
        auto mesh = MeshRenderer::create("MeshRendererTest/box.c3t");
        mesh->setTexture("Images/CyanSquare.png");
        mesh->setCameraMask((unsigned short)CameraFlag::USER1);

        this->addChild(mesh);

        mesh->setScale(2.0f);
        mesh->setPosition3D(Vec3(-15.0f, 0.0f, 15.0f - 15.0f * i));
        auto moveby = MoveBy::create(2.0f + i, Vec3(30.0f, 0.0f, 0.0f));

        // kinematic objects. zero mass so that it can not be affected by other dynamic objects
        auto rigidbody = Rigidbody3D::create(BoxCollider3D::create(Vec3(2.0f, 2.0f, 2.0f)), 0.001);
        mesh->addComponent(rigidbody);
        rigidbody->setKinematic();

        mesh->runAction(RepeatForever::create(Sequence::create(moveby, moveby->reverse(), nullptr)));
    }

    // create Dynamic
    {
        // create several spheres
        auto collider = SphereCollider3D::create(0.5f);
        float start_x = START_POS_X - ARRAY_SIZE_X / 2;
        float start_y = START_POS_Y + 5.0f;
        float start_z = START_POS_Z - ARRAY_SIZE_Z / 2;

        for (int k = 0; k < ARRAY_SIZE_Y; k++)
        {
            for (int i = 0; i < ARRAY_SIZE_X; i++)
            {
                for (int j = 0; j < ARRAY_SIZE_Z; j++)
                {
                    float x   = 1.0 * i + start_x;
                    float y   = 5.0 + 1.0 * k + start_y;
                    float z   = 1.0 * j + start_z;
                    auto mesh = MeshRenderer::create("MeshRendererTest/sphere.c3b");
                    mesh->setTexture("MeshRendererTest/plane.png");
                    mesh->setCameraMask((unsigned short)CameraFlag::USER1);
                    mesh->setScale(1.0f / mesh->getContentSize().width);
                    this->addChild(mesh);
                    mesh->setPosition3D(Vec3(x, y, z));
                    auto rigidbody = Rigidbody3D::create(collider, 1.0f);
                    mesh->addComponent(rigidbody);
                    rigidbody->syncNodeToPhysics();
                    rigidbody->setSyncFlag(Rigidbody3D::SyncFlag::PHYSICS_TO_NODE);
                }
            }
        }
    }

    setDebugCamera(_camera);
    return true;
}

bool Physics3DTerrainDemo::init()
{
    if (!Physics3DTestDemo::init())
        return false;

    Terrain::DetailMap r("TerrainTest/dirt.jpg"), g("TerrainTest/Grass2.jpg", 10), b("TerrainTest/road.jpg"),
        a("TerrainTest/GreenSkin.jpg", 20);

    Terrain::TerrainData data("TerrainTest/heightmap129.jpg", "TerrainTest/alphamap.png", r, g, b, a, Size(32, 32),
                              20.0f, 1.0f);
    auto terrain = Terrain::create(data, Terrain::CrackFixedType::SKIRT);
    terrain->setMaxDetailMapAmount(4);
    terrain->setCameraMask(2);
    terrain->setDrawWire(false);

    terrain->setSkirtHeightRatio(3);
    terrain->setLODDistance(64, 128, 192);
    terrain->setCameraMask((unsigned short)CameraFlag::USER1);

    // create terrain
    std::vector<float> heidata = terrain->getHeightData();
    auto size                  = terrain->getTerrainSize();
    auto collider = HeightFieldCollider3D::create(size.width, size.height, heidata, 1.0f, terrain->getMinHeight(),
                                                  terrain->getMaxHeight(), true, false);
    this->addChild(terrain);
    terrain->addComponent(collider);
    collider->syncNodeToPhysics();
    collider->setSyncFlag(Rigidbody3D::SyncFlag::NONE);

    // create several spheres
    auto sphereCollider = SphereCollider3D::create(0.5f);
    float start_x       = START_POS_X - ARRAY_SIZE_X / 2 + 5.0f;
    float start_y       = START_POS_Y + 20.0f;
    float start_z       = START_POS_Z - ARRAY_SIZE_Z / 2;

    for (int k = 0; k < ARRAY_SIZE_Y; k++)
    {
        for (int i = 0; i < ARRAY_SIZE_X; i++)
        {
            for (int j = 0; j < ARRAY_SIZE_Z; j++)
            {
                float x = 1.0 * i + start_x;
                float y = 5.0 + 1.0 * k + start_y;
                float z = 1.0 * j + start_z;

                auto mesh = MeshRenderer::create("MeshRendererTest/sphere.c3b");
                mesh->setTexture("MeshRendererTest/plane.png");
                mesh->setCameraMask((unsigned short)CameraFlag::USER1);
                mesh->setScale(1.0f / mesh->getContentSize().width);
                mesh->setPosition3D(Vec3(x, y, z));
                this->addChild(mesh);
                auto rigidbody = Rigidbody3D::create(sphereCollider, 1.f);
                mesh->addComponent(rigidbody);
                rigidbody->syncNodeToPhysics();
                rigidbody->setSyncFlag(Rigidbody3D::SyncFlag::PHYSICS_TO_NODE);
            }
        }
    }

    // create mesh

    for (int i = 0; i < boss_count; i++)
    {
        std::vector<Vec3> trianglesList = Bundle3D::getTrianglesList(boss[i]);

        auto collider = MeshCollider3D::create(trianglesList);

        auto mesh = MeshRenderer::create(boss[i]);
        mesh->setRotation3D(Vec3(-90.0f, 0.0f, 0.0f));
        mesh->setPosition3D(Vec3(-5 + 15.0f * i, 15.0f, 0.0f));
        mesh->setCameraMask(2);
        this->addChild(mesh);
        mesh->addComponent(collider);
        collider->syncNodeToPhysics();
        collider->setSyncFlag(Rigidbody3D::SyncFlag::NONE);

        std::vector<std::pair<Collider3D*, Mat4>> shapeList;
        {
            Mat4 localTrans;
            auto bodyshape = BoxCollider3D::create(Vec3(2.0f, 4.0f, 2.0f));
            Mat4::createTranslation(0.0f, 2.0f, 0.0f, &localTrans);
            shapeList.emplace_back(std::make_pair(bodyshape, localTrans));
            auto headshape = SphereCollider3D::create(1.5f);
            Mat4::createTranslation(0.6f, 5.0f, -1.5f, &localTrans);
            shapeList.emplace_back(std::make_pair(headshape, localTrans));
            auto lhandshape = BoxCollider3D::create(Vec3(1.0f, 3.0f, 1.0f));
            Mat4::createRotation(Vec3(1.0f, 0.0f, 0.0f), AX_DEGREES_TO_RADIANS(15.0f), &localTrans);
            localTrans.m[12] = -1.5f;
            localTrans.m[13] = 2.5f;
            localTrans.m[14] = -2.5f;
            shapeList.emplace_back(std::make_pair(lhandshape, localTrans));
            auto rhandshape = BoxCollider3D::create(Vec3(1.0f, 3.0f, 1.0f));
            Mat4::createRotation(Vec3(1.0f, 0.0f, 0.0f), AX_DEGREES_TO_RADIANS(-15.0f), &localTrans);
            localTrans.m[12] = 2.0f;
            localTrans.m[13] = 2.5f;
            localTrans.m[14] = 1.f;
            shapeList.emplace_back(std::make_pair(rhandshape, localTrans));

            auto rigidbody = Rigidbody3D::create(CompoundCollider3D::create(shapeList), 10.f);
            auto mesh      = MeshRenderer::create("MeshRendererTest/orc.c3b");
            mesh->addComponent(rigidbody);
            mesh->setRotation3D(Vec3(0.0f, 180.0f, 0.0f));
            mesh->setPosition3D(Vec3(-5.0f, 20.0f, 0.0f));
            mesh->setScale(0.4f);
            mesh->setCameraMask(2);
            this->addChild(mesh);
        }
    }

    setDebugCamera(_camera);
    return true;
}

std::string Physics3DTerrainDemo::subtitle() const
{
    return "Physics3D Terrain";
}

std::string Physics3DCollisionCallbackDemo::subtitle() const
{
    return "Physics3D HitEvent c3b/obj";
}

bool Physics3DCollisionCallbackDemo::init()
{
    if (!Physics3DTestDemo::init())
        return false;

    getPhysicsWorld3D()->setGlobalEventEnabled(ContactEventBits::Hit, true);

    {
        for (size_t i = 0; i < boss_count; i++)
        {
            float scale                     = 2.0f;
            std::vector<Vec3> trianglesList = Bundle3D::getTrianglesList(boss[i]);
            for (auto&& it : trianglesList)
            {
                it *= scale;
            }

            auto rigidbody = Rigidbody3D::create(MeshCollider3D::create(trianglesList));
            auto mesh      = MeshRenderer::create(boss[i]);
            mesh->addComponent(rigidbody);
            mesh->setRotation3D(Vec3(-90.0f, 0.0f, 0.0f));
            mesh->setPosition3D(Vec3(-5 + 15.0f * i, -5.0f, 0.0f));
            mesh->setScale(scale);
            mesh->setCameraMask((unsigned short)CameraFlag::USER1);
            this->addChild(mesh);

            rigidbody->setEventEnabled(ContactEventBits::Hit, true);
        }

        auto contactListener            = ContactEventListener3D::create();
        contactListener->onCollisionHit = [this](ContactEvent3D* contactEvent) {
            auto ps = PUParticleSystem3D::create("Particle3D/scripts/mp_hit_04.pu");
            ps->setPosition3D(contactEvent->getContactInfo().points[0].pointB);
            ps->setScale(0.05f);
            ps->startParticleSystem();
            ps->setCameraMask(2);
            this->addChild(ps);
            ps->runAction(Sequence::create(DelayTime::create(1.0f), CallFunc::create([=]() { ps->removeFromParent(); }),
                                           nullptr));

            // AXLOGD("------------BoxB Collision Info------------");
            // AXLOGD("Collision Point Num: {}", ci.collisionPointList.size());
            // for (auto&& iter : ci.collisionPointList){
            //	AXLOGD("Collision Position On A: ({:.2},{:.2}, {:.2})", iter.worldPositionOnA.x,
            // iter.worldPositionOnA.y,
            // iter.worldPositionOnA.z); 	AXLOGD("Collision Position On B: ({:.2}, {:.2}, {:.2})",
            // iter.worldPositionOnB.x, iter.worldPositionOnB.y, iter.worldPositionOnB.z); AXLOGD("Collision
            // Normal
            // On B: ({:.2}, {:.2}, {:.2})", iter.worldNormalOnB.x, iter.worldNormalOnB.y,
            // iter.worldNormalOnB.z);
            // }
            // AXLOGD("------------BoxB Collision Info------------");
        };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
    }

    setDebugCamera(_camera);
    return true;
}

std::string Physics3DColliderDemo::subtitle() const
{
    return "Physics3D Trigger";
}

bool Physics3DColliderDemo::init()
{
    if (!Physics3DTestDemo::init())
        return false;

    getPhysicsWorld3D()->setGlobalEventEnabled(ContactEventBits::Sensor, true);

    auto playerBody = Rigidbody3D::create(BoxCollider3D::create(Vec3(3.0f, 3.0f, 3.f)), 1.0f);
    playerBody->setKinematic();
    playerBody->setEventEnabled(ContactEventBits::Sensor, true);
    auto mesh = MeshRenderer::create("MeshRendererTest/box.c3t");
    mesh->setTexture("MeshRendererTest/plane.png");
    mesh->setScale(3.f);
    mesh->setPosition3D(Vec3(0.0f, 0.f, 30.f));
    mesh->addComponent(playerBody);
    mesh->setCameraMask((unsigned short)CameraFlag::USER1);
    auto moveby = MoveBy::create(5.0f, Vec3(0.0f, 0.0f, -60.0f));
    mesh->runAction(RepeatForever::create(Sequence::create(moveby, moveby->reverse(), nullptr)));
    this->addChild(mesh);

    {
        auto collider = SphereCollider3D::create(10.0f);
        collider->setSensor(true);
        auto node = Node::create();
        node->addComponent(collider);
        node->setCameraMask((unsigned short)CameraFlag::USER1);
        this->addChild(node);

        auto rigidbody = Rigidbody3D::create(BoxCollider3D::create(Vec3(10.0f, 10.0f, 1.f)), 1.0f);
        rigidbody->setKinematic();
        auto doorLeft = MeshRenderer::create("MeshRendererTest/box.c3t");
        doorLeft->setTexture("MeshRendererTest/plane.png");
        doorLeft->setScaleX(10.0f);
        doorLeft->setScaleY(10.0f);
        doorLeft->setScaleZ(1.0f);
        doorLeft->setPosition3D(Vec3(-5.0f, 0.0f, 0.0f));
        doorLeft->addComponent(rigidbody);
        doorLeft->setCameraMask((unsigned short)CameraFlag::USER1);
        node->addChild(doorLeft);

        rigidbody = Rigidbody3D::create(BoxCollider3D::create(Vec3(10.0f, 10.0f, 1.f)), 1.0f);
        rigidbody->setKinematic();
        auto doorRight = MeshRenderer::create("MeshRendererTest/box.c3t");
        doorRight->setTexture("MeshRendererTest/plane.png");
        doorRight->setScaleX(10.0f);
        doorRight->setScaleY(10.0f);
        doorRight->setScaleZ(1.0f);
        doorRight->setPosition3D(Vec3(5.0f, 0.0f, 0.0f));
        doorRight->addComponent(rigidbody);
        doorRight->setCameraMask((unsigned short)CameraFlag::USER1);
        node->addChild(doorRight);

        auto contactListener           = ContactEventListener3D::create();
        contactListener->onSensorBegin = [playerBody, doorLeft, doorRight](ContactEvent3D* contactEvent) {
            auto actorA = contactEvent->getActorB();
            if (actorA == playerBody)
            {
                auto moveby = MoveBy::create(1.0f, Vec3(-5.0f, 0.0f, 0.0f));
                doorLeft->runAction(moveby);
                doorRight->runAction(moveby->reverse());
            }
        };
        contactListener->onSensorEnd = [playerBody, doorLeft, doorRight](ContactEvent3D* contactEvent) {
            auto actorA = contactEvent->getActorB();
            if (actorA == playerBody)
            {
                auto moveby = MoveBy::create(1.0f, Vec3(5.0f, 0.0f, 0.0f));
                doorLeft->runAction(moveby);
                doorRight->runAction(moveby->reverse());
            }
        };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
    }

    setDebugCamera(_camera);
    return true;
}

#endif
