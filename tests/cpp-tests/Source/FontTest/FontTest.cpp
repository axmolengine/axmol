/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "FontTest.h"
#include "../testResource.h"
#include "axmol/2d/FontAtlasCache.h"
#include "axmol/2d/FontFreeType.h"

using namespace ax;

enum
{
    kTagLabel1,
    kTagLabel2,
    kTagLabel3,
    kTagLabel4,

    kTagColor1,
    kTagColor2,
    kTagColor3,
};

// you don't need any ifdef anymore
static std::string fontList[] = {
    "fonts/A Damn Mess.ttf", "fonts/Abberancy.ttf",   "fonts/Abduction.ttf",
    "fonts/Paint Boy.ttf",   "fonts/Schwarzwald.ttf", "fonts/Scissor Cuts.ttf",
};

static int vAlignIdx                      = 0;
static TextVAlignment verticalAlignment[] = {
    TextVAlignment::TOP,
    TextVAlignment::CENTER,
    TextVAlignment::BOTTOM,
};

FontTests::FontTests()
{
    for (auto&& fontFile : fontList)
    {
        addTestCase("FontTests", [&]() {
            vAlignIdx = 0;
            return FontTest::create(fontFile);
        });
    }

    for (auto&& fontFile : fontList)
    {
        addTestCase("FontTests", [&]() {
            vAlignIdx = 1;
            return FontTest::create(fontFile);
        });
    }

    for (auto&& fontFile : fontList)
    {
        addTestCase("FontTests", [&]() {
            vAlignIdx = 2;
            return FontTest::create(fontFile);
        });
    }
    ADD_TEST_CASE(FontNoReplacementTest);
}

void FontTest::showFont(std::string_view fontFile)
{
    auto s = Director::getInstance()->getCanvasSize();

    auto blockSize = Size(s.width / 3, 200);
    float fontSize = 26;

    removeChildByTag(kTagLabel1, true);
    removeChildByTag(kTagLabel2, true);
    removeChildByTag(kTagLabel3, true);
    removeChildByTag(kTagLabel4, true);
    removeChildByTag(kTagColor1, true);
    removeChildByTag(kTagColor2, true);
    removeChildByTag(kTagColor3, true);

    auto top    = Label::createWithTTF(fontFile, fontFile, 10);
    auto left   = Label::createWithTTF("alignment left", fontFile, fontSize, blockSize, TextHAlignment::LEFT,
                                       verticalAlignment[vAlignIdx]);
    auto center = Label::createWithTTF("alignment center", fontFile, fontSize, blockSize, TextHAlignment::CENTER,
                                       verticalAlignment[vAlignIdx]);
    auto right  = Label::createWithTTF("alignment right", fontFile, fontSize, blockSize, TextHAlignment::RIGHT,
                                       verticalAlignment[vAlignIdx]);

    auto leftColor   = LayerColor::create(Color32(100, 100, 100, 255), blockSize.width, blockSize.height);
    auto centerColor = LayerColor::create(Color32(200, 100, 100, 255), blockSize.width, blockSize.height);
    auto rightColor  = LayerColor::create(Color32(100, 100, 200, 255), blockSize.width, blockSize.height);

    leftColor->setIgnoreAnchorPointForPosition(false);
    centerColor->setIgnoreAnchorPointForPosition(false);
    rightColor->setIgnoreAnchorPointForPosition(false);

    top->setAnchorPoint(Vec2(0.5, 1));
    left->setAnchorPoint(Vec2(0, 0.5));
    leftColor->setAnchorPoint(Vec2(0, 0.5));
    center->setAnchorPoint(Vec2(0, 0.5));
    centerColor->setAnchorPoint(Vec2(0, 0.5));
    right->setAnchorPoint(Vec2(0, 0.5));
    rightColor->setAnchorPoint(Vec2(0, 0.5));

    top->setPosition(s.width / 2, s.height - 45);
    left->setPosition(0, s.height / 2);
    leftColor->setPosition(left->getPosition());
    center->setPosition(blockSize.width, s.height / 2);
    centerColor->setPosition(center->getPosition());
    right->setPosition(blockSize.width * 2, s.height / 2);
    rightColor->setPosition(right->getPosition());

    this->addChild(leftColor, -1, kTagColor1);
    this->addChild(left, 0, kTagLabel1);
    this->addChild(rightColor, -1, kTagColor2);
    this->addChild(right, 0, kTagLabel2);
    this->addChild(centerColor, -1, kTagColor3);
    this->addChild(center, 0, kTagLabel3);
    this->addChild(top, 0, kTagLabel4);
}

