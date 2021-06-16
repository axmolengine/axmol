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

//
// Accelerometer + Chipmunk physics + multi touches example
// a cocos2d example
// http://www.cocos2d-x.org
//

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

//#define GRABBABLE_MASK_BIT (1 << 31)
//cpShapeFilter GRAB_FILTER          = {CP_NO_GROUP, GRABBABLE_MASK_BIT, GRABBABLE_MASK_BIT};
//cpShapeFilter NOT_GRABBABLE_FILTER = {CP_NO_GROUP, ~GRABBABLE_MASK_BIT, ~GRABBABLE_MASK_BIT};

static inline cpFloat frand(void) {
    return (cpFloat) rand() / (cpFloat) RAND_MAX;
}


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

static const int image_width      = 188;
static const int image_height     = 35;
static const int image_row_length = 24;

static const int image_bitmap[] = {15, -16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, -64,
    15, 63, -32, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, -64, 15, 127, -125, -1, -128, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 127, -64, 15, 127, 15, -1, -64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, -1, -64, 15, -2, 31, -1, -64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -1, -64, 0, -4, 63, -1, -32,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -1, -64, 15, -8, 127, -1, -32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, -1, -64, 0, -8, -15, -1, -32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, -31, -1, -64, 15,
    -8, -32, -1, -32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, -15, -1, -64, 9, -15, -32, -1, -32, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, -15, -1, -64, 0, -15, -32, -1, -32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    63, -7, -1, -64, 9, -29, -32, 127, -61, -16, 63, 15, -61, -1, -8, 31, -16, 15, -8, 126, 7, -31, -8, 31, -65, -7, -1,
    -64, 9, -29, -32, 0, 7, -8, 127, -97, -25, -1, -2, 63, -8, 31, -4, -1, 15, -13, -4, 63, -1, -3, -1, -64, 9, -29,
    -32, 0, 7, -8, 127, -97, -25, -1, -2, 63, -8, 31, -4, -1, 15, -13, -2, 63, -1, -3, -1, -64, 9, -29, -32, 0, 7, -8,
    127, -97, -25, -1, -1, 63, -4, 63, -4, -1, 15, -13, -2, 63, -33, -1, -1, -32, 9, -25, -32, 0, 7, -8, 127, -97, -25,
    -1, -1, 63, -4, 63, -4, -1, 15, -13, -1, 63, -33, -1, -1, -16, 9, -25, -32, 0, 7, -8, 127, -97, -25, -1, -1, 63, -4,
    63, -4, -1, 15, -13, -1, 63, -49, -1, -1, -8, 9, -57, -32, 0, 7, -8, 127, -97, -25, -8, -1, 63, -2, 127, -4, -1, 15,
    -13, -1, -65, -49, -1, -1, -4, 9, -57, -32, 0, 7, -8, 127, -97, -25, -8, -1, 63, -2, 127, -4, -1, 15, -13, -1, -65,
    -57, -1, -1, -2, 9, -57, -32, 0, 7, -8, 127, -97, -25, -8, -1, 63, -2, 127, -4, -1, 15, -13, -1, -1, -57, -1, -1,
    -1, 9, -57, -32, 0, 7, -1, -1, -97, -25, -8, -1, 63, -1, -1, -4, -1, 15, -13, -1, -1, -61, -1, -1, -1, -119, -57,
    -32, 0, 7, -1, -1, -97, -25, -8, -1, 63, -1, -1, -4, -1, 15, -13, -1, -1, -61, -1, -1, -1, -55, -49, -32, 0, 7, -1,
    -1, -97, -25, -8, -1, 63, -1, -1, -4, -1, 15, -13, -1, -1, -63, -1, -1, -1, -23, -49, -32, 127, -57, -1, -1, -97,
    -25, -1, -1, 63, -1, -1, -4, -1, 15, -13, -1, -1, -63, -1, -1, -1, -16, -49, -32, -1, -25, -1, -1, -97, -25, -1, -1,
    63, -33, -5, -4, -1, 15, -13, -1, -1, -64, -1, -9, -1, -7, -49, -32, -1, -25, -8, 127, -97, -25, -1, -1, 63, -33,
    -5, -4, -1, 15, -13, -1, -1, -64, -1, -13, -1, -32, -49, -32, -1, -25, -8, 127, -97, -25, -1, -2, 63, -49, -13, -4,
    -1, 15, -13, -1, -1, -64, 127, -7, -1, -119, -17, -15, -1, -25, -8, 127, -97, -25, -1, -2, 63, -49, -13, -4, -1, 15,
    -13, -3, -1, -64, 127, -8, -2, 15, -17, -1, -1, -25, -8, 127, -97, -25, -1, -8, 63, -49, -13, -4, -1, 15, -13, -3,
    -1, -64, 63, -4, 120, 0, -17, -1, -1, -25, -8, 127, -97, -25, -8, 0, 63, -57, -29, -4, -1, 15, -13, -4, -1, -64, 63,
    -4, 0, 15, -17, -1, -1, -25, -8, 127, -97, -25, -8, 0, 63, -57, -29, -4, -1, -1, -13, -4, -1, -64, 31, -2, 0, 0,
    103, -1, -1, -57, -8, 127, -97, -25, -8, 0, 63, -57, -29, -4, -1, -1, -13, -4, 127, -64, 31, -2, 0, 15, 103, -1, -1,
    -57, -8, 127, -97, -25, -8, 0, 63, -61, -61, -4, 127, -1, -29, -4, 127, -64, 15, -8, 0, 0, 55, -1, -1, -121, -8,
    127, -97, -25, -8, 0, 63, -61, -61, -4, 127, -1, -29, -4, 63, -64, 15, -32, 0, 0, 23, -1, -2, 3, -16, 63, 15, -61,
    -16, 0, 31, -127, -127, -8, 31, -1, -127, -8, 31, -128, 7, -128, 0, 0};

