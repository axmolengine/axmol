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
#include <string>

USING_NS_AX;

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

inline void setNodeIgnoreDesignScale(axis::Node* node)
{
    AXASSERT(node, "node pointer cannot be null.");

    Size actualFrameSize = Director::getInstance()->getOpenGLView()->getFrameSize();
    Size actualWinSize   = Director::getInstance()->getWinSizeInPixels();
    node->setScaleX(actualWinSize.width / actualFrameSize.width);
    node->setScaleY(actualWinSize.height / actualFrameSize.height);
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
    offset                      = Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png",
                                           AX_CALLBACK_1(SimpleSnake::menuCloseCallback, this));

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
    listener->onKeyPressed = AX_CALLBACK_2(SimpleSnake::onKeyPressed, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    // add a label shows "Hello World"
    // create and initialize a label

    auto label1 = Label::createWithTTF("Simple Snake", "fonts/arial.ttf", 24);
    if (label1 == nullptr)
    {
        problemLoading("'fonts/arial.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label1->setPosition(Vec2(origin.x + visibleSize.width / 2,
                                origin.y + visibleSize.height - label1->getContentSize().height*2 - visibleSize.height/2));
        // add the label as a child to this layer
        this->addChild(label1, 1);
    }
    auto label2 = Label::createWithTTF("Eat as fast as you can!", "fonts/arial.ttf", 24);
    if (label2 == nullptr)
    {
        problemLoading("'fonts/arial.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label2->setPosition(
            Vec2(origin.x + visibleSize.width / 2,
                 origin.y + visibleSize.height - label2->getContentSize().height * 3 - visibleSize.height/2));

        // add the label as a child to this layer
        this->addChild(label2, 1);
    }

    // add "AXIS" splash screen"
    auto sprite = Sprite::create("AXIS_white.png"sv);
    setNodeIgnoreDesignScale(sprite);
    if (sprite == nullptr)
    {
        problemLoading("'AXIS_white.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(offset);

        // add the sprite as a child to this layer
        this->addChild(sprite, 9);
        auto drawNode = DrawNode::create();
        drawNode->setPosition(Vec2(0, 0));
        addChild(drawNode, 20);

        drawNode->drawRect(safeArea.origin, safeArea.origin + safeArea.size, Color4F::BLUE);
    }

    srand(time(0));

    mydraw = DrawNode::create();
    addChild(mydraw, 10);

    background = Sprite::create("Background.png");
    background->setPosition(offset);
    background->setScale(1.1);
    addChild(background, 0);

    f.x    = 10;
    f.y    = 10;
    s[0].x = 20;
    s[0].y = 20;

    char buffer[1024];

    for (int i = 0; i < snakeBodies+1; i++)
    {
        myScore[i] = 0.0;
        sprintf(buffer, "%i: %f", i + 1, myScore[i]);
        myScoreLabel[i] = Label::createWithTTF(std::string(buffer), "fonts/arial.ttf", 20);
        myScoreLabel[i]->setAnchorPoint(Vec2(0, 1));
        myScoreLabel[i]->setPosition(Vec2(10, origin.y + visibleSize.height - 10 - i * 20));
        this->addChild(myScoreLabel[i], 1);
    }
    myScoreLabel[snakeBodies]->setString("");//"Eat as fast as you can!");

    scheduleUpdate();
    return true;
}

void SimpleSnake::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    switch (keyCode)
    {
    case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        if (dir != 2)
            dir = 1;
        break;
    case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        if (dir != 1)
            dir = 2;
        break;
    case EventKeyboard::KeyCode::KEY_UP_ARROW:
        if (dir != 3)
            dir = 0;
        break;
    case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
        if (dir != 0)
            dir = 3;
        break;
    case EventKeyboard::KeyCode::KEY_1:
        Director::getInstance()->setStatsAnchor(AnchorPreset::BOTTOM_LEFT);
        break;
    case EventKeyboard::KeyCode::KEY_4:
        Director::getInstance()->setStatsAnchor(AnchorPreset::CENTER_LEFT);
        break;
    case EventKeyboard::KeyCode::KEY_7:
        Director::getInstance()->setStatsAnchor(AnchorPreset::TOP_LEFT);
        break;
    case EventKeyboard::KeyCode::KEY_8:
        Director::getInstance()->setStatsAnchor(AnchorPreset::TOP_CENTER);
        break;
    case EventKeyboard::KeyCode::KEY_9:
        Director::getInstance()->setStatsAnchor(AnchorPreset::TOP_RIGHT);
        break;
    case EventKeyboard::KeyCode::KEY_6:
        Director::getInstance()->setStatsAnchor(AnchorPreset::CENTER_RIGHT);
        break;
    case EventKeyboard::KeyCode::KEY_3:
        Director::getInstance()->setStatsAnchor(AnchorPreset::BOTTOM_RIGHT);
        break;
    case EventKeyboard::KeyCode::KEY_2:
        Director::getInstance()->setStatsAnchor(AnchorPreset::BOTTOM_CENTER);
        break;
    case EventKeyboard::KeyCode::KEY_5:
        Director::getInstance()->setStatsAnchor(AnchorPreset::CENTER);
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
    static bool finish   = false;
    runTime += delta;
    endLevelTime += delta;



    if (finish)
    {
        float myScale = runTime + 1.1;
        if (myScale > 3.0)
            myScale = 3.0;
        background->setScale(myScale);
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
            if (s[0].y >= M) s[0].y = 0;
            break;
        case 1:
            s[0].x--;
            if (s[0].x < 0) s[0].x = N - 1;
            break;
        case 2:
            s[0].x++;
            if (s[0].x >= N) s[0].x = 0;
            break;
        case 3:
            s[0].y--;
            if (s[0].y < 0) s[0].y = M - 1;
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

            // next level
            num++;
            if (num >= (snakeBodies + StartBodies - 1))
            {
                myScoreLabel[snakeBodies]->setString("!!! FINISH !!!");
                finish = true;
                drawAll(finish);
            }
            level -= 0.01;
            startLevelTime = 0.0;
            endLevelTime   = startLevelTime;
        }
        runTime = 0;
    }

    if (!finish)
    {
        drawAll(finish);
    }
}

void SimpleSnake::drawAll(bool finish)
{
    // score
    char buffer[124];
    myScore[num - StartBodies] = endLevelTime;
    sprintf(buffer, "%i: %f", num - StartBodies + 1, myScore[num - StartBodies]);
    myScoreLabel[num - StartBodies]->setString(buffer);


    mydraw->clear();
    // draw snake body
    for (int i = 1; i < num; i++)
    {
        mydraw->drawDot(Vec2(size / 2 + s[i].x * size, size / 2 + s[i].y * size), size / 2, Color4B::BLUE);
    }
    //draw snake head
    mydraw->drawDot(Vec2(size / 2 + s[0].x * size, size / 2 + s[0].y * size), size / 2, Color4B::MAGENTA);

    if (!finish)
    {
        // draw food
        mydraw->drawDot(Vec2(size / 2 + f.x * size, size / 2 + f.y * size), size / 2, Color4B::GREEN);
    }
}