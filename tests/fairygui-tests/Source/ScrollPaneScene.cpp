#include "ScrollPaneScene.h"
#include "GList.h"

using namespace ax;

void ScrollPaneScene::continueInit()
{
    UIPackage::addPackage("UI/ScrollPane");
    UIConfig::horizontalScrollBar = "";
    UIConfig::verticalScrollBar = "";

    _view = UIPackage::createObject("ScrollPane", "Main")->as<GComponent>();
    _groot->addChild(_view);

    _list = _view->getChild("list")->as<GList>();
    _list->itemRenderer = AX_CALLBACK_2(ScrollPaneScene::renderListItem, this);
    _list->setVirtual();
    _list->setNumItems(1000);
    _list->addEventListener(UIEventType::TouchBegin, AX_CALLBACK_1(ScrollPaneScene::onClickList, this));
}

void ScrollPaneScene::renderListItem(int index, GObject* obj)
{
    GButton* item = obj->as<GButton>();
    item->setTitle("Item " + std::to_string(index));
    item->getScrollPane()->setPosX(0); //reset scroll pos

    //Be carefull, RenderListItem is calling repeatedly, add tag to avoid adding duplicately.
    item->getChild("b0")->addClickListener(AX_CALLBACK_1(ScrollPaneScene::onClickStick, this), EventTag(this));
    item->getChild("b1")->addClickListener(AX_CALLBACK_1(ScrollPaneScene::onClickDelete, this), EventTag(this));
}

void ScrollPaneScene::onClickStick(EventContext * context)
{
    _view->getChild("txt")->setText("Stick " + (((GObject*)context->getSender())->getParent())->getText());
}

void ScrollPaneScene::onClickDelete(EventContext * context)
{
    _view->getChild("txt")->setText("Delete " + (((GObject*)context->getSender())->getParent())->getText());
}

void ScrollPaneScene::onClickList(EventContext * context)
{
    //find out if there is an item in edit status
    int cnt = _list->numChildren();
    for (int i = 0; i < cnt; i++)
    {
        GButton* item = _list->getChildAt(i)->as<GButton>();
        if (item->getScrollPane()->getPosX() != 0)
        {
            //Check if clicked on the button
            if (item->getChild("b0")->as<GComponent>()->isAncestorOf(_groot->getTouchTarget())
                || item->getChild("b1")->as<GComponent>()->isAncestorOf(_groot->getTouchTarget()))
            {
                return;
            }
            item->getScrollPane()->setPosX(0, true);
            //avoid scroll pane default behavior��
            item->getScrollPane()->cancelDragging();
            _list->getScrollPane()->cancelDragging();
            break;
        }
    }
}
