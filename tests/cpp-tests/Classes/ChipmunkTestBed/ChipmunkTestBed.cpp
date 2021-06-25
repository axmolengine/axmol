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

// Meh, just max out 16 bit index size.
#define VERTEX_MAX (64 * 1024)
#define INDEX_MAX  (4 * VERTEX_MAX)

// static sg_buffer VertexBuffer, IndexBuffer;
static size_t VertexCount, IndexCount;

static Vertex Vertexes[VERTEX_MAX];
static Index Indexes[INDEX_MAX];

#define GRABBABLE_MASK_BIT (1 << 31)
cpShapeFilter GRAB_FILTER          = {CP_NO_GROUP, GRABBABLE_MASK_BIT, GRABBABLE_MASK_BIT};
cpShapeFilter NOT_GRABBABLE_FILTER = {CP_NO_GROUP, ~GRABBABLE_MASK_BIT, ~GRABBABLE_MASK_BIT};

void ChipmunkDemoDefaultDrawImpl(cpSpace* space){};
static Vertex* push_vertexes(size_t vcount, const Index* index_src, size_t icount) {
    //   cpAssertHard(VertexCount + vcount <= VERTEX_MAX && IndexCount + icount <= INDEX_MAX, "Geometry buffer full.");

    Vertex* vertex_dst = Vertexes + VertexCount;
    size_t base        = VertexCount;
    VertexCount += vcount;

    Index* index_dst = Indexes + IndexCount;
    for (size_t i = 0; i < icount; i++)
        index_dst[i] = index_src[i] + (Index) base;
    IndexCount += icount;

    return vertex_dst;
}


cocos2d::DrawNode* drawCP = NULL;

void ChipmunkDebugDrawDot(cpFloat size, cpVect pos, cpSpaceDebugColor fillColor) {
    drawCP->drawPoint(
        Vec2(pos.x, pos.y) + physicsDebugNodeOffset, 1, Color4F(fillColor.r, fillColor.g, fillColor.b, fillColor.a));
}

void ChipmunkDebugDrawCircle(
    cpVect pos, cpFloat angle, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor) {
    drawCP->drawCircle(Vec2(pos.x, pos.y) + physicsDebugNodeOffset, 100, CC_DEGREES_TO_RADIANS(90), 50, true, 1.0f,
        2.0f, Color4F(fillColor.r, fillColor.g, fillColor.b, fillColor.a));
}

void ChipmunkDebugDrawSegment(cpVect a, cpVect b, cpSpaceDebugColor color) {
    auto aa = Vec2(a.x, a.y) + physicsDebugNodeOffset;
    auto bb = Vec2(b.x, b.y) + physicsDebugNodeOffset;

    drawCP->drawLine(aa, bb, Color4F(color.r, color.g, color.b, color.a));
}

void ChipmunkDebugDrawFatSegment(
    cpVect a, cpVect b, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor) {
    auto aa = Vec2(a.x, a.y) + physicsDebugNodeOffset;
    auto bb = Vec2(b.x, b.y) + physicsDebugNodeOffset;

    drawCP->drawSegment(aa, bb, radius, Color4F(outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a));
}

#define MAX_POLY_VERTEXES 64
// Fill needs (count - 2) triangles.
// Outline needs 4*count triangles.
#define MAX_POLY_INDEXES (3 * (5 * MAX_POLY_VERTEXES - 2))

