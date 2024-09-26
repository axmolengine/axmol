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

#include "UIRichTextTest.h"
#include "cocostudio/ArmatureDataManager.h"
#include "cocostudio/Armature.h"

using namespace ax;
using namespace ax::ui;

UIRichTextTests::UIRichTextTests()
{
    ADD_TEST_CASE(UIRichTextTest);
    ADD_TEST_CASE(UIRichTextXMLBasic);
    ADD_TEST_CASE(UIRichTextXMLSmallBig);
    ADD_TEST_CASE(UIRichTextXMLColor);
    ADD_TEST_CASE(UIRichTextXMLSUIB);
    ADD_TEST_CASE(UIRichTextXMLSUIB2);
    ADD_TEST_CASE(UIRichTextXMLSUIB3);
    ADD_TEST_CASE(UIRichTextXMLImg);
    ADD_TEST_CASE(UIRichTextXMLUrl);
    ADD_TEST_CASE(UIRichTextXMLUrlImg);
    ADD_TEST_CASE(UIRichTextXMLFace);
    ADD_TEST_CASE(UIRichTextXMLBR);
    ADD_TEST_CASE(UIRichTextXMLInvalid);
    ADD_TEST_CASE(UIRichTextXMLOutline);
    ADD_TEST_CASE(UIRichTextXMLShadow);
    ADD_TEST_CASE(UIRichTextXMLGlow);
    ADD_TEST_CASE(UIRichTextXMLExtend);
    ADD_TEST_CASE(UIRichTextXMLSpace);
    ADD_TEST_CASE(UIRichTextNewline);
    ADD_TEST_CASE(UIRichTextHeaders);
    ADD_TEST_CASE(UIRichTextParagraph);
    ADD_TEST_CASE(UIRichTextScrollTo);
}

//
// UIRichTextTestBase
//
bool UIRichTextTestBase::init()
{
    if (!UIScene::init())
    {
        return false;
    }

    _defaultContentSize = Size(100, 100);

    return true;
}

void UIRichTextTestBase::touchEvent(Object* pSender, Widget::TouchEventType type)
{
    switch (type)
    {
    case Widget::TouchEventType::ENDED:
    {
        if (_richText->isIgnoreContentAdaptWithSize())
        {
            _richText->ignoreContentAdaptWithSize(false);
            _richText->setContentSize(_defaultContentSize);
        }
        else
        {
            _richText->ignoreContentAdaptWithSize(true);
        }
    }
    break;

    default:
        break;
    }
}

void UIRichTextTestBase::switchWrapMode(Object* pSender, Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::ENDED)
    {
        auto wrapMode = _richText->getWrapMode();
        wrapMode      = (wrapMode == RichText::WRAP_PER_WORD) ? RichText::WRAP_PER_CHAR : RichText::WRAP_PER_WORD;
        _richText->setWrapMode(wrapMode);
    }
}

void UIRichTextTestBase::switchHorizontalAlignment(Object* sender, Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::ENDED)
    {
        auto alignment = _richText->getHorizontalAlignment();
        alignment      = static_cast<RichText::HorizontalAlignment>(
            (static_cast<std::underlying_type_t<RichText::HorizontalAlignment>>(alignment) + 1) % 3);
        _richText->setHorizontalAlignment(alignment);
    }
}

void UIRichTextTestBase::switchVerticalAlignment(ax::Object* sender, ax::ui::Widget::TouchEventType type)
{
    if (type == Widget::TouchEventType::ENDED)
    {
        auto alignment = _richText->getVerticalAlignment();
        alignment      = static_cast<RichText::VerticalAlignment>(
            (static_cast<std::underlying_type_t<RichText::VerticalAlignment>>(alignment) + 1) % 3);
        _richText->setVerticalAlignment(alignment);
    }
}

