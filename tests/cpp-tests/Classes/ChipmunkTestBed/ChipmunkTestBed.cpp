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


#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>

#include "chipmunk/chipmunk_private.h"
#include "chipmunk/chipmunk.h"

#include "ChipmunkTestBed.h"


USING_NS_CC;
USING_NS_CC_EXT;

enum {
    kTagParentNode = 1,
};

enum {
    Z_PHYSICS_DEBUG = 100,
};

cpVect ChipmunkDemoMouse;
cpVect ChipmunkDemoKeyboard;
cpBool ChipmunkDemoRightClick;
cpBool ChipmunkDemoRightDown;
cpBool ChipmunkDemoLeftDown = cpFalse;

cpBody* mouse_body        = cpBodyNewKinematic();
cpConstraint* mouse_joint = NULL;

char const* ChipmunkDemoMessageString = NULL;

float ChipmunkDebugDrawPointLineScale = 1.0f;

#define GRABBABLE_MASK_BIT (1 << 31)
cpShapeFilter GRAB_FILTER          = {CP_NO_GROUP, GRABBABLE_MASK_BIT, GRABBABLE_MASK_BIT};
cpShapeFilter NOT_GRABBABLE_FILTER = {CP_NO_GROUP, ~GRABBABLE_MASK_BIT, ~GRABBABLE_MASK_BIT};

// cpVect view_translate = {0, 0};
// cpFloat view_scale    = 1.0;

void ChipmunkDemoDefaultDrawImpl(cpSpace* space){};

void ChipmunkDebugDrawDot(cpFloat size, cpVect pos, cpSpaceDebugColor fillColor){};
cpSpaceDebugColor RGBAColor(float r, float g, float b, float a) {
    cpSpaceDebugColor color = {r, g, b, a};
    return color;
};


static void ShapeFreeWrap(cpSpace* space, cpShape* shape, void* unused) {
    cpSpaceRemoveShape(space, shape);
    cpShapeFree(shape);
}

static void PostShapeFree(cpShape* shape, cpSpace* space) {
    cpSpaceAddPostStepCallback(space, (cpPostStepFunc) ShapeFreeWrap, shape, NULL);
}

static void ConstraintFreeWrap(cpSpace* space, cpConstraint* constraint, void* unused) {
    cpSpaceRemoveConstraint(space, constraint);
    cpConstraintFree(constraint);
}

static void PostConstraintFree(cpConstraint* constraint, cpSpace* space) {
    cpSpaceAddPostStepCallback(space, (cpPostStepFunc) ConstraintFreeWrap, constraint, NULL);
}

static void BodyFreeWrap(cpSpace* space, cpBody* body, void* unused) {
    cpSpaceRemoveBody(space, body);
    cpBodyFree(body);
}

static void PostBodyFree(cpBody* body, cpSpace* space) {
    cpSpaceAddPostStepCallback(space, (cpPostStepFunc) BodyFreeWrap, body, NULL);
}

// Safe and future proof way to remove and free all objects that have been added to the space.
void ChipmunkDemoFreeSpaceChildren(cpSpace* space) {
    // Must remove these BEFORE freeing the body or you will access dangling pointers.
    cpSpaceEachShape(space, (cpSpaceShapeIteratorFunc) PostShapeFree, space);
    cpSpaceEachConstraint(space, (cpSpaceConstraintIteratorFunc) PostConstraintFree, space);

    cpSpaceEachBody(space, (cpSpaceBodyIteratorFunc) PostBodyFree, space);
}


void updateMouseBody(void) {
    cpVect new_point = cpvlerp(mouse_body->p, ChipmunkDemoMouse, 0.25f);
    mouse_body->v    = cpvmult(cpvsub(new_point, mouse_body->p), 60.0f);
    mouse_body->p    = new_point;
}


