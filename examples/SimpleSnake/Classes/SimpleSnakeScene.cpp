/*
 * Copyright (c) 2022 @aismann; Peter Eismann, Germany; dreifrankensoft
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "SimpleSnakeScene.h"
#include <time.h>

USING_NS_CC;

struct Snake
{
    int x, y;
} s[snakeBodies + StartBodies];

struct Fruit
{
    int x, y;
} f;

// Print useful error message instead of segfaulting when files are not there.

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf(
        "Depending on how you compiled you might have to add 'Resources/' in front of filenames in "
        "HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool SimpleSnake::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin      = Director::getInstance()->getVisibleOrigin();
    auto safeArea    = Director::getInstance()->getSafeAreaRect();
    auto safeOrigin  = safeArea.origin;

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png",
                                           CC_CALLBACK_1(SimpleSnake::menuCloseCallback, this));

    if (closeItem == nullptr || closeItem->getContentSize().width <= 0 || closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = safeOrigin.x + safeArea.size.width - closeItem->getContentSize().width / 2;
        float y = safeOrigin.y + closeItem->getContentSize().height / 2;
        closeItem->setPosition(Vec2(x, y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    auto listener          = EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(SimpleSnake::onKeyPressed, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    // add a label shows "Hello World"
    // create and initialize a label

    auto label = Label::createWithTTF("Example - Simple Snake", "fonts/Marker Felt.ttf", 24);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label->setPosition(
            Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - label->getContentSize().height));

        // add the label as a child to this layer
        this->addChild(label, 1);
    }

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("ADXE.png"sv);
    if (sprite == nullptr)
    {
        problemLoading("'ADXE.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        offset = Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);
        sprite->setPosition(offset);

        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
        auto drawNode = DrawNode::create();
        drawNode->setPosition(Vec2(0, 0));
        addChild(drawNode);

        drawNode->drawRect(safeArea.origin, safeArea.origin + safeArea.size, Color4F::BLUE);
    }

    srand(time(0));

    mydraw = DrawNode::create();
    addChild(mydraw, 10);

    f.x    = 10;
    f.y    = 10;
    s[0].x = 20;
    s[0].y = 20;

    char buffer[1024];

    for (int i = 0; i < snakeBodies; i++)
    {
        myScore[i] = 0.0;
        sprintf(buffer, "Level %i Time : %f", i + 1, myScore[i]);
        myScoreLabel[i] = Label::createWithTTF(std::string(buffer), "fonts/Marker Felt.ttf", 24);
        myScoreLabel[i]->setPosition(
            Vec2(130, origin.y + visibleSize.height - label->getContentSize().height - i * 30));
        this->addChild(myScoreLabel[i], 1);
    }

    scheduleUpdate();
    return true;
}

void SimpleSnake::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    switch (keyCode)
    {
    case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        if (dir != 2)
            dir = 1;
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        if (dir != 1)
            dir = 2;
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
        if (dir != 3)
            dir = 0;
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
        if (dir != 0)
            dir = 3;
        break;
    default:
        break;
    }
}

void SimpleSnake::menuCloseCallback(Ref* sender)
{
    Director::getInstance()->end();
}

void SimpleSnake::update(float delta)
{
    static float runTime = 0;
    static bool finish = false;
    runTime += delta;
    endLevelTime += delta;

    if (finish)
    {
        return;
    }

    if (runTime > level)
    {
        for (int i = num; i > 0; --i)
        {
            s[i] = s[i - 1];
        }
        switch (dir)
        {
        case 0:
            s[0].y++;
            if (s[0].y >= M)
                s[0].y = 0;
            break;
        case 1:
            s[0].x--;
            if (s[0].x < 0)
                s[0].x = N;
            break;
        case 2:
            s[0].x++;
            if (s[0].x >= N)
                s[0].x = 0;
            break;
        case 3:
            s[0].y--;
            if (s[0].y < 0)
                s[0].y = M;
            break;
        default:
            break;
        }

        if ((s[0].x == f.x) && (s[0].y == f.y))
        {
            bool posOk;
            do
            {
                posOk = true;
                f.x   = rand() % N;
                f.y   = rand() % M;
                for (int i = 0; i < num; i++)
                {
                    if (f.x == s[i].x && f.y == s[i].y)
                    {
                        posOk = false;
                        break;
                    }
                }
            } while (!posOk);

            // score
            char buffer[124];
            myScore[num - StartBodies] = endLevelTime;
            sprintf(buffer, "Level %i Time : %f", num - StartBodies + 1, myScore[num - StartBodies]);
            myScoreLabel[num - StartBodies]->setString(buffer);
            if (num >= (snakeBodies + StartBodies-1))
            {
                myScoreLabel[num - StartBodies]->setString("FINISH");
                finish = true;
            }

            // next level
            num++;
            level -= 0.01;
            startLevelTime = 0.0;
            endLevelTime   = startLevelTime;
        }
        runTime = 0;
    }

    //////// draw  ///////
    mydraw->clear();

    // Draw food
    mydraw->drawDot(Vec2(size / 2 + f.x * size, size / 2 + f.y * size), size / 2, Color4B::GREEN);

    // Draw snake
    for (int i = 0; i < num; i++)
    {
        mydraw->drawDot(Vec2(size / 2 + s[i].x * size, size / 2 + s[i].y * size), size / 2, Color4B::BLUE);
    }
}