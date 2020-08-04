#include "HitTestScene.h"

USING_NS_CC;

void HitTestScene::continueInit()
{
    UIPackage::addPackage("UI/HitTest");

    _view = UIPackage::createObject("HitTest", "Main")->as<GComponent>();
    _groot->addChild(_view);
}