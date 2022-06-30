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

inline void setNodeIgnoreDesignScale(cocos2d::Node* node)
{
    CCASSERT(node, "node pointer cannot be null.");

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
    offset           = Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add "Background" splash screen"
    background = Sprite::create("Background.png");
    if (background == nullptr)
        problemLoading("'Background.png'");
    else
    {
        background->setPosition(offset);
        background->setScale(1.1);
        addChild(background, 0);
    }

    /////////////////////////////
    // 3. add your codes below...

    goBack();

    // add a label shows "Hello World"
    // create and initialize a label

    auto traps = Sprite::create("hole_effect.png"sv);

    return true;
}

void SimpleSnake::goBack()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin      = Director::getInstance()->getVisibleOrigin();
    // add "ADXE" splash screen"
    //  auto sprite = Sprite::create("ADXE_white.png"sv);
    auto sprite = Sprite::create("axe_logo.png"sv);
    setNodeIgnoreDesignScale(sprite);
    if (sprite == nullptr)
        problemLoading("'ADXE_white.png'");
    else
    {
        sprite->setPosition(offset - Vec2(0, visibleSize.height / 3));
        this->addChild(sprite, 9);
    }
    auto label0 = Label::createWithTTF("POWERED", "fonts/arial.ttf", 16);
    if (label0 == nullptr)
    {
        problemLoading("'fonts/arial.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label0->setPosition(offset - Vec2(0, visibleSize.height / 2.5));
        this->addChild(label0, 1);
    }

    auto label1 = Label::createWithTTF("Simple Snake", "fonts/arial.ttf", 50);
    label1->enableGlow(Color4B::BLUE);
    if (label1 == nullptr)
    {
        problemLoading("'fonts/arial.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label1->setPosition(offset + Vec2(0, visibleSize.height / 3));
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
        label2->setPosition(offset + Vec2(0, visibleSize.height / 4 + 20));
        // add the label as a child to this layer
        this->addChild(label2, 1);
    }

    // Events
    MenuItemFont::setFontName("fonts/Marker Felt.ttf");
    auto Start     = MenuItemFont::create("Start", CC_CALLBACK_1(SimpleSnake::startGame, this));
    auto Credits   = MenuItemFont::create("Credits", CC_CALLBACK_1(SimpleSnake::showCredits, this));
    auto Highscore = MenuItemFont::create("Highscore", CC_CALLBACK_1(SimpleSnake::showHighScore, this));
   // auto Back      = MenuItemFont::create("Highscore", CC_CALLBACK_1(SimpleSnake::goBack, this));

    auto menu = Menu::create(Start, Credits, Highscore, nullptr);
    menu->alignItemsVertically();
    this->addChild(menu, 1);

    auto listener          = EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(SimpleSnake::onKeyPressed, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void SimpleSnake::startGame(Ref* sender)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin      = Director::getInstance()->getVisibleOrigin();
    f.x              = 10;
    f.y              = 10;
    s[0].x           = 20;
    s[0].y           = 20;
    char buffer[1024];

    for (int i = 0; i < snakeBodies + 1; i++)
    {
        myScore[i] = 0.0;
        sprintf(buffer, "%i: %f", i + 1, myScore[i]);
        myScoreLabel[i] = Label::createWithTTF(std::string(buffer), "fonts/arial.ttf", 20);
        myScoreLabel[i]->setAnchorPoint(Vec2(0, 1));
        myScoreLabel[i]->setPosition(Vec2(10, origin.y + visibleSize.height - 10 - i * 20));
        this->addChild(myScoreLabel[i], 1);
    }
    myScoreLabel[snakeBodies]->setString("");  //"Eat as fast as you can!");

    srand(time(0));

    mydraw = DrawNode::create();
    addChild(mydraw, 10);

    scheduleUpdate();
}
void SimpleSnake::showCredits(Ref* sender)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin      = Director::getInstance()->getVisibleOrigin();

    auto c1 = Label::createWithTTF("Credits:  Cocos2D 4.0\nADXE\nFamTrinli (original source of a snake game)",
                                   "fonts/arial.ttf", 20);
    // position the label on the center of the screen
    c1->setPosition(offset + Vec2(0, visibleSize.height / 3));
    this->addChild(c1, 1);
}
void SimpleSnake::showHighScore(Ref* sender)
{

    auto scene = Scene::create();
    Director::getInstance()->pushScene(scene);

    // add "Background" splash screen"
    background = Sprite::create("Background.png");
    if (background == nullptr)
        problemLoading("'Background.png'");
    else
    {
        background->setPosition(offset);
        background->setScale(1.1);
        addChild(background, 0);
    }

    // Events
    MenuItemFont::setFontName("fonts/Marker Felt.ttf");
    // Bugs Item
    auto Start = MenuItemFont::create("Start", CC_CALLBACK_1(SimpleSnake::startGame, this));

    // Font Item
    auto Credits = MenuItemFont::create("Credits", CC_CALLBACK_1(SimpleSnake::showCredits, this));

    auto Highscore = MenuItemFont::create("Highscore", CC_CALLBACK_1(SimpleSnake::showHighScore, this));

    /*  auto color_action = TintBy::create(0.5f, 0, -255, -255);
      auto color_back   = color_action->reverse();
      auto seq          = Sequence::create(color_action, color_back, nullptr);
      item7->runAction(RepeatForever::create(seq));*/

    auto menu = Menu::create(Start, Credits, Highscore, nullptr);
    menu->alignItemsVertically();
    this->addChild(menu, 1);
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
            if (s[0].y >= M)
                s[0].y = 0;
            break;
        case 1:
            s[0].x--;
            if (s[0].x < 0)
                s[0].x = N - 1;
            break;
        case 2:
            s[0].x++;
            if (s[0].x >= N)
                s[0].x = 0;
            break;
        case 3:
            s[0].y--;
            if (s[0].y < 0)
                s[0].y = M - 1;
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
    // draw snake head
    mydraw->drawDot(Vec2(size / 2 + s[0].x * size, size / 2 + s[0].y * size), size / 2, Color4B::MAGENTA);

    if (!finish)
    {
        // draw food
        mydraw->drawDot(Vec2(size / 2 + f.x * size, size / 2 + f.y * size), size / 2, Color4B::GREEN);
    }
}