void UIRichTextTestBase::createButtonPanel()
{
    auto& widgetSize = _widget->getContentSize();

    auto* buttonPanel = Layout::create();
    buttonPanel->setContentSize(Vec2(widgetSize.width, 1));
    buttonPanel->setLayoutType(Layout::Type::CENTER_HORIZONTAL);
    buttonPanel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _widget->addChild(buttonPanel, 10);

    auto maxButtonHeight = 0.f;

    Button* button = Button::create("cocosui/animationbuttonnormal.png", "cocosui/animationbuttonpressed.png");
    button->setTouchEnabled(true);
    button->setTitleText("switch");
    button->addTouchEventListener(AX_CALLBACK_2(UIRichTextTest::touchEvent, this));
    buttonPanel->addChild(button);
    maxButtonHeight = std::max(maxButtonHeight, button->getContentSize().height);

    Button* button2 = Button::create("cocosui/animationbuttonnormal.png", "cocosui/animationbuttonpressed.png");
    button2->setTouchEnabled(true);
    button2->setTitleText("wrap mode");
    button2->addTouchEventListener(AX_CALLBACK_2(UIRichTextTest::switchWrapMode, this));
    buttonPanel->addChild(button2);
    maxButtonHeight = std::max(maxButtonHeight, button2->getContentSize().height);

    Button* button3 = Button::create("cocosui/animationbuttonnormal.png", "cocosui/animationbuttonpressed.png");
    button3->setTouchEnabled(true);
    button3->setTitleText("H-alignment");
    button3->addTouchEventListener(AX_CALLBACK_2(UIRichTextTest::switchHorizontalAlignment, this));
    buttonPanel->addChild(button3);
    maxButtonHeight = std::max(maxButtonHeight, button3->getContentSize().height);

    Button* button4 = Button::create("cocosui/animationbuttonnormal.png", "cocosui/animationbuttonpressed.png");
    button4->setTouchEnabled(true);
    button4->setTitleText("V-alignment");
    button4->addTouchEventListener(AX_CALLBACK_2(UIRichTextTest::switchVerticalAlignment, this));
    buttonPanel->addChild(button4);
    maxButtonHeight = std::max(maxButtonHeight, button4->getContentSize().height);

    buttonPanel->setContentSize(Vec2(widgetSize.width, maxButtonHeight));
    buttonPanel->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2 + maxButtonHeight * 2.5f));
}

//
// UIRichTextTest
//
bool UIRichTextTest::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

        auto config = Configuration::getInstance();
        config->loadConfigFile("configs/config-test-ok.plist");

        std::string str1 = config->getValue("Chinese").asString();
        std::string str2 = config->getValue("Japanese").asString();
        AXLOGD("str1:{} ascii length = {}, utf8 length = {}, substr = {}", str1,
              static_cast<int32_t>(str1.length()), StringUtils::getCharacterCountInUTF8String(str1),
              Helper::getSubStringOfUTF8String(str1, 0, 5));
        AXLOGD("str2:{} ascii length = {}, utf8 length = {}, substr = {}", str2,
              static_cast<int32_t>(str2.length()), StringUtils::getCharacterCountInUTF8String(str2),
              Helper::getSubStringOfUTF8String(str2, 0, 2));

        // Add the alert
        Text* alert = Text::create("RichText", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125f));
        _widget->addChild(alert);

        createButtonPanel();

        // RichText
        _richText = RichText::create();
        _richText->ignoreContentAdaptWithSize(false);
        _richText->setContentSize(_defaultContentSize);

        RichElementText* re1 = RichElementText::create(1, Color3B::WHITE, 255, str1, "SimSun", 10);
        RichElementText* re2 =
            RichElementText::create(2, Color3B::YELLOW, 255, "And this is yellow. ", "Helvetica", 10);
        RichElementText* re3 = RichElementText::create(3, Color3B::GRAY, 255, str2, "Yu Mincho", 10);
        RichElementText* re4 = RichElementText::create(4, Color3B::GREEN, 255, "And green with TTF support. ",
                                                       "fonts/Marker Felt.ttf", 10);
        RichElementText* re5 = RichElementText::create(5, Color3B::RED, 255, "Last one is red ", "Helvetica", 10);

        RichElementImage* reimg = RichElementImage::create(6, Color3B::WHITE, 255, "cocosui/sliderballnormal.png");

        //        TODO
        //        cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo("cocosui/100/100.ExportJson");
        //        cocostudio::Armature *pAr = cocostudio::Armature::create("100"); //
        //        pAr->getAnimation()->play("Animation1");

        //        RichElementCustomNode* recustom = RichElementCustomNode::create(1, Color3B::WHITE, 255, pAr);
        RichElementText* re6 = RichElementText::create(7, Color3B::ORANGE, 255, "Have fun!! ", "Helvetica", 10);
        _richText->pushBackElement(re1);
        _richText->insertElement(re2, 1);
        _richText->pushBackElement(re3);
        _richText->pushBackElement(re4);
        _richText->pushBackElement(re5);
        _richText->insertElement(reimg, 2);
        //        _richText->pushBackElement(recustom);
        _richText->pushBackElement(re6);

        _richText->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2));
        _richText->setLocalZOrder(10);

        _widget->addChild(_richText);

        // test remove all children, this call won't effect the test
        _richText->removeAllChildren();

        return true;
    }
    return false;
}

