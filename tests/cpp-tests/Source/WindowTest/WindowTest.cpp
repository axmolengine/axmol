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

#include "WindowTest.h"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC || AX_TARGET_PLATFORM == AX_PLATFORM_WIN32 || \
     AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
USING_NS_AX;

WindowTests::WindowTests()
{
    ADD_TEST_CASE(WindowTestWindowed1);
    ADD_TEST_CASE(WindowTestWindowed2);
    ADD_TEST_CASE(WindowTestFullscreen1);
    ADD_TEST_CASE(WindowTestFullscreen2);
    ADD_TEST_CASE(WindowTestFullscreen3);
    ADD_TEST_CASE(WindowTestResizedAndPositioned);
    ADD_TEST_CASE(WindowTestClose);
}

std::string WindowTest::title() const
{
    return "Window Test";
}

void WindowTestWindowed1::onEnter()
{
    WindowTest::onEnter();
    GLViewImpl* view = (GLViewImpl*)Director::getInstance()->getGLView();
    view->setWindowed(480, 320);
}

std::string WindowTestWindowed1::subtitle() const
{
    return "Windowed 480x320";
}

void WindowTestWindowed2::onEnter()
{
    WindowTest::onEnter();
    GLViewImpl* view = (GLViewImpl*)Director::getInstance()->getGLView();
    view->setWindowed(960, 640);
}

std::string WindowTestWindowed2::subtitle() const
{
    return "Windowed 960x640";
}

bool WindowTestFullscreen1::init()
{
    // @remark: Set full screen before layout renderer elements to ensure VisibleRect is
    // correct with full screen window size
    GLViewImpl* view = (GLViewImpl*)Director::getInstance()->getGLView();
    view->setFullscreen();
    return WindowTest::init();
}

std::string WindowTestFullscreen1::subtitle() const
{
    return "Fullscreen";
}

bool WindowTestFullscreen2::init()
{
    GLViewImpl* view = (GLViewImpl*)Director::getInstance()->getGLView();
    view->setFullscreen(1);
    return WindowTest::init();
}

std::string WindowTestFullscreen2::subtitle() const
{
    return "Fullscreen Monitor 2 (if present)";
}

bool WindowTestFullscreen3::init()
{
    GLViewImpl* view = (GLViewImpl*)Director::getInstance()->getGLView();
    view->setFullscreen(2);
    return WindowTest::init();
}

std::string WindowTestFullscreen3::subtitle() const
{
    return "Fullscreen Monitor 3 (if present)";
}

void WindowTestResizedAndPositioned::onEnter()
{
    WindowTest::onEnter();

    auto s = _director->getWinSize();
    auto glView = static_cast<GLViewImpl*>(_director->getGLView());

    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    glView->getWindowPosition(&x, &y);
    glView->getWindowSize(&width, &height);

    label1 = Label::createWithTTF(StringUtils::format("pos : %d, %d", x, y), "fonts/Marker Felt.ttf", 16.0f);
    label1->setPosition(s.width / 3 * 1, s.height / 2);
    addChild(label1);

    label2 = Label::createWithTTF(StringUtils::format("size : %d, %d", width, height), "fonts/Marker Felt.ttf", 16.0f);
    label2->setPosition(s.width / 3 * 2, s.height / 2);
    addChild(label2);

    _director->getEventDispatcher()->addCustomEventListener(
        GLViewImpl::EVENT_WINDOW_POSITIONED,
        AX_CALLBACK_1(WindowTestResizedAndPositioned::onWindowPositioned, this));
    _director->getEventDispatcher()->addCustomEventListener(
        GLViewImpl::EVENT_WINDOW_RESIZED,
        AX_CALLBACK_1(WindowTestResizedAndPositioned::onWindowResized, this));
}

void WindowTestResizedAndPositioned::onExit()
{
    WindowTest::onExit();
    _director->getEventDispatcher()->removeCustomEventListeners(GLViewImpl::EVENT_WINDOW_POSITIONED);
    _director->getEventDispatcher()->removeCustomEventListeners(GLViewImpl::EVENT_WINDOW_RESIZED);
}

std::string WindowTestResizedAndPositioned::subtitle() const
{
    return "Window Position And Size";
}

void WindowTestResizedAndPositioned::onWindowPositioned(EventCustom* e)
{
    auto pos = static_cast<Vec2*>(e->getUserData());
    if (pos == nullptr)
        return;

    label1->setString(StringUtils::format("pos : %d, %d", (int)pos->x, (int)pos->y));
}

void WindowTestResizedAndPositioned::onWindowResized(EventCustom* e)
{
    auto size = static_cast<Size*>(e->getUserData());
    if (size == nullptr)
        return;

    label2->setString(StringUtils::format("size : %d, %d", (int)size->width, (int)size->height));
}

void WindowTestClose::onEnter()
{
    WindowTest::onEnter();

    label = nullptr;

    _director->getEventDispatcher()->addCustomEventListener(GLViewImpl::EVENT_WINDOW_CLOSE,
                                                            AX_CALLBACK_1(WindowTestClose::onWindowClose, this));
}

void WindowTestClose::onExit()
{
    WindowTest::onExit();
    _director->getEventDispatcher()->removeCustomEventListeners(GLViewImpl::EVENT_WINDOW_CLOSE);
}

std::string WindowTestClose::subtitle() const
{
    return "Window close callback test";
}

void WindowTestClose::onWindowClose(EventCustom* e)
{
    auto isClose = static_cast<bool*>(e->getUserData());
    if (isClose == nullptr)
        return;

    // false prevents the window from closing 
    *isClose = false;

    this->stopActionByTag(1);
    if (label != nullptr)
    {
        label->removeFromParent();
        label = nullptr;
    }

    auto s = _director->getWinSize();
    label  = Label::createWithTTF("Window close button callback!", "fonts/Marker Felt.ttf", 16.0f);
    label->setPosition(s.width / 2, s.height / 2);
    addChild(label);

    auto delay    = DelayTime::create(3.0);
    auto callFunc = CallFunc::create(
        [this]()
        {
            if (label != nullptr)
            {
                label->removeFromParent();
                label = nullptr;
            }
        });
    auto sequence = Sequence::create(delay, callFunc, nullptr);
    sequence->setTag(1);
    this->runAction(sequence);
}

#endif
