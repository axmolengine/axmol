#include "BagScene.h"

USING_NS_CC;

BagScene::BagScene() :_bagWindow(nullptr)
{
}

BagScene::~BagScene()
{
    CC_SAFE_RELEASE(_bagWindow);
}

void BagScene::continueInit()
{
    UIPackage::addPackage("UI/Bag");
    UIConfig::horizontalScrollBar = "";
    UIConfig::verticalScrollBar = "";

    _view = UIPackage::createObject("Bag", "Main")->as<GComponent>();
    _groot->addChild(_view);

    _bagWindow = BagWindow::create();
    _bagWindow->retain();
    _view->getChild("bagBtn")->addClickListener([this](EventContext*) { _bagWindow->show(); });
}