//
// UIRichTextXMLBasic
//
bool UIRichTextXMLBasic::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

        // Add the alert
        Text* alert = Text::create("RichText", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125));
        _widget->addChild(alert);

        createButtonPanel();

        // RichText
        _richText = RichText::createWithXML(
            "This is just simple text, with no XML tags. Testing the basics. Testing word-wrapping. Testing (\"punctuation\") "
            "Testing, Testing");
        _richText->ignoreContentAdaptWithSize(false);
        _richText->setContentSize(_defaultContentSize);

        _richText->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2));
        _richText->setLocalZOrder(10);

        _widget->addChild(_richText);

        // test remove all children, this call won't effect the test
        _richText->removeAllChildren();

        return true;
    }
    return false;
}

//
// UIRichTextXMLSmallBig
//
bool UIRichTextXMLSmallBig::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

        // Add the alert
        Text* alert = Text::create("RichText", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125));
        _widget->addChild(alert);

        createButtonPanel();

        // RichText
        _richText = RichText::createWithXML(
            "Regular size.<small>smaller size.</small><big>bigger.<small>normal.</small>bigger</big>.normal.");
        _richText->ignoreContentAdaptWithSize(false);
        _richText->setContentSize(_defaultContentSize);

        _richText->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2));
        _richText->setLocalZOrder(10);

        _widget->addChild(_richText);

        // test remove all children, this call won't effect the test
        _richText->removeAllChildren();

        return true;
    }
    return false;
}

//
// UIRichTextXMLColor
//
bool UIRichTextXMLColor::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

        // Add the alert
        Text* alert = Text::create("RichText", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125));
        _widget->addChild(alert);

        createButtonPanel();

        // RichText
        _richText = RichText::createWithXML(
            "Default color.<font color='#ff0000'>red.<font color='#00ff00'>green</font>red again.</font>default again");
        _richText->ignoreContentAdaptWithSize(false);
        _richText->setContentSize(_defaultContentSize);

        _richText->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2));
        _richText->setLocalZOrder(10);

        _widget->addChild(_richText);

        // test remove all children, this call won't effect the test
        _richText->removeAllChildren();

        return true;
    }
    return false;
}

//
// UIRichTextXMLSUIB
//
bool UIRichTextXMLSUIB::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

        // Add the alert
        Text* alert = Text::create("RichText", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125));
        _widget->addChild(alert);

        createButtonPanel();

        // RichText
        _richText =
            RichText::createWithXML("system font: <u>underline</u><i>italics</i><b>bold</b><del>strike-through</del>");
        _richText->ignoreContentAdaptWithSize(false);
        _richText->setContentSize(_defaultContentSize);

        _richText->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2));
        _richText->setLocalZOrder(10);

        _widget->addChild(_richText);

        // test remove all children, this call won't effect the test
        _richText->removeAllChildren();

        return true;
    }
    return false;
}

//
// UIRichTextXMLSUIB2
//
bool UIRichTextXMLSUIB2::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

        // Add the alert
        Text* alert = Text::create("RichText", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125));
        _widget->addChild(alert);

        createButtonPanel();

        // RichText
        _richText = RichText::createWithXML(
            "<font face='fonts/Marker Felt.ttf' size='24'>ttf font: "
            "<u>underline</u><i>italics</i><b>bold</b><del>strike-through</del></font>");
        _richText->ignoreContentAdaptWithSize(false);
        _richText->setContentSize(_defaultContentSize);

        _richText->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2));
        _richText->setLocalZOrder(10);

        _widget->addChild(_richText);

        // test remove all children, this call won't effect the test
        _richText->removeAllChildren();

        return true;
    }
    return false;
}

