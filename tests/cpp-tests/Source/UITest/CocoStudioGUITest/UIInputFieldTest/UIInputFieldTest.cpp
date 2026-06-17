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

#include "UIInputFieldTest.h"

using namespace ax;
using namespace ax::ui;

UIInputFieldTests::UIInputFieldTests()
{
    ADD_TEST_CASE(UIInputFieldTest);
    ADD_TEST_CASE(UIInputFieldTest_MaxLength);
    ADD_TEST_CASE(UIInputFieldTest_Password);
    ADD_TEST_CASE(UIInputFieldTest_LineWrap);
    ADD_TEST_CASE(UIInputFieldTest_TrueTypeFont);
    ADD_TEST_CASE(UIInputFieldTest_BMFont);
    ADD_TEST_CASE(UIInputFieldTest_PlaceHolderColor);
}

// UIInputFieldTest
UIInputFieldTest::UIInputFieldTest() : _displayValueLabel(nullptr) {}

UIInputFieldTest::~UIInputFieldTest() {}

bool UIInputFieldTest::init()
{
    if (UIScene::init())
    {
        Size widgetSize = _widget->getContentSize();

        // Add a label in which the inputfield events will be displayed
        _displayValueLabel = Text::create("No Event", "fonts/Marker Felt.ttf", 32);
        _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
        _displayValueLabel->setPosition(Vec2(
            widgetSize.width / 2.0f, widgetSize.height / 2.0f + _displayValueLabel->getContentSize().height * 1.5f));
        _uiLayer->addChild(_displayValueLabel);

        // Add the alert
        Text* alert = Text::create("InputField", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color32(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.075f));
        _uiLayer->addChild(alert);

        // Create the inputfield
        InputField* inputField = InputField::create("input words here", "Arial", 30);
        inputField->setTextHorizontalAlignment(TextHAlignment::CENTER);
        inputField->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f));

        // Set up event listener for InputField
        inputField->addEventListener([this](Object* sender, InputField::EventType ev) {
            switch (ev)
            {
            case InputField::EventType::ATTACH_WITH_IME:
                _displayValueLabel->setString("attach with IME");
                break;
            case InputField::EventType::DETACH_WITH_IME:
                _displayValueLabel->setString("detach with IME");
                break;
            case InputField::EventType::INSERT_TEXT:
                _displayValueLabel->setString("insert words");
                break;
            case InputField::EventType::DELETE_BACKWARD:
                _displayValueLabel->setString("delete word");
                break;
            }
        });

        _uiLayer->addChild(inputField);

        return true;
    }
    return false;
}

// UIInputFieldTest_MaxLength
UIInputFieldTest_MaxLength::UIInputFieldTest_MaxLength() : _displayValueLabel(nullptr) {}

UIInputFieldTest_MaxLength::~UIInputFieldTest_MaxLength() {}

bool UIInputFieldTest_MaxLength::init()
{
    if (UIScene::init())
    {
        Size screenSize = Director::getInstance()->getCanvasSize();

        // Add a label in which the inputfield events will be displayed
        _displayValueLabel = Text::create("No Event", "fonts/Marker Felt.ttf", 32);
        _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
        _displayValueLabel->setPosition(Vec2(
            screenSize.width / 2.0f, screenSize.height / 2.0f + _displayValueLabel->getContentSize().height * 1.5f));
        _uiLayer->addChild(_displayValueLabel);

        // Add the alert
        Text* alert = Text::create("InputField max length", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color32(159, 168, 176));
        alert->setPosition(
            Vec2(screenSize.width / 2.0f, screenSize.height / 2.0f - alert->getContentSize().height * 3.075f));
        _uiLayer->addChild(alert);

        // Create the inputfield
        InputField* inputField = InputField::create("input words here", "Arial", 30);
        inputField->setCharLimit(3);
        inputField->setTextHorizontalAlignment(TextHAlignment::CENTER);
        inputField->setPosition(Vec2(screenSize.width / 2.0f, screenSize.height / 2.0f));

        // Set up event listener for InputField
        inputField->addEventListener([this, inputField](Object* sender, InputField::EventType ev) {
            switch (ev)
            {
            case InputField::EventType::ATTACH_WITH_IME:
                _displayValueLabel->setString(fmt::format("attach with IME max length {}", inputField->getCharLimit()));
                break;
            case InputField::EventType::DETACH_WITH_IME:
                _displayValueLabel->setString(fmt::format("detach with IME max length {}", inputField->getCharLimit()));
                break;
            case InputField::EventType::INSERT_TEXT:
                _displayValueLabel->setString(fmt::format("insert words max length {}", inputField->getCharLimit()));
                break;
            case InputField::EventType::DELETE_BACKWARD:
                _displayValueLabel->setString(fmt::format("delete word max length {}", inputField->getCharLimit()));
                break;
            }
        });

        _uiLayer->addChild(inputField);

        return true;
    }
    return false;
}

// UIInputFieldTest_Password
UIInputFieldTest_Password::UIInputFieldTest_Password() : _displayValueLabel(nullptr) {}

UIInputFieldTest_Password::~UIInputFieldTest_Password() {}

