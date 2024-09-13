/****************************************************************************
 Copyright (c) 2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#include "MeshRendererTest.h"
#include "DrawNode3D.h"
#include "2d/CameraBackgroundBrush.h"
#include "3d/MeshMaterial.h"
#include "3d/MotionStreak3D.h"

#include "Particle3D/PU/PUParticleSystem3D.h"

#include <algorithm>
#include "../testResource.h"

using namespace ax;

MeshRendererTests::MeshRendererTests()
{
    ADD_TEST_CASE(MeshRendererBasicTest);
    ADD_TEST_CASE(MeshRendererStaticInstancingBasicTest);
    ADD_TEST_CASE(MeshRendererDynamicInstancingBasicTest);
    ADD_TEST_CASE(MeshRendererPreallocatedInstancingBufferTest);
    ADD_TEST_CASE(MeshRendererInstancingStressTest);
    ADD_TEST_CASE(MeshRendererHitTest);
    ADD_TEST_CASE(AsyncLoadMeshRendererTest);
    //    // 3DEffect use custom shader which is not supported on WP8/WinRT yet.
    ADD_TEST_CASE(MeshRendererEffectTest);
    ADD_TEST_CASE(MeshRendererUVAnimationTest);
    ADD_TEST_CASE(MeshRendererFakeShadowTest);
    ADD_TEST_CASE(MeshRendererBasicToonShaderTest);
    ADD_TEST_CASE(MeshRendererLightMapTest);
    ADD_TEST_CASE(MeshRendererWithSkinTest);
    ADD_TEST_CASE(MeshRendererWithSkinOutlineTest);
    ADD_TEST_CASE(Animate3DTest);
    ADD_TEST_CASE(AttachmentTest);
    ADD_TEST_CASE(MeshRendererReskinTest);
    ADD_TEST_CASE(MeshRendererWithOBBPerformanceTest);
    ADD_TEST_CASE(MeshRendererMirrorTest);
    ADD_TEST_CASE(QuaternionTest);
    ADD_TEST_CASE(MeshRendererEmptyTest);
    ADD_TEST_CASE(UseCaseMeshRenderer);
    ADD_TEST_CASE(MeshRendererForceDepthTest);
    ADD_TEST_CASE(MeshRendererCubeMapTest);
    ADD_TEST_CASE(NodeAnimationTest);
    ADD_TEST_CASE(Issue9767);
    ADD_TEST_CASE(MeshRendererClippingTest);
    ADD_TEST_CASE(Animate3DCallbackTest);
    ADD_TEST_CASE(CameraBackgroundClearTest);
    ADD_TEST_CASE(MeshRendererVertexColorTest);
    ADD_TEST_CASE(MotionStreak3DTest);
    ADD_TEST_CASE(MeshRendererPropertyTest);
    ADD_TEST_CASE(MeshRendererNormalMappingTest);
    ADD_TEST_CASE(Issue16155Test);
};

//------------------------------------------------------------------
//
// MeshRendererTestDemo
//
//------------------------------------------------------------------

std::string MeshRendererTestDemo::title() const
{
    return "No title";
}

MeshRendererTestDemo::~MeshRendererTestDemo()
{
    MeshMaterial::releaseCachedMaterial();
}

//------------------------------------------------------------------
//
// MeshRendererForceDepthTest
//
//------------------------------------------------------------------
MeshRendererForceDepthTest::MeshRendererForceDepthTest()
{
    auto orc = ax::MeshRenderer::create("MeshRendererTest/orc.c3b");
    orc->setScale(5);
    orc->setPositionNormalized(Vec2(.5f, .3f));
    orc->setPositionZ(40);
    orc->setRotation3D(Vec3(0.0f, 180.0f, 0.0f));
    orc->setGlobalZOrder(-1);

    addChild(orc);

    auto ship = MeshRenderer::create("MeshRendererTest/boss1.obj");
    ship->setScale(5);
    ship->setTexture("MeshRendererTest/boss.png");
    ship->setPositionNormalized(Vec2(.5f, .5f));
    ship->setRotation3D(Vec3(90.0f, 0.0f, 0.0f));
    ship->setForceDepthWrite(true);

    addChild(ship);
}

std::string MeshRendererForceDepthTest::title() const
{
    return "Force Depth Write Error Test";
}

std::string MeshRendererForceDepthTest::subtitle() const
{
    return "Ship should always appear behind orc";
}

//------------------------------------------------------------------
//
// MeshRendererEmptyTest
//
//------------------------------------------------------------------
MeshRendererEmptyTest::MeshRendererEmptyTest()
{
    auto s = MeshRenderer::create();
    s->setPositionNormalized(Vec2(.5f, .5f));
    auto l = Label::create();
    l->setString("Test");
    s->addChild(l);
    addChild(s);
}

std::string MeshRendererEmptyTest::title() const
{
    return "Testing MeshRenderer Container";
}

std::string MeshRendererEmptyTest::subtitle() const
{
    return "MeshRenderer can act as containers for 2D objects";
}

//------------------------------------------------------------------
//
// MeshRendererBasicTest
//
//------------------------------------------------------------------

MeshRendererBasicTest::MeshRendererBasicTest()
{
    auto listener            = EventListenerTouchAllAtOnce::create();
    listener->onTouchesEnded = AX_CALLBACK_2(MeshRendererBasicTest::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    auto s = Director::getInstance()->getWinSize();
    addNewMeshWithCoords(Vec2(s.width / 2, s.height / 2));
}

void MeshRendererBasicTest::addNewMeshWithCoords(Vec2 p)
{
    // int idx = (int)(AXRANDOM_0_1() * 1400.0f / 100.0f);
    // int x = (idx%5) * 85;
    // int y = (idx/5) * 121;

    //    //option 1: load a obj that contain the texture in it
    //    auto mesh = MeshRenderer::create("mesh3dTest/scene01.obj");

    // option 2: load obj and assign the texture
    auto mesh = MeshRenderer::create("MeshRendererTest/boss1.obj");
    mesh->setScale(3.f);
    mesh->setTexture("MeshRendererTest/boss.png");

    //
    // mesh->setEffect(ax::EFFECT_OUTLINE);

    // add to scene
    addChild(mesh);

    mesh->setPosition(Vec2(p.x, p.y));

    ActionInterval* action;
    float random = AXRANDOM_0_1();

    if (random < 0.20)
        action = ScaleBy::create(3, 2);
    else if (random < 0.40)
        action = RotateBy::create(3, 360);
    else if (random < 0.60)
        action = Blink::create(1, 3);
    else if (random < 0.8)
        action = TintBy::create(2, 0, -255, -255);
    else
        action = FadeOut::create(2);
    auto action_back = action->reverse();
    auto seq         = Sequence::create(action, action_back, nullptr);

    mesh->runAction(RepeatForever::create(seq));
}

void MeshRendererBasicTest::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    for (auto&&touch : touches)
    {
        auto location = touch->getLocation();

        addNewMeshWithCoords(location);
    }
}

std::string MeshRendererBasicTest::title() const
{
    return "Testing MeshRenderer";
}

std::string MeshRendererBasicTest::subtitle() const
{
    return "Tap screen to add more meshes";
}

//------------------------------------------------------------------
//
// MeshRendererStaticInstancingBasicTest
//
//------------------------------------------------------------------

MeshRendererStaticInstancingBasicTest::MeshRendererStaticInstancingBasicTest()
{
    auto mesh = MeshRenderer::create("MeshRendererTest/boss1.obj");
    mesh->setScale(3.f);
    mesh->setTexture("MeshRendererTest/boss.png");

    auto& s = Director::getInstance()->getWinSize();
    mesh->setPosition(s.width / 2, s.height / 2);

    mesh->enableInstancing(ax::MeshMaterial::InstanceMaterialType::UNLIT_INSTANCE);
    mesh->setDynamicInstancing(false);  // false by default

    FastRNG r{};

    for (int i = 0; i < 1000; i++)
    {
        auto inst = Node::create();
        inst->setPosition(70 * r.rangef(), 50 * r.rangef());
        inst->setRotation(r.maxf(360));
        mesh->addInstanceChild(inst, false);  // false by default parameter
    }

    mesh->shrinkToFitInstances();

    addChild(mesh);
}

std::string MeshRendererStaticInstancingBasicTest::title() const
{
    return "Testing Static Instancing";
}

std::string MeshRendererStaticInstancingBasicTest::subtitle() const
{
    return "1000 instances of the same mesh";
}

//------------------------------------------------------------------
//
// MeshRendererDynamicInstancingBasicTest
//
//------------------------------------------------------------------

MeshRendererDynamicInstancingBasicTest::MeshRendererDynamicInstancingBasicTest()
{
    auto mesh = MeshRenderer::create("MeshRendererTest/boss1.obj");
    mesh->setScale(3.f);
    mesh->setTexture("MeshRendererTest/boss.png");

    auto& s = Director::getInstance()->getWinSize();
    mesh->setPosition(s.width / 2, s.height / 2);

    mesh->enableInstancing(ax::MeshMaterial::InstanceMaterialType::UNLIT_INSTANCE);
    mesh->setDynamicInstancing(true);

    FastRNG r{};

    for (int i = 0; i < 1000; i++)
    {
        auto inst = Node::create();
        inst->setPosition(70 * r.rangef(), 50 * r.rangef());
        auto rot = RepeatForever::create(RotateBy::create(r.rangef(0.5, 2), Vec3(0, 0, 360)));
        inst->runAction(rot);
        mesh->addInstanceChild(inst, true);
    }

    mesh->shrinkToFitInstances();

    addChild(mesh);
}

std::string MeshRendererDynamicInstancingBasicTest::title() const
{
    return "Testing Dynamic Instancing";
}

std::string MeshRendererDynamicInstancingBasicTest::subtitle() const
{
    return "1000 instances of the same mesh with actions";
}

//------------------------------------------------------------------
//
// MeshRendererPreallocatedInstancingBufferTest
//
//------------------------------------------------------------------

MeshRendererPreallocatedInstancingBufferTest::MeshRendererPreallocatedInstancingBufferTest()
{
    auto mesh = MeshRenderer::create("MeshRendererTest/boss1.obj");
    mesh->setScale(3.f);
    mesh->setTexture("MeshRendererTest/boss.png");

    auto& s = Director::getInstance()->getWinSize();
    mesh->setPosition(s.width / 2, s.height / 2);

    mesh->enableInstancing(ax::MeshMaterial::InstanceMaterialType::UNLIT_INSTANCE, 1000);
    mesh->setDynamicInstancing(true);

    FastRNG r{};

    for (int i = 0; i < 1000; i++)
    {
        auto inst = Node::create();
        inst->setPosition(70 * r.rangef(), 50 * r.rangef());
        auto rot = RepeatForever::create(RotateBy::create(r.rangef(0.5, 2), Vec3(0, 0, 360)));
        inst->runAction(rot);
        mesh->addInstanceChild(inst, true);
    }

    addChild(mesh);
}

std::string MeshRendererPreallocatedInstancingBufferTest::title() const
{
    return "Testing Preallocated Instancing";
}

std::string MeshRendererPreallocatedInstancingBufferTest::subtitle() const
{
    return "1000 instances of the same mesh with actions";
}

//------------------------------------------------------------------
//
// MeshRendererInstancingStressTest
//
//------------------------------------------------------------------

MeshRendererInstancingStressTest::MeshRendererInstancingStressTest()
{
    auto mesh = MeshRenderer::create("MeshRendererTest/boss1.obj");
    mesh->setScale(3.f);
    mesh->setTexture("MeshRendererTest/boss.png");

    auto& s = Director::getInstance()->getWinSize();
    mesh->setPosition(s.width / 2, s.height / 2);

    mesh->enableInstancing(ax::MeshMaterial::InstanceMaterialType::UNLIT_INSTANCE, 10000);
    mesh->setDynamicInstancing(false); // false by default

    FastRNG r{};

    for (int i = 0; i < 10000; i++)
    {
        auto inst = Node::create();
        inst->setPosition(70 * r.rangef(), 50 * r.rangef());
        inst->setRotation(r.maxf(360));
        mesh->addInstanceChild(inst, false);  // false by default parameter
    }

    addChild(mesh);
}

std::string MeshRendererInstancingStressTest::title() const
{
    return "Stress Testing Instancing";
}

std::string MeshRendererInstancingStressTest::subtitle() const
{
    return "10000 instances of the same mesh";
}

//------------------------------------------------------------------
//
// MeshRendererUVAnimationTest
//
//------------------------------------------------------------------

MeshRendererUVAnimationTest::MeshRendererUVAnimationTest()
{
    // the offset use to translating texture
    _cylinder_texture_offset = 0;
    _shining_duration        = 0;
    Size visibleSize         = Director::getInstance()->getVisibleSize();

    // use custom camera
    auto camera = Camera::createPerspective(60, visibleSize.width / visibleSize.height, 0.1f, 200);
    camera->setCameraFlag(CameraFlag::USER1);

    // create cylinder
    auto cylinder = MeshRenderer::create("MeshRendererTest/cylinder.c3b");
    auto mat      = MeshMaterial::createWithFilename("MeshRendererTest/UVAnimation.material");
    _state        = mat->getTechniqueByIndex(0)->getPassByIndex(0)->getProgramState();
    cylinder->setMaterial(mat);

    this->addChild(cylinder);
    this->setCameraMask(2);
    this->addChild(camera);

    // adjust cylinder's position & rotation
    cylinder->setPosition3D(Vec3(0.0f, -15.0f, -50.0f));
    cylinder->setRotation3D(Vec3(-90.0f, 0.0f, 0.0f));

    // the callback function update cylinder's texcoord
    schedule(AX_SCHEDULE_SELECTOR(MeshRendererUVAnimationTest::cylinderUpdate));

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    _backToForegroundListener = EventListenerCustom::create(EVENT_COME_TO_FOREGROUND, [=](EventCustom*) {
        auto mat = MeshMaterial::createWithFilename("MeshRendererTest/UVAnimation.material");

        cylinder->setMaterial(mat);
        _state = mat->getTechniqueByIndex(0)->getPassByIndex(0)->getProgramState();
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, -1);
#endif
}

MeshRendererUVAnimationTest::~MeshRendererUVAnimationTest()
{
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundListener);
#endif
}

std::string MeshRendererUVAnimationTest::title() const
{
    return "Testing UV Animation";
}

std::string MeshRendererUVAnimationTest::subtitle() const
{
    return "";
}

void MeshRendererUVAnimationTest::cylinderUpdate(float dt)
{
    // callback function to update cylinder's texcoord
    static bool fade_in = true;
    _cylinder_texture_offset += 0.3 * dt;
    _cylinder_texture_offset = (_cylinder_texture_offset > 1) ? 0 : _cylinder_texture_offset;
    if (fade_in)
    {
        _shining_duration += 0.5 * dt;
        if (_shining_duration > 1)
            fade_in = false;
    }
    else
    {
        _shining_duration -= 0.5 * dt;
        if (_shining_duration < 0)
            fade_in = true;
    }

    // pass the result to shader
    auto offsetLoc   = _state->getUniformLocation("offset");
    auto durationLoc = _state->getUniformLocation("duration");
    _state->setUniform(offsetLoc, &_cylinder_texture_offset, sizeof(_cylinder_texture_offset));
    _state->setUniform(durationLoc, &_shining_duration, sizeof(_shining_duration));
}

//------------------------------------------------------------------
//
// MeshRendererFakeShadowTest
//
//------------------------------------------------------------------
MeshRendererFakeShadowTest::MeshRendererFakeShadowTest()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();

    auto listener            = EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = AX_CALLBACK_2(MeshRendererFakeShadowTest::onTouchesBegan, this);
    listener->onTouchesMoved = AX_CALLBACK_2(MeshRendererFakeShadowTest::onTouchesMoved, this);
    listener->onTouchesEnded = AX_CALLBACK_2(MeshRendererFakeShadowTest::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    auto layer = Layer::create();
    addChild(layer, 0);
    // create Camera
    _camera = Camera::createPerspective(60, visibleSize.width / visibleSize.height, 0.1f, 200);
    _camera->setCameraFlag(CameraFlag::USER1);
    _camera->setPosition3D(Vec3(0.0f, 20.0f, 25.0f));
    _camera->setRotation3D(Vec3(-60.0f, 0.0f, 0.0f));

    // create a plane
    _plane = MeshRenderer::create("MeshRendererTest/plane.c3t");
    _plane->setRotation3D(Vec3(90.0f, 0.0f, 0.0f));

    auto mat = MeshMaterial::createWithFilename("MeshRendererTest/FakeShadow.material");
    _state   = mat->getTechniqueByIndex(0)->getPassByIndex(0)->getProgramState();
    _plane->setMaterial(mat);

    auto location  = _state->getUniformLocation("u_model_matrix");
    auto transform = _plane->getNodeToWorldTransform();
    _state->setUniform(location, transform.m, sizeof(transform.m));

    layer->addChild(_plane);

    // create the orc
    _orc = MeshRenderer::create("MeshRendererTest/orc.c3b");
    _orc->setScale(0.2f);
    _orc->setRotation3D(Vec3(0.0f, 180.0f, 0.0f));
    _orc->setPosition3D(Vec3(0.0f, 0.0f, 10.0f));
    _targetPos = _orc->getPosition3D();
    location   = _state->getUniformLocation("u_target_pos");
    _state->setUniform(location, &_targetPos, sizeof(_targetPos));
    layer->addChild(_orc);
    layer->addChild(_camera);
    layer->setCameraMask(2);

    schedule(AX_SCHEDULE_SELECTOR(MeshRendererFakeShadowTest::updateCamera), 0.0f);

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    _backToForegroundListener = EventListenerCustom::create(EVENT_COME_TO_FOREGROUND, [this](EventCustom*) {
        auto mat = MeshMaterial::createWithFilename("MeshRendererTest/FakeShadow.material");
        _state   = mat->getTechniqueByIndex(0)->getPassByIndex(0)->getProgramState();
        _plane->setMaterial(mat);
        backend::UniformLocation loc = _state->getUniformLocation("u_model_matrix");
        auto trans                   = _plane->getNodeToWorldTransform();
        _state->setUniform(loc, &trans, sizeof(trans));
        loc      = _state->getUniformLocation("u_target_pos");
        auto pos = _orc->getPosition3D();
        _state->setUniform(loc, &pos, sizeof(pos));
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, -1);
#endif
}

MeshRendererFakeShadowTest::~MeshRendererFakeShadowTest()
{
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundListener);
#endif
}

std::string MeshRendererFakeShadowTest::title() const
{
    return "fake shadow effect";
}

std::string MeshRendererFakeShadowTest::subtitle() const
{
    return "touch the screen to move around";
}

void MeshRendererFakeShadowTest::Move(ax::Object* sender, int value)
{
    _orc->setPositionX(_orc->getPositionX() + value);
}

void MeshRendererFakeShadowTest::updateCamera(float fDelta)
{
    updateState(fDelta);
    if (isState(_curState, State_Move))
    {
        move3D(fDelta);
        if (isState(_curState, State_Rotate))
        {
            Vec3 curPos = _orc->getPosition3D();

            Vec3 newFaceDir = _targetPos - curPos;
            newFaceDir.y    = 0;
            newFaceDir.normalize();
            Vec3 up;
            _orc->getNodeToWorldTransform().getUpVector(&up);
            up.normalize();
            Vec3 right;
            Vec3::cross(-newFaceDir, up, &right);
            right.normalize();
            Vec3 pos = Vec3(0, 0, 0);
            Mat4 mat;
            mat.m[0] = right.x;
            mat.m[1] = right.y;
            mat.m[2] = right.z;
            mat.m[3] = 0.0f;

            mat.m[4] = up.x;
            mat.m[5] = up.y;
            mat.m[6] = up.z;
            mat.m[7] = 0.0f;

            mat.m[8]  = newFaceDir.x;
            mat.m[9]  = newFaceDir.y;
            mat.m[10] = newFaceDir.z;
            mat.m[11] = 0.0f;

            mat.m[12] = pos.x;
            mat.m[13] = pos.y;
            mat.m[14] = pos.z;
            mat.m[15] = 1.0f;
            _orc->setAdditionalTransform(&mat);
        }
    }
}

void MeshRendererFakeShadowTest::move3D(float elapsedTime)
{
    Vec3 curPos     = _orc->getPosition3D();
    Vec3 newFaceDir = _targetPos - curPos;
    newFaceDir.y    = 0.0f;
    newFaceDir.normalize();
    Vec3 offset = newFaceDir * 25.0f * elapsedTime;
    curPos += offset;
    _orc->setPosition3D(curPos);
    offset.x = offset.x;
    offset.z = offset.z;
    // pass the newest orc position
    auto location = _state->getUniformLocation("u_target_pos");
    auto position = _orc->getPosition3D();
    _state->setUniform(location, &position, sizeof(position));
}

void MeshRendererFakeShadowTest::updateState(float elapsedTime)
{
    Vec3 curPos = _orc->getPosition3D();
    Vec3 curFaceDir;
    _orc->getNodeToWorldTransform().getForwardVector(&curFaceDir);
    curFaceDir = -curFaceDir;
    curFaceDir.normalize();
    Vec3 newFaceDir = _targetPos - curPos;
    newFaceDir.y    = 0.0f;
    newFaceDir.normalize();
    float cosAngle = std::fabs(Vec3::dot(curFaceDir, newFaceDir) - 1.0f);
    float dist     = curPos.distanceSquared(_targetPos);
    if (dist <= 4.0f)
    {
        if (cosAngle <= 0.01f)
            _curState = State_Idle;
        else
            _curState = State_Rotate;
    }
    else
    {
        if (cosAngle > 0.01f)
            _curState = State_Rotate | State_Move;
        else
            _curState = State_Move;
    }
}

bool MeshRendererFakeShadowTest::isState(unsigned int state, unsigned int bit) const
{
    return (state & bit) == bit;
}

void MeshRendererFakeShadowTest::onTouchesBegan(const std::vector<Touch*>& touches, ax::Event* event) {}

void MeshRendererFakeShadowTest::onTouchesMoved(const std::vector<Touch*>& touches, ax::Event* event) {}

void MeshRendererFakeShadowTest::onTouchesEnded(const std::vector<Touch*>& touches, ax::Event* event)
{
    for (auto&&item : touches)
    {
        auto touch    = item;
        auto location = touch->getLocationInView();
        if (_camera)
        {
            if (_orc)
            {
                Vec3 nearP(location.x, location.y, -1.0f), farP(location.x, location.y, 1.0f);

                auto size = Director::getInstance()->getWinSize();
                nearP     = _camera->unproject(nearP);
                farP      = _camera->unproject(farP);
                Vec3 dir(farP - nearP);
                float dist = 0.0f;
                float ndd  = Vec3::dot(Vec3(0, 1, 0), dir);
                if (ndd == 0)
                    dist = 0.0f;
                float ndo = Vec3::dot(Vec3(0, 1, 0), nearP);
                dist      = (0 - ndo) / ndd;
                Vec3 p    = nearP + dist * dir;

                if (p.x > 100)
                    p.x = 100;
                if (p.x < -100)
                    p.x = -100;
                if (p.z > 100)
                    p.z = 100;
                if (p.z < -100)
                    p.z = -100;

                _targetPos = p;
            }
        }
    }
}

//------------------------------------------------------------------
//
// MeshRendererBasicToonShaderTest
//
//------------------------------------------------------------------
MeshRendererBasicToonShaderTest::MeshRendererBasicToonShaderTest()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    auto _camera     = Camera::createPerspective(60, visibleSize.width / visibleSize.height, 0.1f, 200);
    _camera->setCameraFlag(CameraFlag::USER1);
    // create a teapot
    auto teapot = MeshRenderer::create("MeshRendererTest/teapot.c3b");
    auto mat    = MeshMaterial::createWithFilename("MeshRendererTest/BasicToon.material");
    _state      = mat->getTechniqueByIndex(0)->getPassByIndex(0)->getProgramState();
    teapot->setMaterial(mat);

    teapot->setPosition3D(Vec3(0.0f, -5.0f, -20.0f));
    teapot->setRotation3D(Vec3(-90.0f, 180.0f, 0.0f));
    auto rotate_action = RotateBy::create(1.5f, Vec3(0.0f, 30.0f, 0.0f));
    teapot->runAction(RepeatForever::create(rotate_action));
    addChild(teapot);
    addChild(_camera);
    setCameraMask(2);
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    _backToForegroundListener = EventListenerCustom::create(EVENT_COME_TO_FOREGROUND, [=](EventCustom*) {
        auto mat = MeshMaterial::createWithFilename("MeshRendererTest/BasicToon.material");
        _state   = mat->getTechniqueByIndex(0)->getPassByIndex(0)->getProgramState();
        teapot->setMaterial(mat);
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, -1);
#endif
}

MeshRendererBasicToonShaderTest::~MeshRendererBasicToonShaderTest()
{
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundListener);
#endif
}

std::string MeshRendererBasicToonShaderTest::title() const
{
    return "basic toon shader test";
}

std::string MeshRendererBasicToonShaderTest::subtitle() const
{
    return " ";
}

//------------------------------------------------------------------
//
// MeshRendererLightMapTest
//
//------------------------------------------------------------------
MeshRendererLightMapTest::MeshRendererLightMapTest()
{
    // the assets are from the OpenVR demo
    // get the visible size.
    Size visibleSize = Director::getInstance()->getVisibleSize();
    _camera          = Camera::createPerspective(60, visibleSize.width / visibleSize.height, 0.1f, 200);
    _camera->setCameraFlag(CameraFlag::USER1);
    _camera->setPosition3D(Vec3(0.0f, 25.0f, 15.0f));
    _camera->setRotation3D(Vec3(-35.0f, 0.0f, 0.0f));
    auto LightMapScene = MeshRenderer::create("MeshRendererTest/LightMapScene.c3b");
    LightMapScene->setScale(0.1f);
    addChild(LightMapScene);
    addChild(_camera);
    setCameraMask(2);

    // add a point light
    auto light = PointLight::create(Vec3(35, 75, -20.5), Color3B(255, 255, 255), 150);
    addChild(light);
    // set the ambient light
    auto ambient = AmbientLight::create(Color3B(55, 55, 55));
    addChild(ambient);

    // create a listener
    auto listener            = EventListenerTouchAllAtOnce::create();
    listener->onTouchesMoved = AX_CALLBACK_2(MeshRendererLightMapTest::onTouchesMoved, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
MeshRendererLightMapTest::~MeshRendererLightMapTest() {}
std::string MeshRendererLightMapTest::title() const
{
    return "light map test";
}

std::string MeshRendererLightMapTest::subtitle() const
{
    return "drag the screen to move around";
}

void MeshRendererLightMapTest::onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    if (touches.size() == 1)
    {
        float delta           = Director::getInstance()->getDeltaTime();
        auto touch            = touches[0];
        auto location         = touch->getLocation();
        auto PreviousLocation = touch->getPreviousLocation();
        Point newPos          = PreviousLocation - location;

        Vec3 cameraDir;
        Vec3 cameraRightDir;
        _camera->getNodeToWorldTransform().getForwardVector(&cameraDir);
        cameraDir.normalize();
        cameraDir.y = 0;
        _camera->getNodeToWorldTransform().getRightVector(&cameraRightDir);
        cameraRightDir.normalize();
        cameraRightDir.y = 0;
        Vec3 cameraPos   = _camera->getPosition3D();
        cameraPos += cameraDir * newPos.y * delta;
        cameraPos += cameraRightDir * newPos.x * delta;
        _camera->setPosition3D(cameraPos);
    }
}

//------------------------------------------------------------------
//
// MeshRendererHitTest
//
//------------------------------------------------------------------
MeshRendererHitTest::MeshRendererHitTest()
{
    auto s = Director::getInstance()->getWinSize();

    auto mesh1 = MeshRenderer::create("MeshRendererTest/boss1.obj");

    mesh1->setScale(4.f);
    mesh1->setTexture("MeshRendererTest/boss.png");
    mesh1->setPosition(Vec2(s.width / 2, s.height / 2));

    // add to scene
    addChild(mesh1);
    mesh1->runAction(RepeatForever::create(RotateBy::create(3.0f, 360.0f)));

    auto mesh2 = MeshRenderer::create("MeshRendererTest/boss1.obj");

    mesh2->setScale(4.f);
    mesh2->setTexture("MeshRendererTest/boss.png");
    mesh2->setPosition(Vec2(s.width / 2, s.height / 2));
    mesh2->setAnchorPoint(Vec2(0.5f, 0.5f));

    // add to scene
    addChild(mesh2);
    mesh2->runAction(RepeatForever::create(RotateBy::create(3.0f, -360.0f)));

    // Make mesh1 touchable
    auto listener1 = EventListenerTouchOneByOne::create();
    listener1->setSwallowTouches(true);

    listener1->onTouchBegan = [](Touch* touch, Event* event) {
        auto target = static_cast<MeshRenderer*>(event->getCurrentTarget());

        Rect rect = target->getBoundingBox();
        if (rect.containsPoint(touch->getLocation()))
        {
            AXLOGD("mesh3d began... x = {}, y = {}", touch->getLocation().x, touch->getLocation().y);
            target->setOpacity(100);
            return true;
        }
        return false;
    };

    listener1->onTouchMoved = [](Touch* touch, Event* event) {
        auto target = static_cast<MeshRenderer*>(event->getCurrentTarget());
        target->setPosition(target->getPosition() + touch->getDelta());
    };

    listener1->onTouchEnded = [=](Touch* touch, Event* event) {
        auto target = static_cast<MeshRenderer*>(event->getCurrentTarget());
        AXLOGD("mesh3d onTouchesEnded.. ");
        target->setOpacity(255);
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, mesh1);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener1->clone(), mesh2);
}

std::string MeshRendererHitTest::title() const
{
    return "Testing MeshRenderer Touch in 2D";
}

std::string MeshRendererHitTest::subtitle() const
{
    return "Tap MeshRenderer and Drag";
}

MeshRendererEffectTest::MeshRendererEffectTest()
{
    auto s = Director::getInstance()->getWinSize();
    addNewMeshWithCoords(Vec2(s.width / 2, s.height / 2));

    auto listener            = EventListenerTouchAllAtOnce::create();
    listener->onTouchesEnded = AX_CALLBACK_2(MeshRendererEffectTest::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    _backToForegroundListener = EventListenerCustom::create(EVENT_COME_TO_FOREGROUND, [this](EventCustom*) {
        auto material = MeshMaterial::createWithFilename("MeshRendererTest/outline.material");
        material->setTechnique("outline_noneskinned");
        for (auto&&mesh : _meshes)
        {
            mesh->setMaterial(material->clone());
        }
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, 1);
#endif
}

MeshRendererEffectTest::~MeshRendererEffectTest()
{
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundListener);
#endif
}

std::string MeshRendererEffectTest::title() const
{
    return "Testing MeshRenderer";
}
std::string MeshRendererEffectTest::subtitle() const
{
    return "MeshRenderer with effects";
}

void MeshRendererEffectTest::addNewMeshWithCoords(Vec2 p)
{
    // option 2: load obj and assign the texture
    auto mesh   = MeshRenderer::create("MeshRendererTest/boss1.obj");
    auto material = MeshMaterial::createWithFilename("MeshRendererTest/outline.material");
    material->setTechnique("outline_noneskinned");
    mesh->setMaterial(material);
    mesh->setScale(6.f);
    Director::getInstance()->getTextureCache()->removeUnusedTextures();

    // add to scene
    addChild(mesh);

    mesh->setPosition(Vec2(p.x, p.y));

    ActionInterval* action;
    float random = AXRANDOM_0_1();

    if (random < 0.20)
        action = ScaleBy::create(3, 2);
    else if (random < 0.40)
        action = RotateBy::create(3, 360);
    else if (random < 0.60)
        action = Blink::create(1, 3);
    else if (random < 0.8)
        action = TintBy::create(2, 0, -255, -255);
    else
        action = FadeOut::create(2);
    auto action_back = action->reverse();
    auto seq         = Sequence::create(action, action_back, nullptr);

    mesh->runAction(RepeatForever::create(seq));
    _meshes.emplace_back(mesh);
}

void MeshRendererEffectTest::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    for (auto&&touch : touches)
    {
        auto location = touch->getLocation();

        addNewMeshWithCoords(location);
    }
}

AsyncLoadMeshRendererTest::AsyncLoadMeshRendererTest()
{
    _paths.emplace_back("MeshRendererTest/girl.c3b");
    _paths.emplace_back("MeshRendererTest/orc.c3b");
    _paths.emplace_back("MeshRendererTest/ReskinGirl.c3b");
    _paths.emplace_back("MeshRendererTest/axe.c3b");

    TTFConfig ttfConfig("fonts/arial.ttf", 15);
    auto label1 = Label::createWithTTF(ttfConfig, "AsyncLoad MeshRenderer");
    auto item1 =
        MenuItemLabel::create(label1, AX_CALLBACK_1(AsyncLoadMeshRendererTest::menuCallback_asyncLoadMesh, this));

    auto s = Director::getInstance()->getWinSize();
    item1->setPosition(s.width * .5f, s.height * .8f);

    auto pMenu1 = Menu::create(item1, nullptr);
    pMenu1->setPosition(Vec2(0.0f, 0.0f));
    this->addChild(pMenu1, 10);

    auto node = Node::create();
    node->setTag(101);
    this->addChild(node);

    menuCallback_asyncLoadMesh(nullptr);
}

AsyncLoadMeshRendererTest::~AsyncLoadMeshRendererTest() {}

std::string AsyncLoadMeshRendererTest::title() const
{
    return "Testing MeshRenderer::createAsync";
}
std::string AsyncLoadMeshRendererTest::subtitle() const
{
    return "";
}

void AsyncLoadMeshRendererTest::menuCallback_asyncLoadMesh(Object* sender)
{
    auto node = getChildByTag(101);
    node->removeAllChildren();  // remove all loaded mesh

    // remove cache data
    MeshRendererCache::getInstance()->removeAllMeshRenderData();
    int32_t index = 0;
    for (const auto& path : _paths)
    {
        MeshRenderer::createAsync(path, AX_CALLBACK_2(AsyncLoadMeshRendererTest::asyncLoad_Callback, this), reinterpret_cast<void*>(static_cast<uintptr_t>(++index)));
    }
}

void AsyncLoadMeshRendererTest::asyncLoad_Callback(MeshRenderer* mesh, void* param)
{
    auto index  = static_cast<int>((uintptr_t)param);
    auto node   = getChildByTag(101);
    auto s      = Director::getInstance()->getWinSize();
    float width = s.width / _paths.size();
    Vec2 point(width * (0.5f + index), s.height / 2.f);
    mesh->setPosition(point);
    node->addChild(mesh);
}

MeshRendererWithSkinTest::MeshRendererWithSkinTest()
{
    auto listener            = EventListenerTouchAllAtOnce::create();
    listener->onTouchesEnded = AX_CALLBACK_2(MeshRendererWithSkinTest::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    // switch animation quality. In fact, you can set the mesh3d out of frustum to Animate3DQuality::QUALITY_NONE, it
    // can save a lot of cpu time
    MenuItemFont::setFontName("fonts/arial.ttf");
    MenuItemFont::setFontSize(15);
    _animateQuality = (int)Animate3DQuality::QUALITY_LOW;
    _menuItem       = MenuItemFont::create(getAnimationQualityMessage(),
                                           AX_CALLBACK_1(MeshRendererWithSkinTest::switchAnimationQualityCallback, this));
    _menuItem->setColor(Color3B(0, 200, 20));
    auto menu = Menu::create(_menuItem, NULL);
    menu->setPosition(Vec2::ZERO);
    _menuItem->setPosition(VisibleRect::left().x + 50, VisibleRect::top().y - 70);
    addChild(menu, 1);

    _meshes.clear();

    auto s = Director::getInstance()->getWinSize();
    addNewMeshWithCoords(Vec2(s.width / 2, s.height / 2));
}
std::string MeshRendererWithSkinTest::title() const
{
    return "Testing MeshRenderer";
}
std::string MeshRendererWithSkinTest::subtitle() const
{
    return "Tap screen to add more mesh3D";
}

void MeshRendererWithSkinTest::addNewMeshWithCoords(Vec2 p)
{
    std::string fileName = "MeshRendererTest/orc.c3b";
    auto mesh          = MeshRenderer::create(fileName);
    mesh->setScale(3);
    mesh->setRotation3D(Vec3(0.0f, 180.0f, 0.0f));
    mesh->setPosition(Vec2(p.x, p.y));
    addChild(mesh);
    _meshes.emplace_back(mesh);

    auto animation = Animation3D::create(fileName);
    if (animation)
    {
        auto animate = Animate3D::create(animation);
        bool inverse = (std::rand() % 3 == 0);

        int rand2   = std::rand();
        float speed = 1.0f;
        if (rand2 % 3 == 1)
        {
            speed = animate->getSpeed() + AXRANDOM_0_1();
        }
        else if (rand2 % 3 == 2)
        {
            speed = animate->getSpeed() - 0.5 * AXRANDOM_0_1();
        }
        animate->setSpeed(inverse ? -speed : speed);
        animate->setTag(110);
        animate->setQuality((Animate3DQuality)_animateQuality);
        auto repeat = RepeatForever::create(animate);
        repeat->setTag(110);
        mesh->runAction(repeat);
    }
}

std::string MeshRendererWithSkinTest::getAnimationQualityMessage() const
{
    if (_animateQuality == (int)Animate3DQuality::QUALITY_NONE)
        return "None Quality";
    else if (_animateQuality == (int)Animate3DQuality::QUALITY_LOW)
        return "Low Quality";
    else if (_animateQuality == (int)Animate3DQuality::QUALITY_HIGH)
        return "High Quality";

    return "";
}

void MeshRendererWithSkinTest::switchAnimationQualityCallback(Object* sender)
{
    ++_animateQuality;
    if (_animateQuality > (int)Animate3DQuality::QUALITY_HIGH)
        _animateQuality = (int)Animate3DQuality::QUALITY_NONE;

    _menuItem->setString(getAnimationQualityMessage());

    for (auto&&iter : _meshes)
    {
        RepeatForever* repAction = dynamic_cast<RepeatForever*>(iter->getActionByTag(110));
        Animate3D* animate3D     = dynamic_cast<Animate3D*>(repAction->getInnerAction());
        animate3D->setQuality((Animate3DQuality)_animateQuality);
    }
}

void MeshRendererWithSkinTest::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    for (auto&&touch : touches)
    {
        auto location = touch->getLocation();

        addNewMeshWithCoords(location);
    }
}

MeshRendererWithSkinOutlineTest::MeshRendererWithSkinOutlineTest()
{
    auto listener            = EventListenerTouchAllAtOnce::create();
    listener->onTouchesEnded = AX_CALLBACK_2(MeshRendererWithSkinOutlineTest::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    auto s = Director::getInstance()->getWinSize();
    addNewMeshWithCoords(Vec2(s.width / 2, s.height / 2));

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    _backToForegroundListener = EventListenerCustom::create(EVENT_COME_TO_FOREGROUND, [this](EventCustom*) {
        auto material = MeshMaterial::createWithFilename("MeshRendererTest/outline.material");
        material->setTechnique("outline_skinned");
        for (auto&&mesh : _meshes)
        {
            mesh->setMaterial(material->clone());
        }
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, 1);
#endif
}
MeshRendererWithSkinOutlineTest::~MeshRendererWithSkinOutlineTest()
{
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundListener);
#endif
}
std::string MeshRendererWithSkinOutlineTest::title() const
{
    return "Testing MeshRenderer for skinned outline";
}
std::string MeshRendererWithSkinOutlineTest::subtitle() const
{
    return "Tap screen to add more mesh3D";
}

void MeshRendererWithSkinOutlineTest::addNewMeshWithCoords(Vec2 p)
{

    std::string fileName = "MeshRendererTest/orc.c3b";
    auto mesh          = MeshRenderer::create(fileName);

    auto material = MeshMaterial::createWithFilename("MeshRendererTest/outline.material");
    material->setTechnique("outline_skinned");
    mesh->setMaterial(material);

    mesh->setScale(3);
    mesh->setRotation3D(Vec3(0.0f, 180.0f, 0.0f));
    addChild(mesh);
    mesh->setPosition(Vec2(p.x, p.y));

    auto animation = Animation3D::create(fileName);
    if (animation)
    {
        auto animate = Animate3D::create(animation);
        bool inverse = (std::rand() % 3 == 0);

        int rand2   = std::rand();
        float speed = 1.0f;
        if (rand2 % 3 == 1)
        {
            speed = animate->getSpeed() + AXRANDOM_0_1();
        }
        else if (rand2 % 3 == 2)
        {
            speed = animate->getSpeed() - 0.5 * AXRANDOM_0_1();
        }
        animate->setSpeed(inverse ? -speed : speed);

        mesh->runAction(RepeatForever::create(animate));
    }
}

void MeshRendererWithSkinOutlineTest::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    for (auto&&touch : touches)
    {
        auto location = touch->getLocation();

        addNewMeshWithCoords(location);
    }
}

Animate3DTest::Animate3DTest()
    : _mesh(nullptr), _swim(nullptr), _hurt(nullptr), _elapseTransTime(0.f), _moveAction(nullptr)
{
    addMeshRenderer();

    auto listener            = EventListenerTouchAllAtOnce::create();
    listener->onTouchesEnded = AX_CALLBACK_2(Animate3DTest::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    scheduleUpdate();
}

Animate3DTest::~Animate3DTest()
{
    AX_SAFE_RELEASE(_moveAction);
    AX_SAFE_RELEASE(_hurt);
    AX_SAFE_RELEASE(_swim);
}

std::string Animate3DTest::title() const
{
    return "Testing Animate3D";
}

std::string Animate3DTest::subtitle() const
{
    return "Touch to beat the tortoise";
}

void Animate3DTest::update(float dt)
{
    if (_state == State::HURT_TO_SWIMMING)
    {
        _elapseTransTime += dt;

        if (_elapseTransTime >= Animate3D::getTransitionTime())
        {
            _mesh->stopAction(_hurt);
            _state = State::SWIMMING;
        }
    }
    else if (_state == State::SWIMMING_TO_HURT)
    {
        _elapseTransTime += dt;
        if (_elapseTransTime >= Animate3D::getTransitionTime())
        {
            _mesh->stopAction(_swim);
            _state = State::HURT;
        }
    }
}

void Animate3DTest::addMeshRenderer()
{
    std::string fileName = "MeshRendererTest/tortoise.c3b";
    auto mesh          = MeshRenderer::create(fileName);
    mesh->setScale(0.1f);
    auto s = Director::getInstance()->getWinSize();
    mesh->setPosition(Vec2(s.width * 4.f / 5.f, s.height / 2.f));
    addChild(mesh);
    _mesh        = mesh;
    auto animation = Animation3D::create(fileName);
    if (animation)
    {
        auto animate = Animate3D::create(animation, 0.f, 1.933f);
        _swim        = RepeatForever::create(animate);
        mesh->runAction(_swim);

        _swim->retain();
        _hurt = Animate3D::create(animation, 1.933f, 2.8f);
        _hurt->retain();
        _state = State::SWIMMING;
    }

    _moveAction = MoveTo::create(4.f, Vec2(s.width / 5.f, s.height / 2.f));
    _moveAction->retain();
    auto seq =
        Sequence::create(_moveAction, CallFunc::create(AX_CALLBACK_0(Animate3DTest::reachEndCallBack, this)), nullptr);
    seq->setTag(100);
    mesh->runAction(seq);
}

void Animate3DTest::reachEndCallBack()
{
    auto s = Director::getInstance()->getWinSize();
    _mesh->stopActionByTag(100);
    auto inverse = MoveTo::create(4.f, Vec2(s.width - _mesh->getPositionX(), s.height / 2.f));
    inverse->retain();
    _moveAction->release();
    _moveAction = inverse;
    auto rot    = RotateBy::create(1.f, Vec3(0.f, 180.f, 0.f));
    auto seq    = Sequence::create(rot, _moveAction,
                                   CallFunc::create(AX_CALLBACK_0(Animate3DTest::reachEndCallBack, this)), nullptr);
    seq->setTag(100);
    _mesh->runAction(seq);
}

void Animate3DTest::renewCallBack()
{
    // rerun swim action
    _mesh->runAction(_swim);
    _state           = State::HURT_TO_SWIMMING;
    _elapseTransTime = 0.0f;
}

void Animate3DTest::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    for (auto&&touch : touches)
    {
        auto location = touch->getLocation();

        if (_mesh)
        {
            float len = (_mesh->getPosition() - location).length();
            if (len < 40)
            {
                // hurt the tortoise
                if (_state == State::SWIMMING)
                {
                    _elapseTransTime = 0.0f;
                    _state           = State::SWIMMING_TO_HURT;
                    _mesh->stopAction(_hurt);
                    _mesh->runAction(_hurt);
                    auto delay = DelayTime::create(_hurt->getDuration() - Animate3D::getTransitionTime());
                    auto seq   = Sequence::create(
                          delay, CallFunc::create(AX_CALLBACK_0(Animate3DTest::renewCallBack, this)), nullptr);
                    seq->setTag(101);
                    _mesh->runAction(seq);
                }
                return;
            }
        }
    }
}

AttachmentTest::AttachmentTest() : _hasWeapon(false), _mesh(nullptr)
{
    auto s = Director::getInstance()->getWinSize();
    addNewMeshWithCoords(Vec2(s.width / 2, s.height / 2));

    auto listener            = EventListenerTouchAllAtOnce::create();
    listener->onTouchesEnded = AX_CALLBACK_2(AttachmentTest::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
std::string AttachmentTest::title() const
{
    return "Testing MeshRenderer Attachment";
}
std::string AttachmentTest::subtitle() const
{
    return "touch to switch weapon";
}

void AttachmentTest::addNewMeshWithCoords(Vec2 p)
{
    std::string fileName = "MeshRendererTest/orc.c3b";
    auto mesh          = MeshRenderer::create(fileName);
    mesh->setScale(5);
    mesh->setRotation3D(Vec3(0.0f, 180.0f, 0.0f));
    addChild(mesh);
    mesh->setPosition(Vec2(p.x, p.y));

    // test attach
    auto sp = MeshRenderer::create("MeshRendererTest/axe.c3b");
    mesh->getAttachNode("Bip001 R Hand")->addChild(sp);

    auto animation = Animation3D::create(fileName);
    if (animation)
    {
        auto animate = Animate3D::create(animation);

        mesh->runAction(RepeatForever::create(animate));
    }
    _mesh    = mesh;
    _hasWeapon = true;
}

void AttachmentTest::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    if (_hasWeapon)
    {
        _mesh->removeAllAttachNode();
    }
    else
    {
        auto sp = MeshRenderer::create("MeshRendererTest/axe.c3b");
        _mesh->getAttachNode("Bip001 R Hand")->addChild(sp);
    }
    _hasWeapon = !_hasWeapon;
}

MeshRendererReskinTest::MeshRendererReskinTest() : _mesh(nullptr)
{
    auto s = Director::getInstance()->getWinSize();
    addNewMeshWithCoords(Vec2(s.width / 2, s.height / 2));

    auto listener            = EventListenerTouchAllAtOnce::create();
    listener->onTouchesEnded = AX_CALLBACK_2(MeshRendererReskinTest::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    TTFConfig ttfConfig("fonts/arial.ttf", 20);
    auto label1 = Label::createWithTTF(ttfConfig, "Hair");
    auto item1  = MenuItemLabel::create(label1, AX_CALLBACK_1(MeshRendererReskinTest::menuCallback_reSkin, this));
    auto label2 = Label::createWithTTF(ttfConfig, "Glasses");
    auto item2  = MenuItemLabel::create(label2, AX_CALLBACK_1(MeshRendererReskinTest::menuCallback_reSkin, this));
    auto label3 = Label::createWithTTF(ttfConfig, "Coat");
    auto item3  = MenuItemLabel::create(label3, AX_CALLBACK_1(MeshRendererReskinTest::menuCallback_reSkin, this));
    auto label4 = Label::createWithTTF(ttfConfig, "Pants");
    auto item4  = MenuItemLabel::create(label4, AX_CALLBACK_1(MeshRendererReskinTest::menuCallback_reSkin, this));
    auto label5 = Label::createWithTTF(ttfConfig, "Shoes");
    auto item5  = MenuItemLabel::create(label5, AX_CALLBACK_1(MeshRendererReskinTest::menuCallback_reSkin, this));
    item1->setPosition(Vec2(VisibleRect::left().x + 50, VisibleRect::bottom().y + item1->getContentSize().height * 4));
    item2->setPosition(Vec2(VisibleRect::left().x + 50, VisibleRect::bottom().y + item1->getContentSize().height * 5));
    item3->setPosition(Vec2(VisibleRect::left().x + 50, VisibleRect::bottom().y + item1->getContentSize().height * 6));
    item4->setPosition(Vec2(VisibleRect::left().x + 50, VisibleRect::bottom().y + item1->getContentSize().height * 7));
    item5->setPosition(Vec2(VisibleRect::left().x + 50, VisibleRect::bottom().y + item1->getContentSize().height * 8));
    item1->setUserData((void*)SkinType::HAIR);
    item2->setUserData((void*)SkinType::GLASSES);
    item3->setUserData((void*)SkinType::UPPER_BODY);
    item4->setUserData((void*)SkinType::PANTS);
    item5->setUserData((void*)SkinType::SHOES);
    auto pMenu1 = Menu::create(item1, item2, item3, item4, item5, nullptr);
    pMenu1->setPosition(Vec2(0.0f, 0.0f));
    this->addChild(pMenu1, 10);
}
void MeshRendererReskinTest::menuCallback_reSkin(Object* sender)
{
    auto index = static_cast<int>((uintptr_t)(((MenuItemLabel*)sender)->getUserData()));
    if (index < (int)SkinType::MAX_TYPE)
    {
        _curSkin[index] = (_curSkin[index] + 1) % _skins[index].size();
        applyCurSkin();
    }
}

std::string MeshRendererReskinTest::title() const
{
    return "Testing MeshRenderer Reskin";
}
std::string MeshRendererReskinTest::subtitle() const
{
    return "";
}

void MeshRendererReskinTest::addNewMeshWithCoords(Vec2 p)
{
    std::string fileName = "MeshRendererTest/ReskinGirl.c3b";
    auto mesh          = MeshRenderer::create(fileName);
    mesh->setScale(4);
    mesh->setRotation3D(Vec3(0.0f, 0.0f, 0.0f));
    addChild(mesh);
    mesh->setPosition(Vec2(p.x, p.y - 60));
    auto animation = Animation3D::create(fileName);
    if (animation)
    {
        auto animate = Animate3D::create(animation);

        mesh->runAction(RepeatForever::create(animate));
    }
    _mesh = mesh;

    auto& body = _skins[(int)SkinType::UPPER_BODY];
    body.emplace_back("Girl_UpperBody01");
    body.emplace_back("Girl_UpperBody02");

    auto& pants = _skins[(int)SkinType::PANTS];
    pants.emplace_back("Girl_LowerBody01");
    pants.emplace_back("Girl_LowerBody02");

    auto& shoes = _skins[(int)SkinType::SHOES];
    shoes.emplace_back("Girl_Shoes01");
    shoes.emplace_back("Girl_Shoes02");

    auto& hair = _skins[(int)SkinType::HAIR];
    hair.emplace_back("Girl_Hair01");
    hair.emplace_back("Girl_Hair02");

    auto& face = _skins[(int)SkinType::FACE];
    face.emplace_back("Girl_Face01");
    face.emplace_back("Girl_Face02");

    auto& hand = _skins[(int)SkinType::HAND];
    hand.emplace_back("Girl_Hand01");
    hand.emplace_back("Girl_Hand02");

    auto& glasses = _skins[(int)SkinType::GLASSES];
    glasses.emplace_back("");
    glasses.emplace_back("Girl_Glasses01");

    memset(_curSkin, 0, (int)SkinType::MAX_TYPE * sizeof(int));

    applyCurSkin();
}

void MeshRendererReskinTest::onTouchesEnded(const std::vector<Touch*>& touches, Event* event) {}

void MeshRendererReskinTest::applyCurSkin()
{
    for (ssize_t i = 0; i < _mesh->getMeshCount(); i++)
    {
        auto mesh      = _mesh->getMeshByIndex(static_cast<int>(i));
        bool isVisible = false;
        for (int j = 0; j < (int)SkinType::MAX_TYPE; j++)
        {
            if (mesh->getName() == _skins[j].at(_curSkin[j]))
            {
                isVisible = true;
                break;
            }
        }
        _mesh->getMeshByIndex(static_cast<int>(i))->setVisible(isVisible);
    }
}

MeshRendererWithOBBPerformanceTest::MeshRendererWithOBBPerformanceTest()
{
    auto listener            = EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = AX_CALLBACK_2(MeshRendererWithOBBPerformanceTest::onTouchesBegan, this);
    listener->onTouchesEnded = AX_CALLBACK_2(MeshRendererWithOBBPerformanceTest::onTouchesEnded, this);
    listener->onTouchesMoved = AX_CALLBACK_2(MeshRendererWithOBBPerformanceTest::onTouchesMoved, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    auto s = Director::getInstance()->getWinSize();
    initDrawBox();

    addNewMeshWithCoords(Vec2(s.width / 2, s.height / 2));
    MenuItemFont::setFontName("fonts/arial.ttf");
    MenuItemFont::setFontSize(65);
    auto decrease = MenuItemFont::create(" - ", AX_CALLBACK_1(MeshRendererWithOBBPerformanceTest::delOBBCallback, this));
    decrease->setColor(Color3B(0, 200, 20));
    auto increase = MenuItemFont::create(" + ", AX_CALLBACK_1(MeshRendererWithOBBPerformanceTest::addOBBCallback, this));
    increase->setColor(Color3B(0, 200, 20));

    auto menu = Menu::create(decrease, increase, nullptr);
    menu->alignItemsHorizontally();
    menu->setPosition(Vec2(s.width / 2, s.height - 65));
    addChild(menu, 1);

    TTFConfig ttfCount("fonts/Marker Felt.ttf", 30);
    _labelCubeCount = Label::createWithTTF(ttfCount, "0 cubes");
    _labelCubeCount->setColor(Color3B(0, 200, 20));
    _labelCubeCount->setPosition(Vec2(s.width / 2, s.height - 90));
    addChild(_labelCubeCount);
    _hasCollider = false;
    addOBBCallback(nullptr);
    scheduleUpdate();
}
std::string MeshRendererWithOBBPerformanceTest::title() const
{
    return "OBB Collision Performance Test";
}
std::string MeshRendererWithOBBPerformanceTest::subtitle() const
{
    return "";
}
void MeshRendererWithOBBPerformanceTest::addNewOBBWithCoords(Vec2 p)
{
    Vec3 extents = Vec3(10, 10, 10);
    AABB aabb(-extents, extents);
    auto obb    = OBB(aabb);
    obb._center = Vec3(p.x, p.y, 0);
    _obb.emplace_back(obb);
}

void MeshRendererWithOBBPerformanceTest::onTouchesBegan(const std::vector<Touch*>& touches, Event* event)
{
    for (const auto& touch : touches)
    {
        auto location = touch->getLocationInView();
        auto obbSize  = _obb.size();
        if (obbSize)
        {
            _intersetList.clear();
            Ray ray;
            calculateRayByLocationInView(&ray, location);
            for (decltype(obbSize) i = 0; i < obbSize; i++)
            {
                if (ray.intersects(_obb[i]))
                {
                    _intersetList.insert((int)i);
                    return;
                }
            }
        }
    }
}

void MeshRendererWithOBBPerformanceTest::onTouchesEnded(const std::vector<Touch*>& touches, Event* event) {}

void MeshRendererWithOBBPerformanceTest::onTouchesMoved(const std::vector<Touch*>& touches, Event* event)
{
    for (const auto& touch : touches)
    {
        auto location = touch->getLocation();
        auto obbSize  = _obb.size();

        for (decltype(obbSize) i = 0; i < obbSize; i++)
        {
            if (_intersetList.find((int)i) != _intersetList.end())
                _obb[i]._center = Vec3(location.x, location.y, 0);
        }
    }
}

void MeshRendererWithOBBPerformanceTest::update(float dt)
{
    char szText[16];
    sprintf(szText, "%lu cubes", static_cast<unsigned long>(_obb.size()));
    _labelCubeCount->setString(szText);

    if (_drawDebug)
    {
        _drawDebug->clear();

        Mat4 mat = _mesh->getNodeToWorldTransform();

        _obbt = _obbtOri;
        _obbt.transform(mat);

        Vec3 corners[8] = {};
        _obbt.getCorners(corners);
        _drawDebug->drawCube(corners, Color4F(0, 0, 1, 1));
    }
    if (_obb.size() > 0)
    {
        _drawOBB->clear();
        auto obbSize = _obb.size();
        for (decltype(obbSize) i = 0; i < obbSize; i++)
        {
            Vec3 corners[8] = {};
            _obb[i].getCorners(corners);
            _drawOBB->drawCube(corners, _obbt.intersects(_obb[i]) ? Color4F(1, 0, 0, 1) : Color4F(0, 1, 0, 1));
        }
    }
}

void MeshRendererWithOBBPerformanceTest::initDrawBox()
{
    _drawOBB = DrawNode3D::create();
    addChild(_drawOBB);
}

void MeshRendererWithOBBPerformanceTest::addNewMeshWithCoords(Vec2 p)
{
    std::string fileName = "MeshRendererTest/tortoise.c3b";
    auto mesh          = MeshRenderer::create(fileName);
    AABB aabb            = mesh->getAABB();
    _obbt                = OBB(aabb);
    _obbtOri             = _obbt;

    mesh->setScale(0.1f);
    auto s = Director::getInstance()->getWinSize();
    mesh->setPosition(Vec2(s.width * 4.f / 5.f, s.height / 2.f));
    addChild(mesh);
    _mesh        = mesh;
    auto animation = Animation3D::create(fileName);
    if (animation)
    {
        auto animate = Animate3D::create(animation, 0.f, 1.933f);
        mesh->runAction(RepeatForever::create(animate));
    }

    _moveAction = MoveTo::create(4.f, Vec2(s.width / 5.f, s.height / 2.f));
    _moveAction->retain();
    auto seq = Sequence::create(
        _moveAction, CallFunc::create(AX_CALLBACK_0(MeshRendererWithOBBPerformanceTest::reachEndCallBack, this)), nullptr);
    seq->setTag(100);
    mesh->runAction(seq);

    _drawDebug = DrawNode3D::create();
    addChild(_drawDebug);
}

void MeshRendererWithOBBPerformanceTest::reachEndCallBack()
{
    auto s = Director::getInstance()->getWinSize();
    _mesh->stopActionByTag(100);
    auto inverse = MoveTo::create(4.f, Vec2(s.width - _mesh->getPositionX(), s.height / 2.f));
    inverse->retain();
    _moveAction->release();
    _moveAction = inverse;
    auto rot    = RotateBy::create(1.0f, Vec3(0.f, 180.f, 0.f));
    auto seq    = Sequence::create(rot, _moveAction,
                                   CallFunc::create(AX_CALLBACK_0(MeshRendererWithOBBPerformanceTest::reachEndCallBack, this)),
                                   nullptr);
    seq->setTag(100);
    _mesh->runAction(seq);
}

void MeshRendererWithOBBPerformanceTest::addOBBCallback(Object* sender)
{
    addOBBWithCount(10);
}

void MeshRendererWithOBBPerformanceTest::addOBBWithCount(float value)
{
    for (int i = 0; i < value; i++)
    {
        Vec2 randompos = Vec2(AXRANDOM_0_1() * Director::getInstance()->getWinSize().width,
                              AXRANDOM_0_1() * Director::getInstance()->getWinSize().height);
        Vec3 extents   = Vec3(10, 10, 10);
        AABB aabb(-extents, extents);
        auto obb    = OBB(aabb);
        obb._center = Vec3(randompos.x, randompos.y, 0);
        _obb.emplace_back(obb);
    }
}

void MeshRendererWithOBBPerformanceTest::delOBBCallback(Object* sender)
{
    delOBBWithCount(10);
}

void MeshRendererWithOBBPerformanceTest::delOBBWithCount(float value)
{
    if (_obb.size() >= 10)
    {
        _obb.erase(_obb.begin(), _obb.begin() + value);
        _drawOBB->clear();
    }
    else
        return;
}
void MeshRendererWithOBBPerformanceTest::unproject(const Mat4& viewProjection, const Size* viewport, Vec3* src, Vec3* dst)
{
    assert(dst);

    assert(viewport->width != 0.0f && viewport->height != 0.0f);
    Vec4 screen(src->x / viewport->width, ((viewport->height - src->y)) / viewport->height, src->z, 1.0f);

    screen.x = screen.x * 2.0f - 1.0f;
    screen.y = screen.y * 2.0f - 1.0f;
    screen.z = screen.z * 2.0f - 1.0f;

    viewProjection.getInversed().transformVector(screen, &screen);

    if (screen.w != 0.0f)
    {
        screen.x /= screen.w;
        screen.y /= screen.w;
        screen.z /= screen.w;
    }

    dst->set(screen.x, screen.y, screen.z);
}

void MeshRendererWithOBBPerformanceTest::calculateRayByLocationInView(Ray* ray, const Vec2& location)
{
    auto dir  = Director::getInstance();
    auto view = dir->getWinSize();
    auto mat  = dir->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);

    Vec3 src = Vec3(location.x, location.y, -1);
    Vec3 nearPoint;
    unproject(mat, &view, &src, &nearPoint);

    src = Vec3(location.x, location.y, 1);
    Vec3 farPoint;
    unproject(mat, &view, &src, &farPoint);

    Vec3 direction;
    Vec3::subtract(farPoint, nearPoint, &direction);
    direction.normalize();

    ray->_origin    = nearPoint;
    ray->_direction = direction;
}

MeshRendererMirrorTest::MeshRendererMirrorTest() : _mesh(nullptr), _mirrorMesh(nullptr)
{
    auto s = Director::getInstance()->getWinSize();
    addNewMeshWithCoords(Vec2(s.width / 2, s.height / 2));
}
std::string MeshRendererMirrorTest::title() const
{
    return "MeshRenderer Mirror Test";
}
std::string MeshRendererMirrorTest::subtitle() const
{
    return "";
}

void MeshRendererMirrorTest::addNewMeshWithCoords(Vec2 p)
{
    std::string fileName = "MeshRendererTest/orc.c3b";
    auto mesh          = MeshRenderer::create(fileName);
    mesh->setScale(5);
    mesh->setRotation3D(Vec3(0.0f, 180.0f, 0.0f));
    addChild(mesh);
    mesh->setPosition(Vec2(p.x - 80, p.y));

    // test attach
    auto sp = MeshRenderer::create("MeshRendererTest/axe.c3b");
    mesh->getAttachNode("Bip001 R Hand")->addChild(sp);

    auto animation = Animation3D::create(fileName);
    if (animation)
    {
        auto animate = Animate3D::create(animation);

        mesh->runAction(RepeatForever::create(animate));
    }
    _mesh    = mesh;
    _hasWeapon = true;

    // create mirror MeshRenderer
    mesh = MeshRenderer::create(fileName);
    mesh->setScale(5);
    mesh->setScaleX(-5);
    mesh->setCullFace(CullFaceSide::FRONT);
    mesh->setRotation3D(Vec3(0.0f, 180.0f, 0.0f));
    addChild(mesh);
    mesh->setPosition(Vec2(p.x + 80, p.y));

    // test attach
    sp = MeshRenderer::create("MeshRendererTest/axe.c3b");
    mesh->getAttachNode("Bip001 R Hand")->addChild(sp);

    animation = Animation3D::create(fileName);
    if (animation)
    {
        auto animate = Animate3D::create(animation);

        mesh->runAction(RepeatForever::create(animate));
    }
    _mirrorMesh = mesh;
}

QuaternionTest::QuaternionTest() : _arcSpeed(AX_DEGREES_TO_RADIANS(90)), _radius(100.f), _accAngle(0.f)
{
    auto s = Director::getInstance()->getWinSize();
    addNewMeshWithCoords(Vec2(s.width / 2.f, s.height / 2.f));
    scheduleUpdate();
}
std::string QuaternionTest::title() const
{
    return "Test Rotation With Quaternion";
}
std::string QuaternionTest::subtitle() const
{
    return "";
}

void QuaternionTest::addNewMeshWithCoords(Vec2 p)
{
    std::string fileName = "MeshRendererTest/tortoise.c3b";
    auto mesh          = MeshRenderer::create(fileName);
    mesh->setScale(0.1f);
    auto s = Director::getInstance()->getWinSize();
    mesh->setPosition(Vec2(s.width / 2.f + _radius * cosf(_accAngle), s.height / 2.f + _radius * sinf(_accAngle)));
    addChild(mesh);
    _mesh        = mesh;
    auto animation = Animation3D::create(fileName);
    if (animation)
    {
        auto animate = Animate3D::create(animation, 0.f, 1.933f);
        mesh->runAction(RepeatForever::create(animate));
    }
}

void QuaternionTest::update(float delta)
{
    _accAngle += delta * _arcSpeed;
    const float pi = (float)M_PI;
    if (_accAngle >= 2 * pi)
        _accAngle -= 2 * pi;

    auto s = Director::getInstance()->getWinSize();
    _mesh->setPosition(Vec2(s.width / 2.f + _radius * cosf(_accAngle), s.height / 2.f + _radius * sinf(_accAngle)));

    Quaternion quat;
    Quaternion::createFromAxisAngle(Vec3(0.f, 0.f, 1.f), _accAngle - pi * 0.5f, &quat);
    _mesh->setRotationQuat(quat);
}

UseCaseMeshRenderer::UseCaseMeshRenderer() : _caseIdx(0)
{
    auto s = Director::getInstance()->getWinSize();

    _useCaseTitles[0] = "transparent 3d mesh and 2d mesh";
    _useCaseTitles[1] = "ui - 3d - ui";

    auto itemPrev = MenuItemImage::create("Images/b1.png", "Images/b2.png", [&](Object* sender) {
        _caseIdx--;
        if (_caseIdx < 0)
            _caseIdx = (int)USECASE::MAX_CASE_NUM - 1;
        this->switchCase();
    });

    auto itemNext = MenuItemImage::create("Images/f1.png", "Images/f2.png", [&](Object* sender) {
        _caseIdx++;
        if (_caseIdx >= (int)USECASE::MAX_CASE_NUM)
            _caseIdx = 0;
        this->switchCase();
    });

    auto menu = Menu::create(itemPrev, itemNext, nullptr);
    menu->alignItemsHorizontally();
    menu->setScale(0.5);
    menu->setAnchorPoint(Vec2(0.0f, 0.0f));
    menu->setPosition(Vec2(s.width / 2, 70.0f));

    _label = Label::create();
    _label->setPosition(s.width * 0.5f, s.height * 0.8f);
    addChild(_label);

    addChild(menu);

    // setup camera
    auto camera = Camera::createPerspective(40, s.width / s.height, 0.01f, 1000.f);
    camera->setCameraFlag(CameraFlag::USER1);
    camera->setPosition3D(Vec3(0.f, 30.f, 100.f));
    camera->lookAt(Vec3(0.f, 0.f, 0.f));
    addChild(camera);

    switchCase();
}

std::string UseCaseMeshRenderer::title() const
{
    return "Use Case For 2D + 3D";
}

std::string UseCaseMeshRenderer::subtitle() const
{
    return "";
}

void UseCaseMeshRenderer::switchCase()
{
    removeChildByTag(101);

    auto s = Director::getInstance()->getWinSize();
    _label->setString(_useCaseTitles[_caseIdx]);
    if (_caseIdx == 0)  // use case 1, 3d transparent mesh + 2d mesh
    {
        std::string filename = "MeshRendererTest/girl.c3b";
        auto mesh          = MeshRenderer::create(filename);
        mesh->setScale(0.15f);
        auto animation = Animation3D::create(filename);
        if (animation)
        {
            auto animate = Animate3D::create(animation);
            mesh->runAction(RepeatForever::create(animate));
        }

        auto circleBack = MeshRenderer::create();
        auto circle     = Sprite::create("MeshRendererTest/circle.png");
        circleBack->setScale(0.5f);
        circleBack->addChild(circle);
        circle->runAction(RepeatForever::create(RotateBy::create(3, Vec3(0.f, 0.f, 360.f))));

        circleBack->setRotation3D(Vec3(-90.0f, -90.0f, 0.0f));

        auto pos = mesh->getPosition3D();
        circleBack->setPosition3D(Vec3(pos.x, pos.y, pos.z - 1));

        mesh->setOpacity(250);
        mesh->setCameraMask(2);
        circleBack->setCameraMask(2);
        mesh->setTag(3);
        circleBack->setTag(2);

        auto node = Node::create();
        node->addChild(mesh);
        node->addChild(circleBack);
        node->setTag(101);
        addChild(node);

        scheduleUpdate();
        update(0.f);
    }
    else if (_caseIdx == 1)  // use case 2, ui - 3d - ui, last ui should on the top
    {
        auto layer = LayerColor::create(Color4B(0, 0, 100, 255), s.width / 2.f, s.height / 2.f);
        layer->setPosition(s.width * 0.25f, s.height * 0.25f);
        layer->setGlobalZOrder(-1);
        addChild(layer);

        std::string filename = "MeshRendererTest/girl.c3b";
        auto mesh          = MeshRenderer::create(filename);
        mesh->setScale(0.5f);
        auto animation = Animation3D::create(filename);
        if (animation)
        {
            auto animate = Animate3D::create(animation);
            mesh->runAction(RepeatForever::create(animate));
        }
        mesh->setPosition(s.width * 0.25f, s.height * 0.125f);
        layer->addChild(mesh);

        TTFConfig ttfConfig("fonts/arial.ttf", 15);
        auto label1 = Label::createWithTTF(ttfConfig, "Message");
        auto item1  = MenuItemLabel::create(label1, AX_CALLBACK_1(UseCaseMeshRenderer::menuCallback_Message, this));
        auto label2 = Label::createWithTTF(ttfConfig, "Message");
        auto item2  = MenuItemLabel::create(label2, AX_CALLBACK_1(UseCaseMeshRenderer::menuCallback_Message, this));

        item1->setPosition(Vec2(s.width * 0.5f - item1->getContentSize().width * 0.5f,
                                s.height * 0.5f - item1->getContentSize().height));
        item2->setPosition(Vec2(s.width * 0.5f - item1->getContentSize().width * 0.5f,
                                s.height * 0.5f - item1->getContentSize().height * 2.f));

        auto pMenu1 = Menu::create(item1, item2, nullptr);
        pMenu1->setPosition(Vec2(0.0f, 0.0f));
        layer->addChild(pMenu1);

        layer->setTag(101);
    }
}

void UseCaseMeshRenderer::menuCallback_Message(Object* sender)
{
    auto layer   = getChildByTag(101);
    auto message = layer->getChildByTag(102);  // message layer
    if (message)
        layer->removeChild(message);
    else
    {
        // create a new message layer on the top
        auto s            = layer->getContentSize();
        auto messagelayer = LayerColor::create(Color4B(100, 100, 0, 255));
        messagelayer->setContentSize(Size(s.width * 0.5f, s.height * 0.5f));
        messagelayer->setPosition(Vec2(s.width * 0.25f, s.height * 0.25f));
        auto label = Label::create();
        label->setString("This Message Layer \n Should Be On Top");
        label->setPosition(Vec2(s.width * 0.25f, s.height * 0.25f));
        messagelayer->addChild(label);
        messagelayer->setTag(102);
        layer->addChild(messagelayer);
    }
}

void UseCaseMeshRenderer::update(float delta)
{
    if (_caseIdx == 0)
    {
        static float accAngle = 0.f;
        accAngle += delta * AX_DEGREES_TO_RADIANS(60);

        float radius = 30.f;
        float x = cosf(accAngle) * radius, z = sinf(accAngle) * radius;

        auto node     = getChildByTag(101);
        auto mesh3d = node->getChildByTag(3);
        auto circle   = node->getChildByTag(2);

        mesh3d->setPositionX(x);
        mesh3d->setPositionZ(z);
        circle->setPositionX(x);
        circle->setPositionZ(z);
    }
}

/////////////////////////////////////////////
// Node Frame Animation
NodeAnimationTest::NodeAnimationTest() : _vectorIndex(0)
{
    auto s = Director::getInstance()->getWinSize();

    auto itemPrev = MenuItemImage::create("Images/b1.png", "Images/b2.png", [&](Object* sender) {
        _meshes[_vectorIndex]->setVisible(false);

        int tIndex = _vectorIndex - 1;
        if (tIndex < 0)
            _vectorIndex = (int)_meshes.size() - 1;
        else
            _vectorIndex--;

        _meshes[_vectorIndex]->setVisible(true);
    });

    auto itemNext = MenuItemImage::create("Images/f1.png", "Images/f2.png", [&](Object* sender) {
        _meshes[_vectorIndex]->setVisible(false);

        int tIndex = _vectorIndex + 1;
        if (tIndex >= static_cast<int>(_meshes.size()))
            _vectorIndex = 0;
        else
            _vectorIndex++;

        _meshes[_vectorIndex]->setVisible(true);
    });

    auto menu = Menu::create(itemPrev, itemNext, nullptr);
    menu->alignItemsHorizontally();
    menu->setScale(0.5f);
    menu->setAnchorPoint(Vec2(0.0f, 0.0f));
    menu->setPosition(Vec2(s.width / 2, 70.0f));
    addChild(menu);

    addNewMeshWithCoords(Vec2(s.width / 2.f, s.height / 2.f));
}
std::string NodeAnimationTest::title() const
{
    return "Node Animation Test";
}
std::string NodeAnimationTest::subtitle() const
{
    return "Jumping animation";
}

void NodeAnimationTest::addNewMeshWithCoords(Vec2 p)
{
    auto s = Director::getInstance()->getWinSize();

    // add jumping ball
    std::string fileName = "MeshRendererTest/ball.c3b";
    auto mesh          = MeshRenderer::create(fileName);
    mesh->setRotation3D(Vec3(0.0f, 180.0f, 0.0f));
    mesh->setScale(3);
    mesh->setPosition(Vec2(s.width / 2.f, s.height / 3.f));
    mesh->setTexture("MeshRendererTest/teapot.png");

    auto light1 = PointLight::create(Vec3(s.width * 0.2f, s.height * 0.8f, 100.0f), Color3B(200, 200, 200), 10000.0f);
    addChild(light1);
    auto light2 = AmbientLight::create(Color3B(100, 100, 100));
    addChild(light2);

    auto animation = Animation3D::create(fileName);
    if (animation)
    {
        auto animate = Animate3D::create(animation);
        auto act     = RepeatForever::create(animate);
        act->setTag(0);
        mesh->runAction(act);
    }
    addChild(mesh);
    _meshes.emplace_back(mesh);

    // add jumping orc
    fileName = "MeshRendererTest/orc_jump.c3t";
    mesh   = MeshRenderer::create(fileName);
    mesh->setRotation3D(Vec3(0.0f, 180.0f, 0.0f));
    mesh->setScale(3);
    mesh->setPosition(Vec2(s.width / 2.f, s.height / 3.f));
    mesh->setVisible(false);
    animation = Animation3D::create(fileName);
    if (animation)
    {
        auto animate = Animate3D::create(animation);
        auto act     = RepeatForever::create(animate);
        act->setTag(0);
        mesh->runAction(act);
    }
    addChild(mesh);
    _meshes.emplace_back(mesh);
}

MeshRendererCubeMapTest::MeshRendererCubeMapTest() : _textureCube(nullptr), _skyBox(nullptr), _teapot(nullptr)
{
    auto s = Director::getInstance()->getWinSize();
    addNewMeshWithCoords(Vec2(s.width / 2, s.height / 2));
}

MeshRendererCubeMapTest::~MeshRendererCubeMapTest()
{
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundListener);
#endif

    _teapot->release();
    _skyBox->release();
    _textureCube->release();
}

std::string MeshRendererCubeMapTest::title() const
{
    return "CubeMap & Skybox Test";
}

std::string MeshRendererCubeMapTest::subtitle() const
{
    return "";
}

void MeshRendererCubeMapTest::addNewMeshWithCoords(Vec2 p)
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    _camera          = Camera::createPerspective(60, visibleSize.width / visibleSize.height, 10, 1000);
    _camera->setPosition3D(Vec3(0.f, 0.f, 50.f));
    _camera->setCameraFlag(CameraFlag::USER1);

    auto listener            = EventListenerTouchAllAtOnce::create();
    listener->onTouchesMoved = AX_CALLBACK_2(MeshRendererCubeMapTest::onTouchesMoved, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    // create a teapot
    _teapot = MeshRenderer::create("MeshRendererTest/teapot.c3b");
    _teapot->retain();

    // create the second texture for cylinder
    _textureCube = TextureCube::create("MeshRendererTest/skybox/left.jpg", "MeshRendererTest/skybox/right.jpg",
                                       "MeshRendererTest/skybox/top.jpg", "MeshRendererTest/skybox/bottom.jpg",
                                       "MeshRendererTest/skybox/front.jpg", "MeshRendererTest/skybox/back.jpg");

    _textureCube->retain();

    // set texture parameters
    Texture2D::TexParams tRepeatParams;
    tRepeatParams.magFilter    = backend::SamplerFilter::LINEAR;
    tRepeatParams.minFilter    = backend::SamplerFilter::LINEAR;
    tRepeatParams.sAddressMode = backend::SamplerAddressMode::CLAMP_TO_EDGE;
    tRepeatParams.tAddressMode = backend::SamplerAddressMode::CLAMP_TO_EDGE;
    _textureCube->setTexParameters(tRepeatParams);

    auto mat   = MeshMaterial::createWithFilename("MeshRendererTest/CubeMap.material");
    auto state = mat->getTechniqueByIndex(0)->getPassByIndex(0)->getProgramState();
    _teapot->setMaterial(mat);

    // pass the texture sampler to our custom shader
    auto cubeTexLoc = state->getUniformLocation("u_cubeTex");
    state->setTexture(cubeTexLoc, 0, _textureCube->getBackendTexture());

    _teapot->setPosition3D(Vec3(0.0f, -5.0f, 0.0f));
    _teapot->setRotation3D(Vec3(-90.0f, 180.0f, 0.0f));

    auto rotate_action = RotateBy::create(1.5, Vec3(0.0f, 30.0f, 0.0f));
    _teapot->runAction(RepeatForever::create(rotate_action));

    addChild(_teapot);

    {
        // config skybox
        _skyBox = Skybox::create();
        _skyBox->retain();

        _skyBox->setTexture(_textureCube);
        addChild(_skyBox);
    }

    addChild(_camera);
    setCameraMask(2);
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    _backToForegroundListener = EventListenerCustom::create(EVENT_COME_TO_FOREGROUND, [this](EventCustom*) {
        AX_SAFE_RELEASE(_textureCube);
        _textureCube = TextureCube::create("MeshRendererTest/skybox/left.jpg", "MeshRendererTest/skybox/right.jpg",
                                           "MeshRendererTest/skybox/top.jpg", "MeshRendererTest/skybox/bottom.jpg",
                                           "MeshRendererTest/skybox/front.jpg", "MeshRendererTest/skybox/back.jpg");

        _textureCube->retain();
        // set texture parameters
        Texture2D::TexParams tRepeatParams;
        tRepeatParams.magFilter    = backend::SamplerFilter::LINEAR;
        tRepeatParams.minFilter    = backend::SamplerFilter::LINEAR;
        tRepeatParams.sAddressMode = backend::SamplerAddressMode::CLAMP_TO_EDGE;
        tRepeatParams.tAddressMode = backend::SamplerAddressMode::CLAMP_TO_EDGE;
        _textureCube->setTexParameters(tRepeatParams);

        auto mat   = MeshMaterial::createWithFilename("MeshRendererTest/CubeMap.material");
        auto state = mat->getTechniqueByIndex(0)->getPassByIndex(0)->getProgramState();
        _teapot->setMaterial(mat);
        _skyBox->setTexture(_textureCube);
        // pass the texture sampler to our custom shader
        auto cubeTexLoc = state->getUniformLocation("u_cubeTex");
        state->setTexture(cubeTexLoc, 0, _textureCube->getBackendTexture());
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, 1);
#endif
}

void MeshRendererCubeMapTest::onTouchesMoved(const std::vector<Touch*>& touches, ax::Event* event)
{
    if (touches.size())
    {
        auto touch = touches[0];
        auto delta = touch->getDelta();

        static float _angle = 0.f;
        _angle -= AX_DEGREES_TO_RADIANS(delta.x);
        _camera->setPosition3D(Vec3(50.0f * sinf(_angle), 0.0f, 50.0f * cosf(_angle)));
        _camera->lookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
    }
}

Issue9767::Issue9767()
{
    _shaderType = Issue9767::ShaderType::SHADER_TEX;

    auto s      = Director::getInstance()->getWinSize();
    auto mesh = MeshRenderer::create("MeshRendererTest/boss1.obj");
    mesh->setScale(3.f);
    mesh->setTexture("MeshRendererTest/boss.png");
    addChild(mesh);
    mesh->setPosition(Vec2(s.width / 2, s.height / 2));
    _mesh = mesh;

    TTFConfig ttfConfig("fonts/arial.ttf", 15);
    auto label1 = Label::createWithTTF(ttfConfig, "switch shader");
    auto item1  = MenuItemLabel::create(label1, AX_CALLBACK_1(Issue9767::menuCallback_SwitchShader, this));

    item1->setPosition(
        Vec2(s.width * 0.9f - item1->getContentSize().width * 0.5f, s.height * 0.5f - item1->getContentSize().height));

    auto pMenu1 = Menu::create(item1, nullptr);
    pMenu1->setPosition(Vec2(0.0f, 0.0f));
    addChild(pMenu1);
}

Issue9767::~Issue9767() {}

void Issue9767::menuCallback_SwitchShader(ax::Object* sender)
{
    AX_SAFE_RELEASE_NULL(_programState);
    if (_shaderType == Issue9767::ShaderType::SHADER_TEX)
    {
        _shaderType   = Issue9767::ShaderType::SHADER_COLOR;
        auto program  = backend::Program::getBuiltinProgram(backend::ProgramType::POSITION_3D);
        _programState = new backend::ProgramState(program);
    }
    else
    {
        _shaderType   = Issue9767::ShaderType::SHADER_TEX;
        auto program  = backend::Program::getBuiltinProgram(backend::ProgramType::POSITION_TEXTURE_3D);
        _programState = new backend::ProgramState(program);
    }
    _mesh->setProgramState(_programState);
}

std::string Issue9767::title() const
{
    return "Issue9767: test setGLProgram";
}

std::string Issue9767::subtitle() const
{
    return "";
}

MeshRendererClippingTest::MeshRendererClippingTest()
{
    auto size         = Director::getInstance()->getWinSize();
    auto stencil      = Sprite::create("Images/close.png");
    auto clipMeshRenderer = ClippingNode::create();
    clipMeshRenderer->setStencil(stencil);
    this->addChild(clipMeshRenderer);
    clipMeshRenderer->setScale(3.0f);

    auto mesh3D = MeshRenderer::create("MeshRendererTest/orc.c3b");
    mesh3D->setScale(1.0f);
    mesh3D->setRotation3D(Vec3(0.0f, 180.0f, 0.0f));
    clipMeshRenderer->addChild(mesh3D);  // 5

    clipMeshRenderer->setPosition(Vec2(size.width / 2, size.height / 2));

    auto seq = Sequence::create(ScaleTo::create(2.f, 3), ScaleTo::create(2.f, 0.5f), NULL);
    mesh3D->runAction(RepeatForever::create(seq));
    auto animation = Animation3D::create("MeshRendererTest/orc.c3b");
    auto animate   = Animate3D::create(animation);
    mesh3D->runAction(RepeatForever::create(animate));
    mesh3D->getMaterial()->setForce2DQueue(true);
}

MeshRendererClippingTest::~MeshRendererClippingTest() {}

std::string MeshRendererClippingTest::title() const
{
    return "MeshRenderer Clipping Test";
}

std::string MeshRendererClippingTest::subtitle() const
{
    return "";
}

Animate3DCallbackTest::Animate3DCallbackTest()
{
    FileUtils::getInstance()->addSearchPath("Particle3D/materials");
    FileUtils::getInstance()->addSearchPath("Particle3D/scripts");

    auto s    = Director::getInstance()->getWinSize();
    _meshRenderer = MeshRenderer::create("MeshRendererTest/ReskinGirl.c3b");
    _meshRenderer->setPosition(Vec2(s.width / 2.0f, s.height / 3.0f));
    _meshRenderer->setScale(3.0f);
    _meshRenderer->setRotation3D(Vec3(0.0f, 90.0f, 0.0f));
    this->addChild(_meshRenderer);

    _meshRenderer->getMeshByName("Girl_UpperBody02")->setVisible(false);
    _meshRenderer->getMeshByName("Girl_LowerBody02")->setVisible(false);
    _meshRenderer->getMeshByName("Girl_Shoes02")->setVisible(false);
    _meshRenderer->getMeshByName("Girl_Hair02")->setVisible(false);

    auto rootps = PUParticleSystem3D::create("explosionSystem.pu");
    rootps->stopParticleSystem();
    rootps->setScale(4.0f);
    this->addChild(rootps, 0, 100);

    auto animation = Animation3D::create("MeshRendererTest/ReskinGirl.c3b");
    if (animation)
    {
        auto animate = Animate3D::create(animation);
        _meshRenderer->runAction(RepeatForever::create(animate));

        ValueMap valuemap0;
        animate->setKeyFrameUserInfo(275, valuemap0);

        auto listener = EventListenerCustom::create(Animate3DDisplayedNotification, [&](EventCustom* event) {
            auto info = (Animate3D::Animate3DDisplayedEventInfo*)event->getUserData();
            auto node = getChildByTag(100);
            if (node)
            {
                auto mat = _meshRenderer->getNodeToWorldTransform() *
                           _meshRenderer->getSkeleton()->getBoneByName("Bip01 R Hand")->getWorldMat();
                node->setPosition3D(Vec3(mat.m[12] + 100, mat.m[13], mat.m[14]));
                ((PUParticleSystem3D*)node)->startParticleSystem();
            }

            AXLOGD("frame {}", info->frame);
        });
        Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, -1);
    }
}

Animate3DCallbackTest::~Animate3DCallbackTest() {}

std::string Animate3DCallbackTest::title() const
{
    return "Testing Animate3D Callback";
}

std::string Animate3DCallbackTest::subtitle() const
{
    return "";
}

MeshRendererVertexColorTest::MeshRendererVertexColorTest()
{
    auto s = Director::getInstance()->getWinSize();

    auto mesh = MeshRenderer::create("MeshRendererTest/box_VertexCol.c3t");
    mesh->setPosition(Vec2(0.0f, 0.0f));
    mesh->setScale(1.0f);
    mesh->setCameraMask(2);
    auto mat = MeshMaterial::createWithFilename("MeshRendererTest/VertexColor.material");
    mesh->setMaterial(mat);
    mesh->runAction(RepeatForever::create(RotateBy::create(1.0f, Vec3(10.0f, 50.0f, 10.0f))));

    this->addChild(mesh);

    // setup camera
    auto camera = Camera::createPerspective(40, s.width / s.height, 0.01f, 1000.f);
    camera->setCameraFlag(CameraFlag::USER1);
    camera->setPosition3D(Vec3(0.0f, 0.0f, 10.f));
    camera->lookAt(Vec3(0.f, 0.f, 0.f));
    addChild(camera);

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    _backToForegroundListener = EventListenerCustom::create(EVENT_COME_TO_FOREGROUND, [=](EventCustom*) {
        auto mat = MeshMaterial::createWithFilename("MeshRendererTest/VertexColor.material");
        mesh->setMaterial(mat);
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, 1);
#endif
}

MeshRendererVertexColorTest::~MeshRendererVertexColorTest()
{
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundListener);
#endif
}

std::string MeshRendererVertexColorTest::title() const
{
    return "Testing Vertex Color";
}

std::string MeshRendererVertexColorTest::subtitle() const
{
    return "";
}

CameraBackgroundClearTest::CameraBackgroundClearTest()
{
    TTFConfig ttfConfig("fonts/arial.ttf", 20);
    auto label1 = Label::createWithTTF(ttfConfig, "Clear Mode");
    auto item1  = MenuItemLabel::create(label1, AX_CALLBACK_1(CameraBackgroundClearTest::switch_CameraClearMode, this));

    item1->setPosition(Vec2(VisibleRect::left().x + 50, VisibleRect::bottom().y + item1->getContentSize().height * 4));

    auto pMenu1 = Menu::create(item1, nullptr);
    pMenu1->setPosition(Vec2(0.0f, 0.0f));
    this->addChild(pMenu1, 10);

    // setup camera
    auto s  = Director::getInstance()->getWinSize();
    _camera = Camera::createPerspective(40, s.width / s.height, 0.01f, 1000.f);
    _camera->setCameraFlag(CameraFlag::USER1);
    _camera->setPosition3D(Vec3(0.f, 30.f, 100.f));
    _camera->lookAt(Vec3(0.f, 0.f, 0.f));
    addChild(_camera);

    auto mesh = MeshRenderer::create("MeshRendererTest/orc.c3b");
    addChild(mesh);
    mesh->setCameraMask(2);

    _label = Label::createWithTTF(ttfConfig, "Depth Clear Brush");
    addChild(_label);
    _label->setPosition(s.width / 2.f, VisibleRect::top().y * 0.8f);
}

void CameraBackgroundClearTest::switch_CameraClearMode(ax::Object* sender)
{
    auto brush                            = _camera->getBackgroundBrush();
    CameraBackgroundBrush::BrushType type = CameraBackgroundBrush::BrushType::NONE;
    if (!brush)
    {
        AXLOGD("No brash found!");
    }
    else
    {
        type = brush->getBrushType();
    }

    if (type == CameraBackgroundBrush::BrushType::NONE)
    {
        _camera->setBackgroundBrush(CameraBackgroundBrush::createDepthBrush(1.f));
        _label->setString("Depth Clear Brush");
        // Test brush valid when set by user scene setting
        AXLOGD("Background brush valid status is : {}", _camera->isBrushValid() ? "true" : "false");
    }
    else if (type == CameraBackgroundBrush::BrushType::DEPTH)
    {
        _camera->setBackgroundBrush(CameraBackgroundBrush::createColorBrush(Color4F(1.f, 0.f, 0.f, 1.f), 1.f));
        _label->setString("Color Clear Brush");
    }
    else if (type == CameraBackgroundBrush::BrushType::COLOR)
    {
        _camera->setBackgroundBrush(CameraBackgroundBrush::createSkyboxBrush(
            "MeshRendererTest/skybox/left.jpg", "MeshRendererTest/skybox/right.jpg", "MeshRendererTest/skybox/top.jpg",
            "MeshRendererTest/skybox/bottom.jpg", "MeshRendererTest/skybox/front.jpg", "MeshRendererTest/skybox/back.jpg"));
        _label->setString("Skybox Clear Brush");
    }
    else if (type == CameraBackgroundBrush::BrushType::SKYBOX)
    {
        _camera->setBackgroundBrush(CameraBackgroundBrush::createNoneBrush());
        _label->setString("None Clear Brush");
    }
}

std::string CameraBackgroundClearTest::title() const
{
    return "Camera Background Clear Brush";
}

std::string CameraBackgroundClearTest::subtitle() const
{
    return "";
}

MotionStreak3DTest::MotionStreak3DTest()
{
    auto s = Director::getInstance()->getWinSize();

    auto camera = Camera::createPerspective(40, s.width / s.height, 0.01f, 1000.f);
    camera->setCameraFlag(CameraFlag::USER1);
    camera->setPosition3D(Vec3(0.f, 50.f, 200.f));
    camera->lookAt(Vec3(0.f, 0.f, 0.f));
    addChild(camera);

    auto mesh = MeshRenderer::create("MeshRendererTest/orc.c3b");
    mesh->setPosition(20.f, 0.f);
    addChild(mesh);

    auto streak = MotionStreak3D::create(1.0f, 1.0f, 5.f, Color3B(255, 255, 0), "Images/Icon.png");
    addChild(streak);

    setCameraMask(2);

    _mesh = mesh;
    _streak = streak;
    scheduleUpdate();
}
std::string MotionStreak3DTest::title() const
{
    return "MotionStreak3D Test";
}
std::string MotionStreak3DTest::subtitle() const
{
    return "";
}

void MotionStreak3DTest::update(float delta)
{
    static float t = 0;
    t += delta;
    float angle = t * M_PI;
    float r     = 20.f;

    _mesh->setPosition3D(Vec3(r * cosf(angle), 0, r * sinf(angle)));
    _streak->setPosition3D(_mesh->getPosition3D());
    _streak->setSweepAxis(Vec3(cosf(angle), 0, sinf(angle)));
}

MeshRendererNormalMappingTest::MeshRendererNormalMappingTest()
{
    auto s = Director::getInstance()->getWinSize();

    {
        auto mesh = MeshRenderer::create("MeshRendererTest/sphere.c3b");
        mesh->setPosition(Vec2(-30.0f, 0.0f));
        mesh->setRotation3D(Vec3(90.0f, 0.0f, 0.0f));
        mesh->setScale(2.0);
        mesh->setCameraMask(2);
        mesh->setTexture("MeshRendererTest/brickwork-texture.jpg");
        addChild(mesh);
    }

    int maxAttributes = Configuration::getInstance()->getMaxAttributes();
    AXASSERT(maxAttributes > 8, "attributes supported must be greater than 8");
    if (maxAttributes > 8)
    {
        auto mesh = MeshRenderer::create("MeshRendererTest/sphere_bumped.c3b");
        mesh->setPosition(Vec2(30.0f, 0.0f));
        mesh->setRotation3D(Vec3(90.0f, 0.0f, 0.0f));
        mesh->setScale(20.0);
        mesh->setCameraMask(2);
        mesh->setTexture("MeshRendererTest/brickwork-texture.jpg");
        addChild(mesh);
    }

    // setup camera
    auto camera = Camera::createPerspective(60.0, s.width / s.height, 1.0f, 1000.f);
    camera->setCameraFlag(CameraFlag::USER1);
    camera->setPosition3D(Vec3(0.f, 0.f, 100.f));
    camera->lookAt(Vec3(0.f, 0.f, 0.f));
    addChild(camera);

    PointLight* light = PointLight::create(Vec3(0.0, 0.0, 0.0), Color3B(255, 255, 255), 1000);
    light->setTag(100);
    addChild(light);

    scheduleUpdate();
}

void MeshRendererNormalMappingTest::update(float dt)
{
    static float angle     = 0.0f;
    static bool reverseDir = false;
    static float radius    = 100.0f;

    auto light = static_cast<PointLight*>(getChildByTag(100));
    light->setPosition3D(Vec3(radius * cos(angle), 0.0f, radius * sin(angle)));
    if (reverseDir)
    {
        angle -= 0.01f;
        if (angle < 0.0)
            reverseDir = false;
    }
    else
    {
        angle += 0.01f;
        if (3.14159 < angle)
            reverseDir = true;
    }
}

MeshRendererNormalMappingTest::~MeshRendererNormalMappingTest() {}

std::string MeshRendererNormalMappingTest::title() const
{
    return "NormalMapping Test";
}

std::string MeshRendererNormalMappingTest::subtitle() const
{
    return "";
}

MeshRendererPropertyTest::MeshRendererPropertyTest()
{
    auto s = Director::getInstance()->getWinSize();

    auto camera = Camera::createPerspective(40, s.width / s.height, 0.01f, 1000.f);
    camera->setCameraFlag(CameraFlag::USER1);
    camera->setPosition3D(Vec3(0.f, 50.f, 200.f));
    camera->lookAt(Vec3(0.f, 0.f, 0.f));
    addChild(camera);

    _mesh = MeshRenderer::create("MeshRendererTest/orc.c3b");
    _mesh->setPosition(20.f, 0.f);
    _mesh->setRotation3D(Vec3(0.0f, 180.0f, 0.0f));
    _meshTex = _mesh->getMesh()->getTexture();
    _texFile = _meshTex->getPath();
    addChild(_mesh);

    setCameraMask(2);

    // auto listener = EventListenerTouchAllAtOnce::create();
    ////listener->onTouchesEnded = AX_CALLBACK_2(MeshRendererReskinTest::onTouchesEnded, this);
    //_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    TTFConfig ttfConfig("fonts/arial.ttf", 20);

    auto label1 = Label::createWithTTF(ttfConfig, "Print Mesh Name");
    auto item1  = MenuItemLabel::create(label1, AX_CALLBACK_1(MeshRendererPropertyTest::printMeshName, this));
    auto label2 = Label::createWithTTF(ttfConfig, "Remove Used Texture");
    auto item2  = MenuItemLabel::create(label2, AX_CALLBACK_1(MeshRendererPropertyTest::removeUsedTexture, this));
    auto label3 = Label::createWithTTF(ttfConfig, "Reset");
    auto item3  = MenuItemLabel::create(label3, AX_CALLBACK_1(MeshRendererPropertyTest::resetTexture, this));

    item1->setPosition(Vec2(VisibleRect::left().x + 100, VisibleRect::bottom().y + item1->getContentSize().height * 4));
    item2->setPosition(Vec2(VisibleRect::left().x + 100, VisibleRect::bottom().y + item1->getContentSize().height * 5));
    item3->setPosition(Vec2(VisibleRect::left().x + 100, VisibleRect::bottom().y + item1->getContentSize().height * 6));

    auto pMenu1 = Menu::create(item1, item2, item3, nullptr);
    pMenu1->setPosition(Vec2(0.0f, 0.0f));
    this->addChild(pMenu1, 10);

    scheduleUpdate();
}
std::string MeshRendererPropertyTest::title() const
{
    return "MeshRendererPropertyTest Test";
}
std::string MeshRendererPropertyTest::subtitle() const
{
    return "";
}

void MeshRendererPropertyTest::update(float delta) {}
void MeshRendererPropertyTest::printMeshName(ax::Object* sender)
{
    AXLOGD("MeshName Begin");
    Vector<Mesh*> meshes = _mesh->getMeshes();
    for (Mesh* mesh : meshes)
    {
        AXLOGI("MeshName: {} ", mesh->getName());
    }
    AXLOGD("MeshName End");
}
void MeshRendererPropertyTest::removeUsedTexture(ax::Object* sender)
{
    if (_meshTex != nullptr)
    {
        Director::getInstance()->getTextureCache()->removeTexture(_meshTex);
        this->refreshMeshRender();
    }
}

void MeshRendererPropertyTest::resetTexture(ax::Object* sender)
{
    if (_meshTex != nullptr)
    {
        _meshTex = Director::getInstance()->getTextureCache()->addImage(_texFile);
        this->refreshMeshRender();
    }
}

void MeshRendererPropertyTest::refreshMeshRender()
{
    Vector<Mesh*> meshes = _mesh->getMeshes();
    for (Mesh* mesh : meshes)
    {
        std::string file    = mesh->getTextureFileName();
        Texture2D* cacheTex = Director::getInstance()->getTextureCache()->getTextureForKey(file);
        if (cacheTex == nullptr)
        {
            unsigned char data[] = {255, 0, 0, 255};  // 1*1 red picture
            Image* image         = new Image();
            image->initWithRawData(data, sizeof(data), 1, 1, sizeof(unsigned char));
            cacheTex = Director::getInstance()->getTextureCache()->addImage(image, "/dummyTexture");
            image->release();
        }
        mesh->setTexture(cacheTex, ax::NTextureData::Usage::Diffuse, false);
    }
}

//
// Issue16155Test
//
Issue16155Test::Issue16155Test()
{
    auto s = Director::getInstance()->getWinSize();

    auto mesh = MeshRenderer::create("MeshRendererTest/orc.c3b");

    int rcBefore = mesh->getMeshByIndex(0)->getTexture()->getReferenceCount();
    addChild(mesh);
    removeChild(mesh);

    AXLOGI("Issue 16155: Object count:{}. Run this test again. RC should be the same", rcBefore);
}

std::string Issue16155Test::title() const
{
    return "Issue16155 Test";
}
std::string Issue16155Test::subtitle() const
{
    return "Should not leak texture. See console";
}
