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


extern int image_bitmap[];
extern cpSpace* initLogoSmash(void);
extern cpSpace* initPlink(void);
extern cpSpace* initPump(void);
extern cpSpace* initTumble(void);

#define GRABBABLE_MASK_BIT (1 << 31)
cpShapeFilter GRAB_FILTER          = {CP_NO_GROUP, GRABBABLE_MASK_BIT, GRABBABLE_MASK_BIT};
cpShapeFilter NOT_GRABBABLE_FILTER = {CP_NO_GROUP, ~GRABBABLE_MASK_BIT, ~GRABBABLE_MASK_BIT};



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


static void update(cpSpace* space, double dt) {
    cpSpaceStep(space, dt);
}


static void destroy(cpSpace* space) {
    ChipmunkDemoFreeSpaceChildren(space);
    cpSpaceFree(space);
}


ChipmunkTestBed::ChipmunkTestBed() 
{

    scheduleUpdate();
}

ChipmunkTestBed::~ChipmunkTestBed() 
{
    ChipmunkDemoFreeSpaceChildren(_space);
}

void ChipmunkTestBed::initPhysics()
{
    // Physics debug layer
    _debugLayer = PhysicsDebugNode::create(_space);
    this->addChild(_debugLayer, Z_PHYSICS_DEBUG);
}

void ChipmunkTestBed::update(float delta) 
{
    // Should use a fixed size step based on the animation interval.
    int steps = 2;
    float dt  = Director::getInstance()->getAnimationInterval() / (float) steps;

    for (int i = 0; i < steps; i++) {

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
        cpSpaceStep(_space, dt);
#else
        cpHastySpaceStep(_space, dt);
#endif
    }
}

void ChipmunkTestBed::createResetButton() {
    auto reset = MenuItemImage::create("Images/r1.png", "Images/r2.png", CC_CALLBACK_1(ChipmunkTestBed::reset, this));

    auto menu = Menu::create(reset, nullptr);

    menu->setPosition(VisibleRect::center().x, VisibleRect::bottom().y + 30);
    this->addChild(menu, -1);
}

void ChipmunkTestBed::reset(Ref* sender) {
    ChipmunkDemoFreeSpaceChildren(_space);
    getTestSuite()->restartCurrTest();
}

void ChipmunkTestBed::onEnter() {
    TestCase::onEnter();
}


//------------------------------------------------------------------
//
// LogoSmash
//
//------------------------------------------------------------------
void LogoSmash::onEnter() 
{
    ChipmunkTestBed::onEnter();

    _space = initLogoSmash();
    initPhysics();

    log("%s\n", Director::getInstance()->getTextureCache()->getCachedTextureInfo().c_str());
}

std::string  LogoSmash::title() const 
{
    return "Logo Smash";
}

//------------------------------------------------------------------
//
// Plink
//
//------------------------------------------------------------------
void Plink::onEnter() {
    ChipmunkTestBed::onEnter();

    _space = initPlink();
    initPhysics();

    log("%s\n", Director::getInstance()->getTextureCache()->getCachedTextureInfo().c_str());
}

std::string  Plink::title() const {
    return "Plink";
}

//------------------------------------------------------------------
//
// Tumble
//
//------------------------------------------------------------------
void Tumble::onEnter() {
    ChipmunkTestBed::onEnter();

    _space = initTumble();
    initPhysics();

    log("%s\n", Director::getInstance()->getTextureCache()->getCachedTextureInfo().c_str());
}

std::string Tumble::title() const {
    return "Tumble";
}

//------------------------------------------------------------------
//
// Add Example
//
//------------------------------------------------------------------
void AddExample::onEnter() {
    ChipmunkTestBed::onEnter();

    _space = cpSpaceNew();
    initPhysics();

    log("%s\n", Director::getInstance()->getTextureCache()->getCachedTextureInfo().c_str());
}

std::string AddExample::title() const {
    return "  Here is the place for your\n Chipmunk2D example";
}



ChipmunkTestBedTests::ChipmunkTestBedTests() {
    ADD_TEST_CASE(LogoSmash);
    ADD_TEST_CASE(Plink);
    ADD_TEST_CASE(Tumble);

    ADD_TEST_CASE(AddExample);
}