ChipmunkTestBed::ChipmunkTestBed() {

    //// Resize (expand) window
    // static Size resourceSize(1280, 720);
    // auto director    = Director::getInstance();
    // GLViewImpl* view = (GLViewImpl*) Director::getInstance()->getOpenGLView();
    // view->setWindowed(resourceSize.width, resourceSize.height);
    // orgSize = view->getDesignResolutionSize();
    // view->setDesignResolutionSize(480, 320, ResolutionPolicy::NO_BORDER);

    _mouseListener                = EventListenerMouse::create();
    _mouseListener->onMouseMove   = CC_CALLBACK_1(ChipmunkTestBed::onMouseMove, this);
    _mouseListener->onMouseUp     = CC_CALLBACK_1(ChipmunkTestBed::onMouseUp, this);
    _mouseListener->onMouseDown   = CC_CALLBACK_1(ChipmunkTestBed::onMouseDown, this);
    _mouseListener->onMouseScroll = CC_CALLBACK_1(ChipmunkTestBed::onMouseScroll, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_mouseListener, this);

    scheduleUpdate();
}

ChipmunkTestBed::~ChipmunkTestBed() {
    ChipmunkDemoFreeSpaceChildren(_space);

    // static Size resourceSize(960, 640);
    // GLViewImpl* view = (GLViewImpl*) Director::getInstance()->getOpenGLView();
    // view->setWindowed(resourceSize.width, resourceSize.height);


    // auto director = Director::getInstance();
    // auto glview   = director->getOpenGLView();
    // view->setDesignResolutionSize(orgSize.width, orgSize.height, ResolutionPolicy::NO_BORDER);

    _eventDispatcher->removeEventListener(_mouseListener);
}

void ChipmunkTestBed::initPhysics() {
    // Physics debug layer
    _debugLayer = PhysicsDebugNode::create(_space);
    this->addChild(_debugLayer, Z_PHYSICS_DEBUG);
}

void ChipmunkTestBed::update(float delta) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
    cpSpaceStep(_space, delta);
#else
    cpHastySpaceStep(_space, delta);
#endif
}

void ChipmunkTestBed::createResetButton() {
    auto reset = MenuItemImage::create("Images/r1.png", "Images/r2.png", CC_CALLBACK_1(ChipmunkTestBed::reset, this));
    auto menu  = Menu::create(reset, nullptr);
    menu->setPosition(VisibleRect::center().x, VisibleRect::bottom().y + 30);
    this->addChild(menu, -1);
}

void ChipmunkTestBed::reset(Ref* sender) {
    ChipmunkDemoFreeSpaceChildren(_space);
    getTestSuite()->restartCurrTest();
}

void ChipmunkTestBed::onEnter() {
    TestCase::onEnter();
    physicsDebugNodeOffset = VisibleRect::center();
}


void ChipmunkTestBed::onMouseDown(Event* event) {
    EventMouse* e = (EventMouse*) event;
    if ((int) e->getMouseButton() == 0) {
        ChipmunkDemoLeftDown  = cpTrue;
        ChipmunkDemoRightDown = cpFalse;

        // give the mouse click a little radius to make it easier to click small shapes.
        cpFloat radius = 5.0;

        cpPointQueryInfo info = {0};
        cpShape* shape        = cpSpacePointQueryNearest(_space, ChipmunkDemoMouse, radius, GRAB_FILTER, &info);

        if (shape && cpBodyGetMass(cpShapeGetBody(shape)) < INFINITY) {
            // Use the closest point on the surface if the click is outside of the shape.
            cpVect nearest = (info.distance > 0.0f ? info.point : ChipmunkDemoMouse);

            cpBody* body           = cpShapeGetBody(shape);
            mouse_joint            = cpPivotJointNew2(mouse_body, body, cpvzero, cpBodyWorldToLocal(body, nearest));
            mouse_joint->maxForce  = 50000.0f;
            mouse_joint->errorBias = cpfpow(1.0f - 0.15f, 60.0f);
            cpSpaceAddConstraint(_space, mouse_joint);
        }
    } else if ((int) e->getMouseButton() == 1) {

        if (mouse_joint) {
            cpSpaceRemoveConstraint(_space, mouse_joint);
            cpConstraintFree(mouse_joint);
            mouse_joint = NULL;
        }

        ChipmunkDemoLeftDown  = cpFalse;
        ChipmunkDemoRightDown = cpTrue;
    }
}

void ChipmunkTestBed::onMouseUp(Event* event) {
    EventMouse* e = (EventMouse*) event;

    ChipmunkDemoLeftDown = cpFalse;

    if (mouse_joint) {
        cpSpaceRemoveConstraint(_space, mouse_joint);
        cpConstraintFree(mouse_joint);
        mouse_joint = NULL;
    }
}

