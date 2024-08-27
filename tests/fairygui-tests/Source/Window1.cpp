#include "Window1.h"

using namespace ax;

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
        item->setTitle(std::to_string(i));
        item->setIcon("ui://Basics/r4");
    }
}