std::string FontTest::title() const
{
    return "Font test";
}

FontNoReplacementTest* FontNoReplacementTest::create()
{
    auto ret = new FontNoReplacementTest;
    if (ret->init())
    {
        ret->autorelease();
    }
    else
    {
        delete ret;
        ret = nullptr;
    }

    return ret;
}

FontNoReplacementTest::FontNoReplacementTest() {}

FontNoReplacementTest::~FontNoReplacementTest()
{
    // need to clear the caches since we change the lookup dictionary after the application init.
    FontAtlasCache::unloadFontAtlasTTF("fonts/A Damn Mess.ttf");
    FontFreeType::releaseFont("fonts/A Damn Mess.ttf");
    FontAtlasCache::unloadFontAtlasTTF("fonts/Abberancy.ttf");
    FontFreeType::releaseFont("fonts/Abberancy.ttf");
    FontAtlasCache::unloadFontAtlasTTF("fonts/Abduction.ttf");
    FontFreeType::releaseFont("fonts/Abduction.ttf");
    FontAtlasCache::unloadFontAtlasTTF("fonts/Schwarzwald.ttf");
    FontFreeType::releaseFont("fonts/Schwarzwald.ttf");
}

void FontNoReplacementTest::onEnter()
{
    TestCase::onEnter();

    std::string suffix;
    auto s = Director::getInstance()->getCanvasSize();

    auto blockSize = Size(s.width / 3, 200);
    float fontSize = 26;

    removeChildByTag(kTagLabel1, true);
    removeChildByTag(kTagLabel2, true);
    removeChildByTag(kTagLabel3, true);
    removeChildByTag(kTagLabel4, true);
    removeChildByTag(kTagColor1, true);
    removeChildByTag(kTagColor2, true);
    removeChildByTag(kTagColor3, true);

    auto top    = Label::createWithTTF("fonts/A Damn Mess.ttf" + suffix, "fonts/A Damn Mess.ttf", 10);
    auto left   = Label::createWithTTF("fonts/Abberancy.ttf" + suffix, "fonts/Abberancy.ttf", fontSize, blockSize,
                                       TextHAlignment::LEFT, verticalAlignment[0]);
    auto center = Label::createWithTTF("fonts/Abduction.ttf" + suffix, "fonts/Abduction.ttf", fontSize, blockSize,
                                       TextHAlignment::CENTER, verticalAlignment[0]);
    auto right  = Label::createWithTTF("fonts/Schwarzwald.ttf" + suffix, "fonts/Schwarzwald.ttf", fontSize, blockSize,
                                       TextHAlignment::RIGHT, verticalAlignment[0]);

    auto leftColor   = LayerColor::create(Color32(100, 100, 100, 255), blockSize.width, blockSize.height);
    auto centerColor = LayerColor::create(Color32(200, 100, 100, 255), blockSize.width, blockSize.height);
    auto rightColor  = LayerColor::create(Color32(100, 100, 200, 255), blockSize.width, blockSize.height);

    leftColor->setIgnoreAnchorPointForPosition(false);
    centerColor->setIgnoreAnchorPointForPosition(false);
    rightColor->setIgnoreAnchorPointForPosition(false);

    top->setAnchorPoint(Vec2(0.5, 1));
    left->setAnchorPoint(Vec2(0, 0.5));
    leftColor->setAnchorPoint(Vec2(0, 0.5));
    center->setAnchorPoint(Vec2(0, 0.5));
    centerColor->setAnchorPoint(Vec2(0, 0.5));
    right->setAnchorPoint(Vec2(0, 0.5));
    rightColor->setAnchorPoint(Vec2(0, 0.5));

    top->setPosition(s.width / 2, s.height - 45);
    left->setPosition(0, s.height / 2);
    leftColor->setPosition(left->getPosition());
    center->setPosition(blockSize.width, s.height / 2);
    centerColor->setPosition(center->getPosition());
    right->setPosition(blockSize.width * 2, s.height / 2);
    rightColor->setPosition(right->getPosition());

    this->addChild(leftColor, -1, kTagColor1);
    this->addChild(left, 0, kTagLabel1);
    this->addChild(rightColor, -1, kTagColor2);
    this->addChild(right, 0, kTagLabel2);
    this->addChild(centerColor, -1, kTagColor3);
    this->addChild(center, 0, kTagLabel3);
    this->addChild(top, 0, kTagLabel4);
}

std::string FontNoReplacementTest::title() const
{
    return "Font no replacement test";
}