static inline int get_pixel(int x, int y) {
    return (image_bitmap[(x >> 3) + y * image_row_length] >> (~x & 0x7)) & 1;
}

static int bodyCount = 0;

static void update(cpSpace* space, double dt) {
    cpSpaceStep(space, dt);
}

static cpShape* make_ball(cpFloat x, cpFloat y) {
    cpBody* body = cpBodyNew(1.0, INFINITY);
    cpBodySetPosition(body, cpv(x, y));

    cpShape* shape = cpCircleShapeNew(body, 0.95, cpvzero);
    cpShapeSetElasticity(shape, 0.0);
    cpShapeSetFriction(shape, 0.0);

    return shape;
}

static cpSpace* initLogoSmash(void) {
    cpSpace* space = cpSpaceNew();
    cpSpaceSetIterations(space, 1);

    // The space will contain a very large number of similary sized objects.
    // This is the perfect candidate for using the spatial hash.
    // Generally you will never need to do this.
    cpSpaceUseSpatialHash(space, 2.0, 10000);

    bodyCount = 0;

    cpBody* body;
    cpShape* shape;

    for (int y = 0; y < image_height; y++) {
        for (int x = 0; x < image_width; x++) {
            if (!get_pixel(x, y))
                continue;

            cpFloat x_jitter = 0.05 * frand();
            cpFloat y_jitter = 0.05 * frand();

            shape = make_ball(2 * (x - image_width / 2 + x_jitter) + VisibleRect::center().x,
                2 * (image_height / 2 - y + y_jitter) + VisibleRect::center().y);
            cpSpaceAddBody(space, cpShapeGetBody(shape));
            cpSpaceAddShape(space, shape);

            bodyCount++;
        }
    }

    body = cpSpaceAddBody(space, cpBodyNew(1e9, INFINITY));
    cpBodySetPosition(body, cpv(VisibleRect::left().x - 100, +VisibleRect::center().y - 10));
    cpBodySetVelocity(body, cpv(400, 0));

    shape = cpSpaceAddShape(space, cpCircleShapeNew(body, 8.0f, cpvzero));
    cpShapeSetElasticity(shape, 0.0);
    cpShapeSetFriction(shape, 0.0);
  //  cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);
    bodyCount++;

    body = cpSpaceAddBody(space, cpBodyNew(1e9, INFINITY));
    cpBodySetPosition(body, cpv(VisibleRect::right().x + 100, +VisibleRect::center().y ));
    cpBodySetVelocity(body, cpv(-400, 0));

    shape = cpSpaceAddShape(space, cpCircleShapeNew(body, 12.0f, cpvzero));
    cpShapeSetElasticity(shape, 0.0);
    cpShapeSetFriction(shape, 0.0);
  //  cpShapeSetFilter(shape, NOT_GRABBABLE_FILTER);
    bodyCount++;

    return space;
}

static void destroy(cpSpace* space) {
    ChipmunkDemoFreeSpaceChildren(space);
    cpSpaceFree(space);
}


// callback to remove Shapes from the Space

