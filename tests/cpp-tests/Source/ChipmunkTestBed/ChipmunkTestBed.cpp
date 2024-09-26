/****************************************************************************
 * Copyright (c) 2021 @aismann; Peter Eismann, Germany; dreifrankensoft

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

using namespace ax;
USING_NS_AX_EXT;

enum
{
    kTagParentNode = 1,
};

enum
{
    Z_PHYSICS_DEBUG = 100,
};

extern ChipmunkDemo Example;  // Use as template for new Chipmunk2D demos
extern ChipmunkDemo LogoSmash;
extern ChipmunkDemo PyramidStack;
extern ChipmunkDemo Plink;
extern ChipmunkDemo Tumble;
extern ChipmunkDemo PyramidTopple;
extern ChipmunkDemo Planet;
extern ChipmunkDemo Springies;
extern ChipmunkDemo Pump;
extern ChipmunkDemo TheoJansen;
extern ChipmunkDemo Query;
extern ChipmunkDemo OneWay;
extern ChipmunkDemo PlatformerPlayer;
extern ChipmunkDemo Joints;
extern ChipmunkDemo Tank;
extern ChipmunkDemo Chains;
extern ChipmunkDemo Crane;
extern ChipmunkDemo Buoyancy;
extern ChipmunkDemo ContactGraph;
extern ChipmunkDemo Slice;
extern ChipmunkDemo Convex;
extern ChipmunkDemo Unicycle;
extern ChipmunkDemo Sticky;
extern ChipmunkDemo Shatter;
extern ChipmunkDemo bench_list[];
extern int bench_count;
int bench = 16;

static Vec2 cpVert2Point(const cpVect& vert)
{
    return Vec2(vert.x, vert.y) + physicsDebugNodeOffset;
}

ChipmunkDemo demos[] = {LogoSmash, PyramidStack, Plink,      Tumble,       PyramidTopple, Planet,
                        Springies, Pump,         TheoJansen, Query,        OneWay,        Joints,
                        Tank,      Chains,       Crane,      ContactGraph, Buoyancy,      PlatformerPlayer,
                        Slice,     Convex,       Unicycle,   Sticky,       Shatter};

int demo_count = sizeof(demos);

cpVect ChipmunkDemoMouse;
cpVect ChipmunkDemoKeyboard;
cpBool ChipmunkDemoRightClick = cpFalse;
cpBool ChipmunkDemoRightDown  = cpFalse;
cpBool ChipmunkDemoLeftDown   = cpFalse;
double ChipmunkDemoTime;

cpBody* mouse_body        = cpBodyNewKinematic();
static cpConstraint* mouse_joint = nullptr;

char const* ChipmunkDemoMessageString = nullptr;

#define GRABBABLE_MASK_BIT (1 << 31)
cpShapeFilter GRAB_FILTER          = {CP_NO_GROUP, (unsigned int)GRABBABLE_MASK_BIT, (unsigned int)GRABBABLE_MASK_BIT};
cpShapeFilter NOT_GRABBABLE_FILTER = {CP_NO_GROUP, ~GRABBABLE_MASK_BIT, ~GRABBABLE_MASK_BIT};

ax::DrawNode* drawCP = nullptr;

void ChipmunkDemoDefaultDrawImpl(cpSpace* space){};

void ChipmunkDebugDrawDot(cpFloat size, cpVect pos, cpSpaceDebugColor fillColor)
{

    drawCP->drawPoint(Vec2(pos.x, pos.y) + physicsDebugNodeOffset, size,
                      Color4F(fillColor.r, fillColor.g, fillColor.b, fillColor.a));
}

void ChipmunkDebugDrawCircle(cpVect pos,
                             cpFloat angle,
                             cpFloat radius,
                             cpSpaceDebugColor outlineColor,
                             cpSpaceDebugColor fillColor)
{

    drawCP->drawCircle(Vec2(pos.x, pos.y) + physicsDebugNodeOffset, 100, AX_DEGREES_TO_RADIANS(90), 50, true, 1.0f,
                       2.0f, Color4F(fillColor.r, fillColor.g, fillColor.b, fillColor.a));
}

void ChipmunkDebugDrawSegment(cpVect a, cpVect b, cpSpaceDebugColor color)
{
    drawCP->drawLine(Vec2(a.x, a.y) + physicsDebugNodeOffset, Vec2(b.x, b.y) + physicsDebugNodeOffset,
                     Color4F(color.r, color.g, color.b, color.a));
}

void ChipmunkDebugDrawFatSegment(cpVect a,
                                 cpVect b,
                                 cpFloat radius,
                                 cpSpaceDebugColor outlineColor,
                                 cpSpaceDebugColor fillColor)
{

    drawCP->drawSegment(Vec2(a.x, a.y) + physicsDebugNodeOffset, Vec2(b.x, b.y) + physicsDebugNodeOffset, radius,
                        Color4F(outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a));
}

void ChipmunkDebugDrawPolygon(int count,
                              const cpVect* verts,
                              cpFloat radius,
                              cpSpaceDebugColor outlineColor,
                              cpSpaceDebugColor fillColor)
{

    Vec2* vec = new Vec2[count];
    for (size_t i = 0; i < count; i++)
    {
        vec[i] = cpVert2Point(verts[i]);
    }
    drawCP->drawPolygon(vec, count, Color4F(1.0f, 0.0f, 0.0f, 0.5f), radius, Color4F(0.0f, 0.0f, 1.0f, 1.0f));

    delete[] vec;
}

void ChipmunkDebugDrawBB(cpBB bb, cpSpaceDebugColor color)
{
    Vec2 verts[] = {
        Vec2(bb.r, bb.b) + physicsDebugNodeOffset,
        Vec2(bb.r, bb.t) + physicsDebugNodeOffset,
        Vec2(bb.l, bb.t) + physicsDebugNodeOffset,
        Vec2(bb.l, bb.b) + physicsDebugNodeOffset,
    };
    drawCP->drawPolygon(verts, sizeof(verts) / sizeof(verts[0]), Color4F(1.0f, 0.0f, 0.0f, 0.0f), 1,
                        Color4F(0.0f, 0.0f, 1.0f, 1.0f));
}

ax::Label* label;

static int max_arbiters    = 0;
static int max_points      = 0;
static int max_constraints = 0;

void ChipmunkTestBed::DrawInfo()
{
    int arbiters = _space->arbiters->num;
    int points   = 0;

    for (int i = 0; i < arbiters; i++)
        points += ((cpArbiter*)(_space->arbiters->arr[i]))->count;

    int constraints = (_space->constraints->num + points) * _space->iterations;

    max_arbiters    = arbiters > max_arbiters ? arbiters : max_arbiters;
    max_points      = points > max_points ? points : max_points;
    max_constraints = constraints > max_constraints ? constraints : max_constraints;

    char buffer[1024];
    const char* format =
        "Arbiters: %d (%d) - "
        "Contact Points: %d (%d)\n"
        "Other Constraints: %d, Iterations: %d\n"
        "Constraints x Iterations: %d (%d)\n"
        "Time:% 5.2fs, KE:% 5.2e";

    cpArray* bodies = _space->dynamicBodies;
    cpFloat ke      = 0.0f;
    for (int i = 0; i < bodies->num; i++)
    {
        cpBody* body = (cpBody*)bodies->arr[i];
        if (body->m == INFINITY || body->i == INFINITY)
            continue;

        ke += body->m * cpvdot(body->v, body->v) + body->i * body->w * body->w;
    }

    sprintf(buffer, format, arbiters, max_arbiters, points, max_points, _space->constraints->num, _space->iterations,
            constraints, max_constraints, ChipmunkDemoTime, (ke < 1e-10f ? 0.0f : ke));

    drawInfo->setString(buffer);
}

static char PrintStringBuffer[1024 * 8];
static char* PrintStringCursor;
void ChipmunkDemoPrintString(char const* fmt, ...)
{
    if (PrintStringCursor == nullptr)
    {
        return;
    }

    ChipmunkDemoMessageString = PrintStringBuffer;

    va_list args;
    va_start(args, fmt);
    int remaining   = sizeof(PrintStringBuffer) - (PrintStringCursor - PrintStringBuffer);
    int would_write = vsnprintf(PrintStringCursor, remaining, fmt, args);
    if (would_write > 0 && would_write < remaining)
    {
        PrintStringCursor += would_write;
    }
    else
    {
        // encoding error or overflow, prevent further use until reinitialized
        PrintStringCursor = nullptr;
    }
    va_end(args);

    label->setString(ChipmunkDemoMessageString);
}

cpSpaceDebugColor RGBAColor(float r, float g, float b, float a)
{
    cpSpaceDebugColor color = {r, g, b, a};
    return color;
};

cpSpaceDebugColor LAColor(float l, float a)
{
    cpSpaceDebugColor color = {l, l, l, a};
    return color;
}

static void ShapeFreeWrap(cpSpace* space, cpShape* shape, void* unused)
{
    cpSpaceRemoveShape(space, shape);
    cpShapeFree(shape);
}

static void PostShapeFree(cpShape* shape, cpSpace* space)
{
    cpSpaceAddPostStepCallback(space, (cpPostStepFunc)ShapeFreeWrap, shape, nullptr);
}

static void ConstraintFreeWrap(cpSpace* space, cpConstraint* constraint, void* unused)
{
    mouse_joint = nullptr;
    cpSpaceRemoveConstraint(space, constraint);
    cpConstraintFree(constraint);
}

static void PostConstraintFree(cpConstraint* constraint, cpSpace* space)
{
    cpSpaceAddPostStepCallback(space, (cpPostStepFunc)ConstraintFreeWrap, constraint, nullptr);
}

static void BodyFreeWrap(cpSpace* space, cpBody* body, void* unused)
{
    cpSpaceRemoveBody(space, body);
    cpBodyFree(body);
}

static void PostBodyFree(cpBody* body, cpSpace* space)
{
    cpSpaceAddPostStepCallback(space, (cpPostStepFunc)BodyFreeWrap, body, nullptr);
}

// Safe and future proof way to remove and free all objects that have been added to the space.
void ChipmunkDemoFreeSpaceChildren(cpSpace* space)
{
    // Must remove these BEFORE freeing the body or you will access dangling pointers.
    cpSpaceEachShape(space, (cpSpaceShapeIteratorFunc)PostShapeFree, space);
    cpSpaceEachConstraint(space, (cpSpaceConstraintIteratorFunc)PostConstraintFree, space);
    cpSpaceEachBody(space, (cpSpaceBodyIteratorFunc)PostBodyFree, space);
}

void updateMouseBody(void)
{
    cpVect new_point = cpvlerp(mouse_body->p, ChipmunkDemoMouse, 0.25f);
    mouse_body->v    = cpvmult(cpvsub(new_point, mouse_body->p), 60.0f);
    mouse_body->p    = new_point;
}

ChipmunkTestBed::ChipmunkTestBed()
{
    // halx99: since axmol init scene default camera at 'initWithXXX' function, only change design size at scene
    // construct is ok see also: https://github.com/axmolengine/axmol/commit/581a7921554c09746616759d5a5ca6ce9d3eaa22
    auto director = Director::getInstance();
    auto glView   = director->getGLView();
    Size designSize(g_designSize.width * 0.85, g_designSize.height * 0.85);
    glView->setDesignResolutionSize(designSize.width, designSize.height, ResolutionPolicy::SHOW_ALL);

    // creating a keyboard event listener
    auto listener          = EventListenerKeyboard::create();
    listener->onKeyPressed = [](EventKeyboard::KeyCode keyCode, Event* event) {
        if ((int)keyCode == 26)  // Left
        {
            ChipmunkDemoKeyboard.x--;
        }
        if ((int)keyCode == 27)  // Right
        {
            ChipmunkDemoKeyboard.x++;
        }
        if ((int)keyCode == 28)  // Up
        {
            ChipmunkDemoKeyboard.y++;
        }
        if ((int)keyCode == 29)  // Down
        {
            ChipmunkDemoKeyboard.y--;
        }
    };

    listener->onKeyReleased = [](EventKeyboard::KeyCode keyCode, Event* event) { ChipmunkDemoKeyboard = {0, 0}; };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    // creating a mouse event listener
    _mouseListener              = EventListenerMouse::create();
    _mouseListener->onMouseMove = AX_CALLBACK_1(ChipmunkTestBed::onMouseMove, this);
    _mouseListener->onMouseUp   = AX_CALLBACK_1(ChipmunkTestBed::onMouseUp, this);
    _mouseListener->onMouseDown = AX_CALLBACK_1(ChipmunkTestBed::onMouseDown, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_mouseListener, this);

    // Some info text
    auto label1 = Label::createWithTTF("Use the mouse to grab objects.", "fonts/Marker Felt.ttf", 12.0f);
    label1->setPosition(VisibleRect::center().x, VisibleRect::top().y - 100);
    label1->setColor(Color3B::WHITE);
    this->addChild(label1, 1000);

    // ChipmunkDemoMessageString
    label = Label::createWithTTF("", "fonts/Marker Felt.ttf", 12.0f);
    label->setPosition(VisibleRect::center().x, VisibleRect::top().y - 130);
    label->setColor(Color3B::MAGENTA);
    this->addChild(label, 1000);

    drawInfo = Label::createWithTTF("Use the mouse to grab objects.", "fonts/Marker Felt.ttf", 12.0f);
    drawInfo->setAnchorPoint(Vec2(0, 0));
    drawInfo->setPosition(VisibleRect::left().x + 10, VisibleRect::top().y - 60);
    drawInfo->setColor(Color3B::WHITE);
    this->addChild(drawInfo, 1000);

    draw = DrawNode::create();
    addChild(draw, 100);

    drawCP = draw;

    scheduleUpdate();
}

ChipmunkTestBed::~ChipmunkTestBed()
{
    ChipmunkDemoFreeSpaceChildren(_space);
    _eventDispatcher->removeEventListener(_mouseListener);
}

void ChipmunkTestBed::initPhysics()
{
    if (ChipmunkDemoMessageString)
    {
        label->setString(ChipmunkDemoMessageString);
    }
    else
    {
        label->setString("");
    }
    drawCP->clear();
    // Physics debug layer
    _debugLayer = PhysicsDebugNodeChipmunk2D::create(_space);
    this->addChild(_debugLayer, Z_PHYSICS_DEBUG);
}

void ChipmunkTestBed::update(float delta)
{
    //#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
    //    cpSpaceStep(_space, delta);
    //#else
    //    cpHastySpaceStep(_space, delta);
    //#endif
}

void ChipmunkTestBed::createResetButton()
{
    auto reset = MenuItemImage::create("Images/r1.png", "Images/r2.png", AX_CALLBACK_1(ChipmunkTestBed::reset, this));
    auto menu  = Menu::create(reset, nullptr);
    menu->setPosition(VisibleRect::center().x, VisibleRect::bottom().y);
    this->addChild(menu, -1);
}

void ChipmunkTestBed::reset(Object* sender)
{
    ChipmunkDemoFreeSpaceChildren(_space);
    getTestSuite()->restartCurrTest();
}

void ChipmunkTestBed::onEnter()
{
    TestCase::onEnter();
    physicsDebugNodeOffset = VisibleRect::center();
    physicsDebugNodeOffset.y += 20;
    ChipmunkDemoMessageString = "";
    label->setString("");
}

void ChipmunkTestBed::onMouseDown(Event* event)
{
    EventMouse* e = (EventMouse*)event;

    if ((int)e->getMouseButton() == 0)
    {
        ChipmunkDemoLeftDown = cpTrue;
        // give the mouse click a little radius to make it easier to click small shapes.
        cpFloat radius = 5.0;

        cpPointQueryInfo info = {0};
        cpShape* shape        = cpSpacePointQueryNearest(_space, ChipmunkDemoMouse, radius, GRAB_FILTER, &info);

        if (shape && cpBodyGetMass(cpShapeGetBody(shape)) < INFINITY)
        {
            // Use the closest point on the surface if the click is outside of the shape.
            cpVect nearest = (info.distance > 0.0f ? info.point : ChipmunkDemoMouse);

            cpBody* body           = cpShapeGetBody(shape);
            mouse_joint            = cpPivotJointNew2(mouse_body, body, cpvzero, cpBodyWorldToLocal(body, nearest));
            mouse_joint->maxForce  = 50000.0f;
            mouse_joint->errorBias = cpfpow(1.0f - 0.15f, 60.0f);
            cpSpaceAddConstraint(_space, mouse_joint);
        }
    }
    else if ((int)e->getMouseButton() == 1)
    {
        if (mouse_joint)
        {
            cpSpaceRemoveConstraint(_space, mouse_joint);
            cpConstraintFree(mouse_joint);
            mouse_joint = nullptr;
        }
        ChipmunkDemoRightDown  = cpTrue;
        ChipmunkDemoRightClick = cpTrue;
    }
}

void ChipmunkTestBed::onMouseUp(Event* event)
{
    EventMouse* e = (EventMouse*)event;
    mousePresses  = false;
    if (mouse_joint)
    {
        cpSpaceRemoveConstraint(_space, mouse_joint);
        cpConstraintFree(mouse_joint);
        mouse_joint = nullptr;
    }
    ChipmunkDemoLeftDown   = cpFalse;
    ChipmunkDemoRightDown  = cpFalse;
    ChipmunkDemoRightClick = cpFalse;
}

void ChipmunkTestBed::onMouseMove(Event* event)
{
    EventMouse* e = (EventMouse*)event;
    auto pt = e->getLocation();
    ChipmunkDemoMouse.x = pt.x - physicsDebugNodeOffset.x;
    ChipmunkDemoMouse.y = pt.y - physicsDebugNodeOffset.y;

    cpBodySetPosition(mouse_body, ChipmunkDemoMouse);
}

void ChipmunkTestBed::updateInit(ChipmunkDemo tt)
{
    PrintStringBuffer[0] = 0;
    PrintStringCursor    = PrintStringBuffer;

    drawCP->clear();
    updateMouseBody();
    ChipmunkDemoTime += tt.timestep;
    ChipmunkTestBed::DrawInfo();
    tt.updateFunc(_space, tt.timestep);
}

//------------------------------------------------------------------
//
// LogoSmashDemo
//
//------------------------------------------------------------------
void LogoSmashDemo::onEnter()
{
    ChipmunkTestBed::onEnter();
    initPhysics();
}

std::string LogoSmashDemo::title() const
{
    return LogoSmash.name;
}

void LogoSmashDemo::initPhysics()
{
    _space = LogoSmash.initFunc();
    ChipmunkTestBed::initPhysics();
}

void LogoSmashDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(LogoSmash);
}

//------------------------------------------------------------------
//
// PlinkDemo
//
//------------------------------------------------------------------
void PlinkDemo::onEnter()
{
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string PlinkDemo::title() const
{
    return Plink.name;
}

void PlinkDemo::initPhysics()
{
    _space = Plink.initFunc();
    ChipmunkTestBed::initPhysics();
}

void PlinkDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(Plink);
}

//------------------------------------------------------------------
//
// TumbleDemo
//
//------------------------------------------------------------------
void TumbleDemo::onEnter()
{
    ChipmunkTestBed::onEnter();
    initPhysics();
}

std::string TumbleDemo::title() const
{
    return Tumble.name;
}

void TumbleDemo::initPhysics()
{
    _space = Tumble.initFunc();
    ChipmunkTestBed::initPhysics();
}

void TumbleDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(Tumble);
}

//------------------------------------------------------------------
//
// PyramidStackDemo
//
//------------------------------------------------------------------
void PyramidStackDemo::onEnter()
{
    ChipmunkTestBed::onEnter();
    initPhysics();
}

std::string PyramidStackDemo::title() const
{
    return PyramidStack.name;
}

void PyramidStackDemo::initPhysics()
{
    _space = PyramidStack.initFunc();
    ChipmunkTestBed::initPhysics();
}

void PyramidStackDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(PyramidStack);
}

//------------------------------------------------------------------
//
// PyramidToppleDemo
//
//------------------------------------------------------------------
void PyramidToppleDemo::onEnter()
{
    ChipmunkTestBed::onEnter();
    initPhysics();
}

std::string PyramidToppleDemo::title() const
{
    return PyramidTopple.name;
}

void PyramidToppleDemo::initPhysics()
{
    _space = PyramidTopple.initFunc();
    ChipmunkTestBed::initPhysics();
}

void PyramidToppleDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(PyramidTopple);
}

//------------------------------------------------------------------
//
// ChainDemo
//
//------------------------------------------------------------------
void ChainsDemo::onEnter()
{
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string ChainsDemo::title() const
{
    return Chains.name;
}

void ChainsDemo::initPhysics()
{
    _space = Chains.initFunc();
    ChipmunkTestBed::initPhysics();
}

void ChainsDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(Chains);
}

//------------------------------------------------------------------
//
// OneWayDemo
//
//------------------------------------------------------------------
void OneWayDemo::onEnter()
{
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string OneWayDemo::title() const
{
    return OneWay.name;
}

void OneWayDemo::initPhysics()
{
    _space = OneWay.initFunc();
    ChipmunkTestBed::initPhysics();
}

void OneWayDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(OneWay);
}

//------------------------------------------------------------------
//
// PlanetDemo
//
//------------------------------------------------------------------
void PlanetDemo::onEnter()
{
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string PlanetDemo::title() const
{
    return Planet.name;
}

void PlanetDemo::initPhysics()
{
    _space = Planet.initFunc();
    ChipmunkTestBed::initPhysics();
}

void PlanetDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(Planet);
}

//------------------------------------------------------------------
//
// TheoJansenDemo
//
//------------------------------------------------------------------
void TheoJansenDemo::onEnter()
{
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string TheoJansenDemo::title() const
{
    return TheoJansen.name;
}

void TheoJansenDemo::initPhysics()
{
    _space = TheoJansen.initFunc();
    ChipmunkTestBed::initPhysics();
}

void TheoJansenDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(TheoJansen);
}

//------------------------------------------------------------------
//
// TankDemo
//
//------------------------------------------------------------------
void TankDemo::onEnter()
{
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string TankDemo::title() const
{
    return Tank.name;
}

void TankDemo::initPhysics()
{
    _space = Tank.initFunc();
    ChipmunkTestBed::initPhysics();
}

void TankDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(Tank);
}

//------------------------------------------------------------------
//
// BenchDemo
//
//------------------------------------------------------------------
void BenchDemo::onEnter()
{
    ChipmunkTestBed::onEnter();

    auto itemPrev = MenuItemImage::create("Images/b1.png", "Images/b2.png", [&](Object* sender) {
        bench = (bench > 0) ? bench - 1 : (bench_count - 1);
        reset(sender);
    });

    auto itemNext = MenuItemImage::create("Images/f1.png", "Images/f2.png", [&](Object* sender) {
        bench = (bench < (bench_count - 1)) ? bench + 1 : 0;
        reset(sender);
    });

    auto s = Director::getInstance()->getWinSize();

    auto menuPrev = Menu::create(itemPrev, nullptr);
    menuPrev->alignItemsHorizontally();
    menuPrev->setScale(0.4);
    menuPrev->setAnchorPoint(Vec2(0.0f, 0.0f));
    menuPrev->setPosition(Vec2(s.width / 2 - 45, 23.0f));
    addChild(menuPrev);

    auto menuNext = Menu::create(itemNext, nullptr);
    menuNext->alignItemsHorizontally();
    menuNext->setScale(0.4);
    menuNext->setAnchorPoint(Vec2(0.0f, 0.0f));
    menuNext->setPosition(Vec2(s.width / 2 + 45, 23.0f));
    addChild(menuNext);

    initPhysics();
}

std::string BenchDemo::title() const
{
    return "Bench";
}

std::string BenchDemo::subtitle() const
{
    std::string s = std::to_string(bench + 1) + ". " + bench_list[bench].name;
    return s.c_str();
}

void BenchDemo::initPhysics()
{
    _space = bench_list[bench].initFunc();
    ChipmunkTestBed::initPhysics();
}

void BenchDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(bench_list[bench]);
}

//------------------------------------------------------------------
//
// SpringiesDemo
//
//------------------------------------------------------------------
void SpringiesDemo::onEnter()
{
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string SpringiesDemo::title() const
{
    return Springies.name;
}

void SpringiesDemo::initPhysics()
{
    _space = Springies.initFunc();
    ChipmunkTestBed::initPhysics();
}

void SpringiesDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(Springies);
}

//------------------------------------------------------------------
//
// ShatterDemo
//
//------------------------------------------------------------------
void ShatterDemo::onEnter()
{
    ChipmunkTestBed::onEnter();
    initPhysics();
}

std::string ShatterDemo::title() const
{
    return Shatter.name;
}

void ShatterDemo::initPhysics()
{
    _space = Shatter.initFunc();
    ChipmunkTestBed::initPhysics();
}

void ShatterDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(Shatter);
}

//------------------------------------------------------------------
//
// StickyDemo
//
//------------------------------------------------------------------
void StickyDemo::onEnter()
{
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string StickyDemo::title() const
{
    return Sticky.name;
}

void StickyDemo::initPhysics()
{
    _space = Sticky.initFunc();
    ChipmunkTestBed::initPhysics();
}

void StickyDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(Sticky);
}

//------------------------------------------------------------------
//
// CraneDemo
//
//------------------------------------------------------------------
void CraneDemo::onEnter()
{
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string CraneDemo::title() const
{
    return Crane.name;
}

void CraneDemo::initPhysics()
{
    _space = Crane.initFunc();
    ChipmunkTestBed::initPhysics();
}

void CraneDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(Crane);
}

//------------------------------------------------------------------
//
// JointsDemo
//
//------------------------------------------------------------------
void JointsDemo::onEnter()
{
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string JointsDemo::title() const
{
    return Joints.name;
}

void JointsDemo::initPhysics()
{
    _space = Joints.initFunc();
    ChipmunkTestBed::initPhysics();
}

void JointsDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(Joints);
}

//------------------------------------------------------------------
//
// ConvexDemo
//
//------------------------------------------------------------------
void ConvexDemo::onEnter()
{
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string ConvexDemo::title() const
{
    return Convex.name;
}

void ConvexDemo::initPhysics()
{
    _space = Convex.initFunc();
    ChipmunkTestBed::initPhysics();
}

void ConvexDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(Convex);
}

//------------------------------------------------------------------
//
// PumpDemo
//
//------------------------------------------------------------------
void PumpDemo::onEnter()
{
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string PumpDemo::title() const
{
    return Pump.name;
}

void PumpDemo::initPhysics()
{
    _space = Pump.initFunc();
    ChipmunkTestBed::initPhysics();
}

void PumpDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(Pump);
}

//------------------------------------------------------------------
//
// PlatformerPlayerDemo
//
//------------------------------------------------------------------
void PlatformerPlayerDemo::onEnter()
{
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string PlatformerPlayerDemo::title() const
{
    return PlatformerPlayer.name;
}

void PlatformerPlayerDemo::initPhysics()
{
    _space = PlatformerPlayer.initFunc();
    ChipmunkTestBed::initPhysics();
}

void PlatformerPlayerDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(PlatformerPlayer);
}

//------------------------------------------------------------------
//
// QueryDemo
//
//------------------------------------------------------------------
void QueryDemo::onEnter()
{
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string QueryDemo::title() const
{
    return Query.name;
}

void QueryDemo::initPhysics()
{
    _space = Query.initFunc();
    ChipmunkTestBed::initPhysics();
}

void QueryDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(Query);
}

//------------------------------------------------------------------
//
// ContactGraphDemo
//
//------------------------------------------------------------------
void ContactGraphDemo::onEnter()
{
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string ContactGraphDemo::title() const
{
    return ContactGraph.name;
}

void ContactGraphDemo::initPhysics()
{
    _space = ContactGraph.initFunc();
    ChipmunkTestBed::initPhysics();
}

void ContactGraphDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(ContactGraph);
}

//------------------------------------------------------------------
//
// BuoyancyDemo
//
//------------------------------------------------------------------
void BuoyancyDemo::onEnter()
{
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string BuoyancyDemo::title() const
{
    return Buoyancy.name;
}

void BuoyancyDemo::initPhysics()
{
    _space = Buoyancy.initFunc();
    ChipmunkTestBed::initPhysics();
}

void BuoyancyDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(Buoyancy);
}

//------------------------------------------------------------------
//
// SliceDemo
//
//------------------------------------------------------------------
void SliceDemo::onEnter()
{
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string SliceDemo::title() const
{
    return Slice.name;
}

void SliceDemo::initPhysics()
{
    _space = Slice.initFunc();
    ChipmunkTestBed::initPhysics();
}

void SliceDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(Slice);
}

//------------------------------------------------------------------
//
// UnicycleDemo
//
//------------------------------------------------------------------
void UnicycleDemo::onEnter()
{
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string UnicycleDemo::title() const
{
    return Unicycle.name;
}

void UnicycleDemo::initPhysics()
{
    _space = Unicycle.initFunc();
    ChipmunkTestBed::initPhysics();
}

void UnicycleDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(Unicycle);
}

//------------------------------------------------------------------
//
// ExampleDemo
//
//------------------------------------------------------------------
void ExampleDemo::onEnter()
{
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string ExampleDemo::title() const
{
    return Example.name;
}

void ExampleDemo::initPhysics()
{
    _space = Example.initFunc();
    ChipmunkTestBed::initPhysics();
}

void ExampleDemo::update(float delta)
{
    ChipmunkTestBed::updateInit(Example);
}

ChipmunkTestBedTests::ChipmunkTestBedTests()
{
    ADD_TEST_CASE(BenchDemo);
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

    ADD_TEST_CASE(SpringiesDemo);

    ADD_TEST_CASE(ShatterDemo);
    ADD_TEST_CASE(StickyDemo);
    ADD_TEST_CASE(CraneDemo);
    ADD_TEST_CASE(JointsDemo);

    ADD_TEST_CASE(ConvexDemo);

    ADD_TEST_CASE(PumpDemo);
    ADD_TEST_CASE(PlatformerPlayerDemo);

    ADD_TEST_CASE(QueryDemo);
    ADD_TEST_CASE(ContactGraphDemo);
    ADD_TEST_CASE(BuoyancyDemo);
    ADD_TEST_CASE(SliceDemo);
    ADD_TEST_CASE(UnicycleDemo);

    ADD_TEST_CASE(ExampleDemo);  //  the template for your one examples/Demos
}
