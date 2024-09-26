#include "VirtualListScene.h"
#include "MailItem.h"

using namespace ax;

void VirtualListScene::continueInit()
{
    UIPackage::addPackage("UI/VirtualList");
    UIConfig::horizontalScrollBar = "";
    UIConfig::verticalScrollBar = "";
    UIObjectFactory::setPackageItemExtension("ui://VirtualList/mailItem", []() {return MailItem::create(); });

    _view = UIPackage::createObject("VirtualList", "Main")->as<GComponent>();
    _groot->addChild(_view);

    _view->getChild("n6")->addClickListener([this](EventContext*) { _list->addSelection(500, true); });
    _view->getChild("n7")->addClickListener([this](EventContext*) { _list->getScrollPane()->scrollTop(); });
    _view->getChild("n8")->addClickListener([this](EventContext*) { _list->getScrollPane()->scrollBottom(); });

    _list = _view->getChild("mailList")->as<GList>();
    _list->itemRenderer = AX_CALLBACK_2(VirtualListScene::renderListItem, this);
    _list->setVirtual();
    _list->setNumItems(1000);
}

void VirtualListScene::renderListItem(int index, GObject* obj)
{
    MailItem* item = obj->as<MailItem>();
    item->setFetched(index % 3 == 0);
    item->setRead(index % 2 == 0);
    item->setTime("5 Nov 2015 16:24:33");
    item->setText(std::to_string(index) + " Mail title here");
}