//
// UIRichTextXMLSUIB3
//
bool UIRichTextXMLSUIB3::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

        // Add the alert
        Text* alert = Text::create("RichText", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125));
        _widget->addChild(alert);

        createButtonPanel();

        // RichText
        _richText = RichText::createWithXML(
            "<font face='fonts/Marker Felt.ttf' size='20'>ttf font: <i><u>italics and underline</u></i><del><b>bold "
            "and strike-through</b></del></font>");
        _richText->ignoreContentAdaptWithSize(false);
        _richText->setContentSize(_defaultContentSize);

        _richText->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2));
        _richText->setLocalZOrder(10);

        _widget->addChild(_richText);

        // test remove all children, this call won't effect the test
        _richText->removeAllChildren();

        return true;
    }
    return false;
}

//
// UIRichTextXMLImg
//
bool UIRichTextXMLImg::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

        // Add the alert
        Text* alert = Text::create("RichText", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125));
        _widget->addChild(alert);

        createButtonPanel();

#ifdef AX_PLATFORM_PC
        _defaultContentSize = Size(290, 290);
#endif

        // RichText
        _richText = RichText::createWithXML(
            "Image: <img src='cocosui/sliderballnormal.png'/> and this is text again."
            "Image with width/height: <img src='cocosui/sliderballnormal.png' width='30' height='30' /> and "
            "Image with scaleX/scaleY: <img src='cocosui/sliderballnormal.png' scaleX='3' scaleY='3' /> and "
            "Image with width%/height%: <img src='cocosui/sliderballnormal.png' width='150%' height='150%' /> and "
            "Image with empty width/height%: <img src='cocosui/sliderballnormal.png' width='' height='150%' /> and "
            "Image with scale: <img src='cocosui/sliderballnormal.png' scale='3.5' /> and "
            "Image with w/h/sX/sY: <img src='cocosui/sliderballnormal.png' width='30' height='30' scaleX='0.5' scaleY='0.5' /> and "
            "goes text again");
        _richText->ignoreContentAdaptWithSize(false);
        _richText->setContentSize(_defaultContentSize);

        _richText->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2));
        _richText->setLocalZOrder(10);

        _widget->addChild(_richText);

        // test remove all children, this call won't effect the test
        _richText->removeAllChildren();

        return true;
    }
    return false;
}

//
// UIRichTextXMLUrl
//
bool UIRichTextXMLUrl::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

        // Add the alert
        Text* alert = Text::create("RichText", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125));
        _widget->addChild(alert);

        createButtonPanel();

        // RichText
        _richText = RichText::createWithXML(
            "This link will redirect you to google: <a href='http://www.google.com'>click me</a>");
        _richText->ignoreContentAdaptWithSize(false);
        _richText->setContentSize(_defaultContentSize);

        _richText->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2));
        _richText->setLocalZOrder(10);

        _widget->addChild(_richText);

        // test remove all children, this call won't effect the test
        _richText->removeAllChildren();

        return true;
    }
    return false;
}

//
// UIRichTextXMLUrlImg
//
bool UIRichTextXMLUrlImg::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

        // Add the alert
        Text* alert = Text::create("RichText", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125));
        _widget->addChild(alert);

        createButtonPanel();

        // RichText
        _richText = RichText::createWithXML(
            "This link will redirect you to google: <a href='http://www.google.com'><img src=\"cocosui/ccicon.png\" "
            "height=\"48\" width=\"48\" /></a>");
        _richText->ignoreContentAdaptWithSize(false);
        _richText->setContentSize(_defaultContentSize);

        _richText->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2));
        _richText->setLocalZOrder(10);

        _widget->addChild(_richText);

        // test remove all children, this call won't effect the test
        _richText->removeAllChildren();

        return true;
    }
    return false;
}

//
// UIRichTextXMLFace
//
bool UIRichTextXMLFace::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

        // Add the alert
        Text* alert = Text::create("RichText", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125));
        _widget->addChild(alert);

        createButtonPanel();

        _defaultContentSize = Size(120, 100);

        // RichText
        _richText = RichText::createWithXML(
            "<font size='20' face='fonts/Marker Felt.ttf'>Marker Felt 20.<font face='fonts/arial.ttf'>Arial "
            "20.</font></font><font face='font/Thonburi.ttf' size='24' color='#0000ff'>Thonburi 24 blue</font>");
        _richText->ignoreContentAdaptWithSize(false);
        _richText->setContentSize(_defaultContentSize);

        _richText->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2));
        _richText->setLocalZOrder(10);

        _widget->addChild(_richText);

        // test remove all children, this call won't effect the test
        _richText->removeAllChildren();

        return true;
    }
    return false;
}

