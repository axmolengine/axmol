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

//
// Bug-350
// http://code.google.com/p/cocos2d-iphone/issues/detail?id=350
//

#include "Bug-350.h"

using namespace ax;

bool Bug350Layer::init()
{
    if (BugsTestBase::init())
    {
        // auto size       = Director::getInstance()->getWinSize();
        // auto background = Sprite::create("Hello.png");
        // background->setPosition(size.width / 2, size.height / 2);
        // addChild(background);

        auto visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 origin      = Director::getInstance()->getVisibleOrigin();

        //    https:  // rezghob.com/different-sprite-types/

        auto spriteCache = SpriteFrameCache::getInstance();
        //      spriteCache->addSpriteFramesWithFile("sprites_quad.plist");
        spriteCache->addSpriteFramesWithFile("sprites_poly.plist");

        for (int i = 0; i < 10000; i++)
        {
            auto sprite1 = Sprite::createWithSpriteFrameName("PolySprite1.png");
    //       sprite1->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
            sprite1->setPosition(50*AXRANDOM_MINUS1_1() + origin.x + visibleSize.width / 2,
                                 50*AXRANDOM_MINUS1_1() + origin.y + visibleSize.height / 2);
            sprite1->setScale(5);
            sprite1->setColor(Color3B::RED);
            this->addChild(sprite1);

            auto sprite2 = Sprite::createWithSpriteFrameName("PolySprite2.png");
            sprite2->setPosition(origin.x + visibleSize.width / 4, origin.y + visibleSize.height / 2);
            sprite2->setScale(5);
            sprite2->setTexture(sprite1->getTexture());
            //    sprite2->setColor(Color3B::BLUE);
            this->addChild(sprite2);

            auto sprite3 = Sprite::createWithSpriteFrameName("PolySprite3.png");
            sprite3->setPosition(origin.x + visibleSize.width / 1.3, origin.y + visibleSize.height / 2);
            sprite3->setScale(5);
                sprite3->setColor(Color3B::BLUE);
            this->addChild(sprite3);
        }

        return true;
    }

    return false;
}
