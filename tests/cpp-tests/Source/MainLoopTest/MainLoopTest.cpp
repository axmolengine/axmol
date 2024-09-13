/****************************************************************************
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

#include "MainLoopTest.h"
#include <string>

using namespace ax;

int MainLoopTest::sFps = 0;
int MainLoopTest::sUpdateThrottleMs = 0;


MainLoopTests::MainLoopTests()
{
    ADD_TEST_CASE(MainLoopTest);
}


MainLoopTest::MainLoopTest()
{
    auto window = Director::getInstance()->getWinSize();
    if (sFps == 0)
        Director::getInstance()->setAnimationInterval(0.0f);
    else
        Director::getInstance()->setAnimationInterval(1.0f / float(sFps));

    auto menuRequest = Menu::create();
    menuRequest->setPosition(Vec2::ZERO);
    addChild(menuRequest);

    const float left = window.width / 2.0f - 150.0f;
    const float top = window.height - 130.0f;
    const float colWidth = 100.0f;
    const float rowHeight = 30.0f;

    auto throttleLabel = Label::createWithTTF("Throttle: 0ms", "fonts/arial.ttf", 18);
    throttleLabel->setPosition(window.width / 2, window.height - 80);
    addChild(throttleLabel);

    auto fpsLabel = Label::createWithTTF("Fps: 60", "fonts/arial.ttf", 18);
    fpsLabel->setPosition(window.width / 2, window.height - 100);
    addChild(fpsLabel);

    auto sprite = Sprite::create("Images/blocks.png");
    sprite->setPosition(window.width / 2 + 150, window.height / 2 + 100);
    addChild(sprite);
    sprite->runAction(RepeatForever::create(RotateBy::create(1.0f, 90.0f)));

    auto addFpsButton = [=](int col, int row, const std::string& text, const std::function<void()>& callback)
    {
        auto label = Label::createWithTTF(text, "fonts/arial.ttf", 20);
        auto item  = MenuItemLabel::create(label, [=](Object*)
        {
            callback();
            fpsLabel->setString(StringUtils::format("Fps: %d", sFps));
            if (sFps == 0)
                Director::getInstance()->setAnimationInterval(0.0f);
            else
                Director::getInstance()->setAnimationInterval(1.0f / float(sFps));
        });
        item->setPosition(left + col * colWidth, top - row * rowHeight);
        menuRequest->addChild(item);
    };

    auto addUpdateButton = [=](int col, int row, const std::string& text, const std::function<void()>& callback)
    {
        auto label = Label::createWithTTF(text, "fonts/arial.ttf", 20);
        auto item  = MenuItemLabel::create(label, [=](Object*)
        {
            callback();
            throttleLabel->setString(StringUtils::format("Throttle: %dms", sUpdateThrottleMs));
        });
        item->setPosition(left + col * colWidth, top - row * rowHeight);
        menuRequest->addChild(item);
    };

    addFpsButton(0, 0, "Reset", [this] { sFps = 0; });
    addFpsButton(0, 1, "-1", [this] { sFps -= 1; });
    addFpsButton(1, 1, "+1", [this] { sFps += 1; });
    addFpsButton(0, 2, "-10", [this] { sFps -= 10; });
    addFpsButton(1, 2, "+10", [this] { sFps += 10; });
    addFpsButton(0, 3, "15", [this] { sFps = 15; });
    addFpsButton(1, 3, "30", [this] { sFps = 30; });
    addFpsButton(0, 4, "60", [this] { sFps = 60; });
    addFpsButton(1, 4, "120", [this] { sFps = 120; });

    addUpdateButton(2, 0, "Reset", [this] { sUpdateThrottleMs = 0; });
    addUpdateButton(2, 1, "-1ms", [this] { sUpdateThrottleMs -= 1; });
    addUpdateButton(3, 1, "+1ms", [this] { sUpdateThrottleMs += 1; });
    addUpdateButton(2, 2, "-10ms", [this] { sUpdateThrottleMs -= 10; });
    addUpdateButton(3, 2, "+10ms", [this] { sUpdateThrottleMs += 10; });
    addUpdateButton(2, 3, "16ms", [this] { sUpdateThrottleMs = 16; });
    addUpdateButton(3, 3, "33ms", [this] { sUpdateThrottleMs = 33; });
    addUpdateButton(2, 4, "50ms", [this] { sUpdateThrottleMs = 50; });
    addUpdateButton(3, 4, "100ms", [this] { sUpdateThrottleMs = 100; });

    scheduleUpdate();
}

void MainLoopTest::update(float dt)
{
    // Throttle update
    auto end = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(sUpdateThrottleMs);
    while (std::chrono::high_resolution_clock::now() < end)
    {
        // Do nothing
    }
}

MainLoopTest::~MainLoopTest()
{
}