//
// UIRichTextXMLBR
//
bool UIRichTextXMLBR::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

        // Add the alert
        Text* alert = Text::create("RichText", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125));
        _widget->addChild(alert);

        createButtonPanel();

        // RichText
        _richText = RichText::createWithXML(
            "this is one line.<br/>this should be in another line.<br/>and this is another line");
        _richText->ignoreContentAdaptWithSize(false);
        _richText->setContentSize(_defaultContentSize);

        _richText->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2));
        _richText->setLocalZOrder(10);

        _widget->addChild(_richText);

        // test remove all children, this call won't effect the test
        _richText->removeAllChildren();

        return true;
    }
    return false;
}

//
// UIRichTextXMLInvalid
//
bool UIRichTextXMLInvalid::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

        // Add the alert
        Text* alert = Text::create("Invalid XML test", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125));
        _widget->addChild(alert);

        // RichText
        _richText = RichText::createWithXML("this is an invalid xml. <i>no closing tag");
        if (_richText)
        {
            _richText->ignoreContentAdaptWithSize(false);
            _richText->setContentSize(_defaultContentSize);

            _richText->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2));
            _richText->setLocalZOrder(10);

            _widget->addChild(_richText);

            // test remove all children, this call won't effect the test
            _richText->removeAllChildren();
        }
        return true;
    }
    return false;
}

//
// UIRichTextXMLOutline
//
bool UIRichTextXMLOutline::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

        // Add the alert
        Text* alert = Text::create("Outline", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125));
        _widget->addChild(alert);

        createButtonPanel();

        // RichText
        _richText = RichText::createWithXML(
            "<font face='fonts/Marker Felt.ttf' size=\"24\"><outline color=\"#D2B48C\" "
            "size=\"2\">OUTLINE</outline></font>");
        _richText->ignoreContentAdaptWithSize(false);
        _richText->setContentSize(_defaultContentSize);

        _richText->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2));
        _richText->setLocalZOrder(10);

        _widget->addChild(_richText);

        // test remove all children, this call won't effect the test
        _richText->removeAllChildren();

        return true;
    }
    return false;
}

//
// UIRichTextXMLShadow
//
bool UIRichTextXMLShadow::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

        // Add the alert
        Text* alert = Text::create("Shadow", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125));
        _widget->addChild(alert);

        createButtonPanel();

        _defaultContentSize = Size(150, 100);

        // RichText
        _richText = RichText::createWithXML(
            "<font size=\"24\"><shadow color=\"#4169E1\" offsetWidth=\"8\" offsetHeight=\"-8\" "
            "blurRadius=\"2\">SHADOW</shadow></font>");
        _richText->ignoreContentAdaptWithSize(false);
        _richText->setContentSize(_defaultContentSize);

        _richText->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2));
        _richText->setLocalZOrder(10);

        _widget->addChild(_richText);

        // test remove all children, this call won't effect the test
        _richText->removeAllChildren();

        return true;
    }
    return false;
}

//
// UIRichTextXMLGlow
//
bool UIRichTextXMLGlow::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

        // Add the alert
        Text* alert = Text::create("Glow", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125));
        _widget->addChild(alert);

        createButtonPanel();

        // RichText
        _richText = RichText::createWithXML(
            "<font face=\"fonts/Marker Felt.ttf\" size=\"24\"><glow color=\"#AFEEEE\">GLOW</glow></font>");
        _richText->ignoreContentAdaptWithSize(false);
        _richText->setContentSize(_defaultContentSize);

        _richText->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2));
        _richText->setLocalZOrder(10);

        _widget->addChild(_richText);

        // test remove all children, this call won't effect the test
        _richText->removeAllChildren();

        return true;
    }
    return false;
}

