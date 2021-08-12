/****************************************************************************
 * Copyright (c) 2021 @aismann; Peter Eismann, Germany; dreifrankensoft

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

#include "platform/CCPlatformConfig.h"
#include "Box2DTestBed.h"
#include "extensions/cocos-ext.h"
#include "tests/test.h"
#include "tests/settings.h"
#include "ImGuiEXT/CCImGuiEXT.h"

USING_NS_CC;
USING_NS_CC_EXT;

enum {
	kTagParentNode = 1,
};



#define kAccelerometerFrequency 30
#define FRAMES_BETWEEN_PRESSES_FOR_DOUBLE_CLICK 10


#define PTM_RATIO 32

Settings settings;

cocos2d::Label* labelDebugDraw;

enum
{
	kTagBox2DNode,
};


TestEntry g_testEntries[MAX_TESTS] = { {nullptr} };
int g_testCount = 0;

int RegisterTest(const char* category, const char* name, TestCreateFcn* fcn)
{
	int index = g_testCount;
	if (index < MAX_TESTS)
	{
		g_testEntries[index] = { category, name, fcn };
		++g_testCount;
		return index;
	}

	return -1;
}

Box2DTestBedTests::Box2DTestBedTests()
{
	for (int entryId = 0; entryId < g_testCount; ++entryId)
	{
		addTestCase(g_testEntries[entryId].name, [entryId]() {
			return Box2DTestBed::createWithEntryID(entryId);
			});
	}
}

//------------------------------------------------------------------
//
// Box2dTestBed
//
//------------------------------------------------------------------

Box2DTestBed::Box2DTestBed()
{
}

Box2DTestBed::~Box2DTestBed()
{
	Layer::_eventDispatcher->removeEventListener(_touchListener);
}

Box2DTestBed* Box2DTestBed::createWithEntryID(int entryId)
{
	auto layer = new (std::nothrow) Box2DTestBed();
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
	auto director = Director::getInstance();
	Vec2 visibleOrigin = director->getVisibleOrigin();
	Size visibleSize = director->getVisibleSize();

	m_entryID = entryId;

	m_entry = g_testEntries + entryId;
	m_test = m_entry->createFcn();

	debugDrawNode = g_debugDraw.GetDrawNode();
	m_test->debugDrawNode = g_debugDraw.GetDrawNode();
	m_test->g_debugDrawTestBed = g_debugDraw;

	TestCase::addChild(debugDrawNode, 100);
	//	drawBox2D->setOpacity(150);

	// init physics
	this->initPhysics();


	auto label = Label::createWithTTF(m_entry->name, "fonts/arial.ttf", 28);
	TestCase::addChild(label, 1);
	label->setPosition(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height - 50);

	// Adds touch event listener
	_touchListener = EventListenerTouchOneByOne::create();
	_touchListener->setSwallowTouches(true);
	_touchListener->onTouchBegan = CC_CALLBACK_2(Box2DTestBed::onTouchBegan, this);
	_touchListener->onTouchMoved = CC_CALLBACK_2(Box2DTestBed::onTouchMoved, this);
	_touchListener->onTouchEnded = CC_CALLBACK_2(Box2DTestBed::onTouchEnded, this);
	TestCase::_eventDispatcher->addEventListenerWithFixedPriority(_touchListener, -10);

	// Adds Keyboard event listener
	_keyboardListener = EventListenerKeyboard::create();
	_keyboardListener->onKeyPressed = CC_CALLBACK_2(Box2DTestBed::onKeyPressed, this);
	_keyboardListener->onKeyReleased = CC_CALLBACK_2(Box2DTestBed::onKeyReleased, this);
	TestCase::_eventDispatcher->addEventListenerWithFixedPriority(_keyboardListener, -11);


	// Demo messageString
	labelDebugDraw = Label::createWithTTF("TEST", "fonts/Marker Felt.ttf", 8.0f);
	labelDebugDraw->setAnchorPoint(Vec2(0, 0));
	labelDebugDraw->setPosition(VisibleRect::left().x, VisibleRect::top().y - 20);
	labelDebugDraw->setColor(Color3B::WHITE);
	TestCase::addChild(labelDebugDraw, 100);

	TestCase::scheduleUpdate();

	return true;
}

bool Box2DTestBed::onTouchBegan(Touch* touch, Event* event)
{
	auto location = touch->getLocation() - g_debugDraw.debugNodeOffset;
	b2Vec2 pos = { location.x / g_debugDraw.mRatio, location.y / g_debugDraw.mRatio };
	return m_test->MouseDown(pos);
}

void Box2DTestBed::onTouchMoved(Touch* touch, Event* event)
{
	auto location = touch->getLocation() - g_debugDraw.debugNodeOffset;
	b2Vec2 pos = { location.x / g_debugDraw.mRatio, location.y / g_debugDraw.mRatio };

	m_test->MouseMove(pos);
}

void Box2DTestBed::onTouchEnded(Touch* touch, Event* event)
{
	auto location = touch->getLocation() - g_debugDraw.debugNodeOffset;
	b2Vec2 pos = { location.x / g_debugDraw.mRatio, location.y / g_debugDraw.mRatio };

	m_test->MouseUp(pos);
}

void Box2DTestBed::onKeyPressed(EventKeyboard::KeyCode code, Event* event)
{
	log("Box2dView:onKeyPressed, keycode: %d", static_cast<int>(code));
	m_test->Keyboard((static_cast<int>(code) - 59));

}

void Box2DTestBed::onKeyReleased(EventKeyboard::KeyCode code, Event* event)
{
	log("onKeyReleased, keycode: %d", static_cast<int>(code));
	m_test->Keyboard((static_cast<int>(code) - 59));

}

void Box2DTestBed::onEnter()
{
	Scene::onEnter();
	ImGuiEXT::getInstance()->addFont(FileUtils::getInstance()->fullPathForFilename("fonts/arial.ttf"));
	ImGuiEXT::getInstance()->addRenderLoop("#im01", CC_CALLBACK_0(Box2DTestBed::onDrawImGui, this), this);
}
void Box2DTestBed::onExit()
{
	Scene::onExit();
	ImGuiEXT::getInstance()->removeRenderLoop("#im01");
}

void Box2DTestBed::update(float dt)
{
	// Debug draw
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
	g_debugDraw.mRatio = PTM_RATIO / 4;
	m_test->m_world->SetDebugDraw(&g_debugDraw);
	m_test->g_debugDraw = g_debugDraw;
	g_debugDraw.debugNodeOffset = { 250, 70 };
	settings.m_hertz = 60;
}

void Box2DTestBed::onDrawImGui()
{
	m_test->UpdateUI();
}