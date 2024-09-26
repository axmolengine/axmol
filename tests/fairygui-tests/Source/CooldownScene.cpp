#include "CooldownScene.h"

using namespace ax;

void CooldownScene::continueInit()
{
    UIPackage::addPackage("UI/Cooldown");

    _view = UIPackage::createObject("Cooldown", "Main")->as<GComponent>();
    _groot->addChild(_view);

    GProgressBar* btn0 = _view->getChild("b0")->as<GProgressBar>();
    GProgressBar* btn1 = _view->getChild("b1")->as<GProgressBar>();
    btn0->getChild("icon")->setIcon("icons/k0.png");
    btn1->getChild("icon")->setIcon("icons/k1.png");

    GTween::to(0, 100, 5)->setTarget(btn0, TweenPropType::Progress)->setRepeat(-1);
    GTween::to(10, 0, 10)->setTarget(btn1, TweenPropType::Progress)->setRepeat(-1);
}