//
// UIRichTextXMLExtend
//
bool UIRichTextXMLExtend::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

        // Add the alert
        Text* alert = Text::create("Extend", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125));
        _widget->addChild(alert);

        createButtonPanel();

        /* Tag extension */
        RichText::setTagDescription("CloseNormal", false, [](const ValueMap& tagAttrValueMap) {
            RichElementImage* richElement = RichElementImage::create(0, Color3B::WHITE, 255, "cocosui/CloseNormal.png");
            return make_pair(ValueMap(), richElement);
        });
        RichText::setTagDescription("CloseSelected", false, [](const ValueMap& tagAttrValueMap) {
            RichElementImage* richElement =
                RichElementImage::create(0, Color3B::WHITE, 255, "cocosui/CloseSelected.png");
            return make_pair(ValueMap(), richElement);
        });

        /* Defaults */
        ValueMap defaults;
        defaults[RichText::KEY_FONT_COLOR_STRING]        = "#FFF";
        defaults[RichText::KEY_FONT_SIZE]                = 12.0f;
        defaults[RichText::KEY_FONT_FACE]                = "fonts/Marker Felt.ttf";
        defaults[RichText::KEY_ANCHOR_FONT_COLOR_STRING] = "#f0f8ff";
        defaults[RichText::KEY_ANCHOR_TEXT_BOLD]         = false;
        defaults[RichText::KEY_ANCHOR_TEXT_ITALIC]       = false;
        // defaults[RichText::KEY_ANCHOR_TEXT_LINE] = RichText::VALUE_TEXT_LINE_NONE;
        // defaults[RichText::KEY_ANCHOR_TEXT_LINE] = RichText::VALUE_TEXT_LINE_DEL;
        defaults[RichText::KEY_ANCHOR_TEXT_LINE] = RichText::VALUE_TEXT_LINE_UNDER;
        // defaults[RichText::KEY_ANCHOR_TEXT_STYLE] = RichText::VALUE_TEXT_STYLE_NONE;
        // defaults[RichText::KEY_ANCHOR_TEXT_STYLE] = RichText::VALUE_TEXT_STYLE_OUTLINE;
        // defaults[RichText::KEY_ANCHOR_TEXT_STYLE] = RichText::VALUE_TEXT_STYLE_SHADOW;
        // defaults[RichText::KEY_ANCHOR_TEXT_STYLE] = RichText::VALUE_TEXT_STYLE_GLOW;
        defaults[RichText::KEY_ANCHOR_TEXT_OUTLINE_COLOR]        = "#D2B48C";
        defaults[RichText::KEY_ANCHOR_TEXT_OUTLINE_SIZE]         = 4;
        defaults[RichText::KEY_ANCHOR_TEXT_SHADOW_COLOR]         = "#4169E1";
        defaults[RichText::KEY_ANCHOR_TEXT_SHADOW_OFFSET_WIDTH]  = 4.0f;
        defaults[RichText::KEY_ANCHOR_TEXT_SHADOW_OFFSET_HEIGHT] = -4.0f;
        defaults[RichText::KEY_ANCHOR_TEXT_SHADOW_BLUR_RADIUS]   = 0;
        defaults[RichText::KEY_ANCHOR_TEXT_GLOW_COLOR]           = "#AFEEEE";

        // RichText
        _richText = RichText::createWithXML(
            "<span>CloseNormal-tag:<br /><CloseNormal /><br /><br />CloseSelected-tag:<br "
            "/><CloseSelected></CloseSelected></span>",
            defaults, [](std::string_view url) { Application::getInstance()->openURL(url); });
        _richText->ignoreContentAdaptWithSize(false);
        _richText->setContentSize(_defaultContentSize);

        _richText->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2));
        _richText->setLocalZOrder(10);

        _widget->addChild(_richText);

        // test remove all children, this call won't effect the test
        _richText->removeAllChildren();

        return true;
    }
    return false;
}

//
// UIRichTextXMLSpace
//
bool UIRichTextXMLSpace::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

        // Add the alert
        Text* alert = Text::create("Space", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125));
        _widget->addChild(alert);

        createButtonPanel();

        _defaultContentSize = Size(50, 100);

        // RichText
        _richText = RichText::createWithXML(
            "words should be divided with space.<br /><br /><font face='fonts/Marker Felt.ttf' color='#ffff00'>HELLO "
            "</font><font color='#ffff00'>WORLD</font><br /><br /><font color='#ff00ff'>HELLO</font><font "
            "face='fonts/Marker Felt.ttf' color='#ff00ff'> WORLD</font>");
        _richText->ignoreContentAdaptWithSize(false);
        _richText->setContentSize(_defaultContentSize);
        _richText->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2));
        _richText->setLocalZOrder(10);

        _widget->addChild(_richText);

        // test remove all children, this call won't effect the test
        _richText->removeAllChildren();

        return true;
    }
    return false;
}

