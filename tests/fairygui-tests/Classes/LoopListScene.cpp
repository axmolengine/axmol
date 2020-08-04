#include "LoopListScene.h"

USING_NS_CC;

void LoopListScene::continueInit()
{
    UIPackage::addPackage("UI/LoopList");
    UIConfig::horizontalScrollBar = "";
    UIConfig::verticalScrollBar = "";

    _view = UIPackage::createObject("LoopList", "Main")->as<GComponent>();
    _groot->addChild(_view);

    _list = _view->getChild("list")->as<GList>();
    _list->itemRenderer = CC_CALLBACK_2(LoopListScene::renderListItem, this);
    _list->setVirtualAndLoop();
    _list->setNumItems(5);
    _list->addEventListener(UIEventType::Scroll, CC_CALLBACK_1(LoopListScene::doSpecialEffect, this));

    doSpecialEffect(nullptr);
}

void LoopListScene::renderListItem(int index, GObject* obj)
{
    obj->setPivot(0.5f, 0.5f);
    obj->setIcon("ui://LoopList/n" + Value(index + 1).asString());
}

void LoopListScene::doSpecialEffect(EventContext*)
{
    //change the scale according to the distance to middle
    float midX = _list->getScrollPane()->getPosX() + _list->getViewWidth() / 2;
    int cnt = _list->numChildren();
    for (int i = 0; i < cnt; i++)
    {
        GObject* obj = _list->getChildAt(i);
        float dist = std::abs(midX - obj->getX() - obj->getWidth() / 2);
        if (dist > obj->getWidth()) //no intersection
            obj->setScale(1, 1);
        else
        {
            float ss = 1 + (1 - dist / obj->getWidth()) * 0.24f;
            obj->setScale(ss, ss);
        }
    }

    _view->getChild("n3")->setText(Value((_list->getFirstChildInView() + 1) % _list->getNumItems()).asString());
}