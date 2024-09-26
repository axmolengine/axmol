#include "BagWindow.h"

using namespace ax;

void BagWindow::onInit()
{
    setContentPane(UIPackage::createObject("Bag", "BagWin")->as<GComponent>());
    center();
    setModal(true);

    _list = _contentPane->getChild("list")->as<GList>();
    _list->addEventListener(UIEventType::ClickItem, AX_CALLBACK_1(BagWindow::onClickItem, this));
    _list->itemRenderer = AX_CALLBACK_2(BagWindow::renderListItem, this);
    _list->setNumItems(45);
}

void BagWindow::renderListItem(int index, GObject* obj)
{
    obj->setIcon("icons/i" + std::to_string((int)(rand_0_1() * 10)) + ".png");
    obj->setText(std::to_string((int)(rand_0_1() * 100)));
}

void BagWindow::doShowAnimation()
{
    setScale(0.1f, 0.1f);
    setPivot(0.5f, 0.5f);

    GTween::to(getScale(), Vec2::ONE, 0.3f)->setTarget(this, TweenPropType::Scale)->onComplete(AX_CALLBACK_0(BagWindow::onShown, this));
}

void BagWindow::doHideAnimation()
{
    GTween::to(getScale(), Vec2(0.1f, 0.1f), 0.3f)->setTarget(this, TweenPropType::Scale)->onComplete(AX_CALLBACK_0(BagWindow::hideImmediately, this));
}

void BagWindow::onClickItem(EventContext* context)
{
    GObject* item = (GObject*)context->getData();
    _contentPane->getChild("n11")->setIcon(item->getIcon());
    _contentPane->getChild("n13")->setText(item->getText());
}