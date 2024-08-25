/****************************************************************************
Copyright (c) 2009-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
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

#include "2d/Transition.h"
#include "2d/ActionInterval.h"
#include "2d/ActionInstant.h"
#include "2d/ActionEase.h"
#include "2d/ActionCamera.h"
#include "2d/ActionTiledGrid.h"
#include "2d/ActionGrid.h"
#include "2d/Layer.h"
#include "2d/RenderTexture.h"
#include "2d/NodeGrid.h"
#include "base/Director.h"
#include "base/EventDispatcher.h"

namespace ax
{

const unsigned int kSceneFade = 0xFADEFADE;

TransitionScene::TransitionScene()
    : _inScene(nullptr), _outScene(nullptr), _duration(0.0f), _isInSceneOnTop(false), _isSendCleanupToScene(false)
{}

TransitionScene::~TransitionScene()
{
    AX_SAFE_RELEASE(_inScene);
    AX_SAFE_RELEASE(_outScene);
}

TransitionScene* TransitionScene::create(float t, Scene* scene)
{
    TransitionScene* pScene = new TransitionScene();
    if (pScene->initWithDuration(t, scene))
    {
        pScene->autorelease();
        return pScene;
    }
    AX_SAFE_DELETE(pScene);
    return nullptr;
}

bool TransitionScene::initWithDuration(float t, Scene* scene)
{
    AXASSERT(scene != nullptr, "Argument scene must be non-nil");

    if (Scene::init())
    {
        _duration = t;

        // retain
#if AX_ENABLE_GC_FOR_NATIVE_OBJECTS
        auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
        if (sEngine)
        {
            sEngine->retainScriptObject(this, scene);
        }
#endif  // AX_ENABLE_GC_FOR_NATIVE_OBJECTS
        _inScene = scene;
        _inScene->retain();
        _outScene = _director->getRunningScene();
        if (_outScene == nullptr)
        {
            _outScene = Scene::create();
            // just change its state is running that can run actions later
            // issue: https://github.com/cocos2d/cocos2d-x/issues/17442
            _outScene->onEnter();
        }
        _outScene->retain();

        AXASSERT(_inScene != _outScene, "Incoming scene must be different from the outgoing scene");

        sceneOrder();

        return true;
    }
    else
    {
        return false;
    }
}

void TransitionScene::sceneOrder()
{
    _isInSceneOnTop = true;
}

void TransitionScene::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    Scene::draw(renderer, transform, flags);

    if (_isInSceneOnTop)
    {
        _outScene->visit(renderer, transform, flags);
        _inScene->visit(renderer, transform, flags);
    }
    else
    {
        _inScene->visit(renderer, transform, flags);
        _outScene->visit(renderer, transform, flags);
    }
}

void TransitionScene::setWaitForFinishCount(int count)
{
    _waitForFinishCount = count;
}

void TransitionScene::finish()
{
    _waitForFinishCount--;

    if(_waitForFinishCount == 0)
    {
        // clean up
        _inScene->setVisible(true);
        _inScene->setPosition(0, 0);
        _inScene->setScale(1.0f);
        _inScene->setRotation(0.0f);
        _inScene->setAdditionalTransform(nullptr);

        _outScene->setVisible(false);
        _outScene->setPosition(0, 0);
        _outScene->setScale(1.0f);
        _outScene->setRotation(0.0f);
        _outScene->setAdditionalTransform(nullptr);

        //[self schedule:@selector(setNewScene:) interval:0];
        this->schedule(AX_SCHEDULE_SELECTOR(TransitionScene::setNewScene), 0);
    }
}

void TransitionScene::setNewScene(float /*dt*/)
{
    this->unschedule(AX_SCHEDULE_SELECTOR(TransitionScene::setNewScene));

    // Before replacing, save the "send cleanup to scene"
    _isSendCleanupToScene = _director->isSendCleanupToScene();

    _director->replaceScene(_inScene);
#if AX_ENABLE_GC_FOR_NATIVE_OBJECTS
    auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
    if (sEngine)
    {
        sEngine->releaseScriptObject(this, _inScene);
    }
#endif  // AX_ENABLE_GC_FOR_NATIVE_OBJECTS

    // issue #267
    _outScene->setVisible(true);
}

void TransitionScene::hideOutShowIn()
{
    _inScene->setVisible(true);
    _outScene->setVisible(false);
}

// custom onEnter
void TransitionScene::onEnter()
{
    Scene::onEnter();

    // disable events while transitions
    _eventDispatcher->setEnabled(false);

    // outScene should not receive the onEnter callback
    // only the onExitTransitionDidStart
    _outScene->onExitTransitionDidStart();

    _inScene->onEnter();
}

// custom onExit
void TransitionScene::onExit()
{
    Scene::onExit();

    // enable events while transitions
    _eventDispatcher->setEnabled(true);
    _outScene->onExit();

    // _inScene should not receive the onEnter callback
    // only the onEnterTransitionDidFinish
    _inScene->onEnterTransitionDidFinish();

#if AX_ENABLE_SCRIPT_BINDING
    if (ScriptEngineManager::getInstance()->getScriptEngine())
        ScriptEngineManager::getInstance()->getScriptEngine()->garbageCollect();
#endif  // AX_ENABLE_SCRIPT_BINDING
}