ChipmunkTestBed::ChipmunkTestBed() {   
    // enable events

    auto touchListener = EventListenerTouchAllAtOnce::create();
    touchListener->onTouchesEnded = CC_CALLBACK_2(ChipmunkTestBed::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    Device::setAccelerometerEnabled(true);
    auto accListener = EventListenerAcceleration::create(CC_CALLBACK_2(ChipmunkTestBed::onAcceleration, this));
    _eventDispatcher->addEventListenerWithSceneGraphPriority(accListener, this);
    
    // title
    auto label = Label::createWithTTF("Logo Smash", "fonts/Marker Felt.ttf", 20.0f);
    label->setPosition(VisibleRect::center().x, VisibleRect::top().y - 30);
    this->addChild(label, -1);

    // reset button
    createResetButton();

    // init physics
    initPhysics();


    //// menu for debug layer
    //MenuItemFont::setFontSize(18);
    //auto item = MenuItemFont::create("Toggle debug", CC_CALLBACK_1(ChipmunkTestBed::toggleDebugCallback, this));

    //auto menu = Menu::create(item, nullptr);
    //this->addChild(menu);
    //menu->setPosition(VisibleRect::right().x-100, VisibleRect::top().y-60);

    scheduleUpdate();
    
}

void ChipmunkTestBed::toggleDebugCallback(Ref* sender) {
    _debugLayer->setVisible(! _debugLayer->isVisible());
}

ChipmunkTestBed::~ChipmunkTestBed() {

    ChipmunkDemoFreeSpaceChildren(_space);

    Device::setAccelerometerEnabled(false);
}

void ChipmunkTestBed::initPhysics()
{

    _space = initLogoSmash();

    // Physics debug layer
    _debugLayer = PhysicsDebugNode::create(_space);
    this->addChild(_debugLayer, Z_PHYSICS_DEBUG);

}

void ChipmunkTestBed::update(float delta) 
{
    // Should use a fixed size step based on the animation interval.
    int steps = 2;
    float dt = Director::getInstance()->getAnimationInterval()/(float)steps;

    for(int i=0; i<steps; i++){

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
		cpSpaceStep(_space, dt);
#else
		cpHastySpaceStep(_space, dt);
#endif
    }
}

void ChipmunkTestBed::createResetButton() 
{
    auto reset = MenuItemImage::create("Images/r1.png", "Images/r2.png", CC_CALLBACK_1(ChipmunkTestBed::reset, this));

    auto menu = Menu::create(reset, nullptr);

    menu->setPosition(VisibleRect::center().x, VisibleRect::bottom().y + 30);
    this->addChild(menu, -1);
}

void ChipmunkTestBed::reset(Ref* sender) 
{
    ChipmunkDemoFreeSpaceChildren(_space);
    getTestSuite()->restartCurrTest();
}

//void ChipmunkTestBed::addNewSpriteAtPosition(cocos2d::Vec2 pos) 
//{
//    int posx, posy;
//
//    auto parent = getChildByTag(kTagParentNode);
//
//    posx = CCRANDOM_0_1() * 200.0f;
//    posy = CCRANDOM_0_1() * 200.0f;
//
//    posx = (posx % 4) * 85;
//    posy = (posy % 3) * 121;
//
//
//    int num = 4;
//    cpVect verts[] = {
//        cpv(-24,-54),
//        cpv(-24, 54),
//        cpv( 24, 54),
//        cpv( 24,-54),
//    };
//
//    cpBody *body = cpBodyNew(1.0f, cpMomentForPoly(1.0f, num, verts, cpvzero, 0.0f));
//
//    cpBodySetPosition(body, cpv(pos.x, pos.y));
//    cpSpaceAddBody(_space, body);
//
//    cpShape* shape = cpPolyShapeNew(body, num, verts, cpTransformIdentity, 0.0f);
//    cpShapeSetElasticity(shape, 0.5f);
//    cpShapeSetFriction(shape, 0.5f);
//    cpSpaceAddShape(_space, shape);
//
//    auto sprite = PhysicsSpriteChipmunk2D::createWithTexture(_spriteTexture, cocos2d::Rect(posx, posy, 85, 121));
//    parent->addChild(sprite);
//
//    sprite->setCPBody(body);
//    sprite->setPosition(pos);
//
//}

void ChipmunkTestBed::onEnter() 
{
    TestCase::onEnter();
}

void ChipmunkTestBed::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    //Add a new body/atlas sprite at the touched location

    for( auto &touch: touches)
    {
        auto location = touch->getLocation();

        //addNewSpriteAtPosition( location );
    }
}

void ChipmunkTestBed::onAcceleration(Acceleration* acc, Event* event)
{
    static float prevX=0, prevY=0;

#define kFilterFactor 0.05f

    float accelX = (float) acc->x * kFilterFactor + (1- kFilterFactor)*prevX;
    float accelY = (float) acc->y * kFilterFactor + (1- kFilterFactor)*prevY;

    prevX = accelX;
    prevY = accelY;

    auto v = cocos2d::Vec2( accelX, accelY);
    v = v * 200;
    cpSpaceSetGravity(_space, cpv(v.x, v.y));
}

ChipmunkTestBedTests::ChipmunkTestBedTests()
{
    ADD_TEST_CASE(ChipmunkTestBed);
}