void ChipmunkTestBed::onMouseMove(Event* event) {
    EventMouse* e = (EventMouse*) event;

    ChipmunkDemoMouse.x = e->getCursorX() - physicsDebugNodeOffset.x;
    ChipmunkDemoMouse.y = e->getCursorY() - physicsDebugNodeOffset.y;


    cpBodySetPosition(mouse_body, ChipmunkDemoMouse);
}

void ChipmunkTestBed::onMouseScroll(Event* event) {
    EventMouse* e = (EventMouse*) event;
    CCLOG("Mouse Scroll detected, X: %i ", e->getScrollX());
    CCLOG("Mouse Scroll detected, Y: %i ", e->getScrollY());
}


//------------------------------------------------------------------
//
// LogoSmashDemo
//
//------------------------------------------------------------------
void LogoSmashDemo::onEnter() {
    ChipmunkTestBed::onEnter();
    initPhysics();
}

std::string LogoSmashDemo::title() const {
    return LogoSmash.name;
}

void LogoSmashDemo::initPhysics() {
    _space = LogoSmash.initFunc();
    ChipmunkTestBed::initPhysics();
}

void LogoSmashDemo::update(float delta) {
    updateMouseBody();
    LogoSmash.updateFunc(_space, LogoSmash.timestep);
}

