/****************************************************************************

Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

https://axmol.dev/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "PopupMenu.h"
#include "GRoot.h"
#include "UIPackage.h"
#include "GList.h"
#include "GButton.h"
#include "UIConfig.h"

NS_FGUI_BEGIN
using namespace ax;

PopupMenu* PopupMenu::create(const std::string & resourceURL)
{
    PopupMenu *pRet = new PopupMenu();
    if (pRet->init(resourceURL))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

PopupMenu::PopupMenu() :
    _contentPane(nullptr),
    _list(nullptr)
{
}

PopupMenu::~PopupMenu()
{
    AX_SAFE_RELEASE(_contentPane);
}

bool PopupMenu::init(const std::string & resourceURL)
{
    std::string url = resourceURL;
    if (url.empty())
    {
        url = UIConfig::popupMenu;
        if (url.empty())
        {
            AXLOGW("FairyGUI: UIConfig.popupMenu not defined");
            return false;
        }
    }

    _contentPane = UIPackage::createObjectFromURL(url)->as<GComponent>();
    _contentPane->retain();
    _contentPane->addEventListener(UIEventType::Enter, AX_CALLBACK_1(PopupMenu::onEnter, this));

    _list = _contentPane->getChild("list")->as<GList>();
    _list->removeChildrenToPool();

    _list->addRelation(_contentPane, RelationType::Width);
    _list->removeRelation(_contentPane, RelationType::Height);
    _contentPane->addRelation(_list, RelationType::Height);

    _list->addEventListener(UIEventType::ClickItem, AX_CALLBACK_1(PopupMenu::onClickItem, this));

    return true;
}

GButton * PopupMenu::addItem(const std::string & caption, EventCallback callback)
{
    GButton* item = _list->addItemFromPool()->as<GButton>();
    item->setTitle(caption);
    item->setGrayed(false);
    GController* c = item->getController("checked");
    if (c != nullptr)
        c->setSelectedIndex(0);
    item->removeEventListener(UIEventType::ClickMenu);
    if (callback)
        item->addEventListener(UIEventType::ClickMenu, callback);

    return item;
}

GButton * PopupMenu::addItemAt(const std::string & caption, int index, EventCallback callback)
{
    GButton* item = _list->getFromPool(_list->getDefaultItem())->as<GButton>();
    _list->addChildAt(item, index);

    item->setTitle(caption);
    item->setGrayed(false);
    GController* c = item->getController("checked");
    if (c != nullptr)
        c->setSelectedIndex(0);
    item->removeEventListener(UIEventType::ClickMenu);
    if (callback)
        item->addEventListener(UIEventType::ClickMenu, callback);

    return item;
}

void PopupMenu::addSeperator()
{
    if (UIConfig::popupMenu_seperator.empty())
    {
        AXLOGW("FairyGUI: UIConfig.popupMenu_seperator not defined");
        return;
    }

    _list->addItemFromPool(UIConfig::popupMenu_seperator);
}

const std::string & PopupMenu::getItemName(int index) const
{
    GButton* item = _list->getChildAt(index)->as<GButton>();
    return item->name;
}

void PopupMenu::setItemText(const std::string & name, const std::string & caption)
{
    GButton* item = _list->getChild(name)->as<GButton>();
    item->setTitle(caption);
}

void PopupMenu::setItemVisible(const std::string & name, bool visible)
{
    GButton* item = _list->getChild(name)->as<GButton>();
    if (item->isVisible() != visible)
    {
        item->setVisible(visible);
        _list->setBoundsChangedFlag();
    }
}

void PopupMenu::setItemGrayed(const std::string & name, bool grayed)
{
    GButton* item = _list->getChild(name)->as<GButton>();
    item->setGrayed(grayed);
}

void PopupMenu::setItemCheckable(const std::string & name, bool checkable)
{
    GButton* item = _list->getChild(name)->as<GButton>();
    GController* c = item->getController("checked");
    if (c != nullptr)
    {
        if (checkable)
        {
            if (c->getSelectedIndex() == 0)
                c->setSelectedIndex(1);
        }
        else
            c->setSelectedIndex(0);
    }
}

void PopupMenu::setItemChecked(const std::string & name, bool check)
{
    GButton* item = _list->getChild(name)->as<GButton>();
    GController* c = item->getController("checked");
    if (c != nullptr)
        c->setSelectedIndex(check ? 2 : 1);
}

bool PopupMenu::isItemChecked(const std::string & name) const
{
    GButton* item = _list->getChild(name)->as<GButton>();
    GController* c = item->getController("checked");
    if (c != nullptr)
        return c->getSelectedIndex() == 2;
    else
        return false;
}

bool PopupMenu::removeItem(const std::string & name)
{
    GObject* item = _list->getChild(name);
    if (item != nullptr)
    {
        int index = _list->getChildIndex(item);
        _list->removeChildToPoolAt(index);
        item->removeEventListener(UIEventType::ClickMenu);

        return true;
    }
    else
        return false;
}

void PopupMenu::clearItems()
{
    int cnt = _list->numChildren();
    for (int i = 0; i < cnt; i++)
        _list->getChildAt(i)->removeEventListener(UIEventType::ClickMenu);
    _list->removeChildrenToPool();
}

int PopupMenu::getItemCount() const
{
    return _list->numChildren();
}

void PopupMenu::show(GObject * target, PopupDirection dir)
{
    GRoot* r = target != nullptr ? target->getRoot() : UIRoot;
    r->showPopup(_contentPane, dynamic_cast<GRoot*>(target) ? nullptr : target, dir);
}

void PopupMenu::onClickItem(EventContext * context)
{
    GButton* item = ((GObject*)context->getData())->as<GButton>();
    if (item == nullptr)
        return;

    if (item->isGrayed())
    {
        _list->setSelectedIndex(-1);
        return;
    }

    GController* c = item->getController("checked");
    if (c != nullptr && c->getSelectedIndex() != 0)
    {
        if (c->getSelectedIndex() == 1)
            c->setSelectedIndex(2);
        else
            c->setSelectedIndex(1);
    }

    GRoot* r = (GRoot*)_contentPane->getParent();
    r->hidePopup(_contentPane);

    item->dispatchEvent(UIEventType::ClickMenu, context->getData());
}

void PopupMenu::onEnter(EventContext * context)
{
    _list->setSelectedIndex(-1);
    _list->resizeToFit(INT_MAX, 10);
}

NS_FGUI_END