void ChipmunkDebugDrawPolygon(
    int count, const cpVect* verts, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor) {
    return;


    //    auto s = Director::getInstance()->getWinSize();


    // star poly (triggers buggs)


    float inset = (float) -cpfmax(0, 2 * ChipmunkDebugDrawPointLineScale - radius);
    // float outset = (float) radius + ChipmunkDebugDrawPointLineScale;
    // float r      = outset - inset;

    Index indexes[MAX_POLY_INDEXES];

    Vertex* vertexes = push_vertexes(4 * count, indexes, 3 * (5 * count - 2));
    for (int i = 0; i < count; i++) {
        cpVect v0     = verts[i];
        cpVect v_prev = verts[(i + (count - 1)) % count];
        cpVect v_next = verts[(i + (count + 1)) % count];

        cpVect n1 = cpvnormalize(cpvrperp(cpvsub(v0, v_prev)));
        cpVect n2 = cpvnormalize(cpvrperp(cpvsub(v_next, v0)));
        cpVect of = cpvmult(cpvadd(n1, n2), 1.0 / (cpvdot(n1, n2) + 1.0f));
        cpVect v  = cpvadd(v0, cpvmult(of, inset));

        // vertexes[4 * i + 0] = (Vertex){{(float) v.x, (float) v.y}, {0.0f, 0.0f}, 0.0f, fill, outline};
        // vertexes[4 * i + 1] = (Vertex){{(float) v.x, (float) v.y}, {(float) n1.x, (float) n1.y}, r, fill, outline};
        // vertexes[4 * i + 2] = (Vertex){{(float) v.x, (float) v.y}, {(float) of.x, (float) of.y}, r, fill, outline};
        // vertexes[4 * i + 3] = (Vertex){{(float) v.x, (float) v.y}, {(float) n2.x, (float) n2.y}, r, fill, outline};
    }


    const float o = 80;
    const float w = 20;
    const float h = 50;
    Vec2 star[]   = {
        Vec2(o + w, o - h), Vec2(o + w * 2, o), // lower spike
        Vec2(o + w * 2 + h, o + w), Vec2(o + w * 2, o + w * 2), // right spike
        {o + w, o + w * 2 + h}, {o, o + w * 2}, // top spike
        {o - h, o + w}, {o, o}, // left spike
    };


    drawCP->drawPolygon(
        star, sizeof(star) / sizeof(star[0]), Color4F(1.0f, 0.0f, 0.0f, 0.5f), 1, Color4F(0.0f, 0.0f, 1.0f, 1.0f));


    // RGBA8 fill = cp_to_rgba(fillColor), outline = cp_to_rgba(outlineColor);


    //// Polygon fill triangles.
    // for (int i = 0; i < count - 2; i++) {
    //    indexes[3 * i + 0] = 0;
    //    indexes[3 * i + 1] = 4 * (i + 1);
    //    indexes[3 * i + 2] = 4 * (i + 2);
    //}

    //// Polygon outline triangles.
    // Index* cursor = indexes + 3 * (count - 2);
    // for (int i0 = 0; i0 < count; i0++) {
    //    int i1               = (i0 + 1) % count;
    //    cursor[12 * i0 + 0]  = 4 * i0 + 0;
    //    cursor[12 * i0 + 1]  = 4 * i0 + 1;
    //    cursor[12 * i0 + 2]  = 4 * i0 + 2;
    //    cursor[12 * i0 + 3]  = 4 * i0 + 0;
    //    cursor[12 * i0 + 4]  = 4 * i0 + 2;
    //    cursor[12 * i0 + 5]  = 4 * i0 + 3;
    //    cursor[12 * i0 + 6]  = 4 * i0 + 0;
    //    cursor[12 * i0 + 7]  = 4 * i0 + 3;
    //    cursor[12 * i0 + 8]  = 4 * i1 + 0;
    //    cursor[12 * i0 + 9]  = 4 * i0 + 3;
    //    cursor[12 * i0 + 10] = 4 * i1 + 0;
    //    cursor[12 * i0 + 11] = 4 * i1 + 1;
    //}
}

void ChipmunkDebugDrawBB(cpBB bb, cpSpaceDebugColor color) {
    Vec2 verts[] = {
        Vec2(bb.r, bb.b) + physicsDebugNodeOffset,
        Vec2(bb.r, bb.t) + physicsDebugNodeOffset,
        Vec2(bb.l, bb.t) + physicsDebugNodeOffset,
        Vec2(bb.l, bb.b) + physicsDebugNodeOffset,
    };
    drawCP->drawPolygon(
        verts, sizeof(verts) / sizeof(verts[0]), Color4F(1.0f, 0.0f, 0.0f, 0.0f), 1, Color4F(0.0f, 0.0f, 1.0f, 1.0f));
}


cocos2d::Label* label;

static char PrintStringBuffer[1024 * 8];
static char* PrintStringCursor;

