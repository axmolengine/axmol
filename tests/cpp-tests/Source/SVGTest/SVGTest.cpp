/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md)

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

#include "SVGTest.h"
#include "SVG.h"

#include <cmath>
#include <algorithm>

using namespace ax;

enum
{
    kTagSpriteBatchNode = 1,
};

SVGTests::SVGTests()
{
    ADD_TEST_CASE(SVG);
    ADD_TEST_CASE(SVG1);
};

//------------------------------------------------------------------
//
// SVG
//
//------------------------------------------------------------------

SVG::SVG()
{
    auto listener           = PointerEventListener::create();
    listener->onPointerDown = AX_CALLBACK_1(SVG::onPointerDown, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    auto s = Director::getInstance()->getCanvasSize();
    addNewSpriteWithCoords(Vec2(s.width / 2, s.height / 2));
}

void SVG::addNewSpriteWithCoords(Vec2 p)
{
    ActionInterval* action;
    float random = AXRANDOM_0_1();

    if (random < 0.20f)
        action = ScaleBy::create(0.5, 2);
    else if (random < 0.40f)
        action = RotateBy::create(0.5, 360);
    else if (random < 0.60f)
        action = Blink::create(0.5, 3);
    else if (random < 0.8f)
        action = TintBy::create(0.5, 0, -255, -255);
    else
        action = FadeOut::create(2);
    auto action_back = action->reverse();
    auto seq         = Sequence::create(action, action_back, nullptr);

    ax::Sprite* sprite;
    if (AXRANDOM_MINUS1_1() > 0.5f)
    {
        sprite = ax::ext::createSVGSprite("SVGs/camera.svg");
        sprite->setScale(0.5f);
    }
    else
        sprite = ax::ext::createSVGSprite("SVGs/splash.svg");

    addChild(sprite);
    sprite->setPosition(Vec2(p.x, p.y));
    sprite->runAction(RepeatForever::create(seq));
}

bool SVG::onPointerDown(PointerEvent* event)
{
    Vec2 location = event->getWorldPoint();
    addNewSpriteWithCoords(location);
    return true;
}

std::string SVG::title() const
{
    return "Sprite (SVG)";
}

std::string SVG::subtitle() const
{
    return "Tap screen to add more sprites";
}

//------------------------------------------------------------------
//
// SVG1
//
//------------------------------------------------------------------

SVG1::SVG1()
{
    auto listener           = PointerEventListener::create();
    listener->onPointerDown = AX_CALLBACK_1(SVG1::onPointerDown, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    auto BatchNode = SpriteBatchNode::createWithTexture(ax::ext::loadSVGTexture("SVGs/camera.svg"));
    addChild(BatchNode, 0, kTagSpriteBatchNode);

    auto s = Director::getInstance()->getCanvasSize();
    addNewSpriteWithCoords(Vec2(s.width / 2, s.height / 2));
}

void SVG1::addNewSpriteWithCoords(Vec2 p)
{
    auto BatchNode = static_cast<SpriteBatchNode*>(getChildByTag(kTagSpriteBatchNode));

    float x     = MIN(AXRANDOM_0_1() * 100, 50);
    float y     = MIN(AXRANDOM_0_1() * 100, 50);
    auto sprite = Sprite::createWithTexture(BatchNode->getTexture(), Rect(x, y, 50, 50));
    BatchNode->addChild(sprite);

    sprite->setPosition(Vec2(p.x, p.y));

    ActionInterval* action;
    float random = AXRANDOM_0_1();

    if (random < 0.2f)
        action = ScaleBy::create(3, 2);
    if (random < 0.4f)
        action = RotateBy::create(3, 360);
    else if (random < 0.6f)
        action = Blink::create(1, 3);
    else if (random < 0.8f)
        action = TintBy::create(2, 0, -255, -255);
    else
        action = FadeOut::create(2);

    auto action_back = action->reverse();
    auto seq         = Sequence::create(action, action_back, nullptr);

    sprite->runAction(RepeatForever::create(seq));
}

bool SVG1::onPointerDown(PointerEvent* event)
{
    {
        auto location = event->getWorldPoint();

        addNewSpriteWithCoords(location);
    }
    return true;
}

std::string SVG1::title() const
{
    return "SpriteBatchNode (SVG)";
}

std::string SVG1::subtitle() const
{
    return "Tap screen to add more sprites";
}
