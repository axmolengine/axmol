/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

//
//  NewEventDispatcherTest.h
//  samples
//
//  Created by James Chen on 9/13/13.
//
//

#ifndef __samples__NewEventDispatcherTest__
#define __samples__NewEventDispatcherTest__

#include "axmol/axmol.h"
#include "../BaseTest.h"

DEFINE_TEST_SUITE(EventDispatcherTests);

class EventDispatcherTestDemo : public TestCase
{
public:
    std::string title() const override;
};

class TouchableSpriteTest : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(TouchableSpriteTest);
    void onEnter() override;
    std::string title() const override;
    std::string subtitle() const override;
};

class FixedPriorityTest : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(FixedPriorityTest);
    void onEnter() override;
    std::string title() const override;
    std::string subtitle() const override;
};

class RemovePointerListenerOnPointerDown : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(RemovePointerListenerOnPointerDown);
    void onEnter() override;
    std::string title() const override;
    std::string subtitle() const override;
};

class RemoveListenerWhenDispatching : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(RemoveListenerWhenDispatching);
    void onEnter() override;
    std::string title() const override;
    std::string subtitle() const override;
};

class CustomEventTest : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(CustomEventTest);
    void onEnter() override;
    void onExit() override;
    std::string title() const override;
    std::string subtitle() const override;

private:
    ax::CustomEventListener* _listener;
    ax::CustomEventListener* _listener2;
};

class LabelKeyboardEventTest : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(LabelKeyboardEventTest);
    void onEnter() override;
    std::string title() const override;
    std::string subtitle() const override;
};

class SpriteAccelerationEventTest : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(SpriteAccelerationEventTest);
    void onEnter() override;
    void onExit() override;
    std::string title() const override;
    std::string subtitle() const override;
};

class RemoveAndRetainNodeTest : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(RemoveAndRetainNodeTest);
    void onEnter() override;
    void onExit() override;
    std::string title() const override;
    std::string subtitle() const override;

private:
    ax::Sprite* _sprite;
    bool _spriteSaved;
};

class RemoveListenerAfterAddingTest : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(RemoveListenerAfterAddingTest);
    void onEnter() override;
    void onExit() override;
    std::string title() const override;
    std::string subtitle() const override;
};

class DirectorEventTest : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(DirectorEventTest);
    DirectorEventTest();
    void onEnter() override;
    void onExit() override;

    void update(float dt) override;

    void onEvent1(ax::CustomEvent* event);
    void onEvent2(ax::CustomEvent* event);

    std::string title() const override;
    std::string subtitle() const override;

protected:
    int _count1, _count2, _count3, _count4;
    ax::Label *_label1, *_label2, *_label3, *_label4;
    ax::CustomEventListener *_event1, *_event2, *_event3, *_event4;
};

class GlobalZTouchTest : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(GlobalZTouchTest);
    GlobalZTouchTest();

    void update(float dt) override;

    std::string title() const override;
    std::string subtitle() const override;

protected:
    ax::Sprite* _sprite;
    float _accum;
};

class StopPropagationTest : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(StopPropagationTest);
    StopPropagationTest();

    std::string title() const override;
    std::string subtitle() const override;

protected:
    bool isPointInNode(ax::Vec2 pt, ax::Node* node);
    bool isPointInTopHalfAreaOfScreen(ax::Vec2 pt);
};

class PauseResumeTargetTest : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(PauseResumeTargetTest);
    PauseResumeTargetTest();
    ~PauseResumeTargetTest();

    std::string title() const override;
    std::string subtitle() const override;

private:
};

class PauseResumeTargetTest2 : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(PauseResumeTargetTest2);
    PauseResumeTargetTest2();
    ~PauseResumeTargetTest2();

    std::string title() const override;
    std::string subtitle() const override;

private:
    ax::Sprite* _touchableSprite;
    ax::MenuItemFont* _itemPauseTouch;
    ax::MenuItemFont* _itemResumeTouch;
    ax::MenuItemFont* _itemAddToScene;
    ax::MenuItemFont* _itemRemoveFromScene;
};

class PauseResumeTargetTest3 : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(PauseResumeTargetTest3);
    PauseResumeTargetTest3();
    ~PauseResumeTargetTest3();

    std::string title() const override;
    std::string subtitle() const override;

private:
    ax::Sprite* _touchableSprite;
};

class Issue4129 : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(Issue4129);
    Issue4129();
    ~Issue4129();

    std::string title() const override;
    std::string subtitle() const override;

private:
    ax::CustomEventListener* _customlistener;
    bool _bugFixed;
};

class Issue4160 : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(Issue4160);
    Issue4160();
    ~Issue4160();

    std::string title() const override;
    std::string subtitle() const override;

private:
};

class DanglingNodePointersTest : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(DanglingNodePointersTest);
    DanglingNodePointersTest();
    ~DanglingNodePointersTest();

    std::string title() const override;
    std::string subtitle() const override;
};

class RegisterAndUnregisterWhileEventHanldingTest : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(RegisterAndUnregisterWhileEventHanldingTest);
    RegisterAndUnregisterWhileEventHanldingTest();

    std::string title() const override;
    std::string subtitle() const override;
};

class WindowEventsTest : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(WindowEventsTest);
    WindowEventsTest();

    std::string title() const override;
    std::string subtitle() const override;
};

class Issue8194 : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(Issue8194);
    Issue8194();
    ~Issue8194();

    std::string title() const override;
    std::string subtitle() const override;

private:
    ax::CustomEventListener* _listener;
};

class Issue9898 : public EventDispatcherTestDemo
{
public:
    CREATE_FUNC(Issue9898);
    Issue9898();

    std::string title() const override;
    std::string subtitle() const override;

private:
    ax::CustomEventListener* _listener;
};

#endif /* defined(__samples__NewEventDispatcherTest__) */
