/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#include "MouseTest.h"

using namespace ax;

MouseTests::MouseTests()
{
    ADD_TEST_CASE(MouseEventTest);
    ADD_TEST_CASE(HideMouseTest);
}

//------------------------------------------------------------------
//
// MouseEventTest
//
//------------------------------------------------------------------
MouseEventTest::MouseEventTest()
{
    auto s = Director::getInstance()->getCanvasSize();

    // Create a label to display the mouse action
    _labelAction = Label::createWithTTF("Click mouse button and see this change", "fonts/arial.ttf", 22);
    _labelAction->setPosition(Vec2(s.width / 2, s.height * 2 / 3));
    addChild(_labelAction, 0);

    // Create a label to display the mouse position
    _labelPosition = Label::createWithTTF("Mouse not supported on this device", "fonts/arial.ttf", 22);
    _labelPosition->setPosition(Vec2(s.width / 2, s.height / 3));
    addChild(_labelPosition);

    _mouseListener                  = PointerEventListener::create();
    _mouseListener->onPointerMove   = AX_CALLBACK_1(MouseEventTest::onMouseMove, this);
    _mouseListener->onPointerUp     = AX_CALLBACK_1(MouseEventTest::onMouseUp, this);
    _mouseListener->onPointerDown   = AX_CALLBACK_1(MouseEventTest::onMouseDown, this);
    _mouseListener->onPointerScroll = AX_CALLBACK_1(MouseEventTest::onMouseScroll, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(_mouseListener, this);
}

MouseEventTest::~MouseEventTest()
{
    _eventDispatcher->removeEventListener(_mouseListener);
}

bool MouseEventTest::onMouseDown(PointerEvent* e)
{
    std::string str = fmt::format("Mouse Down detected, button: {}", static_cast<int>(e->getButton()));
    _labelAction->setString(str);

    return true;
}

void MouseEventTest::onMouseUp(PointerEvent* e)
{
    std::string str = fmt::format("Mouse Up detected, button: {}", static_cast<int>(e->getButton()));
    _labelAction->setString(str);
}

bool MouseEventTest::onMouseMove(PointerEvent* e)
{
    auto loc        = e->getLocation();
    auto delta      = e->getDelta();
    std::string str = fmt::format("MousePosition:({},{})\nMouseDelta:({},{})", loc.x, loc.y, delta.x, delta.y);
    _labelPosition->setString(str);
    return true;
}

bool MouseEventTest::onMouseScroll(PointerEvent* e)
{
    std::string str = fmt::format("Mouse Scroll detected, X:{} Y:{}", e->getScrollX(), e->getScrollY());
    _labelAction->setString(str.c_str());

    return true;
}

std::string MouseEventTest::title() const
{
    return "Mouse Event Test";
}

std::string MouseEventTest::subtitle() const
{
    return "This tests the mouse events";
}

//------------------------------------------------------------------
//
// HideMouseTest
//
//------------------------------------------------------------------

HideMouseTest::HideMouseTest()
{

    _lis                = PointerEventListener::create();
    _lis->onPointerDown = [](PointerEvent* e) -> bool {
        Director::getInstance()->getRenderView()->setCursorVisible(false);
        return true;
    };

    _lis->onPointerUp = [](PointerEvent* e) -> bool {
        Director::getInstance()->getRenderView()->setCursorVisible(true);
        return true;
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(_lis, this);
}

HideMouseTest::~HideMouseTest()
{
    _eventDispatcher->removeEventListener(_lis);
}

std::string HideMouseTest::title() const
{
    return "Hide/Show Mouse";
}

std::string HideMouseTest::subtitle() const
{
    return "Click to hide mouse";
}