//
// UIRichTextNewline
//
bool UIRichTextNewline::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

                // Add the alert
        Text* alert = Text::create("Axmol Issue #1170", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125));
        _widget->addChild(alert);

        createButtonPanel();

        _defaultContentSize = Size(50, 100);

        _richText = RichText::create();
        _richText->setHorizontalAlignment(ui::RichText::HorizontalAlignment::CENTER);
        auto* textElement =
            ui::RichElementText::create(1, Color3B::WHITE, 255, "Line1\nLine2\n", "fonts/Marker Felt.ttf", 32, 0);
        _richText->pushBackElement(textElement);
        textElement = ui::RichElementText::create(2, Color3B::WHITE, 255, "Line3", "fonts/Marker Felt.ttf", 32, 0);
        _richText->pushBackElement(textElement);
        _richText->ignoreContentAdaptWithSize(false);
        _richText->setContentSize(_defaultContentSize);
        _richText->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2));
        _richText->setLocalZOrder(10);

        _widget->addChild(_richText);

        // test remove all children, this call won't effect the test
        _richText->removeAllChildren();

        return true;
    }
    return false;
}

bool UIRichTextHeaders::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

        // Add the alert
        Text* alert = Text::create("Header Tags", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125));
        _widget->addChild(alert);

        createButtonPanel();

#ifdef AX_PLATFORM_PC
        _defaultContentSize = Size(290, 290);
#endif

        // RichText
        _richText = RichText::createWithXML(
            R"(<h1 face="fonts/arial.ttf">h1. HEADING</h1><h2 face="fonts/Marker Felt.ttf">h2. HEADING</h2><h3 face="fonts/American Typewriter.ttf">h3. HEADING</h3><h4>h4. HEADING</h4><h5>h5. HEADING</h5><h6>h6. HEADING</h6>)");
        _richText->ignoreContentAdaptWithSize(false);
        _richText->setContentSize(_defaultContentSize);

        _richText->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2));
        _richText->setLocalZOrder(10);

        _widget->addChild(_richText);

        // test remove all children, this call won't effect the test
        _richText->removeAllChildren();

        return true;
    }
    return false;
}

bool UIRichTextParagraph::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

        // Add the alert
        Text* alert = Text::create("Paragraph Tag", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125));
        _widget->addChild(alert);

        createButtonPanel();

#ifdef AX_PLATFORM_PC
        _defaultContentSize = Size(290, 290);
#endif

        // RichText
        _richText = RichText::createWithXML(
            "<p><b>Paragraph1: </b>Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et "
            "dolore magna aliqua. Feugiat scelerisque varius morbi enim nunc. Dis parturient montes nascetur ridiculus "
            "mus mauris vitae ultricies.</p><p><b>Paragraph2: </b>Lectus urna duis convallis convallis tellus id interdum velit. "
            "Convallis a cras semper auctor neque vitae tempus quam pellentesque. Congue quisque egestas diam in arcu "
            "cursus euismod quis.</p>");
        _richText->ignoreContentAdaptWithSize(false);
        _richText->setContentSize(_defaultContentSize);

        _richText->setPosition(Vec2(widgetSize.width / 2, widgetSize.height / 2));
        _richText->setLocalZOrder(10);

        _widget->addChild(_richText);

        // test remove all children, this call won't effect the test
        _richText->removeAllChildren();

        return true;
    }
    return false;
}