bool UIInputFieldTest_Password::init()
{
    if (UIScene::init())
    {
        Size screenSize = Director::getInstance()->getCanvasSize();

        // Add a label in which the inputfield events will be displayed
        _displayValueLabel = Text::create("No Event", "fonts/Marker Felt.ttf", 32);
        _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
        _displayValueLabel->setPosition(Vec2(
            screenSize.width / 2.0f, screenSize.height / 2.0f + _displayValueLabel->getContentSize().height * 1.5f));
        _uiLayer->addChild(_displayValueLabel);

        // Add the alert
        Text* alert = Text::create("InputField password", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color32(159, 168, 176));
        alert->setPosition(
            Vec2(screenSize.width / 2.0f, screenSize.height / 2.0f - alert->getContentSize().height * 3.075f));
        _uiLayer->addChild(alert);

        // Create the inputfield
        InputField* inputField = InputField::create("input password here", "Arial", 30);
        inputField->setPasswordEnabled(true);
        inputField->setTextHorizontalAlignment(TextHAlignment::CENTER);
        inputField->setPosition(Vec2(screenSize.width / 2.0f, screenSize.height / 2.0f));

        // Set up event listener for InputField
        inputField->addEventListener([this](Object* sender, InputField::EventType ev) {
            switch (ev)
            {
            case InputField::EventType::ATTACH_WITH_IME:
                _displayValueLabel->setString("attach with IME password");
                break;
            case InputField::EventType::DETACH_WITH_IME:
                _displayValueLabel->setString("detach with IME password");
                break;
            case InputField::EventType::INSERT_TEXT:
                _displayValueLabel->setString("insert words password");
                break;
            case InputField::EventType::DELETE_BACKWARD:
                _displayValueLabel->setString("delete word password");
                break;
            }
        });

        _uiLayer->addChild(inputField);

        return true;
    }
    return false;
}

// UIInputFieldTest_LineWrap
UIInputFieldTest_LineWrap::UIInputFieldTest_LineWrap() : _displayValueLabel(nullptr) {}

UIInputFieldTest_LineWrap::~UIInputFieldTest_LineWrap() {}

bool UIInputFieldTest_LineWrap::init()
{
    if (UIScene::init())
    {
        Size widgetSize = _widget->getContentSize();

        // Add a label in which the inputfield events will be displayed
        _displayValueLabel = Text::create("No Event", "fonts/Marker Felt.ttf", 30);
        _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
        _displayValueLabel->setPosition(Vec2(
            widgetSize.width / 2.0f, widgetSize.height / 2.0f + _displayValueLabel->getContentSize().height * 1.5));
        _uiLayer->addChild(_displayValueLabel);

        // Add the alert
        Text* alert = Text::create("InputField line wrap", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color32(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.075));
        _uiLayer->addChild(alert);

        // Create the inputfield
        InputField* inputField = InputField::create("input words here", "fonts/Marker Felt.ttf", 30);
        inputField->setAutoSize(false);
        inputField->setMultilineEnabled(true);
        inputField->setContentSize(Vec2(240.0f, 170.0f));
        inputField->setString("input words here");
        inputField->setTextHorizontalAlignment(TextHAlignment::LEFT);
        inputField->setTextVerticalAlignment(TextVAlignment::TOP);
        inputField->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f));

        // Set up event listener for InputField
        inputField->addEventListener([this](Object* sender, InputField::EventType ev) {
            switch (ev)
            {
            case InputField::EventType::ATTACH_WITH_IME:
                _displayValueLabel->setString("attach with IME");
                break;
            case InputField::EventType::DETACH_WITH_IME:
                _displayValueLabel->setString("detach with IME");
                break;
            case InputField::EventType::INSERT_TEXT:
                _displayValueLabel->setString("insert words");
                break;
            case InputField::EventType::DELETE_BACKWARD:
                _displayValueLabel->setString("delete word");
                break;
            }
        });

        _uiLayer->addChild(inputField);

        return true;
    }
    return false;
}

// UIInputFieldTest_TrueTypeFont
UIInputFieldTest_TrueTypeFont::UIInputFieldTest_TrueTypeFont() : _displayValueLabel(nullptr) {}

UIInputFieldTest_TrueTypeFont::~UIInputFieldTest_TrueTypeFont() {}

bool UIInputFieldTest_TrueTypeFont::init()
{
    if (UIScene::init())
    {
        Size widgetSize = _widget->getContentSize();

        // Add a label in which the inputfield events will be displayed
        _displayValueLabel = Text::create("True Type Font Test - No Event", "fonts/Marker Felt.ttf", 32);
        _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
        _displayValueLabel->setPosition(Vec2(
            widgetSize.width / 2.0f, widgetSize.height / 2.0f + _displayValueLabel->getContentSize().height * 1.5f));
        _uiLayer->addChild(_displayValueLabel);

        // Add the alert
        Text* alert = Text::create("InputField", "fonts/Marker Felt.ttf", 30);
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.075f));
        _uiLayer->addChild(alert);

        // Create the inputfield
        InputField* inputField = InputField::create("input words here", "fonts/A Damn Mess.ttf", 30);
        inputField->setTextHorizontalAlignment(TextHAlignment::CENTER);
        inputField->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f));

        // Set up event listener for InputField
        inputField->addEventListener([this](Object* sender, InputField::EventType ev) {
            switch (ev)
            {
            case InputField::EventType::ATTACH_WITH_IME:
                _displayValueLabel->setString("attach with IME");
                break;
            case InputField::EventType::DETACH_WITH_IME:
                _displayValueLabel->setString("detach with IME");
                break;
            case InputField::EventType::INSERT_TEXT:
                _displayValueLabel->setString("insert words");
                break;
            case InputField::EventType::DELETE_BACKWARD:
                _displayValueLabel->setString("delete word");
                break;
            }
        });

        _uiLayer->addChild(inputField);

        return true;
    }
    return false;
}

