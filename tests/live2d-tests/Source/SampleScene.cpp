/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "SampleScene.h"
#include "LAppView.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppDefine.hpp"
#include "LAppPal.hpp"

using namespace ax;

namespace
{
    SampleScene* instance = NULL;
}

SampleScene::SampleScene()
    : _pView(NULL)
    , _changeItem(NULL)
    , _closeItem(NULL)
{
}

Scene* SampleScene::createScene()
{
    return SampleScene::create();
}

SampleScene * SampleScene::getInstance()
{
    return instance;
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in SampleSceneScene.cpp\n");
}

// on "init" you need to initialize your instance
bool SampleScene::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    // Live2DManager実体化の前に必要となる
    instance = this;

    auto winSize = Director::getInstance()->getWinSize();
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    _closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        AX_CALLBACK_1(SampleScene::menuCloseCallback, this)
    );

    if (_closeItem == nullptr ||
        _closeItem->getContentSize().width <= 0 ||
        _closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }

    // create menu, it's an autorelease object
    auto closeMenu = Menu::create(_closeItem, NULL);
    closeMenu->setScale(0.9f);
    closeMenu->setPosition(Vec2::ZERO);
    this->addChild(closeMenu, 1);

    _changeItem = MenuItemImage::create(
        "icon_gear.png",
        "icon_gear.png",
        AX_CALLBACK_1(SampleScene::menuChangeCallback, this)
    );

    if (_changeItem == nullptr ||
        _changeItem->getContentSize().width <= 0 ||
        _changeItem->getContentSize().height <= 0)
    {
        problemLoading("'icon_gear.png'");
    }

    // create menu, it's an autorelease object
    auto changeMenu = Menu::create(_changeItem, NULL);
    changeMenu->setScale(0.9f);
    changeMenu->setPosition(Point::ZERO);
    this->addChild(changeMenu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add "SampleScene" splash screen"

    auto sprite = Sprite::create(LAppDefine::BackImageName);
    if (sprite == nullptr)
    {
        problemLoading(LAppDefine::BackImageName);
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        sprite->setScale(2);

        // add the sprite as a child to this layer
        this->addChild(sprite);
    }

    // add "Live2DModel"
    _pView = LAppView::createDrawNode();
    this->addChild(_pView);

    // Live2DManager実体化
    LAppLive2DManager::GetInstance();

    if (LAppDefine::DebugLogEnable)
    {
        LAppPal::PrintLog("==============================================");
        LAppPal::PrintLog("            Live2D SDK Sample App             ");
        LAppPal::PrintLog("==============================================");
    }

    if (LAppDefine::DebugDrawRectEnable)
    {
        auto* pDebugRects = DrawNode::create();
        this->addChild(pDebugRects);
        _pView->setDebugRectsNode(pDebugRects);
    }

    // update有効
    this->scheduleUpdate();

    return true;
}

void SampleScene::update(float delta)
{
    Node::update(delta);

    {
        auto winSize = Director::getInstance()->getWinSize();
        auto visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 origin = Director::getInstance()->getVisibleOrigin();
        // 端に隙間なく位置すると端末によっては描画されないことがあるのでマージンを設ける
        Vec2 margin = Vec2(20, 20);

        if (_changeItem)
        {
            float x = origin.x + visibleSize.width - _changeItem->getContentSize().width / 2 - margin.x;
            float y = getContentSize().height - _changeItem->getContentSize().height / 2 - margin.y;
            _changeItem->setPosition(Vec2(x,y));
        }

        if (_closeItem)
        {
            float x = origin.x + visibleSize.width - _closeItem->getContentSize().width / 2 - margin.x;
            float y = origin.y + _closeItem->getContentSize().height / 2 + margin.y;
            _closeItem->setPosition(Vec2(x,y));
        }
    }
}

void SampleScene::onExit()
{
    Node::onExit();

    // LAppModelがRenderingSpriteを使用していることがある。これはCocosのExit以降に開放することが出来ないので、
    // モデルはここですべて消えてもらう
    LAppLive2DManager::GetInstance()->ReleaseAllModel();
}

void SampleScene::menuCloseCallback(Object* pSender)
{
    LAppLive2DManager::ReleaseInstance();

    Director::getInstance()->end();

#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
    exit(0);
#endif
}

void SampleScene::menuChangeCallback(Object* pSender)
{
    LAppLive2DManager::GetInstance()->NextScene();
}
