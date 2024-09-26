#include "MenuScene.h"
#include "BagScene.h"
#include "BasicsScene.h"
#include "ChatScene.h"
#include "GuideScene.h"
#include "HitTestScene.h"
#include "JoystickScene.h"
#include "ListEffectScene.h"
#include "LoopListScene.h"
#include "ModalWaitingScene.h"
#include "PullToRefreshScene.h"
#include "ScrollPaneScene.h"
#include "TransitionDemoScene.h"
#include "TreeViewScene.h"
#include "VirtualListScene.h"
#include "CooldownScene.h"

using namespace ax;

void MenuScene::continueInit()
{
    UIPackage::addPackage("UI/MainMenu");
    _view = UIPackage::createObject("MainMenu", "Main")->as<GComponent>();
    _groot->addChild(_view);

    _view->getChild("n1")->addClickListener([this](EventContext*) {
        TransitionFade* scene = TransitionFade::create(0.5f, BasicsScene::create());
        Director::getInstance()->replaceScene(scene);
    });
    _view->getChild("n2")->addClickListener([this](EventContext*) {
        Director::getInstance()->replaceScene(TransitionDemoScene::create());
    });
    _view->getChild("n4")->addClickListener([this](EventContext*) {
        Director::getInstance()->replaceScene(VirtualListScene::create());
    });
    _view->getChild("n5")->addClickListener([this](EventContext*) {
        Director::getInstance()->replaceScene(LoopListScene::create());
    });
    _view->getChild("n6")->addClickListener([this](EventContext*) {
        Director::getInstance()->replaceScene(HitTestScene::create());
    });
    _view->getChild("n7")->addClickListener([this](EventContext*) {
        Director::getInstance()->replaceScene(PullToRefreshScene::create());
    });
    _view->getChild("n8")->addClickListener([this](EventContext*) {
        Director::getInstance()->replaceScene(ModalWaitingScene::create());
    });
    _view->getChild("n9")->addClickListener([this](EventContext*) {
        Director::getInstance()->replaceScene(JoystickScene::create());
    });
    _view->getChild("n10")->addClickListener([this](EventContext*) {
        Director::getInstance()->replaceScene(BagScene::create());
    });
    _view->getChild("n11")->addClickListener([this](EventContext*) {
        Director::getInstance()->replaceScene(ChatScene::create());
    });
    _view->getChild("n12")->addClickListener([this](EventContext*) {
        Director::getInstance()->replaceScene(ListEffectScene::create());
    });
    _view->getChild("n13")->addClickListener([this](EventContext*) {
        Director::getInstance()->replaceScene(ScrollPaneScene::create());
    });
    _view->getChild("n14")->addClickListener([this](EventContext*) {
        Director::getInstance()->replaceScene(TreeViewScene::create());
    });
    _view->getChild("n15")->addClickListener([this](EventContext*) {
        Director::getInstance()->replaceScene(GuideScene::create());
    });
    _view->getChild("n16")->addClickListener([this](EventContext*) {
        Director::getInstance()->replaceScene(CooldownScene::create());
    });
}