//------------------------------------------------------------------
//
// PlinkDemo
//
//------------------------------------------------------------------
void PlinkDemo::onEnter() {
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string PlinkDemo::title() const {
    return Plink.name;
}

void PlinkDemo::initPhysics() {
    _space = Plink.initFunc();
    ChipmunkTestBed::initPhysics();
}

void PlinkDemo::update(float delta) {
    updateMouseBody();
    Plink.updateFunc(_space, Plink.timestep);
}

//------------------------------------------------------------------
//
// TumbleDemo
//
//------------------------------------------------------------------
void TumbleDemo::onEnter() {
    ChipmunkTestBed::onEnter();
    initPhysics();
}

std::string TumbleDemo::title() const {
    return Tumble.name;
}

void TumbleDemo::initPhysics() {
    _space = Tumble.initFunc();
    ChipmunkTestBed::initPhysics();
}

void TumbleDemo::update(float delta) {
    updateMouseBody();
    Tumble.updateFunc(_space, Tumble.timestep);
}

//------------------------------------------------------------------
//
// PyramidStackDemo
//
//------------------------------------------------------------------
void PyramidStackDemo::onEnter() {
    ChipmunkTestBed::onEnter();
    initPhysics();
}

std::string PyramidStackDemo::title() const {
    return PyramidStack.name;
}

void PyramidStackDemo::initPhysics() {
    _space = PyramidStack.initFunc();
    ChipmunkTestBed::initPhysics();
}

void PyramidStackDemo::update(float delta) {
    updateMouseBody();
    PyramidStack.updateFunc(_space, PyramidStack.timestep);
}


//------------------------------------------------------------------
//
// PyramidToppleDemo
//
//------------------------------------------------------------------
void PyramidToppleDemo::onEnter() {
    ChipmunkTestBed::onEnter();
    initPhysics();
}

std::string PyramidToppleDemo::title() const {
    return PyramidTopple.name;
}

void PyramidToppleDemo::initPhysics() {
    _space = PyramidTopple.initFunc();
    ChipmunkTestBed::initPhysics();
}

void PyramidToppleDemo::update(float delta) {
    updateMouseBody();
    PyramidTopple.updateFunc(_space, PyramidTopple.timestep);
}


//------------------------------------------------------------------
//
// ChainDemo
//
//------------------------------------------------------------------
void ChainsDemo::onEnter() {
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string ChainsDemo::title() const {
    return Chains.name;
}

void ChainsDemo::initPhysics() {
    _space = Chains.initFunc();
    ChipmunkTestBed::initPhysics();
}

void ChainsDemo::update(float delta) {
    updateMouseBody();
    Chains.updateFunc(_space, Chains.timestep);
}

//------------------------------------------------------------------
//
// OneWayDemo
//
//------------------------------------------------------------------
void OneWayDemo::onEnter() {
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string OneWayDemo::title() const {
    return OneWay.name;
}

void OneWayDemo::initPhysics() {
    _space = OneWay.initFunc();
    ChipmunkTestBed::initPhysics();
}

void OneWayDemo::update(float delta) {
    updateMouseBody();
    OneWay.updateFunc(_space, OneWay.timestep);
}

//------------------------------------------------------------------
//
// PlanetDemo
//
//------------------------------------------------------------------
void PlanetDemo::onEnter() {
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string PlanetDemo::title() const {
    return Planet.name;
}

void PlanetDemo::initPhysics() {
    _space = Planet.initFunc();
    ChipmunkTestBed::initPhysics();
}

void PlanetDemo::update(float delta) {
    updateMouseBody();
    Planet.updateFunc(_space, Planet.timestep);
}

//------------------------------------------------------------------
//
// TheoJansenDemo
//
//------------------------------------------------------------------
void TheoJansenDemo::onEnter() {
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string TheoJansenDemo::title() const {
    return TheoJansen.name;
}

void TheoJansenDemo::initPhysics() {
    _space = TheoJansen.initFunc();
    ChipmunkTestBed::initPhysics();
}

void TheoJansenDemo::update(float delta) {
    updateMouseBody();
    TheoJansen.updateFunc(_space, TheoJansen.timestep);
}


//------------------------------------------------------------------
//
// TankDemo
//
//------------------------------------------------------------------
void TankDemo::onEnter() {
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string TankDemo::title() const {
    return Tank.name;
}

void TankDemo::initPhysics() {
    _space = Tank.initFunc();
    ChipmunkTestBed::initPhysics();
}

void TankDemo::update(float delta) {
    updateMouseBody();
    Tank.updateFunc(_space, Tank.timestep);
}


//------------------------------------------------------------------
//
// BouncyHexagonsDemo
//
//------------------------------------------------------------------
// void BouncyHexagonsDemo::onEnter() {
//    ChipmunkTestBed::onEnter();
//
//    initPhysics();
//}
//
// std::string BouncyHexagonsDemo::title() const {
//    return BouncyHexagons.name;
//}
//
// void BouncyHexagonsDemo::initPhysics() {
//    _space = BouncyHexagons.initFunc();
//    ChipmunkTestBed::initPhysics();
//}
//
// void BouncyHexagonsDemo::update(float delta) {
//    BouncyHexagons.updateFunc(_space, BouncyHexagons.timestep);
//}


//------------------------------------------------------------------
//
// SpringiesDemo
//
//------------------------------------------------------------------
// void SpringiesDemo::onEnter() {
//    ChipmunkTestBed::onEnter();
//
//    initPhysics();
//}
//
// std::string SpringiesDemo::title() const {
//    return Springies.name;
//}
//
// void SpringiesDemo::initPhysics() {
//    _space = Springies.initFunc();
//    ChipmunkTestBed::initPhysics();
//}
//
// void SpringiesDemo::update(float delta) {
//    Springies.updateFunc(_space, Springies.timestep);
//}


//------------------------------------------------------------------
//
// Add Example
//
//------------------------------------------------------------------
void AddExample::onEnter() {
    ChipmunkTestBed::onEnter();

    _space = cpSpaceNew();
    ChipmunkTestBed::initPhysics();

    log("%s\n", Director::getInstance()->getTextureCache()->getCachedTextureInfo().c_str());
}

std::string AddExample::title() const {
    return "  Here is the place for your\n Chipmunk2D example";
}

void AddExample::initPhysics() {
    _space = cpSpaceNew();
    ChipmunkTestBed::initPhysics();

    log("%s\n", Director::getInstance()->getTextureCache()->getCachedTextureInfo().c_str());
}

void AddExample::update(float delta) {
    cpSpaceStep(_space, delta);
}


ChipmunkTestBedTests::ChipmunkTestBedTests() {

    ADD_TEST_CASE(LogoSmashDemo);
    ADD_TEST_CASE(PlinkDemo);
    ADD_TEST_CASE(TumbleDemo);
    ADD_TEST_CASE(PyramidToppleDemo);
    ADD_TEST_CASE(PyramidStackDemo);
    ADD_TEST_CASE(ChainsDemo);
    ADD_TEST_CASE(OneWayDemo);
    ADD_TEST_CASE(PlanetDemo);
    ADD_TEST_CASE(TheoJansenDemo);
    ADD_TEST_CASE(TankDemo);
    // ADD_TEST_CASE(BouncyHexagonsDemo);
    // ADD_TEST_CASE(SpringiesDemo);

    ADD_TEST_CASE(AddExample);
}
