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
#include "test.h"
#include "ImGuiEXT/CCImGuiEXT.h"

 //#include "renderer/CCRenderer.h"


USING_NS_CC;
USING_NS_CC_EXT;

static bool show_test_window = true;
static bool show_another_window = true;
static ImVec4 clear_color = ImColor(114, 144, 154);

#define kAccelerometerFrequency 30
#define FRAMES_BETWEEN_PRESSES_FOR_DOUBLE_CLICK 10

extern int g_testCount;

Settings settings;

extern cocos2d::Vec2 physicsDebugNodeOffset;
cocos2d::Label* labelDebugDraw;

enum
{
	kTagBox2DNode,
};

extern cocos2d::DrawNode* drawBox2D;


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
	_eventDispatcher->removeEventListener(_touchListener);
}

Box2DTestBed* Box2DTestBed::createWithEntryID(int entryId)
{
	auto layer = new (std::nothrow) Box2DTestBed();
	layer->initWithEntryID(entryId);
	layer->autorelease();

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

	Box2DView* view = Box2DView::viewWithEntryID(entryId);
	addChild(view, 0, kTagBox2DNode);
	view->setScale(15);
	view->setAnchorPoint(Vec2(0, 0));
	view->setPosition(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height / 3);
	auto label = Label::createWithTTF(view->title().c_str(), "fonts/arial.ttf", 28);
	addChild(label, 1);
	label->setPosition(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height - 50);

	// Adds touch event listener
	_touchListener = EventListenerTouchOneByOne::create();
	_touchListener->setSwallowTouches(true);

	_touchListener->onTouchBegan = CC_CALLBACK_2(Box2DTestBed::onTouchBegan, this);
	_touchListener->onTouchMoved = CC_CALLBACK_2(Box2DTestBed::onTouchMoved, this);

	_eventDispatcher->addEventListenerWithFixedPriority(_touchListener, 1);



	addChild(drawBox2D, 100);

	// Demo messageString
	labelDebugDraw = Label::createWithTTF("TEST", "fonts/Marker Felt.ttf", 8.0f);
	labelDebugDraw->setAnchorPoint(Vec2(0, 0));
	labelDebugDraw->setPosition(VisibleRect::left().x, VisibleRect::top().y - 20);
	labelDebugDraw->setColor(Color3B::WHITE);
	addChild(labelDebugDraw, 100);

	return true;
}

bool Box2DTestBed::onTouchBegan(Touch* touch, Event* event)
{
	return true;
}

