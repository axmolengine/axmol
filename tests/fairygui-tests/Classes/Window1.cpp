#include "Window1.h"

USING_NS_CC;

void Window1::onInit()
{
    setContentPane(UIPackage::createObject("Basics", "WindowA")->as<GComponent>());
    center();
}

void Window1::onShown()
{
    GList* list = _contentPane->getChild("n6")->as<GList>();
    list->removeChildrenToPool();

    for (int i = 0; i < 6; i++)
    {
        GButton* item = list->addItemFromPool()->as<GButton>();
        item->setTitle(Value(i).asString());
        item->setIcon("ui://Basics/r4");
    }
}