void ChipmunkDemoPrintString(char const* fmt, ...) {
    if (PrintStringCursor == NULL) {
        return;
    }

    ChipmunkDemoMessageString = PrintStringBuffer;

    va_list args;
    va_start(args, fmt);
    int remaining   = sizeof(PrintStringBuffer) - (PrintStringCursor - PrintStringBuffer);
    int would_write = vsnprintf(PrintStringCursor, remaining, fmt, args);
    if (would_write > 0 && would_write < remaining) {
        PrintStringCursor += would_write;
    } else {
        // encoding error or overflow, prevent further use until reinitialized
        PrintStringCursor = NULL;
    }
    va_end(args);

    label->setString(ChipmunkDemoMessageString);
}

cpSpaceDebugColor RGBAColor(float r, float g, float b, float a) {
    cpSpaceDebugColor color = {r, g, b, a};
    return color;
};

cpSpaceDebugColor LAColor(float l, float a) {
    cpSpaceDebugColor color = {l, l, l, a};
    return color;
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


void updateMouseBody(void) {
    cpVect new_point = cpvlerp(mouse_body->p, ChipmunkDemoMouse, 0.25f);
    mouse_body->v    = cpvmult(cpvsub(new_point, mouse_body->p), 60.0f);
    mouse_body->p    = new_point;
}

static Rect getRect(Node* node) {
    Rect rc;
    rc.origin = node->getPosition();
    rc.size   = node->getContentSize();
    rc.origin.x -= rc.size.width / 2;
    rc.origin.y -= rc.size.height / 2;
    return rc;
}

ChipmunkTestBed::ChipmunkTestBed() {
    // halx99: since adxe init scene default camera at 'initWithXXX' function, only change design size at scene
    // construct is ok see also: https://github.com/adxeproject/adxe/commit/581a7921554c09746616759d5a5ca6ce9d3eaa22
    auto director = Director::getInstance();
    auto glview   = director->getOpenGLView();
    Size designSize(960 * 0.8, 640 * 0.8);
    glview->setDesignResolutionSize(designSize.width, designSize.height, ResolutionPolicy::NO_BORDER);

    // creating a keyboard event listener
    auto listener          = EventListenerKeyboard::create();
    listener->onKeyPressed = [](EventKeyboard::KeyCode keyCode, Event* event) {
        switch ((int) keyCode) {
        case 28: // Up
            ChipmunkDemoKeyboard.y++;
            break;
        case 29: // Down
            ChipmunkDemoKeyboard.y--;
            break;
        case 27: // Right
            ChipmunkDemoKeyboard.x++;
            break;
        case 26: // Left
            ChipmunkDemoKeyboard.x--;
            break;
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    // creating a mouse event listener
    _mouseListener              = EventListenerMouse::create();
    _mouseListener->onMouseMove = CC_CALLBACK_1(ChipmunkTestBed::onMouseMove, this);
    _mouseListener->onMouseUp   = CC_CALLBACK_1(ChipmunkTestBed::onMouseUp, this);
    _mouseListener->onMouseDown = CC_CALLBACK_1(ChipmunkTestBed::onMouseDown, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_mouseListener, this);

    // ChipmunkDemoMessageString
    label = Label::createWithTTF("", "fonts/Marker Felt.ttf", 20.0f);
    //  label->setAnchorPoint(Vec2(0, 1));
    label->setPosition(VisibleRect::center().x, VisibleRect::top().y - 100);
    label->setColor(Color3B::MAGENTA);
    this->addChild(label, 1000);

    draw = DrawNode::create();
    addChild(draw, 10);

    drawCP = draw;

    scheduleUpdate();
}


ChipmunkTestBed::~ChipmunkTestBed() {
    ChipmunkDemoFreeSpaceChildren(_space);
    _eventDispatcher->removeEventListener(_mouseListener);
}


void ChipmunkTestBed::initPhysics() {
    if (ChipmunkDemoMessageString) {
        label->setString(ChipmunkDemoMessageString);
    } else {
        label->setString("");
    }
    drawCP->clear();
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
    physicsDebugNodeOffset    = VisibleRect::center();
    ChipmunkDemoMessageString = "";
    label->setString("");
}


void ChipmunkTestBed::onMouseDown(Event* event) {
    EventMouse* e = (EventMouse*) event;
    if ((int) e->getMouseButton() == 0) {
        ChipmunkDemoLeftDown   = cpTrue;
        ChipmunkDemoRightDown  = cpFalse;
        ChipmunkDemoRightClick = cpFalse;

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
        ChipmunkDemoLeftDown   = cpFalse;
        ChipmunkDemoRightDown  = cpTrue;
        ChipmunkDemoRightClick = cpTrue;
    }
}

void ChipmunkTestBed::onMouseUp(Event* event) {
    EventMouse* e = (EventMouse*) event;

    ChipmunkDemoLeftDown   = cpFalse;
    ChipmunkDemoRightDown  = cpFalse;
    ChipmunkDemoRightClick = cpFalse;

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
void BouncyHexagonsDemo::onEnter() {
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string BouncyHexagonsDemo::title() const {
    return BouncyHexagons.name;
}

void BouncyHexagonsDemo::initPhysics() {
    _space = BouncyHexagons.initFunc();
    ChipmunkTestBed::initPhysics();
}

void BouncyHexagonsDemo::update(float delta) {
    BouncyHexagons.updateFunc(_space, BouncyHexagons.timestep);
}


//------------------------------------------------------------------
//
// SpringiesDemo
//
//------------------------------------------------------------------
void SpringiesDemo::onEnter() {
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string SpringiesDemo::title() const {
    return Springies.name;
}

void SpringiesDemo::initPhysics() {
    _space = Springies.initFunc();
    ChipmunkTestBed::initPhysics();
}

void SpringiesDemo::update(float delta) {
    Springies.updateFunc(_space, Springies.timestep);
}


//------------------------------------------------------------------
//
// ShatterDemo
//
//------------------------------------------------------------------
void ShatterDemo::onEnter() {
    ChipmunkTestBed::onEnter();
    initPhysics();
}

std::string ShatterDemo::title() const {
    return Shatter.name;
}

void ShatterDemo::initPhysics() {
    _space = Shatter.initFunc();
    ChipmunkTestBed::initPhysics();
}

void ShatterDemo::update(float delta) {
    Shatter.updateFunc(_space, Shatter.timestep);
}

//------------------------------------------------------------------
//
// StickyDemo
//
//------------------------------------------------------------------
void StickyDemo::onEnter() {
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string StickyDemo::title() const {
    return Sticky.name;
}

void StickyDemo::initPhysics() {
    _space = Sticky.initFunc();
    ChipmunkTestBed::initPhysics();
}

void StickyDemo::update(float delta) {
    Sticky.updateFunc(_space, Sticky.timestep);
}

//------------------------------------------------------------------
//
// CraneDemo
//
//------------------------------------------------------------------
void CraneDemo::onEnter() {
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string CraneDemo::title() const {
    return Crane.name;
}

void CraneDemo::initPhysics() {
    _space = Crane.initFunc();
    ChipmunkTestBed::initPhysics();
}

void CraneDemo::update(float delta) {
    Crane.updateFunc(_space, Crane.timestep);
}

//------------------------------------------------------------------
//
// JointsDemo
//
//------------------------------------------------------------------
void JointsDemo::onEnter() {
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string JointsDemo::title() const {
    return Joints.name;
}

void JointsDemo::initPhysics() {
    _space = Joints.initFunc();
    ChipmunkTestBed::initPhysics();
}

void JointsDemo::update(float delta) {
    Joints.updateFunc(_space, Joints.timestep);
}

//------------------------------------------------------------------
//
// ConvexDemo
//
//------------------------------------------------------------------
void ConvexDemo::onEnter() {
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string ConvexDemo::title() const {
    return Convex.name;
}

void ConvexDemo::initPhysics() {
    _space = Convex.initFunc();
    ChipmunkTestBed::initPhysics();
}

void ConvexDemo::update(float delta) {
    Convex.updateFunc(_space, Convex.timestep);
}

//------------------------------------------------------------------
//
// PumpDemo
//
//------------------------------------------------------------------
void PumpDemo::onEnter() {
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string PumpDemo::title() const {
    return Pump.name;
}

void PumpDemo::initPhysics() {
    _space = Pump.initFunc();
    ChipmunkTestBed::initPhysics();
}

void PumpDemo::update(float delta) {
    Pump.updateFunc(_space, Pump.timestep);
}

//------------------------------------------------------------------
//
// PlatformerPlayerDemo
//
//------------------------------------------------------------------
void PlatformerPlayerDemo::onEnter() {
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string PlatformerPlayerDemo::title() const {
    return PlatformerPlayer.name;
}

void PlatformerPlayerDemo::initPhysics() {
    _space = PlatformerPlayer.initFunc();
    ChipmunkTestBed::initPhysics();
}

void PlatformerPlayerDemo::update(float delta) {
    PlatformerPlayer.updateFunc(_space, PlatformerPlayer.timestep);
}

//------------------------------------------------------------------
//
// QueryDemo
//
//------------------------------------------------------------------
void QueryDemo::onEnter() {
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string QueryDemo::title() const {
    return Query.name;
}

void QueryDemo::initPhysics() {
    _space = Query.initFunc();
    ChipmunkTestBed::initPhysics();
}

void QueryDemo::update(float delta) {
    PrintStringBuffer[0] = 0;
    PrintStringCursor    = PrintStringBuffer;
    label->setString("");
    drawCP->clear();
    Query.updateFunc(_space, Query.timestep);
}

//------------------------------------------------------------------
//
// ContactGraphDemo
//
//------------------------------------------------------------------
void ContactGraphDemo::onEnter() {
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string ContactGraphDemo::title() const {
    return ContactGraph.name;
}

void ContactGraphDemo::initPhysics() {
    _space = ContactGraph.initFunc();
    ChipmunkTestBed::initPhysics();
}

void ContactGraphDemo::update(float delta) {
    PrintStringBuffer[0] = 0;
    PrintStringCursor    = PrintStringBuffer;
    label->setString("");
    ContactGraph.updateFunc(_space, ContactGraph.timestep);
}

//------------------------------------------------------------------
//
// BuoyancyDemo
//
//------------------------------------------------------------------
void BuoyancyDemo::onEnter() {
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string BuoyancyDemo::title() const {
    return Buoyancy.name;
}

void BuoyancyDemo::initPhysics() {
    _space = Buoyancy.initFunc();
    ChipmunkTestBed::initPhysics();
}

void BuoyancyDemo::update(float delta) {
    Buoyancy.updateFunc(_space, Buoyancy.timestep);
}

//------------------------------------------------------------------
//
// SliceDemo
//
//------------------------------------------------------------------
void SliceDemo::onEnter() {
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string SliceDemo::title() const {
    return Slice.name;
}

void SliceDemo::initPhysics() {
    _space = Slice.initFunc();
    ChipmunkTestBed::initPhysics();
}

void SliceDemo::update(float delta) {
    Slice.updateFunc(_space, Slice.timestep);
}

//------------------------------------------------------------------
//
// UnicycleDemo
//
//------------------------------------------------------------------
void UnicycleDemo::onEnter() {
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string UnicycleDemo::title() const {
    return Unicycle.name;
}

void UnicycleDemo::initPhysics() {
    _space = Unicycle.initFunc();
    ChipmunkTestBed::initPhysics();
}

void UnicycleDemo::update(float delta) {
    drawCP->clear();
    Unicycle.updateFunc(_space, Unicycle.timestep);
}


//------------------------------------------------------------------
//
// ExampleDemo
//
//------------------------------------------------------------------
void ExampleDemo::onEnter() {
    ChipmunkTestBed::onEnter();

    initPhysics();
}

std::string ExampleDemo::title() const {
    return Example.name;
}

void ExampleDemo::initPhysics() {
    _space = Example.initFunc();
    ChipmunkTestBed::initPhysics();
}

void ExampleDemo::update(float delta) {
    cpSpaceStep(_space, Example.timestep);
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
    ADD_TEST_CASE(BouncyHexagonsDemo);
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


    ADD_TEST_CASE(ExampleDemo); //  the template for your one examples/Demos
}
