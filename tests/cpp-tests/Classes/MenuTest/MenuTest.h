/****************************************************************************
 Copyright (c) 2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://axys1.github.io/

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

#ifndef _MENU_TEST_H_
#define _MENU_TEST_H_

////----#include "cocos2d.h"
#include "../BaseTest.h"

DEFINE_TEST_SUITE(MenuTests);

class MenuTest : public TestCase
{
public:
    CREATE_FUNC(MenuTest);

    virtual bool init() override;
};

class MenuLayerMainMenu : public axis::Layer
{
protected:
    axis::MenuItem* _disabledItem;
    axis::EventListenerTouchOneByOne* _touchListener;

public:
    MenuLayerMainMenu();
    ~MenuLayerMainMenu();

public:
    bool touchBegan(axis::Touch* touch, axis::Event* event);
    void touchEnded(axis::Touch* touch, axis::Event* event);
    void touchCancelled(axis::Touch* touch, axis::Event* event);
    void touchMoved(axis::Touch* touch, axis::Event* event);

    void allowTouches(float dt);
    void menuCallback(axis::Ref* sender);
    void menuCallbackConfig(axis::Ref* sender);
    void menuCallbackDisabled(axis::Ref* sender);
    void menuCallback2(axis::Ref* sender);
    void menuCallbackPriorityTest(axis::Ref* sender);
    void menuCallbackBugsTest(axis::Ref* pSender);
    void onQuit(axis::Ref* sender);
    void menuMovingCallback(axis::Ref* pSender);

    // CREATE_NODE(MenuLayer1);
};

class MenuLayer2 : public axis::Layer
{
protected:
    axis::Vec2 _centeredMenu;
    bool _alignedH;

    void alignMenusH();
    void alignMenusV();

public:
    MenuLayer2();
    ~MenuLayer2();

    void menuCallback(axis::Ref* sender);
    void menuCallbackOpacity(axis::Ref* sender);
    void menuCallbackAlign(axis::Ref* sender);

    // CREATE_NODE(MenuLayer2);
};

class MenuLayer3 : public axis::Layer
{
protected:
    axis::MenuItem* _disabledItem;

public:
    MenuLayer3();
    ~MenuLayer3();
};

class MenuLayer4 : public axis::Layer
{
public:
    MenuLayer4();
    ~MenuLayer4();

    void menuCallback(axis::Ref* sender);
    void backCallback(axis::Ref* sender);
};

class BugsTest : public axis::Layer
{
public:
    BugsTest();

    void issue1410MenuCallback(axis::Ref* pSender);
    void issue1410v2MenuCallback(axis::Ref* pSender);
    void backMenuCallback(axis::Ref* pSender);
};

class RemoveMenuItemWhenMove : public axis::Layer
{
public:
    RemoveMenuItemWhenMove();
    ~RemoveMenuItemWhenMove();
    bool onTouchBegan(axis::Touch* touch, axis::Event* event);
    void onTouchMoved(axis::Touch* touch, axis::Event* event);

    void goBack(axis::Ref* pSender);

private:
    axis::MenuItemFont* item;
    axis::EventListenerTouchOneByOne* _touchListener;
};

#endif
