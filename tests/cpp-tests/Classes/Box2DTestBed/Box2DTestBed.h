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

#ifndef _BOX2D_TESTBED_H_
#define _BOX2D_TESTBED_H_

#include "cocos2d.h"
#include "box2d/box2d.h"
#include "../BaseTest.h"
#include "renderer/CCCustomCommand.h"


DEFINE_TEST_SUITE(Box2DTestBedTests);

//GLFW_KEY_1
//GLFW_KEY_2
//GLFW_KEY_3
//GLFW_KEY_4
//GLFW_KEY_5
//GLFW_KEY_A
//GLFW_KEY_B
//GLFW_KEY_C
//GLFW_KEY_COMMA
//GLFW_KEY_D
//GLFW_KEY_E
//GLFW_KEY_F
//GLFW_KEY_G
//GLFW_KEY_H
//GLFW_KEY_J
//GLFW_KEY_K
//GLFW_KEY_M
//GLFW_KEY_PERIOD
//GLFW_KEY_Q
//GLFW_KEY_S
//GLFW_KEY_W



class Test;
typedef Test* TestCreateFcn();

struct TestEntry
{
	const char* category;
	const char* name;
	TestCreateFcn* createFcn;
};

#define MAX_TESTS 256
extern TestEntry g_testEntries[MAX_TESTS];

int RegisterTest(const char* category, const char* name, TestCreateFcn* fcn);

class Box2DTestBed : public TestCase, cocos2d::Layer
{
public:
	static Box2DTestBed* createWithEntryID(int entryId);

	Box2DTestBed();
	virtual ~Box2DTestBed();

	void onEnter() override;
	void onExit() override;

	void onDrawImGui();

	void initPhysics();
	void update(float dt) override;

	void createResetButton();

	bool initWithEntryID(int entryId);

	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);


	void onMouseDown(cocos2d::Event* event);
	void onMouseUp(cocos2d::Event* event);
	void onMouseMove(cocos2d::Event* event);
	void onMouseScroll(cocos2d::Event* event);

	void onKeyPressed(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* event) override;
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* event) override;

	cocos2d::EventListenerTouchOneByOne* _touchListener;
	cocos2d::EventListenerKeyboard* _keyboardListener;

	TestEntry* m_entry;
	Test* m_test;
	int m_entryID;

private:

	b2World* world;
	cocos2d::Texture2D* _spriteTexture;
	b2Vec2 pos;
	b2Vec2 oldPos;
	bool button[2];

	// Debug stuff to TestBed
	cocos2d::DrawNode* debugDrawNode;
	DebugDraw g_debugDraw;

};

#endif
