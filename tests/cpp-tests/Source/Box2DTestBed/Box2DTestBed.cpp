/****************************************************************************
 * Copyright (c) 2021 @aismann; Peter Eismann, Germany; dreifrankensoft

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

#include "platform/PlatformConfig.h"
#include "extensions/axmol-ext.h"
#include "ImGui/ImGuiPresenter.h"
#include "axmol.h"

#include "Box2DTestBed.h"
#include "tests/test.h"
#include "tests/settings.h"

USING_NS_AX;
USING_NS_AX_EXT;

enum
{
    kTagParentNode = 1,
};

Settings settings;
ax::Label* labelDebugDraw;

enum
{
    kTagBox2DNode,
};

TestEntry g_testEntries[MAX_TESTS] = {{nullptr}};
int g_testCount                    = 0;

int RegisterTest(const char* category, const char* name, TestCreateFcn* fcn)
{
    int index = g_testCount;
    if (index < MAX_TESTS)
    {
        g_testEntries[index] = {category, name, fcn};
        ++g_testCount;
        return index;
    }

    return -1;
}

Box2DTestBedTests::Box2DTestBedTests()
{
    for (int entryId = 0; entryId < g_testCount; ++entryId)
    {
        addTestCase(g_testEntries[entryId].name, [entryId]() { return Box2DTestBed::createWithEntryID(entryId); });
    }
}

//------------------------------------------------------------------
//
// Box2dTestBed
//
//------------------------------------------------------------------

Box2DTestBed::Box2DTestBed() {}

Box2DTestBed::~Box2DTestBed()
{
    Layer::_eventDispatcher->removeEventListener(_touchListener);
}

Box2DTestBed* Box2DTestBed::createWithEntryID(int entryId)
{
    auto layer = new Box2DTestBed();
    layer->initWithEntryID(entryId);
    //	layer->autorelease();

    return layer;
}

bool Box2DTestBed::initWithEntryID(int entryId)
{
    if (!TestCase::init())
    {
        return false;
    }
    auto director      = Director::getInstance();
    Vec2 visibleOrigin = director->getVisibleOrigin();
    Size visibleSize   = director->getVisibleSize();

    m_entryID = entryId;

    m_entry = g_testEntries + entryId;
    m_test  = m_entry->createFcn();

    debugDrawNode         = g_debugDraw.GetDrawNode();
    m_test->debugDrawNode = debugDrawNode;
    m_test->g_debugDraw   = g_debugDraw;

    TestCase::addChild(debugDrawNode, 100);

    // init physics
    this->initPhysics();

    auto label = Label::createWithTTF(m_entry->name, "fonts/arial.ttf", 28);
    TestCase::addChild(label, 1);
    label->setPosition(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height - 50);

    // Adds touch event listener
    _touchListener = EventListenerTouchOneByOne::create();
    _touchListener->setSwallowTouches(true);
    _touchListener->onTouchBegan = AX_CALLBACK_2(Box2DTestBed::onTouchBegan, this);
    _touchListener->onTouchMoved = AX_CALLBACK_2(Box2DTestBed::onTouchMoved, this);
    _touchListener->onTouchEnded = AX_CALLBACK_2(Box2DTestBed::onTouchEnded, this);
    TestCase::_eventDispatcher->addEventListenerWithFixedPriority(_touchListener, 10);

    // Adds Keyboard event listener
    _keyboardListener                = EventListenerKeyboard::create();
    _keyboardListener->onKeyPressed  = AX_CALLBACK_2(Box2DTestBed::onKeyPressed, this);
    _keyboardListener->onKeyReleased = AX_CALLBACK_2(Box2DTestBed::onKeyReleased, this);
    TestCase::_eventDispatcher->addEventListenerWithFixedPriority(_keyboardListener, 11);

    auto _mouseListener           = EventListenerMouse::create();
    _mouseListener->onMouseMove   = AX_CALLBACK_1(Box2DTestBed::onMouseMove, this);
    _mouseListener->onMouseUp     = AX_CALLBACK_1(Box2DTestBed::onMouseUp, this);
    _mouseListener->onMouseDown   = AX_CALLBACK_1(Box2DTestBed::onMouseDown, this);
    _mouseListener->onMouseScroll = AX_CALLBACK_1(Box2DTestBed::onMouseScroll, this);
    TestCase::_eventDispatcher->addEventListenerWithFixedPriority(_mouseListener, 12);

    // Demo messageString
    labelDebugDraw = Label::createWithTTF("TEST", "fonts/arial.ttf", 8.0f);
    labelDebugDraw->setAnchorPoint(Vec2(0, 1));
    labelDebugDraw->setPosition(VisibleRect::left().x, VisibleRect::top().y - 10);
    labelDebugDraw->setColor(Color3B::WHITE);
    TestCase::addChild(labelDebugDraw, 100);

    TestCase::scheduleUpdate();

    return true;
}

bool Box2DTestBed::onTouchBegan(Touch* touch, Event* event)
{
    auto location = touch->getLocation() - g_debugDraw.debugNodeOffset;
    b2Vec2 pos    = {location.x / g_debugDraw.mRatio, location.y / g_debugDraw.mRatio};
    return m_test->MouseDown(pos);
}

void Box2DTestBed::onTouchMoved(Touch* touch, Event* event)
{
    auto location = touch->getLocation() - g_debugDraw.debugNodeOffset;
    b2Vec2 pos    = {location.x / g_debugDraw.mRatio, location.y / g_debugDraw.mRatio};
    m_test->MouseMove(pos);
}

void Box2DTestBed::onTouchEnded(Touch* touch, Event* event)
{
    auto location = touch->getLocation() - g_debugDraw.debugNodeOffset;
    b2Vec2 pos    = {location.x / g_debugDraw.mRatio, location.y / g_debugDraw.mRatio};
    m_test->MouseUp(pos);
}

void Box2DTestBed::onKeyPressed(EventKeyboard::KeyCode code, Event* event)
{
    AXLOG("onKeyPressed, keycode: %d", static_cast<int>(code));
    m_test->Keyboard((static_cast<int>(code) - 59));  // its a bad hack!
}

void Box2DTestBed::onKeyReleased(EventKeyboard::KeyCode code, Event* event)
{
    AXLOG("onKeyPressed, keycode: %d", static_cast<int>(code));
    m_test->KeyboardUp((static_cast<int>(code) - 59));  // its a bad hack!
}

void Box2DTestBed::onMouseDown(Event* event)
{
    EventMouse* e = (EventMouse*)event;
    switch (e->getMouseButton())
    {
        button[(int)EventMouse::MouseButton::BUTTON_LEFT]   = false;
        button[(int)EventMouse::MouseButton::BUTTON_RIGHT]  = false;
        button[(int)EventMouse::MouseButton::BUTTON_MIDDLE] = false;
    case EventMouse::MouseButton::BUTTON_LEFT:
        button[(int)EventMouse::MouseButton::BUTTON_LEFT] = true;
        break;
    case EventMouse::MouseButton::BUTTON_RIGHT:
        button[(int)EventMouse::MouseButton::BUTTON_RIGHT] = true;
        break;
    case EventMouse::MouseButton::BUTTON_MIDDLE:
        button[(int)EventMouse::MouseButton::BUTTON_MIDDLE] = true;
        break;
    }
}

void Box2DTestBed::onMouseUp(Event* event)
{
    button[(int)EventMouse::MouseButton::BUTTON_LEFT]   = false;
    button[(int)EventMouse::MouseButton::BUTTON_RIGHT]  = false;
    button[(int)EventMouse::MouseButton::BUTTON_MIDDLE] = false;
}

void Box2DTestBed::onMouseMove(Event* event)
{
    EventMouse* e = (EventMouse*)event;
    pos           = {e->getCursorX() / g_debugDraw.mRatio, e->getCursorY() / g_debugDraw.mRatio};

    if (button[(int)EventMouse::MouseButton::BUTTON_RIGHT])
    {
        (pos.x > oldPos.x) ? g_debugDraw.debugNodeOffset.x += 4 : g_debugDraw.debugNodeOffset.x -= 4;
        (pos.y < oldPos.y) ? g_debugDraw.debugNodeOffset.y -= 2 : g_debugDraw.debugNodeOffset.y += 2;
    }
    oldPos = pos;
}

void Box2DTestBed::onMouseScroll(Event* event)
{
    EventMouse* e = (EventMouse*)event;
    g_debugDraw.mRatio += e->getScrollY();
}

void Box2DTestBed::onEnter()
{
    Scene::onEnter();
    ImGuiPresenter::getInstance()->addFont(FileUtils::getInstance()->fullPathForFilename("fonts/arial.ttf"));
    ImGuiPresenter::getInstance()->addRenderLoop("#im01", AX_CALLBACK_0(Box2DTestBed::onDrawImGui, this), this);
}
void Box2DTestBed::onExit()
{
    Scene::onExit();
    ImGuiPresenter::getInstance()->removeRenderLoop("#im01");
}

void Box2DTestBed::update(float dt)
{
    // Debug draw
    m_test->debugString = "";
    labelDebugDraw->setString("");
    debugDrawNode->clear();
    m_test->Step(settings);
    m_test->m_world->DebugDraw();
}

void Box2DTestBed::initPhysics()
{
    uint32 flags = 0;
    flags += 1 * b2Draw::e_shapeBit;
    flags += 1 * b2Draw::e_jointBit;
    flags += 0 * b2Draw::e_aabbBit;
    flags += 0 * b2Draw::e_centerOfMassBit;
    g_debugDraw.SetFlags(flags);
    g_debugDraw.mRatio = 8;
    m_test->m_world->SetDebugDraw(&g_debugDraw);
    m_test->g_debugDraw                 = g_debugDraw;
    g_debugDraw.debugNodeOffset         = {250, 70};
    m_test->g_debugDraw.debugNodeOffset = g_debugDraw.debugNodeOffset;

    settings.m_hertz = 60;
}

void Box2DTestBed::onDrawImGui()
{
    m_test->UpdateUI();
}