// custom cleanup
void TransitionScene::cleanup()
{
    Scene::cleanup();

    if (_isSendCleanupToScene)
        _outScene->cleanup();
}

//
// Oriented Transition
//

TransitionSceneOriented::TransitionSceneOriented() {}

TransitionSceneOriented::~TransitionSceneOriented() {}

TransitionSceneOriented* TransitionSceneOriented::create(float t, Scene* scene, Orientation orientation)
{
    TransitionSceneOriented* newScene = new TransitionSceneOriented();
    newScene->initWithDuration(t, scene, orientation);
    newScene->autorelease();
    return newScene;
}

bool TransitionSceneOriented::initWithDuration(float t, Scene* scene, Orientation orientation)
{
    if (TransitionScene::initWithDuration(t, scene))
    {
        _orientation = orientation;
    }
    return true;
}

//
// RotoZoom
//
TransitionRotoZoom::TransitionRotoZoom() {}

TransitionRotoZoom* TransitionRotoZoom::create(float t, Scene* scene)
{
    TransitionRotoZoom* newScene = new TransitionRotoZoom();
    if (newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    AX_SAFE_DELETE(newScene);
    return nullptr;
}

TransitionRotoZoom::~TransitionRotoZoom() {}

void TransitionRotoZoom::onEnter()
{
    TransitionScene::onEnter();

    _inScene->setScale(0.001f);
    _outScene->setScale(1.0f);

    _inScene->setAnchorPoint(Vec2(0.5f, 0.5f));
    _outScene->setAnchorPoint(Vec2(0.5f, 0.5f));

    setWaitForFinishCount(2);

    auto rotozoom = Sequence::create(
        Spawn::create(ScaleBy::create(_duration / 2, 0.001f), RotateBy::create(_duration / 2, 360 * 2), nullptr),
        DelayTime::create(_duration / 2), nullptr);

    _outScene->runAction(
        Sequence::create(rotozoom, CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr));
    _inScene->runAction(
        Sequence::create(rotozoom->reverse(), CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr));
}

//
// JumpZoom
//
TransitionJumpZoom::TransitionJumpZoom() {}
TransitionJumpZoom::~TransitionJumpZoom() {}

TransitionJumpZoom* TransitionJumpZoom::create(float t, Scene* scene)
{
    TransitionJumpZoom* newScene = new TransitionJumpZoom();
    if (newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    AX_SAFE_DELETE(newScene);
    return nullptr;
}

void TransitionJumpZoom::onEnter()
{
    TransitionScene::onEnter();
    Vec2 s = _director->getWinSize();

    _inScene->setScale(0.5f);
    _inScene->setPosition(s.width, 0);
    _inScene->setAnchorPoint(Vec2(0.5f, 0.5f));
    _outScene->setAnchorPoint(Vec2(0.5f, 0.5f));

    setWaitForFinishCount(2);

    ActionInterval* jump     = JumpBy::create(_duration / 4, Vec2(-s.width, 0.0f), s.width / 4, 2);
    ActionInterval* scaleIn  = ScaleTo::create(_duration / 4, 1.0f);
    ActionInterval* scaleOut = ScaleTo::create(_duration / 4, 0.5f);

    auto jumpZoomOut = Sequence::create(scaleOut, jump, nullptr);
    auto jumpZoomIn  = Sequence::create(jump, scaleIn, nullptr);

    ActionInterval* delay = DelayTime::create(_duration / 2);

    _outScene->runAction(
        Sequence::create(jumpZoomOut, CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr));
    _inScene->runAction(
        Sequence::create(delay, jumpZoomIn, CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr));
}

//
// MoveInL
//
TransitionMoveInL::TransitionMoveInL() {}

TransitionMoveInL::~TransitionMoveInL() {}

TransitionMoveInL* TransitionMoveInL::create(float t, Scene* scene)
{
    TransitionMoveInL* newScene = new TransitionMoveInL();
    if (newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    AX_SAFE_DELETE(newScene);
    return nullptr;
}

void TransitionMoveInL::onEnter()
{
    TransitionScene::onEnter();
    this->initScenes();

    ActionInterval* a = this->action();

    _inScene->runAction(Sequence::create(this->easeActionWithAction(a),
                                         CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr));
}

ActionInterval* TransitionMoveInL::action()
{
    return MoveTo::create(_duration, Vec2(0.0f, 0.0f));
}

ActionInterval* TransitionMoveInL::easeActionWithAction(ActionInterval* action)
{
    return EaseOut::create(action, 2.0f);
    //    return [EaseElasticOut actionWithAction:action period:0.4f];
}

void TransitionMoveInL::initScenes()
{
    Vec2 s = _director->getWinSize();
    _inScene->setPosition(-s.width, 0);
}

//
// MoveInR
//
TransitionMoveInR::TransitionMoveInR() {}
TransitionMoveInR::~TransitionMoveInR() {}

TransitionMoveInR* TransitionMoveInR::create(float t, Scene* scene)
{
    TransitionMoveInR* newScene = new TransitionMoveInR();
    if (newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    AX_SAFE_DELETE(newScene);
    return nullptr;
}

void TransitionMoveInR::initScenes()
{
    Vec2 s = _director->getWinSize();
    _inScene->setPosition(s.width, 0);
}

//
// MoveInT
//
TransitionMoveInT::TransitionMoveInT() {}
TransitionMoveInT::~TransitionMoveInT() {}

TransitionMoveInT* TransitionMoveInT::create(float t, Scene* scene)
{
    TransitionMoveInT* newScene = new TransitionMoveInT();
    if (newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    AX_SAFE_DELETE(newScene);
    return nullptr;
}

void TransitionMoveInT::initScenes()
{
    Vec2 s = _director->getWinSize();
    _inScene->setPosition(0, s.height);
}

//
// MoveInB
//
TransitionMoveInB::TransitionMoveInB() {}
TransitionMoveInB::~TransitionMoveInB() {}

TransitionMoveInB* TransitionMoveInB::create(float t, Scene* scene)
{
    TransitionMoveInB* newScene = new TransitionMoveInB();
    if (newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    AX_SAFE_DELETE(newScene);
    return nullptr;
}

void TransitionMoveInB::initScenes()
{
    Vec2 s = _director->getWinSize();
    _inScene->setPosition(0, -s.height);
}

//
// SlideInL
//

// The adjust factor is needed to prevent issue #442
// One solution is to use DONT_RENDER_IN_SUBPIXELS images, but NO
// The other issue is that in some transitions (and I don't know why)
// the order should be reversed (In in top of Out or vice-versa).
#define ADJUST_FACTOR 0.5f
TransitionSlideInL::TransitionSlideInL() {}

TransitionSlideInL::~TransitionSlideInL() {}

void TransitionSlideInL::onEnter()
{
    TransitionScene::onEnter();
    this->initScenes();

    setWaitForFinishCount(2);

    ActionInterval* in  = this->action();
    ActionInterval* out = this->action();

    ActionInterval* inAction = Sequence::create(easeActionWithAction(in), CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr);

    ActionInterval* outAction = Sequence::create(
        easeActionWithAction(out), CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr);

    _inScene->runAction(inAction);
    _outScene->runAction(outAction);
}

void TransitionSlideInL::sceneOrder()
{
    _isInSceneOnTop = false;
}

void TransitionSlideInL::initScenes()
{
    Vec2 s = _director->getWinSize();
    _inScene->setPosition(-(s.width - ADJUST_FACTOR), 0.0f);
}

ActionInterval* TransitionSlideInL::action()
{
    Vec2 s = _director->getWinSize();
    return MoveBy::create(_duration, Vec2(s.width - ADJUST_FACTOR, 0.0f));
}

ActionInterval* TransitionSlideInL::easeActionWithAction(ActionInterval* action)
{
    return EaseOut::create(action, 2.0f);
}

TransitionSlideInL* TransitionSlideInL::create(float t, Scene* scene)
{
    TransitionSlideInL* newScene = new TransitionSlideInL();
    if (newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    AX_SAFE_DELETE(newScene);
    return nullptr;
}

//
// SlideInR
//
TransitionSlideInR::TransitionSlideInR() {}
TransitionSlideInR::~TransitionSlideInR() {}

TransitionSlideInR* TransitionSlideInR::create(float t, Scene* scene)
{
    TransitionSlideInR* newScene = new TransitionSlideInR();
    if (newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    AX_SAFE_DELETE(newScene);
    return nullptr;
}

void TransitionSlideInR::sceneOrder()
{
    _isInSceneOnTop = true;
}

void TransitionSlideInR::initScenes()
{
    Vec2 s = _director->getWinSize();
    _inScene->setPosition(s.width - ADJUST_FACTOR, 0);
}

ActionInterval* TransitionSlideInR::action()
{
    Vec2 s = _director->getWinSize();
    return MoveBy::create(_duration, Vec2(-(s.width - ADJUST_FACTOR), 0.0f));
}

//
// SlideInT
//
TransitionSlideInT::TransitionSlideInT() {}
TransitionSlideInT::~TransitionSlideInT() {}

TransitionSlideInT* TransitionSlideInT::create(float t, Scene* scene)
{
    TransitionSlideInT* newScene = new TransitionSlideInT();
    if (newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    AX_SAFE_DELETE(newScene);
    return nullptr;
}

void TransitionSlideInT::sceneOrder()
{
    _isInSceneOnTop = false;
}

void TransitionSlideInT::initScenes()
{
    Vec2 s = _director->getWinSize();
    _inScene->setPosition(0, s.height - ADJUST_FACTOR);
}

ActionInterval* TransitionSlideInT::action()
{
    Vec2 s = _director->getWinSize();
    return MoveBy::create(_duration, Vec2(0.0f, -(s.height - ADJUST_FACTOR)));
}

//
// SlideInB
//
TransitionSlideInB::TransitionSlideInB() {}
TransitionSlideInB::~TransitionSlideInB() {}

TransitionSlideInB* TransitionSlideInB::create(float t, Scene* scene)
{
    TransitionSlideInB* newScene = new TransitionSlideInB();
    if (newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    AX_SAFE_DELETE(newScene);
    return nullptr;
}

void TransitionSlideInB::sceneOrder()
{
    _isInSceneOnTop = true;
}

void TransitionSlideInB::initScenes()
{
    Vec2 s = _director->getWinSize();
    _inScene->setPosition(0, -(s.height - ADJUST_FACTOR));
}

ActionInterval* TransitionSlideInB::action()
{
    Vec2 s = _director->getWinSize();
    return MoveBy::create(_duration, Vec2(0.0f, s.height - ADJUST_FACTOR));
}

//
// ShrinkGrow Transition
//
TransitionShrinkGrow::TransitionShrinkGrow() {}
TransitionShrinkGrow::~TransitionShrinkGrow() {}

TransitionShrinkGrow* TransitionShrinkGrow::create(float t, Scene* scene)
{
    TransitionShrinkGrow* newScene = new TransitionShrinkGrow();
    if (newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    AX_SAFE_DELETE(newScene);
    return nullptr;
}

void TransitionShrinkGrow::onEnter()
{
    TransitionScene::onEnter();

    _inScene->setScale(0.001f);
    _outScene->setScale(1.0f);

    _inScene->setAnchorPoint(Vec2(2 / 3.0f, 0.5f));
    _outScene->setAnchorPoint(Vec2(1 / 3.0f, 0.5f));

    setWaitForFinishCount(2);

    ActionInterval* scaleOut = ScaleTo::create(_duration, 0.01f);
    ActionInterval* scaleIn  = ScaleTo::create(_duration, 1.0f);

    _inScene->runAction(Sequence::create(this->easeActionWithAction(scaleIn),
                                         CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr));
    _outScene->runAction(Sequence::create(this->easeActionWithAction(scaleOut),
                                          CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr));
}
ActionInterval* TransitionShrinkGrow::easeActionWithAction(ActionInterval* action)
{
    return EaseOut::create(action, 2.0f);
    //    return [EaseElasticOut actionWithAction:action period:0.3f];
}

//
// FlipX Transition
//
TransitionFlipX::TransitionFlipX() {}
TransitionFlipX::~TransitionFlipX() {}

void TransitionFlipX::onEnter()
{
    TransitionSceneOriented::onEnter();

    _inScene->setVisible(false);

    setWaitForFinishCount(2);

    float inDeltaZ, inAngleZ;
    float outDeltaZ, outAngleZ;

    if (_orientation == TransitionScene::Orientation::RIGHT_OVER)
    {
        inDeltaZ  = 90;
        inAngleZ  = 270;
        outDeltaZ = 90;
        outAngleZ = 0;
    }
    else
    {
        inDeltaZ  = -90;
        inAngleZ  = 90;
        outDeltaZ = -90;
        outAngleZ = 0;
    }

    auto inA = Sequence::create(DelayTime::create(_duration / 2), Show::create(),
                                OrbitCamera::create(_duration / 2, 1, 0, inAngleZ, inDeltaZ, 0, 0),
                                CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr);

    auto outA = Sequence::create(OrbitCamera::create(_duration / 2, 1, 0, outAngleZ, outDeltaZ, 0, 0), Hide::create(),
                                 DelayTime::create(_duration / 2),
                                 CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr);

    _inScene->runAction(inA);
    _outScene->runAction(outA);
}

TransitionFlipX* TransitionFlipX::create(float t, Scene* s, Orientation o)
{
    TransitionFlipX* newScene = new TransitionFlipX();
    newScene->initWithDuration(t, s, o);
    newScene->autorelease();

    return newScene;
}

TransitionFlipX* TransitionFlipX::create(float t, Scene* s)
{
    return TransitionFlipX::create(t, s, TransitionScene::Orientation::RIGHT_OVER);
}

//
// FlipY Transition
//
TransitionFlipY::TransitionFlipY() {}
TransitionFlipY::~TransitionFlipY() {}

void TransitionFlipY::onEnter()
{
    TransitionSceneOriented::onEnter();

    _inScene->setVisible(false);

    setWaitForFinishCount(2);

    float inDeltaZ, inAngleZ;
    float outDeltaZ, outAngleZ;

    if (_orientation == TransitionScene::Orientation::UP_OVER)
    {
        inDeltaZ  = 90;
        inAngleZ  = 270;
        outDeltaZ = 90;
        outAngleZ = 0;
    }
    else
    {
        inDeltaZ  = -90;
        inAngleZ  = 90;
        outDeltaZ = -90;
        outAngleZ = 0;
    }

    auto inA  = Sequence::create(DelayTime::create(_duration / 2), Show::create(),
                                 OrbitCamera::create(_duration / 2, 1, 0, inAngleZ, inDeltaZ, 90, 0),
                                 CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr);
    auto outA = Sequence::create(OrbitCamera::create(_duration / 2, 1, 0, outAngleZ, outDeltaZ, 90, 0), Hide::create(),
                                 DelayTime::create(_duration / 2),
                                 CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr);

    _inScene->runAction(inA);
    _outScene->runAction(outA);
}

TransitionFlipY* TransitionFlipY::create(float t, Scene* s, Orientation o)
{
    TransitionFlipY* newScene = new TransitionFlipY();
    newScene->initWithDuration(t, s, o);
    newScene->autorelease();

    return newScene;
}

TransitionFlipY* TransitionFlipY::create(float t, Scene* s)
{
    return TransitionFlipY::create(t, s, TransitionScene::Orientation::UP_OVER);
}

//
// FlipAngular Transition
//
TransitionFlipAngular::TransitionFlipAngular() {}

TransitionFlipAngular::~TransitionFlipAngular() {}

void TransitionFlipAngular::onEnter()
{
    TransitionSceneOriented::onEnter();

    _inScene->setVisible(false);

    setWaitForFinishCount(2);

    float inDeltaZ, inAngleZ;
    float outDeltaZ, outAngleZ;

    if (_orientation == TransitionScene::Orientation::RIGHT_OVER)
    {
        inDeltaZ  = 90;
        inAngleZ  = 270;
        outDeltaZ = 90;
        outAngleZ = 0;
    }
    else
    {
        inDeltaZ  = -90;
        inAngleZ  = 90;
        outDeltaZ = -90;
        outAngleZ = 0;
    }

    auto inA  = Sequence::create(DelayTime::create(_duration / 2), Show::create(),
                                 OrbitCamera::create(_duration / 2, 1, 0, inAngleZ, inDeltaZ, -45, 0),
                                 CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr);
    auto outA = Sequence::create(OrbitCamera::create(_duration / 2, 1, 0, outAngleZ, outDeltaZ, 45, 0), Hide::create(),
                                 DelayTime::create(_duration / 2),
                                 CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr);

    _inScene->runAction(inA);
    _outScene->runAction(outA);
}

TransitionFlipAngular* TransitionFlipAngular::create(float t, Scene* s, Orientation o)
{
    TransitionFlipAngular* newScene = new TransitionFlipAngular();
    newScene->initWithDuration(t, s, o);
    newScene->autorelease();

    return newScene;
}

TransitionFlipAngular* TransitionFlipAngular::create(float t, Scene* s)
{
    return TransitionFlipAngular::create(t, s, TransitionScene::Orientation::RIGHT_OVER);
}

//
// ZoomFlipX Transition
//
TransitionZoomFlipX::TransitionZoomFlipX() {}
TransitionZoomFlipX::~TransitionZoomFlipX() {}

void TransitionZoomFlipX::onEnter()
{
    TransitionSceneOriented::onEnter();

    _inScene->setVisible(false);

    setWaitForFinishCount(2);

    float inDeltaZ, inAngleZ;
    float outDeltaZ, outAngleZ;

    if (_orientation == TransitionScene::Orientation::RIGHT_OVER)
    {
        inDeltaZ  = 90;
        inAngleZ  = 270;
        outDeltaZ = 90;
        outAngleZ = 0;
    }
    else
    {
        inDeltaZ  = -90;
        inAngleZ  = 90;
        outDeltaZ = -90;
        outAngleZ = 0;
    }
    auto inA  = Sequence::create(DelayTime::create(_duration / 2),
                                 Spawn::create(OrbitCamera::create(_duration / 2, 1, 0, inAngleZ, inDeltaZ, 0, 0),
                                               ScaleTo::create(_duration / 2, 1), Show::create(), nullptr),
                                 CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr);
    auto outA = Sequence::create(Spawn::create(OrbitCamera::create(_duration / 2, 1, 0, outAngleZ, outDeltaZ, 0, 0),
                                               ScaleTo::create(_duration / 2, 0.5f), nullptr),
                                 Hide::create(), DelayTime::create(_duration / 2),
                                 CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr);

    _inScene->setScale(0.5f);
    _inScene->runAction(inA);
    _outScene->runAction(outA);
}

TransitionZoomFlipX* TransitionZoomFlipX::create(float t, Scene* s, Orientation o)
{
    TransitionZoomFlipX* newScene = new TransitionZoomFlipX();
    newScene->initWithDuration(t, s, o);
    newScene->autorelease();

    return newScene;
}

TransitionZoomFlipX* TransitionZoomFlipX::create(float t, Scene* s)
{
    return TransitionZoomFlipX::create(t, s, TransitionScene::Orientation::RIGHT_OVER);
}

//
// ZoomFlipY Transition
//
TransitionZoomFlipY::TransitionZoomFlipY() {}

TransitionZoomFlipY::~TransitionZoomFlipY() {}

void TransitionZoomFlipY::onEnter()
{
    TransitionSceneOriented::onEnter();

    _inScene->setVisible(false);

    setWaitForFinishCount(2);

    float inDeltaZ, inAngleZ;
    float outDeltaZ, outAngleZ;

    if (_orientation == TransitionScene::Orientation::UP_OVER)
    {
        inDeltaZ  = 90;
        inAngleZ  = 270;
        outDeltaZ = 90;
        outAngleZ = 0;
    }
    else
    {
        inDeltaZ  = -90;
        inAngleZ  = 90;
        outDeltaZ = -90;
        outAngleZ = 0;
    }

    auto inA = Sequence::create(DelayTime::create(_duration / 2),
                                Spawn::create(OrbitCamera::create(_duration / 2, 1, 0, inAngleZ, inDeltaZ, 90, 0),
                                              ScaleTo::create(_duration / 2, 1), Show::create(), nullptr),
                                CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr);

    auto outA = Sequence::create(Spawn::create(OrbitCamera::create(_duration / 2, 1, 0, outAngleZ, outDeltaZ, 90, 0),
                                               ScaleTo::create(_duration / 2, 0.5f), nullptr),
                                 Hide::create(), DelayTime::create(_duration / 2),
                                 CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr);

    _inScene->setScale(0.5f);
    _inScene->runAction(inA);
    _outScene->runAction(outA);
}

TransitionZoomFlipY* TransitionZoomFlipY::create(float t, Scene* s, Orientation o)
{
    TransitionZoomFlipY* newScene = new TransitionZoomFlipY();
    newScene->initWithDuration(t, s, o);
    newScene->autorelease();

    return newScene;
}

TransitionZoomFlipY* TransitionZoomFlipY::create(float t, Scene* s)
{
    return TransitionZoomFlipY::create(t, s, TransitionScene::Orientation::UP_OVER);
}

//
// ZoomFlipAngular Transition
//
TransitionZoomFlipAngular::TransitionZoomFlipAngular() {}
TransitionZoomFlipAngular::~TransitionZoomFlipAngular() {}

void TransitionZoomFlipAngular::onEnter()
{
    TransitionSceneOriented::onEnter();

    _inScene->setVisible(false);

    setWaitForFinishCount(2);

    float inDeltaZ, inAngleZ;
    float outDeltaZ, outAngleZ;

    if (_orientation == TransitionScene::Orientation::RIGHT_OVER)
    {
        inDeltaZ  = 90;
        inAngleZ  = 270;
        outDeltaZ = 90;
        outAngleZ = 0;
    }
    else
    {
        inDeltaZ  = -90;
        inAngleZ  = 90;
        outDeltaZ = -90;
        outAngleZ = 0;
    }

    auto inA =
        Sequence::create(DelayTime::create(_duration / 2),
                         Spawn::create(OrbitCamera::create(_duration / 2, 1, 0, inAngleZ, inDeltaZ, -45, 0),
                                       ScaleTo::create(_duration / 2, 1), Show::create(), nullptr),
                         Show::create(), CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr);
    auto outA = Sequence::create(Spawn::create(OrbitCamera::create(_duration / 2, 1, 0, outAngleZ, outDeltaZ, 45, 0),
                                               ScaleTo::create(_duration / 2, 0.5f), nullptr),
                                 Hide::create(), DelayTime::create(_duration / 2),
                                 CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr);

    _inScene->setScale(0.5f);
    _inScene->runAction(inA);
    _outScene->runAction(outA);
}

TransitionZoomFlipAngular* TransitionZoomFlipAngular::create(float t, Scene* s, Orientation o)
{
    TransitionZoomFlipAngular* newScene = new TransitionZoomFlipAngular();
    newScene->initWithDuration(t, s, o);
    newScene->autorelease();

    return newScene;
}

TransitionZoomFlipAngular* TransitionZoomFlipAngular::create(float t, Scene* s)
{
    return TransitionZoomFlipAngular::create(t, s, TransitionScene::Orientation::RIGHT_OVER);
}

//
// Fade Transition
//
TransitionFade::TransitionFade() {}
TransitionFade::~TransitionFade() {}

TransitionFade* TransitionFade::create(float duration, Scene* scene, const Color3B& color)
{
    TransitionFade* transition = new TransitionFade();
    transition->initWithDuration(duration, scene, color);
    transition->autorelease();
    return transition;
}

TransitionFade* TransitionFade::create(float duration, Scene* scene)
{
    return TransitionFade::create(duration, scene, Color3B::BLACK);
}

bool TransitionFade::initWithDuration(float duration, Scene* scene, const Color3B& color)
{
    if (TransitionScene::initWithDuration(duration, scene))
    {
        _color.r = color.r;
        _color.g = color.g;
        _color.b = color.b;
        _color.a = 0;
    }
    return true;
}

bool TransitionFade::initWithDuration(float t, Scene* scene)
{
    this->initWithDuration(t, scene, Color3B::BLACK);
    return true;
}

void TransitionFade ::onEnter()
{
    TransitionScene::onEnter();

    LayerColor* l = LayerColor::create(_color);
    _inScene->setVisible(false);

    addChild(l, 2, kSceneFade);
    Node* f = getChildByTag(kSceneFade);

    auto a = Sequence::create(
        FadeIn::create(_duration / 2), CallFunc::create(AX_CALLBACK_0(TransitionScene::hideOutShowIn, this)),
        FadeOut::create(_duration / 2), CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)),

        nullptr);
    f->runAction(a);
}

void TransitionFade::onExit()
{
    TransitionScene::onExit();
    this->removeChildByTag(kSceneFade, false);
}

//
// Cross Fade Transition
//
TransitionCrossFade::TransitionCrossFade() {}
TransitionCrossFade::~TransitionCrossFade() {}

TransitionCrossFade* TransitionCrossFade::create(float t, Scene* scene)
{
    TransitionCrossFade* newScene = new TransitionCrossFade();
    if (newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    AX_SAFE_DELETE(newScene);
    return nullptr;
}

void TransitionCrossFade::draw(Renderer* /*renderer*/, const Mat4& /*transform*/, uint32_t /*flags*/)
{
    // override draw since both scenes (textures) are rendered in 1 scene
}

void TransitionCrossFade::onEnter()
{
    TransitionScene::onEnter();

    // create a transparent color layer
    // in which we are going to add our rendertextures
    Color4B color(0, 0, 0, 0);
    Vec2 size         = _director->getWinSize();
    LayerColor* layer = LayerColor::create(color);

    // create the first render texture for inScene
    RenderTexture* inTexture =
        RenderTexture::create((int)size.width, (int)size.height, backend::PixelFormat::RGBA8, PixelFormat::D24S8, false);

    if (nullptr == inTexture)
    {
        return;
    }

    inTexture->getSprite()->setAnchorPoint(Vec2(0.5f, 0.5f));
    inTexture->setPosition(size.width / 2, size.height / 2);
    inTexture->setAnchorPoint(Vec2(0.5f, 0.5f));

    // render inScene to its texturebuffer
    inTexture->begin();
    _inScene->visit();
    inTexture->end();

    // create the second render texture for outScene
    RenderTexture* outTexture =
        RenderTexture::create((int)size.width, (int)size.height, backend::PixelFormat::RGBA8, PixelFormat::D24S8, false);
    outTexture->getSprite()->setAnchorPoint(Vec2(0.5f, 0.5f));
    outTexture->setPosition(size.width / 2, size.height / 2);
    outTexture->setAnchorPoint(Vec2(0.5f, 0.5f));

    // render outScene to its texturebuffer
    outTexture->begin();
    _outScene->visit();
    outTexture->end();

    // create blend functions

    // set blendfunctions
    inTexture->getSprite()->setBlendFunc(BlendFunc::DISABLE);
    outTexture->getSprite()->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);

    // add render textures to the layer
    layer->addChild(inTexture);
    layer->addChild(outTexture);

    // initial opacity:
    inTexture->getSprite()->setOpacity(255);
    outTexture->getSprite()->setOpacity(255);

    // create the blend action
    Action* layerAction = Sequence::create(FadeTo::create(_duration, 0),
                                           CallFunc::create(AX_CALLBACK_0(TransitionScene::hideOutShowIn, this)),
                                           CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)), nullptr);

    // run the blend action
    outTexture->getSprite()->runAction(layerAction);

    // add the layer (which contains our two rendertextures) to the scene
    addChild(layer, 2, kSceneFade);
}

// clean up on exit
void TransitionCrossFade::onExit()
{
    // remove our layer and release all containing objects
    this->removeChildByTag(kSceneFade, false);
    TransitionScene::onExit();
}

//
// TurnOffTilesTransition
//
TransitionTurnOffTiles::TransitionTurnOffTiles()
{
    _outSceneProxy = NodeGrid::create();
    _outSceneProxy->retain();
}

TransitionTurnOffTiles::~TransitionTurnOffTiles()
{
    AX_SAFE_RELEASE(_outSceneProxy);
}

TransitionTurnOffTiles* TransitionTurnOffTiles::create(float t, Scene* scene)
{
    TransitionTurnOffTiles* newScene = new TransitionTurnOffTiles();
    if (newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    AX_SAFE_DELETE(newScene);
    return nullptr;
}

// override addScenes, and change the order
void TransitionTurnOffTiles::sceneOrder()
{
    _isInSceneOnTop = false;
}

void TransitionTurnOffTiles::onEnter()
{
    TransitionScene::onEnter();
    _outSceneProxy->setTarget(_outScene);
    _outSceneProxy->onEnter();

    Vec2 s       = _director->getWinSize();
    float aspect = s.width / s.height;
    int x        = (int)(12 * aspect);
    int y        = 12;

    TurnOffTiles* toff     = TurnOffTiles::create(_duration, Vec2(x, y));
    ActionInterval* action = easeActionWithAction(toff);
    _outSceneProxy->runAction(Sequence::create(action, CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)),
                                               StopGrid::create(), nullptr));
}

void TransitionTurnOffTiles::onExit()
{
    _outSceneProxy->setTarget(nullptr);
    _outSceneProxy->onExit();
    TransitionScene::onExit();
}

void TransitionTurnOffTiles::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    Scene::draw(renderer, transform, flags);

    if (_isInSceneOnTop)
    {
        _outSceneProxy->visit(renderer, transform, flags);
        _inScene->visit(renderer, transform, flags);
    }
    else
    {
        _inScene->visit(renderer, transform, flags);
        _outSceneProxy->visit(renderer, transform, flags);
    }
}

ActionInterval* TransitionTurnOffTiles::easeActionWithAction(ActionInterval* action)
{
    return action;
}

//
// SplitCols Transition
//
TransitionSplitCols::TransitionSplitCols()
{
    _gridProxy = NodeGrid::create();
    _gridProxy->retain();
}
TransitionSplitCols::~TransitionSplitCols()
{
    AX_SAFE_RELEASE(_gridProxy);
}

TransitionSplitCols* TransitionSplitCols::create(float t, Scene* scene)
{
    TransitionSplitCols* newScene = new TransitionSplitCols();
    if (newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    AX_SAFE_DELETE(newScene);
    return nullptr;
}

void TransitionSplitCols::onEnter()
{
    TransitionScene::onEnter();

    _gridProxy->setTarget(_outScene);
    _gridProxy->onEnter();

    ActionInterval* split = action();
    auto seq =
        Sequence::create(split, CallFunc::create(AX_CALLBACK_0(TransitionSplitCols::switchTargetToInscene, this)),
                         split->reverse(), nullptr);

    _gridProxy->runAction(Sequence::create(easeActionWithAction(seq),
                                           CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)),
                                           StopGrid::create(), nullptr));
}

void TransitionSplitCols::switchTargetToInscene()
{
    _gridProxy->setTarget(_inScene);
}

void TransitionSplitCols::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    Scene::draw(renderer, transform, flags);
    _gridProxy->visit(renderer, transform, flags);
}

void TransitionSplitCols::onExit()
{
    _gridProxy->setTarget(nullptr);
    _gridProxy->onExit();
    TransitionScene::onExit();
}

ActionInterval* TransitionSplitCols::action()
{
    return SplitCols::create(_duration / 2.0f, 3);
}

ActionInterval* TransitionSplitCols::easeActionWithAction(ActionInterval* action)
{
    return EaseInOut::create(action, 3.0f);
}

//
// SplitRows Transition
//
TransitionSplitRows::TransitionSplitRows() {}

TransitionSplitRows::~TransitionSplitRows() {}

ActionInterval* TransitionSplitRows::action()
{
    return SplitRows::create(_duration / 2.0f, 3);
}

TransitionSplitRows* TransitionSplitRows::create(float t, Scene* scene)
{
    TransitionSplitRows* newScene = new TransitionSplitRows();
    if (newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    AX_SAFE_DELETE(newScene);
    return nullptr;
}

//
// FadeTR Transition
//
TransitionFadeTR::TransitionFadeTR()
{
    _outSceneProxy = NodeGrid::create();
    _outSceneProxy->retain();
}
TransitionFadeTR::~TransitionFadeTR()
{
    AX_SAFE_RELEASE(_outSceneProxy);
}

TransitionFadeTR* TransitionFadeTR::create(float t, Scene* scene)
{
    TransitionFadeTR* newScene = new TransitionFadeTR();
    if (newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    AX_SAFE_DELETE(newScene);
    return nullptr;
}

void TransitionFadeTR::sceneOrder()
{
    _isInSceneOnTop = false;
}

void TransitionFadeTR::onEnter()
{
    TransitionScene::onEnter();

    _outSceneProxy->setTarget(_outScene);
    _outSceneProxy->onEnter();

    Vec2 s       = _director->getWinSize();
    float aspect = s.width / s.height;
    int x        = (int)(12 * aspect);
    int y        = 12;

    ActionInterval* action = actionWithSize(Vec2(x, y));

    _outSceneProxy->runAction(Sequence::create(easeActionWithAction(action),
                                               CallFunc::create(AX_CALLBACK_0(TransitionScene::finish, this)),
                                               StopGrid::create(), nullptr));
}

void TransitionFadeTR::onExit()
{
    _outSceneProxy->setTarget(nullptr);
    _outSceneProxy->onExit();
    TransitionScene::onExit();
}

void TransitionFadeTR::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    Scene::draw(renderer, transform, flags);

    if (_isInSceneOnTop)
    {
        _outSceneProxy->visit(renderer, transform, flags);
        _inScene->visit(renderer, transform, flags);
    }
    else
    {
        _inScene->visit(renderer, transform, flags);
        _outSceneProxy->visit(renderer, transform, flags);
    }
}

ActionInterval* TransitionFadeTR::actionWithSize(const Vec2& size)
{
    return FadeOutTRTiles::create(_duration, size);
}

ActionInterval* TransitionFadeTR::easeActionWithAction(ActionInterval* action)
{
    return action;
}

//
// FadeBL Transition
//

TransitionFadeBL::TransitionFadeBL() {}
TransitionFadeBL::~TransitionFadeBL() {}

TransitionFadeBL* TransitionFadeBL::create(float t, Scene* scene)
{
    TransitionFadeBL* newScene = new TransitionFadeBL();
    if (newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    AX_SAFE_DELETE(newScene);
    return nullptr;
}

ActionInterval* TransitionFadeBL::actionWithSize(const Vec2& size)
{
    return FadeOutBLTiles::create(_duration, size);
}

//
// FadeUp Transition
//
TransitionFadeUp::TransitionFadeUp() {}

TransitionFadeUp::~TransitionFadeUp() {}

TransitionFadeUp* TransitionFadeUp::create(float t, Scene* scene)
{
    TransitionFadeUp* newScene = new TransitionFadeUp();
    if (newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    AX_SAFE_DELETE(newScene);
    return nullptr;
}

ActionInterval* TransitionFadeUp::actionWithSize(const Vec2& size)
{
    return FadeOutUpTiles::create(_duration, size);
}

//
// FadeDown Transition
//
TransitionFadeDown::TransitionFadeDown() {}
TransitionFadeDown::~TransitionFadeDown() {}

TransitionFadeDown* TransitionFadeDown::create(float t, Scene* scene)
{
    TransitionFadeDown* newScene = new TransitionFadeDown();
    if (newScene->initWithDuration(t, scene))
    {
        newScene->autorelease();
        return newScene;
    }
    AX_SAFE_DELETE(newScene);
    return nullptr;
}

ActionInterval* TransitionFadeDown::actionWithSize(const Vec2& size)
{
    return FadeOutDownTiles::create(_duration, size);
}

}
