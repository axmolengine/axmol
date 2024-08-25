#include "ListEffectScene.h"
#include "MailItem.h"
#include "GList.h"

using namespace ax;

void ListEffectScene::continueInit()
{
    UIPackage::addPackage("UI/Extension");
    UIConfig::horizontalScrollBar = "";
    UIConfig::verticalScrollBar = "";
    UIObjectFactory::setPackageItemExtension("ui://Extension/mailItem", []() {return MailItem::create(); });

    _view = UIPackage::createObject("Extension", "Main")->as<GComponent>();
    _groot->addChild(_view);

    _list = _view->getChild("mailList")->as<GList>();

    for (int i = 0; i < 10; i++)
    {
        MailItem* item = _list->addItemFromPool()->as<MailItem>();
        item->setFetched(i % 3 == 0);
        item->setRead(i % 2 == 0);
        item->setTime("5 Nov 2015 16:24:33");
        item->setTitle("Mail title here");
    }

    _list->ensureBoundsCorrect();
    float delay = 1.0f;
    for (int i = 0; i < 10; i++)
    {
        MailItem* item = _list->getChildAt(i)->as<MailItem>();
        if (_list->isChildInView(item))
        {
            item->playEffect(delay);
            delay += 0.2f;
        }
        else
            break;
    }
}

