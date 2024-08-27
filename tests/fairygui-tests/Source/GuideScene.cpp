#include "GuideScene.h"

using namespace ax;

GuideScene::GuideScene():_guideLayer(nullptr)
{

}

GuideScene::~GuideScene()
{
    AX_SAFE_RELEASE(_guideLayer);
}

void GuideScene::continueInit()
{
    UIPackage::addPackage("UI/Guide");

    _view = UIPackage::createObject("Guide", "Main")->as<GComponent>();
    _groot->addChild(_view);

    _guideLayer = UIPackage::createObject("Guide", "GuideLayer")->as<GComponent>();
    _guideLayer->makeFullScreen();
    _guideLayer->addRelation(_groot, RelationType::Size);
    _guideLayer->retain();

    GObject* bagBtn = _view->getChild("bagBtn");
    bagBtn->addClickListener([this](EventContext*)
    {
        _guideLayer->removeFromParent();
    });

    _view->getChild("n2")->addClickListener([this, bagBtn](EventContext*)
    {
        _groot->addChild(_guideLayer); //!!Before using TransformRect(or GlobalToLocal), the object must be added first
        Rect rect = bagBtn->transformRect(Rect(Vec2::ZERO, bagBtn->getSize()), _guideLayer);

        GObject* window = _guideLayer->getChild("window");
        window->setSize((int)rect.size.width, (int)rect.size.height);

        GTween::to(window->getPosition(), rect.origin, 0.5f)->setTarget(window, TweenPropType::Position);
    });
}