bool UIRichTextScrollTo::init()
{
    if (UIRichTextTestBase::init())
    {
        auto& widgetSize = _widget->getContentSize();

        // Add the alert
        Text* alert = Text::create("Paragraph Tag", "fonts/Marker Felt.ttf", 30);
        alert->setColor(Color3B(159, 168, 176));
        alert->setPosition(
            Vec2(widgetSize.width / 2.0f, widgetSize.height / 2.0f - alert->getContentSize().height * 3.125));
        _widget->addChild(alert);

        createButtonPanel();

#ifdef AX_PLATFORM_PC
        _defaultContentSize = Size(290, 150);
#endif

        // ScrollView
        _scrollView = ScrollView::create();
        _scrollView->setContentSize(_defaultContentSize);
        _scrollView->setInnerContainerSize(_defaultContentSize);
        _scrollView->setBounceEnabled(true);
        _scrollView->setDirection(ScrollView::Direction::VERTICAL);
        _scrollView->setLayoutType(Layout::Type::VERTICAL);
        _scrollView->setScrollBarEnabled(true);
        _scrollView->setScrollBarWidth(4);
        _scrollView->setScrollBarPositionFromCorner(Vec2(2, 2));
        _scrollView->setScrollBarColor(Color3B::WHITE);
        _scrollView->setScrollBarAutoHideEnabled(false);
        _scrollView->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        _scrollView->setPosition(_widget->getContentSize() / 2);
        _scrollView->setLocalZOrder(10);
        _widget->addChild(_scrollView);

        ValueMap valMap;
        valMap[RichText::KEY_ANCHOR_FONT_COLOR_STRING] = "#00ffdd";
        
        // RichText
        _richText = RichText::createWithXML(
            R"(<h1>Quick Links</h1>
<a href="#fancy_header">Jump To Fancy Header</a><br/>
<a href="#paragraph_2">Jump To Second Paragraph</a><br/>
<a href="#some_link">Jump To Web Search</a><br/>
<br/>
<p>Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et 
dolore magna aliqua. 
Purus faucibus ornare suspendisse sed nisi. Viverra aliquet eget sit amet tellus cras adipiscing. 
Ut tellus elementum sagittis vitae. 
Risus feugiat in ante metus dictum at. Semper eget duis at tellus at. Iaculis eu non diam phasellus vestibulum 
lorem sed risus. 
Sed vulputate odio ut enim. Morbi tristique senectus et netus et malesuada fames.</p>
<h1 id="fancy_header">Fancy Header</h1>
<p id="paragraph_2">This is the second paragraph! Cras sed felis eget velit aliquet sagittis id consectetur purus. 
Turpis nunc eget lorem dolor sed viverra ipsum nunc. 
Ultrices tincidunt arcu non sodales neque sodales ut etiam sit. Risus feugiat in ante metus dictum at tempor. 
Id neque aliquam vestibulum morbi blandit cursus risus.</p>
<p>Tortor condimentum lacinia quis vel eros donec ac. Molestie ac feugiat sed lectus. 
Aliquam id diam maecenas ultricies mi eget mauris. 
Ullamcorper malesuada proin libero nunc consequat interdum varius. 
Sollicitudin nibh sit amet commodo nulla facilisi nullam vehicula ipsum. 
Diam quam nulla porttitor massa id neque aliquam vestibulum morbi. Sed velit dignissim sodales ut. 
Morbi leo urna molestie at elementum eu facilisis. 
Cursus metus aliquam eleifend mi in. Euismod lacinia at quis risus sed vulputate odio. 
Sit amet mattis vulputate enim nulla aliquet porttitor lacus luctus.</p>
<a id="some_link" href="https://google.com">Google!</a>)",
            valMap);
        _richText->ignoreContentAdaptWithSize(false);
        _richText->setContentSize(Size(_defaultContentSize.width, 0));
        _richText->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
        _richText->setOpenUrlHandler([this](std::string_view url) {
            // Check if the href starts with a "#" character
            if (url.starts_with('#'))
            {
                auto* node = _richText->getProtectedChildByName(url.substr(1));
                if (node)
                {
                    // Scroll to the location of that node, and the reason it works is because
                    // the ScrollView inner container is the same height as the RichText
                    _scrollView->scrollToItem(node, Vec2::ANCHOR_MIDDLE_TOP, Vec2::ANCHOR_MIDDLE_TOP);
                }
            }
            else if (!url.empty())
            {
                Application::getInstance()->openURL(url);
            }
        });

        const auto contentSize = _scrollView->getInnerContainerSize();
        _richText->setPosition(Vec2(contentSize.width / 2, contentSize.height));
        _richText->setLocalZOrder(10);

        _richText->formatText();
        _scrollView->addChild(_richText);

        updateScrollViewSize();

        // test remove all children, this call won't effect the test
        _richText->removeAllChildren();

        return true;
    }
    return false;
}

void UIRichTextScrollTo::updateScrollViewSize()
{
    auto newHeight  = 0.f;
    auto&& children = _scrollView->getChildren();
    for (auto&& child : children)
    {
        auto&& contentSize = child->getContentSize();
        newHeight += contentSize.height;
        if (const auto* widget = dynamic_cast<Widget*>(child))
        {
            if (const auto* layoutParam = widget->getLayoutParameter())
            {
                auto&& margin = layoutParam->getMargin();
                newHeight += margin.top + margin.bottom;
            }
        }
    }
    _scrollView->setInnerContainerSize(Size(_scrollView->getInnerContainerSize().width, newHeight));
    _scrollView->scrollToTop(0.f, false);
}
