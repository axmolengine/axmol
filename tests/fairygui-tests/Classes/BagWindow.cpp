#include "BagWindow.h"

USING_NS_CC;

void BagWindow::onInit()
{
    setContentPane(UIPackage::createObject("Bag", "BagWin")->as<GComponent>());
    center();
    setModal(true);

    _list = _contentPane->getChild("list")->as<GList>();
    _list->addEventListener(UIEventType::ClickItem, CC_CALLBACK_1(BagWindow::onClickItem, this));
    _list->itemRenderer = CC_CALLBACK_2(BagWindow::renderListItem, this);
    _list->setNumItems(45);
}

void BagWindow::renderListItem(int index, GObject* obj)
{
    obj->setIcon("icons/i" + Value((int)(rand_0_1() * 10)).asString() + ".png");
    obj->setText(Value((int)(rand_0_1() * 100)).asString());
}

void BagWindow::doShowAnimation()
{
    setScale(0.1f, 0.1f);
    setPivot(0.5f, 0.5f);

    GTween::to(getScale(), Vec2::ONE, 0.3f)->setTarget(this, TweenPropType::Scale)->onComplete(CC_CALLBACK_0(BagWindow::onShown, this));
}

void BagWindow::doHideAnimation()
{
    GTween::to(getScale(), Vec2(0.1f, 0.1f), 0.3f)->setTarget(this, TweenPropType::Scale)->onComplete(CC_CALLBACK_0(BagWindow::hideImmediately, this));
}

void BagWindow::onClickItem(EventContext* context)
{
    GObject* item = (GObject*)context->getData();
    _contentPane->getChild("n11")->setIcon(item->getIcon());
    _contentPane->getChild("n13")->setText(item->getText());
}