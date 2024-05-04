/****************************************************************************
 Copyright (c) 2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://axmolengine.github.io/

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

////----#include "axmol.h"
#include "../BaseTest.h"

DEFINE_TEST_SUITE(MenuTests);

class MenuTest : public TestCase
{
public:
    CREATE_FUNC(MenuTest);

    virtual bool init() override;
};

class MenuLayerMainMenu : public ax::Layer
{
protected:
    ax::MenuItem* _disabledItem;
    ax::EventListenerTouchOneByOne* _touchListener;

public:
    MenuLayerMainMenu();
    ~MenuLayerMainMenu();

public:
    bool touchBegan(ax::Touch* touch, ax::Event* event);
    void touchEnded(ax::Touch* touch, ax::Event* event);
    void touchCancelled(ax::Touch* touch, ax::Event* event);
    void touchMoved(ax::Touch* touch, ax::Event* event);

    void allowTouches(float dt);
    void menuCallback(ax::Object* sender);
    void menuCallbackConfig(ax::Object* sender);
    void menuCallbackDisabled(ax::Object* sender);
    void menuCallback2(ax::Object* sender);
    void menuCallbackPriorityTest(ax::Object* sender);
    void menuCallbackBugsTest(ax::Object* pSender);
    void onQuit(ax::Object* sender);
    void menuMovingCallback(ax::Object* pSender);

    // CREATE_NODE(MenuLayer1);
};

class MenuLayer2 : public ax::Layer
{
protected:
    ax::Vec2 _centeredMenu;
    bool _alignedH;

    void alignMenusH();
    void alignMenusV();

public:
    MenuLayer2();
    ~MenuLayer2();

    void menuCallback(ax::Object* sender);
    void menuCallbackOpacity(ax::Object* sender);
    void menuCallbackAlign(ax::Object* sender);

    // CREATE_NODE(MenuLayer2);
};

class MenuLayer3 : public ax::Layer
{
protected:
    ax::MenuItem* _disabledItem;

public:
    MenuLayer3();
    ~MenuLayer3();
};

class MenuLayer4 : public ax::Layer
{
public:
    MenuLayer4();
    ~MenuLayer4();

    void menuCallback(ax::Object* sender);
    void backCallback(ax::Object* sender);
};

class BugsTest : public ax::Layer
{
public:
    BugsTest();

    void issue1410MenuCallback(ax::Object* pSender);
    void issue1410v2MenuCallback(ax::Object* pSender);
    void backMenuCallback(ax::Object* pSender);
};

class RemoveMenuItemWhenMove : public ax::Layer
{
public:
    RemoveMenuItemWhenMove();
    ~RemoveMenuItemWhenMove();
    bool onTouchBegan(ax::Touch* touch, ax::Event* event);
    void onTouchMoved(ax::Touch* touch, ax::Event* event);

    void goBack(ax::Object* pSender);

private:
    ax::MenuItemFont* item;
    ax::EventListenerTouchOneByOne* _touchListener;
};

#endif