// UIInputFieldTest_BMFont
UIInputFieldTest_BMFont::UIInputFieldTest_BMFont() : _displayValueLabel(nullptr) {}

UIInputFieldTest_BMFont::~UIInputFieldTest_BMFont() {}

bool UIInputFieldTest_BMFont::init()
{
    if (UIScene::init())
    {
        Size widgetSize = _widget->getContentSize();

        // Add a label in which the inputfield events will be displayed
        _displayValueLabel = Text::create("BMFont Test - No Event", "fonts/Marker Felt.ttf", 32);
        _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
        _displayValueLabel->setPosition(Vec2(
            widgetSize.width / 2.0f, widgetSize.height / 2.0f + _displayValueLabel->getContentSize().height * 1.5f));
        _uiLayer->addChild(_displayValueLabel);

        // Add the alert
        Text* alert = Text::create("InputField", "fonts/Marker Felt.ttf", 30);
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.075f));
        _uiLayer->addChild(alert);

        // Create the inputfield
        InputField* inputField = InputField::create("BMFont Text", "fonts/bitmapFontTest3.fnt", 30);
        inputField->setTextHorizontalAlignment(TextHAlignment::CENTER);
        inputField->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f));

        // Set up event listener for InputField
        inputField->addEventListener([this](Object* sender, InputField::EventType ev) {
            switch (ev)
            {
            case InputField::EventType::ATTACH_WITH_IME:
                _displayValueLabel->setString("attach with IME");
                break;
            case InputField::EventType::DETACH_WITH_IME:
                _displayValueLabel->setString("detach with IME");
                break;
            case InputField::EventType::INSERT_TEXT:
                _displayValueLabel->setString("insert words");
                break;
            case InputField::EventType::DELETE_BACKWARD:
                _displayValueLabel->setString("delete word");
                break;
            }
        });

        _uiLayer->addChild(inputField);

        return true;
    }
    return false;
}

// UIInputFieldTest_PlaceHolderColor
UIInputFieldTest_PlaceHolderColor::UIInputFieldTest_PlaceHolderColor() : _displayValueLabel(nullptr) {}

UIInputFieldTest_PlaceHolderColor::~UIInputFieldTest_PlaceHolderColor() {}

bool UIInputFieldTest_PlaceHolderColor::init()
{
    if (UIScene::init())
    {
        Size widgetSize = _widget->getContentSize();

        // Add a label in which the inputfield events will be displayed
        _displayValueLabel =
            Text::create("You should see 16.50000, 34.0000 in the output window the first time you type",
                         "fonts/Marker Felt.ttf", 12);
        _displayValueLabel->setAnchorPoint(Vec2(0.5f, -1.0f));
        _displayValueLabel->setPosition(Vec2(
            widgetSize.width / 2.0f, widgetSize.height / 2.0f + _displayValueLabel->getContentSize().height * 1.5f));
        _uiLayer->addChild(_displayValueLabel);

        // Add the alert
        Text* alert = Text::create("InputField", "fonts/Marker Felt.ttf", 30);
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.075f));
        _uiLayer->addChild(alert);

        // Create the inputfield
        InputField* inputField = InputField::create("input words here", "Arial", 30);
        inputField->setPlaceholderText("input text here");
        inputField->setPlaceholderColor(Color32::green);
        inputField->setTextColor(Color32::red);
        inputField->setTextHorizontalAlignment(TextHAlignment::CENTER);
        inputField->setPosition(Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f));

        // Set up event listener for InputField
        inputField->addEventListener([this, inputField](Object* sender, InputField::EventType ev) {
            switch (ev)
            {
            case InputField::EventType::ATTACH_WITH_IME:
                _displayValueLabel->setString("attach with IME");
                break;
            case InputField::EventType::DETACH_WITH_IME:
                _displayValueLabel->setString("detach with IME");
                break;
            case InputField::EventType::INSERT_TEXT:
                _displayValueLabel->setString("text modified");
                AXLOGD("{}, {}", inputField->getContentSize().width, inputField->getContentSize().height);
                break;
            case InputField::EventType::DELETE_BACKWARD:
                _displayValueLabel->setString("text modified");
                AXLOGD("{}, {}", inputField->getContentSize().width, inputField->getContentSize().height);
                break;
            }
        });

        _uiLayer->addChild(inputField);
        return true;
    }
    return false;
}