void Box2DTestBed::onTouchMoved(Touch* touch, Event* event)
{
	auto diff = touch->getDelta();
	auto node = getChildByTag(kTagBox2DNode);
	auto currentPos = node->getPosition();
	node->setPosition(currentPos + diff);
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

void Box2DTestBed::onDrawImGui()
{
	//// 1. Show a simple window
	//// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
	//{
	//	static float f = 0.0f;
	//	ImGui::Text("Hello, world!");
	//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
	//	ImGui::ColorEdit3("clear color", (float*)&clear_color);
	//	if (ImGui::Button("Test Window")) show_test_window ^= 1;
	//	if (ImGui::Button("Another Window")) show_another_window ^= 1;
	//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	//}

	//// 2. Show another simple window, this time using an explicit Begin/End pair
	//if (show_another_window)
	//{
	//	ImGui::SetNextWindowSize(ImVec2(170, 80), ImGuiCond_FirstUseEver);
	//	ImGui::Begin("Another Window", &show_another_window);

	//	ImGui::Text("Hello");
	//	ImGui::End();
	//}

	//// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	//if (show_test_window)
	//{
	//	ImGui::ShowDemoWindow();
	//}
}


//------------------------------------------------------------------
//
// Box2DView
//
//------------------------------------------------------------------
Box2DView::Box2DView(void)
{
}

Box2DView* Box2DView::viewWithEntryID(int entryId)
{
	Box2DView* pView = new (std::nothrow) Box2DView();

	pView->initWithEntryID(entryId);
	pView->autorelease();

	return pView;
}

bool Box2DView::initWithEntryID(int entryId)
{
	m_entry = g_testEntries + entryId;
	m_test = m_entry->createFcn();


	// Adds Touch Event Listener
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

	listener->onTouchBegan = CC_CALLBACK_2(Box2DView::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(Box2DView::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(Box2DView::onTouchEnded, this);



	_eventDispatcher->addEventListenerWithFixedPriority(listener, -10);
	_touchListener = listener;

	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(Box2DView::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(Box2DView::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithFixedPriority(keyboardListener, -11);
	_keyboardListener = keyboardListener;

	return true;
}

std::string Box2DView::title() const
{
	std::string title = std::string(m_entry->category) + std::string(":") + std::string(m_entry->name);
	return title;
}


void Box2DView::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
	Layer::draw(renderer, transform, flags);

	_customCmd.init(_globalZOrder, transform, flags);
	_customCmd.func = CC_CALLBACK_0(Box2DView::onDraw, this, transform, flags);
	renderer->addCommand(&_customCmd);
}

void Box2DView::onDraw(const Mat4& transform, uint32_t flags)
{
	//Director* director = Director::getInstance();
	//CCASSERT(nullptr != director, "Director is null when setting matrix stack");
	//director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	//director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);

	//drawBox2D->clear();
	//m_test->Step(&settings);
	//m_test->m_world->DebugDraw();

	//director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

	Director* director = Director::getInstance();
	CCASSERT(nullptr != director, "Director is null when setting matrix stack");

	auto oldMV = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewMV);
//	world->DebugDraw();

	drawBox2D->clear();
	m_test->Step(&settings);
	//m_test->m_world->DebugDraw();

	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, oldMV);
}

Box2DView::~Box2DView()
{
	// Removes Touch Event Listener
	_eventDispatcher->removeEventListener(_touchListener);
	_eventDispatcher->removeEventListener(_keyboardListener);
	delete m_test;
}

bool Box2DView::onTouchBegan(Touch* touch, Event* event)
{
	auto touchLocation = touch->getLocation();

	auto nodePosition = convertToNodeSpace(touchLocation);
	log("Box2DView::onTouchBegan, pos: %f,%f -> %f,%f", touchLocation.x, touchLocation.y, nodePosition.x, nodePosition.y);

	m_test->MouseDown(b2Vec2(nodePosition.x, nodePosition.y));

	return false;
}

void Box2DView::onTouchMoved(Touch* touch, Event* event)
{
	auto touchLocation = touch->getLocation();
	auto nodePosition = convertToNodeSpace(touchLocation);

	log("Box2DView::onTouchMoved, pos: %f,%f -> %f,%f", touchLocation.x, touchLocation.y, nodePosition.x, nodePosition.y);

	m_test->MouseMove(b2Vec2(nodePosition.x, nodePosition.y));
}

void Box2DView::onTouchEnded(Touch* touch, Event* event)
{
	auto touchLocation = touch->getLocation();
	auto nodePosition = convertToNodeSpace(touchLocation);

	log("Box2DView::onTouchEnded, pos: %f,%f -> %f,%f", touchLocation.x, touchLocation.y, nodePosition.x, nodePosition.y);

	m_test->MouseUp(b2Vec2(nodePosition.x, nodePosition.y));
}

void Box2DView::onKeyPressed(EventKeyboard::KeyCode code, Event* event)
{
	log("Box2dView:onKeyPressed, keycode: %d", static_cast<int>(code));
//	m_test->Keyboard(static_cast<unsigned char>(code));
	m_test->Keyboard((static_cast<int>(code) - 59));
}

void Box2DView::onKeyReleased(EventKeyboard::KeyCode code, Event* event)
{
	log("onKeyReleased, keycode: %d", static_cast<int>(code));
//	m_test->KeyboardUp(static_cast<unsigned char>(code));
	m_test->Keyboard((static_cast<int>(code) - 59));
}

//void Box2DView::onEnter()
//{
//	onEnter();
//	ImGuiEXT::getInstance()->addFont(FileUtils::getInstance()->fullPathForFilename("fonts/arial.ttf"));
//	ImGuiEXT::getInstance()->addRenderLoop("#im01", CC_CALLBACK_0(Box2DView::onDrawImGui, this), this->getScene());
//
//}
//void Box2DView::onExit()
//{
//	ImGuiEXT::getInstance()->removeRenderLoop("#im01");
//	onExit();
//}

void Box2DView::onDrawImGui()
{
	// 1. Show a simple window
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
	{
		static float f = 0.0f;
		ImGui::Text("Hello, world!");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::ColorEdit3("clear color", (float*)&clear_color);
		if (ImGui::Button("Test Window")) show_test_window ^= 1;
		if (ImGui::Button("Another Window")) show_another_window ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	// 2. Show another simple window, this time using an explicit Begin/End pair
	if (show_another_window)
	{
		ImGui::SetNextWindowSize(ImVec2(170, 80), ImGuiCond_FirstUseEver);
		ImGui::Begin("Another Window", &show_another_window);

		ImGui::Text("Hello");
		ImGui::End();
	}

	// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if (show_test_window)
	{
		ImGui::ShowDemoWindow();
	}
}
