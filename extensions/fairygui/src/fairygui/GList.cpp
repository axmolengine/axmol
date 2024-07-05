#include "GList.h"
#include "GButton.h"
#include "GScrollBar.h"
#include "UIConfig.h"
#include "UIPackage.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
USING_NS_AX;

using namespace std;

GList::ItemInfo::ItemInfo()
{
    obj = nullptr;
    updateFlag = 0;
    selected = false;
}

GList::GList() : foldInvisibleItems(false),
                 _selectionMode(ListSelectionMode::SINGLE),
                 scrollItemToViewOnClick(true),
                 _layout(ListLayoutType::SINGLE_COLUMN),
                 _lineCount(0),
                 _columnCount(0),
                 _lineGap(0),
                 _columnGap(0),
                 _align(TextHAlignment::LEFT),
                 _verticalAlign(TextVAlignment::TOP),
                 _autoResizeItem(true),
                 _selectionController(nullptr),
                 _pool(nullptr),
                 _selectionHandled(false),
                 _lastSelectedIndex(-1),
                 _virtual(false),
                 _loop(0),
                 _numItems(0),
                 _realNumItems(0),
                 _firstIndex(-1),
                 _virtualListChanged(false),
                 _eventLocked(false),
                 _itemInfoVer(0)
{
    _trackBounds = true;
    setOpaque(true);
    _pool = new GObjectPool();
}

GList::~GList()
{
    delete _pool;
    if (_virtualListChanged != 0)
        CALL_LATER_CANCEL(GList, doRefreshVirtualList);

    _selectionController = nullptr;
    scrollItemToViewOnClick = false;
}

void GList::setDefaultItem(const std::string& value)
{ 
    _defaultItem = UIPackage::normalizeURL(value);
}

void GList::setLayout(ListLayoutType value)
{
    if (_layout != value)
    {
        _layout = value;
        setBoundsChangedFlag();
        if (_virtual)
            setVirtualListChangedFlag(true);
    }
}

void GList::setLineCount(int value)
{
    if (_lineCount != value)
    {
        _lineCount = value;
        if (_layout == ListLayoutType::FLOW_VERTICAL || _layout == ListLayoutType::PAGINATION)
        {
            setBoundsChangedFlag();
            if (_virtual)
                setVirtualListChangedFlag(true);
        }
    }
}

void GList::setColumnCount(int value)
{
    if (_columnCount != value)
    {
        _columnCount = value;
        if (_layout == ListLayoutType::FLOW_HORIZONTAL || _layout == ListLayoutType::PAGINATION)
        {
            setBoundsChangedFlag();
            if (_virtual)
                setVirtualListChangedFlag(true);
        }
    }
}

void GList::setLineGap(int value)
{
    if (_lineGap != value)
    {
        _lineGap = value;
        setBoundsChangedFlag();
        if (_virtual)
            setVirtualListChangedFlag(true);
    }
}

void GList::setColumnGap(int value)
{
    if (_columnGap != value)
    {
        _columnGap = value;
        setBoundsChangedFlag();
        if (_virtual)
            setVirtualListChangedFlag(true);
    }
}

void GList::setAlign(ax::TextHAlignment value)
{
    if (_align != value)
    {
        _align = value;
        setBoundsChangedFlag();
        if (_virtual)
            setVirtualListChangedFlag(true);
    }
}

void GList::setVerticalAlign(ax::TextVAlignment value)
{
    if (_verticalAlign != value)
    {
        _verticalAlign = value;
        setBoundsChangedFlag();
        if (_virtual)
            setVirtualListChangedFlag(true);
    }
}

void GList::setAutoResizeItem(bool value)
{
    if (_autoResizeItem != value)
    {
        _autoResizeItem = value;
        setBoundsChangedFlag();
        if (_virtual)
            setVirtualListChangedFlag(true);
    }
}

GObject* GList::getFromPool(const std::string& url)
{
    GObject* ret;
    if (url.length() == 0)
        ret = _pool->getObject(_defaultItem);
    else
        ret = _pool->getObject(url);
    if (ret != nullptr)
        ret->setVisible(true);
    return ret;
}

void GList::returnToPool(GObject* obj)
{
    _pool->returnObject(obj);
}

GObject* GList::addItemFromPool(const std::string& url)
{
    GObject* obj = getFromPool(url);

    return addChild(obj);
}

GObject* GList::addChildAt(GObject* child, int index)
{
    GComponent::addChildAt(child, index);
    if (dynamic_cast<GButton*>(child))
    {
        GButton* button = (GButton*)child;
        button->setSelected(false);
        button->setChangeStateOnClick(false);
    }

    child->addEventListener(UIEventType::TouchBegin, AX_CALLBACK_1(GList::onItemTouchBegin, this), EventTag(this));
    child->addClickListener(AX_CALLBACK_1(GList::onClickItem, this), EventTag(this));
    child->addEventListener(UIEventType::RightClick, AX_CALLBACK_1(GList::onClickItem, this), EventTag(this));

    return child;
}

void GList::removeChildAt(int index)
{
    GObject* child = _children.at(index);
    child->removeClickListener(EventTag(this));
    child->removeEventListener(UIEventType::TouchBegin, EventTag(this));
    child->removeEventListener(UIEventType::RightClick, EventTag(this));

    GComponent::removeChildAt(index);
}

void GList::removeChildToPoolAt(int index)
{
    returnToPool(getChildAt(index));
    removeChildAt(index);
}

void GList::removeChildToPool(GObject* child)
{
    returnToPool(child);
    removeChild(child);
}

void GList::removeChildrenToPool()
{
    removeChildrenToPool(0, -1);
}

void GList::removeChildrenToPool(int beginIndex, int endIndex)
{
    if (endIndex < 0 || endIndex >= _children.size())
        endIndex = (int)_children.size() - 1;

    for (int i = beginIndex; i <= endIndex; ++i)
        removeChildToPoolAt(beginIndex);
}

int GList::getSelectedIndex() const
{
    if (_virtual)
    {
        int cnt = _realNumItems;
        for (int i = 0; i < cnt; i++)
        {
            const ItemInfo& ii = _virtualItems[i];
            if ((dynamic_cast<GButton*>(ii.obj) != nullptr && ((GButton*)ii.obj)->isSelected()) || (ii.obj == nullptr && ii.selected))
            {
                if (_loop)
                    return i % _numItems;
                else
                    return i;
            }
        }
    }
    else
    {
        int cnt = (int)_children.size();
        for (int i = 0; i < cnt; i++)
        {
            GButton* obj = _children.at(i)->as<GButton>();
            if (obj != nullptr && obj->isSelected())
                return i;
        }
    }
    return -1;
}

void GList::setSelectedIndex(int value)
{
    if (value >= 0 && value < getNumItems())
    {
        if (_selectionMode != ListSelectionMode::SINGLE)
            clearSelection();
        addSelection(value, false);
    }
    else
        clearSelection();
}

void GList::setSelectionController(GController* value)
{
    _selectionController = value;
}

void GList::getSelection(std::vector<int>& result) const
{
    result.clear();
    if (_virtual)
    {
        int cnt = _realNumItems;
        for (int i = 0; i < cnt; i++)
        {
            const ItemInfo& ii = _virtualItems[i];
            if ((dynamic_cast<GButton*>(ii.obj) != nullptr && ((GButton*)ii.obj)->isSelected()) || (ii.obj == nullptr && ii.selected))
            {
                int j = i;
                if (_loop)
                {
                    j = i % _numItems;
                    if (std::find(result.cbegin(), result.cend(), j) != result.cend())
                        continue;
                }
                result.push_back(j);
            }
        }
    }
    else
    {
        int cnt = (int)_children.size();
        for (int i = 0; i < cnt; i++)
        {
            GButton* obj = _children.at(i)->as<GButton>();
            if (obj != nullptr && obj->isSelected())
                result.push_back(i);
        }
    }
}

void GList::addSelection(int index, bool scrollItToView)
{
    if (_selectionMode == ListSelectionMode::NONE)
        return;

    checkVirtualList();

    if (_selectionMode == ListSelectionMode::SINGLE)
        clearSelection();

    if (scrollItToView)
        scrollToView(index);

    _lastSelectedIndex = index;
    GButton* obj = nullptr;
    if (_virtual)
    {
        ItemInfo& ii = _virtualItems[index];
        if (ii.obj != nullptr)
            obj = ii.obj->as<GButton>();
        ii.selected = true;
    }
    else
        obj = getChildAt(index)->as<GButton>();

    if (obj != nullptr && !obj->isSelected())
    {
        obj->setSelected(true);
        updateSelectionController(index);
    }
}

void GList::removeSelection(int index)
{
    if (_selectionMode == ListSelectionMode::NONE)
        return;

    GButton* obj = nullptr;
    if (_virtual)
    {
        ItemInfo& ii = _virtualItems[index];
        if (ii.obj != nullptr)
            obj = ii.obj->as<GButton>();
        ii.selected = false;
    }
    else
        obj = getChildAt(index)->as<GButton>();

    if (obj != nullptr)
        obj->setSelected(false);
}

void GList::clearSelection()
{
    if (_virtual)
    {
        int cnt = _realNumItems;
        for (int i = 0; i < cnt; i++)
        {
            ItemInfo& ii = _virtualItems[i];
            if (dynamic_cast<GButton*>(ii.obj))
                ((GButton*)ii.obj)->setSelected(false);
            ii.selected = false;
        }
    }
    else
    {
        int cnt = (int)_children.size();
        for (int i = 0; i < cnt; i++)
        {
            GButton* obj = _children.at(i)->as<GButton>();
            if (obj != nullptr)
                obj->setSelected(false);
        }
    }
}

void GList::clearSelectionExcept(GObject* g)
{
    if (_virtual)
    {
        int cnt = _realNumItems;
        for (int i = 0; i < cnt; i++)
        {
            ItemInfo& ii = _virtualItems[i];
            if (ii.obj != g)
            {
                if (dynamic_cast<GButton*>(ii.obj))
                    ((GButton*)ii.obj)->setSelected(false);
                ii.selected = false;
            }
        }
    }
    else
    {
        int cnt = (int)_children.size();
        for (int i = 0; i < cnt; i++)
        {
            GButton* obj = _children.at(i)->as<GButton>();
            if (obj != nullptr && obj != g)
                obj->setSelected(false);
        }
    }
}

void GList::selectAll()
{
    checkVirtualList();

    int last = -1;
    if (_virtual)
    {
        int cnt = _realNumItems;
        for (int i = 0; i < cnt; i++)
        {
            ItemInfo& ii = _virtualItems[i];
            if (dynamic_cast<GButton*>(ii.obj) && !((GButton*)ii.obj)->isSelected())
            {
                ((GButton*)ii.obj)->setSelected(true);
                last = i;
            }
            ii.selected = true;
        }
    }
    else
    {
        int cnt = (int)_children.size();
        for (int i = 0; i < cnt; i++)
        {
            GButton* obj = _children.at(i)->as<GButton>();
            if (obj != nullptr && !obj->isSelected())
            {
                obj->setSelected(true);
                last = i;
            }
        }
    }

    if (last != -1)
        updateSelectionController(last);
}

void GList::selectReverse()
{
    checkVirtualList();

    int last = -1;
    if (_virtual)
    {
        int cnt = _realNumItems;
        for (int i = 0; i < cnt; i++)
        {
            ItemInfo& ii = _virtualItems[i];
            if (dynamic_cast<GButton*>(ii.obj))
            {
                ((GButton*)ii.obj)->setSelected(!((GButton*)ii.obj)->isSelected());
                if (((GButton*)ii.obj)->isSelected())
                    last = i;
            }
            ii.selected = !ii.selected;
        }
    }
    else
    {
        int cnt = (int)_children.size();
        for (int i = 0; i < cnt; i++)
        {
            GButton* obj = _children.at(i)->as<GButton>();
            if (obj != nullptr)
            {
                obj->setSelected(!obj->isSelected());
                if (obj->isSelected())
                    last = i;
            }
        }
    }

    if (last != -1)
        updateSelectionController(last);
}

void GList::handleArrowKey(int dir)
{
    int index = getSelectedIndex();
    if (index == -1)
        return;

    switch (dir)
    {
    case 1: //up
        if (_layout == ListLayoutType::SINGLE_COLUMN || _layout == ListLayoutType::FLOW_VERTICAL)
        {
            index--;
            if (index >= 0)
            {
                clearSelection();
                addSelection(index, true);
            }
        }
        else if (_layout == ListLayoutType::FLOW_HORIZONTAL || _layout == ListLayoutType::PAGINATION)
        {
            GObject* current = _children.at(index);
            int k = 0;
            int i;
            for (i = index - 1; i >= 0; i--)
            {
                GObject* obj = _children.at(i);
                if (obj->getY() != current->getY())
                {
                    current = obj;
                    break;
                }
                k++;
            }
            for (; i >= 0; i--)
            {
                GObject* obj = _children.at(i);
                if (obj->getY() != current->getY())
                {
                    clearSelection();
                    addSelection(i + k + 1, true);
                    break;
                }
            }
        }
        break;

    case 3: //right
        if (_layout == ListLayoutType::SINGLE_ROW || _layout == ListLayoutType::FLOW_HORIZONTAL || _layout == ListLayoutType::PAGINATION)
        {
            index++;
            if (index < _children.size())
            {
                clearSelection();
                addSelection(index, true);
            }
        }
        else if (_layout == ListLayoutType::FLOW_VERTICAL)
        {
            GObject* current = _children.at(index);
            int k = 0;
            int cnt = (int)_children.size();
            int i;
            for (i = index + 1; i < cnt; i++)
            {
                GObject* obj = _children.at(i);
                if (obj->getX() != current->getX())
                {
                    current = obj;
                    break;
                }
                k++;
            }
            for (; i < cnt; i++)
            {
                GObject* obj = _children.at(i);
                if (obj->getX() != current->getX())
                {
                    clearSelection();
                    addSelection(i - k - 1, true);
                    break;
                }
            }
        }
        break;

    case 5: //down
        if (_layout == ListLayoutType::SINGLE_COLUMN || _layout == ListLayoutType::FLOW_VERTICAL)
        {
            index++;
            if (index < _children.size())
            {
                clearSelection();
                addSelection(index, true);
            }
        }
        else if (_layout == ListLayoutType::FLOW_HORIZONTAL || _layout == ListLayoutType::PAGINATION)
        {
            GObject* current = _children.at(index);
            int k = 0;
            int cnt = (int)_children.size();
            int i;
            for (i = index + 1; i < cnt; i++)
            {
                GObject* obj = _children.at(i);
                if (obj->getY() != current->getY())
                {
                    current = obj;
                    break;
                }
                k++;
            }
            for (; i < cnt; i++)
            {
                GObject* obj = _children.at(i);
                if (obj->getY() != current->getY())
                {
                    clearSelection();
                    addSelection(i - k - 1, true);
                    break;
                }
            }
        }
        break;

    case 7: //left
        if (_layout == ListLayoutType::SINGLE_ROW || _layout == ListLayoutType::FLOW_HORIZONTAL || _layout == ListLayoutType::PAGINATION)
        {
            index--;
            if (index >= 0)
            {
                clearSelection();
                addSelection(index, true);
            }
        }
        else if (_layout == ListLayoutType::FLOW_VERTICAL)
        {
            GObject* current = _children.at(index);
            int k = 0;
            int i;
            for (i = index - 1; i >= 0; i--)
            {
                GObject* obj = _children.at(i);
                if (obj->getX() != current->getX())
                {
                    current = obj;
                    break;
                }
                k++;
            }
            for (; i >= 0; i--)
            {
                GObject* obj = _children.at(i);
                if (obj->getX() != current->getX())
                {
                    clearSelection();
                    addSelection(i + k + 1, true);
                    break;
                }
            }
        }
        break;
    }
}

void GList::onItemTouchBegin(EventContext* context)
{
    GButton* item = (GButton*)context->getSender();
    if (_selectionMode == ListSelectionMode::NONE)
        return;

    _selectionHandled = false;

    if (UIConfig::defaultScrollTouchEffect && (_scrollPane != nullptr || (_parent != nullptr && _parent->getScrollPane() != nullptr)))
        return;

    if (_selectionMode == ListSelectionMode::SINGLE)
    {
        setSelectionOnEvent(item, context->getInput());
    }
    else
    {
        if (!item->isSelected())
            setSelectionOnEvent(item, context->getInput());
    }
}

void GList::onClickItem(EventContext* context)
{
    GButton* item = (GButton*)context->getSender();
    if (!_selectionHandled)
        setSelectionOnEvent(item, context->getInput());
    _selectionHandled = false;

    if (_scrollPane != nullptr && scrollItemToViewOnClick)
        _scrollPane->scrollToView(item, true);

    dispatchItemEvent(item, context);
}

void GList::dispatchItemEvent(GObject* item, EventContext* context)
{
    dispatchEvent(context->getType() == UIEventType::Click ? UIEventType::ClickItem : UIEventType::RightClickItem, item);
}

void GList::setSelectionOnEvent(GObject* item, InputEvent* evt)
{
    if (!(dynamic_cast<GButton*>(item)) || _selectionMode == ListSelectionMode::NONE)
        return;

    _selectionHandled = true;
    bool dontChangeLastIndex = false;
    GButton* button = (GButton*)item;
    int index = childIndexToItemIndex(getChildIndex(item));

    if (_selectionMode == ListSelectionMode::SINGLE)
    {
        if (!button->isSelected())
        {
            clearSelectionExcept(button);
            button->setSelected(true);
        }
    }
    else
    {
        if (evt->isShiftDown())
        {
            if (!button->isSelected())
            {
                if (_lastSelectedIndex != -1)
                {
                    int min = MIN(_lastSelectedIndex, index);
                    int max = MAX(_lastSelectedIndex, index);
                    max = MIN(max, getNumItems() - 1);
                    if (_virtual)
                    {
                        for (int i = min; i <= max; i++)
                        {
                            ItemInfo& ii = _virtualItems[i];
                            if (dynamic_cast<GButton*>(ii.obj))
                                ((GButton*)ii.obj)->setSelected(true);
                            ii.selected = true;
                        }
                    }
                    else
                    {
                        for (int i = min; i <= max; i++)
                        {
                            GButton* obj = getChildAt(i)->as<GButton>();
                            if (obj != nullptr && !obj->isSelected())
                                obj->setSelected(true);
                        }
                    }

                    dontChangeLastIndex = true;
                }
                else
                {
                    button->setSelected(true);
                }
            }
        }
        else if (evt->isCtrlDown() || _selectionMode == ListSelectionMode::MULTIPLE_SINGLECLICK)
        {
            button->setSelected(!button->isSelected());
        }
        else
        {
            if (!button->isSelected())
            {
                clearSelectionExcept(button);
                button->setSelected(true);
            }
            else
                clearSelectionExcept(button);
        }
    }

    if (!dontChangeLastIndex)
        _lastSelectedIndex = index;

    if (button->isSelected())
        updateSelectionController(index);
}

void GList::resizeToFit(int itemCount, int minSize)
{
    ensureBoundsCorrect();

    int curCount = getNumItems();
    if (itemCount > curCount)
        itemCount = curCount;

    if (_virtual)
    {
        int lineCount = ceil((float)itemCount / _curLineItemCount);
        if (_layout == ListLayoutType::SINGLE_COLUMN || _layout == ListLayoutType::FLOW_HORIZONTAL)
            setViewHeight(lineCount * _itemSize.y + MAX(0, lineCount - 1) * _lineGap);
        else
            setViewWidth(lineCount * _itemSize.x + MAX(0, lineCount - 1) * _columnGap);
    }
    else if (itemCount == 0)
    {
        if (_layout == ListLayoutType::SINGLE_COLUMN || _layout == ListLayoutType::FLOW_HORIZONTAL)
            setViewHeight(minSize);
        else
            setViewWidth(minSize);
    }
    else
    {
        int i = itemCount - 1;
        GObject* obj = nullptr;
        while (i >= 0)
        {
            obj = getChildAt(i);
            if (!foldInvisibleItems || obj->isVisible())
                break;
            i--;
        }
        if (i < 0)
        {
            if (_layout == ListLayoutType::SINGLE_COLUMN || _layout == ListLayoutType::FLOW_HORIZONTAL)
                setViewHeight(minSize);
            else
                setViewWidth(minSize);
        }
        else
        {
            float size;
            if (_layout == ListLayoutType::SINGLE_COLUMN || _layout == ListLayoutType::FLOW_HORIZONTAL)
            {
                size = obj->getY() + obj->getHeight();
                if (size < minSize)
                    size = minSize;
                setViewHeight(size);
            }
            else
            {
                size = obj->getX() + obj->getWidth();
                if (size < minSize)
                    size = minSize;
                setViewWidth(size);
            }
        }
    }
}

int GList::getFirstChildInView()
{
    return childIndexToItemIndex(GComponent::getFirstChildInView());
}

void GList::handleSizeChanged()
{
    GComponent::handleSizeChanged();

    setBoundsChangedFlag();
    if (_virtual)
        setVirtualListChangedFlag(true);
}

void GList::handleControllerChanged(GController* c)
{
    GComponent::handleControllerChanged(c);

    if (_selectionController == c)
        setSelectedIndex(c->getSelectedIndex());
}

void GList::updateSelectionController(int index)
{
    if (_selectionController != nullptr && !_selectionController->changing && index < _selectionController->getPageCount())
    {
        GController* c = _selectionController;
        _selectionController = nullptr;
        c->setSelectedIndex(index);
        _selectionController = c;
    }
}

void GList::scrollToView(int index, bool ani, bool setFirst)
{
    if (_virtual)
    {
        if (_numItems == 0)
            return;

        checkVirtualList();

        AXASSERT(index >= 0 && index < (int)_virtualItems.size(), "Invalid child index");

        if (_loop)
            index = floor(_firstIndex / _numItems) * _numItems + index;

        Rect rect;
        ItemInfo& ii = _virtualItems[index];
        if (_layout == ListLayoutType::SINGLE_COLUMN || _layout == ListLayoutType::FLOW_HORIZONTAL)
        {
            float pos = 0;
            for (int i = _curLineItemCount - 1; i < index; i += _curLineItemCount)
                pos += _virtualItems[i].size.y + _lineGap;
            rect.setRect(0, pos, _itemSize.x, ii.size.y);
        }
        else if (_layout == ListLayoutType::SINGLE_ROW || _layout == ListLayoutType::FLOW_VERTICAL)
        {
            float pos = 0;
            for (int i = _curLineItemCount - 1; i < index; i += _curLineItemCount)
                pos += _virtualItems[i].size.x + _columnGap;
            rect.setRect(pos, 0, ii.size.x, _itemSize.y);
        }
        else
        {
            int page = index / (_curLineItemCount * _curLineItemCount2);
            rect.setRect(page * getViewWidth() + (index % _curLineItemCount) * (ii.size.x + _columnGap),
                         (index / _curLineItemCount) % _curLineItemCount2 * (ii.size.y + _lineGap),
                         ii.size.x, ii.size.y);
        }

        setFirst = true;
        if (_scrollPane != nullptr)
            _scrollPane->scrollToView(rect, ani, setFirst);
        else if (_parent != nullptr && _parent->getScrollPane() != nullptr)
            _parent->getScrollPane()->scrollToView(transformRect(rect, _parent), ani, setFirst);
    }
    else
    {
        GObject* obj = getChildAt(index);
        if (_scrollPane != nullptr)
            _scrollPane->scrollToView(obj, ani, setFirst);
        else if (_parent != nullptr && _parent->getScrollPane() != nullptr)
            _parent->getScrollPane()->scrollToView(obj, ani, setFirst);
    }
}

int GList::childIndexToItemIndex(int index)
{
    if (!_virtual)
        return index;

    if (_layout == ListLayoutType::PAGINATION)
    {
        for (int i = _firstIndex; i < _realNumItems; i++)
        {
            if (_virtualItems[i].obj != nullptr)
            {
                index--;
                if (index < 0)
                    return i;
            }
        }

        return index;
    }
    else
    {
        index += _firstIndex;
        if (_loop && _numItems > 0)
            index = index % _numItems;

        return index;
    }
}

int GList::itemIndexToChildIndex(int index)
{
    if (!_virtual)
        return index;

    if (_layout == ListLayoutType::PAGINATION)
    {
        return getChildIndex(_virtualItems[index].obj);
    }
    else
    {
        if (_loop && _numItems > 0)
        {
            int j = _firstIndex % _numItems;
            if (index >= j)
                index = index - j;
            else
                index = _numItems - j + index;
        }
        else
            index -= _firstIndex;

        return index;
    }
}

void GList::setVirtual()
{
    setVirtual(false);
}

void GList::setVirtualAndLoop()
{
    setVirtual(true);
}

void GList::setVirtual(bool loop)
{
    if (!_virtual)
    {
        AXASSERT(_scrollPane != nullptr, "FairyGUI: Virtual list must be scrollable!");

        if (loop)
        {
            AXASSERT(_layout != ListLayoutType::FLOW_HORIZONTAL && _layout != ListLayoutType::FLOW_VERTICAL,
                     "FairyGUI: Loop list is not supported for FlowHorizontal or FlowVertical layout!");

            _scrollPane->setBouncebackEffect(false);
        }

        _virtual = true;
        _loop = loop;
        removeChildrenToPool();

        if (_itemSize.x == 0 || _itemSize.y == 0)
        {
            GObject* obj = getFromPool();
            AXASSERT(obj != nullptr, "FairyGUI: Virtual List must have a default list item resource.");
            _itemSize = obj->getSize();
            _itemSize.x = ceil(_itemSize.x);
            _itemSize.y = ceil(_itemSize.y);
            returnToPool(obj);
        }

        if (_layout == ListLayoutType::SINGLE_COLUMN || _layout == ListLayoutType::FLOW_HORIZONTAL)
        {
            _scrollPane->setScrollStep(_itemSize.y);
            if (_loop)
                _scrollPane->_loop = 2;
        }
        else
        {
            _scrollPane->setScrollStep(_itemSize.x);
            if (_loop)
                _scrollPane->_loop = 1;
        }

        addEventListener(UIEventType::Scroll, AX_CALLBACK_1(GList::onScroll, this));
        setVirtualListChangedFlag(true);
    }
}

int GList::getNumItems()
{
    if (_virtual)
        return _numItems;
    else
        return (int)_children.size();
}

void GList::setNumItems(int value)
{
    if (_virtual)
    {
        AXASSERT(itemRenderer != nullptr, "FairyGUI: Set itemRenderer first!");

        _numItems = value;
        if (_loop)
            _realNumItems = _numItems * 6;
        else
            _realNumItems = _numItems;

        int oldCount = (int)_virtualItems.size();
        if (_realNumItems > oldCount)
        {
            for (int i = oldCount; i < _realNumItems; i++)
            {
                ItemInfo ii;
                ii.size = _itemSize;

                _virtualItems.push_back(ii);
            }
        }
        else
        {
            for (int i = _realNumItems; i < oldCount; i++)
                _virtualItems[i].selected = false;
        }

        if (_virtualListChanged != 0)
            CALL_LATER_CANCEL(GList, doRefreshVirtualList);

        doRefreshVirtualList();
    }
    else
    {
        int cnt = (int)_children.size();
        if (value > cnt)
        {
            for (int i = cnt; i < value; i++)
            {
                if (itemProvider == nullptr)
                    addItemFromPool();
                else
                    addItemFromPool(itemProvider(i));
            }
        }
        else
        {
            removeChildrenToPool(value, cnt);
        }

        if (itemRenderer != nullptr)
        {
            for (int i = 0; i < value; i++)
                itemRenderer(i, getChildAt(i));
        }
    }
}

void GList::refreshVirtualList()
{
    AXASSERT(_virtual, "FairyGUI: not virtual list");

    setVirtualListChangedFlag(false);
}

ax::Vec2 GList::getSnappingPosition(const ax::Vec2& pt)
{
    if (_virtual)
    {
        Vec2 ret = pt;
        if (_layout == ListLayoutType::SINGLE_COLUMN || _layout == ListLayoutType::FLOW_HORIZONTAL)
        {
            int index = getIndexOnPos1(ret.y, false);
            if (index < (int)_virtualItems.size() && pt.y - ret.y > _virtualItems[index].size.y / 2 && index < _realNumItems)
                ret.y += _virtualItems[index].size.y + _lineGap;
        }
        else if (_layout == ListLayoutType::SINGLE_ROW || _layout == ListLayoutType::FLOW_VERTICAL)
        {
            int index = getIndexOnPos2(ret.x, false);
            if (index < (int)_virtualItems.size() && pt.x - ret.x > _virtualItems[index].size.x / 2 && index < _realNumItems)
                ret.x += _virtualItems[index].size.x + _columnGap;
        }
        else
        {
            int index = getIndexOnPos3(ret.x, false);
            if (index < (int)_virtualItems.size() && pt.x - ret.x > _virtualItems[index].size.x / 2 && index < _realNumItems)
                ret.x += _virtualItems[index].size.x + _columnGap;
        }

        return ret;
    }
    else
        return GComponent::getSnappingPosition(pt);
}

void GList::checkVirtualList()
{
    if (_virtualListChanged != 0)
    {
        doRefreshVirtualList();
        CALL_LATER_CANCEL(GList, doRefreshVirtualList);
    }
}

void GList::setVirtualListChangedFlag(bool layoutChanged)
{
    if (layoutChanged)
        _virtualListChanged = 2;
    else if (_virtualListChanged == 0)
        _virtualListChanged = 1;

    CALL_LATER(GList, doRefreshVirtualList);
}

void GList::doRefreshVirtualList()
{
    bool layoutChanged = _virtualListChanged == 2;
    _virtualListChanged = 0;
    _eventLocked = true;

    if (layoutChanged)
    {
        if (_layout == ListLayoutType::SINGLE_COLUMN || _layout == ListLayoutType::SINGLE_ROW)
            _curLineItemCount = 1;
        else if (_layout == ListLayoutType::FLOW_HORIZONTAL)
        {
            if (_columnCount > 0)
                _curLineItemCount = _columnCount;
            else
            {
                _curLineItemCount = floor((_scrollPane->getViewSize().width + _columnGap) / (_itemSize.x + _columnGap));
                if (_curLineItemCount <= 0)
                    _curLineItemCount = 1;
            }
        }
        else if (_layout == ListLayoutType::FLOW_VERTICAL)
        {
            if (_lineCount > 0)
                _curLineItemCount = _lineCount;
            else
            {
                _curLineItemCount = floor((_scrollPane->getViewSize().height + _lineGap) / (_itemSize.y + _lineGap));
                if (_curLineItemCount <= 0)
                    _curLineItemCount = 1;
            }
        }
        else //pagination
        {
            if (_columnCount > 0)
                _curLineItemCount = _columnCount;
            else
            {
                _curLineItemCount = floor((_scrollPane->getViewSize().width + _columnGap) / (_itemSize.x + _columnGap));
                if (_curLineItemCount <= 0)
                    _curLineItemCount = 1;
            }

            if (_lineCount > 0)
                _curLineItemCount2 = _lineCount;
            else
            {
                _curLineItemCount2 = floor((_scrollPane->getViewSize().height + _lineGap) / (_itemSize.y + _lineGap));
                if (_curLineItemCount2 <= 0)
                    _curLineItemCount2 = 1;
            }
        }
    }
    float ch = 0, cw = 0;
    if (_realNumItems > 0)
    {
        int len = ceil((float)_realNumItems / _curLineItemCount) * _curLineItemCount;
        int len2 = MIN(_curLineItemCount, _realNumItems);
        if (_layout == ListLayoutType::SINGLE_COLUMN || _layout == ListLayoutType::FLOW_HORIZONTAL)
        {
            for (int i = 0; i < len; i += _curLineItemCount)
                ch += _virtualItems[i].size.y + _lineGap;
            if (ch > 0)
                ch -= _lineGap;

            if (_autoResizeItem)
                cw = _scrollPane->getViewSize().width;
            else
            {
                for (int i = 0; i < len2; i++)
                    cw += _virtualItems[i].size.x + _columnGap;
                if (cw > 0)
                    cw -= _columnGap;
            }
        }
        else if (_layout == ListLayoutType::SINGLE_ROW || _layout == ListLayoutType::FLOW_VERTICAL)
        {
            for (int i = 0; i < len; i += _curLineItemCount)
                cw += _virtualItems[i].size.x + _columnGap;
            if (cw > 0)
                cw -= _columnGap;

            if (_autoResizeItem)
                ch = _scrollPane->getViewSize().height;
            else
            {
                for (int i = 0; i < len2; i++)
                    ch += _virtualItems[i].size.y + _lineGap;
                if (ch > 0)
                    ch -= _lineGap;
            }
        }
        else
        {
            int pageCount = ceil((float)len / (_curLineItemCount * _curLineItemCount2));
            cw = pageCount * getViewWidth();
            ch = getViewHeight();
        }
    }

    handleAlign(cw, ch);
    _scrollPane->setContentSize(cw, ch);

    _eventLocked = false;

    handleScroll(true);
}

void GList::onScroll(EventContext* context)
{
    handleScroll(false);
}

int GList::getIndexOnPos1(float& pos, bool forceUpdate)
{
    if (_realNumItems < _curLineItemCount)
    {
        pos = 0;
        return 0;
    }

    if (numChildren() > 0 && !forceUpdate)
    {
        float pos2 = getChildAt(0)->getY();
        if (pos2 + (_lineGap > 0 ? 0 : -_lineGap) > pos)
        {
            for (int i = _firstIndex - _curLineItemCount; i >= 0; i -= _curLineItemCount)
            {
                pos2 -= (_virtualItems[i].size.y + _lineGap);
                if (pos2 <= pos)
                {
                    pos = pos2;
                    return i;
                }
            }

            pos = 0;
            return 0;
        }
        else
        {
            float testGap = _lineGap > 0 ? _lineGap : 0;
            for (int i = _firstIndex; i < _realNumItems; i += _curLineItemCount)
            {
                float pos3 = pos2 + _virtualItems[i].size.y;
                if (pos3 + testGap > pos)
                {
                    pos = pos2;
                    return i;
                }
                pos2 = pos3 + _lineGap;
            }

            pos = pos2;
            return _realNumItems - _curLineItemCount;
        }
    }
    else
    {
        float pos2 = 0;
        float testGap = _lineGap > 0 ? _lineGap : 0;
        for (int i = 0; i < _realNumItems; i += _curLineItemCount)
        {
            float pos3 = pos2 + _virtualItems[i].size.y;
            if (pos3 + testGap > pos)
            {
                pos = pos2;
                return i;
            }
            pos2 = pos3 + _lineGap;
        }

        pos = pos2;
        return _realNumItems - _curLineItemCount;
    }
}

int GList::getIndexOnPos2(float& pos, bool forceUpdate)
{
    if (_realNumItems < _curLineItemCount)
    {
        pos = 0;
        return 0;
    }

    if (numChildren() > 0 && !forceUpdate)
    {
        float pos2 = getChildAt(0)->getX();
        if (pos2 + (_columnGap > 0 ? 0 : -_columnGap) > pos)
        {
            for (int i = _firstIndex - _curLineItemCount; i >= 0; i -= _curLineItemCount)
            {
                pos2 -= (_virtualItems[i].size.x + _columnGap);
                if (pos2 <= pos)
                {
                    pos = pos2;
                    return i;
                }
            }

            pos = 0;
            return 0;
        }
        else
        {
            float testGap = _columnGap > 0 ? _columnGap : 0;
            for (int i = _firstIndex; i < _realNumItems; i += _curLineItemCount)
            {
                float pos3 = pos2 + _virtualItems[i].size.x;
                if (pos3 + testGap > pos)
                {
                    pos = pos2;
                    return i;
                }
                pos2 = pos3 + _columnGap;
            }

            pos = pos2;
            return _realNumItems - _curLineItemCount;
        }
    }
    else
    {
        float pos2 = 0;
        float testGap = _columnGap > 0 ? _columnGap : 0;
        for (int i = 0; i < _realNumItems; i += _curLineItemCount)
        {
            float pos3 = pos2 + _virtualItems[i].size.x;
            if (pos3 + testGap > pos)
            {
                pos = pos2;
                return i;
            }
            pos2 = pos3 + _columnGap;
        }

        pos = pos2;
        return _realNumItems - _curLineItemCount;
    }
}

int GList::getIndexOnPos3(float& pos, bool forceUpdate)
{
    if (_realNumItems < _curLineItemCount)
    {
        pos = 0;
        return 0;
    }

    float viewWidth = getViewWidth();
    int page = floor(pos / viewWidth);
    int startIndex = page * (_curLineItemCount * _curLineItemCount2);
    float pos2 = page * viewWidth;
    float testGap = _columnGap > 0 ? _columnGap : 0;
    for (int i = 0; i < _curLineItemCount; i++)
    {
        float pos3 = pos2 + _virtualItems[startIndex + i].size.x;
        if (pos3 + testGap > pos)
        {
            pos = pos2;
            return startIndex + i;
        }
        pos2 = pos3 + _columnGap;
    }

    pos = pos2;
    return startIndex + _curLineItemCount - 1;
}

void GList::handleScroll(bool forceUpdate)
{
    if (_eventLocked)
        return;

    if (_layout == ListLayoutType::SINGLE_COLUMN || _layout == ListLayoutType::FLOW_HORIZONTAL)
    {
        int enterCounter = 0;
        while (handleScroll1(forceUpdate))
        {
            enterCounter++;
            forceUpdate = false;
            if (enterCounter > 20)
            {
                AXLOGD("FairyGUI: list will never be filled as the item renderer function always returns a different size.");
                break;
            }
        }
        handleArchOrder1();
    }
    else if (_layout == ListLayoutType::SINGLE_ROW || _layout == ListLayoutType::FLOW_VERTICAL)
    {
        int enterCounter = 0;
        while (handleScroll2(forceUpdate))
        {
            enterCounter++;
            forceUpdate = false;
            if (enterCounter > 20)
            {
                AXLOGD("FairyGUI: list will never be filled as the item renderer function always returns a different size.");
                break;
            }
        }
        handleArchOrder2();
    }
    else
    {
        handleScroll3(forceUpdate);
    }

    _boundsChanged = false;
}

bool GList::handleScroll1(bool forceUpdate)
{
    float pos = _scrollPane->getScrollingPosY();
    float max = pos + _scrollPane->getViewSize().height;
    bool end = max == _scrollPane->getContentSize().height;

    int newFirstIndex = getIndexOnPos1(pos, forceUpdate);
    if (newFirstIndex == _firstIndex && !forceUpdate)
        return false;

    int oldFirstIndex = _firstIndex;
    _firstIndex = newFirstIndex;
    int curIndex = newFirstIndex;
    bool forward = oldFirstIndex > newFirstIndex;
    int childCount = numChildren();
    int lastIndex = oldFirstIndex + childCount - 1;
    int reuseIndex = forward ? lastIndex : oldFirstIndex;
    float curX = 0, curY = pos;
    bool needRender;
    float deltaSize = 0;
    float firstItemDeltaSize = 0;
    std::string url = _defaultItem;
    int partSize = (int)((_scrollPane->getViewSize().width - _columnGap * (_curLineItemCount - 1)) / _curLineItemCount);

    _itemInfoVer++;
    while (curIndex < _realNumItems && (end || curY < max))
    {
        ItemInfo& ii = _virtualItems[curIndex];

        if (ii.obj == nullptr || forceUpdate)
        {
            if (itemProvider != nullptr)
            {
                url = itemProvider(curIndex % _numItems);
                if (url.size() == 0)
                    url = _defaultItem;
                url = UIPackage::normalizeURL(url);
            }

            if (ii.obj != nullptr && ii.obj->getResourceURL().compare(url) != 0)
            {
                if (dynamic_cast<GButton*>(ii.obj))
                    ii.selected = ((GButton*)ii.obj)->isSelected();
                removeChildToPool(ii.obj);
                ii.obj = nullptr;
            }
        }

        if (ii.obj == nullptr)
        {
            if (forward)
            {
                for (int j = reuseIndex; j >= oldFirstIndex; j--)
                {
                    ItemInfo& ii2 = _virtualItems[j];
                    if (ii2.obj != nullptr && ii2.updateFlag != _itemInfoVer && ii2.obj->getResourceURL().compare(url) == 0)
                    {
                        if (dynamic_cast<GButton*>(ii2.obj))
                            ii2.selected = ((GButton*)ii2.obj)->isSelected();
                        ii.obj = ii2.obj;
                        ii2.obj = nullptr;
                        if (j == reuseIndex)
                            reuseIndex--;
                        break;
                    }
                }
            }
            else
            {
                for (int j = reuseIndex; j <= lastIndex; j++)
                {
                    ItemInfo& ii2 = _virtualItems[j];
                    if (ii2.obj != nullptr && ii2.updateFlag != _itemInfoVer && ii2.obj->getResourceURL().compare(url) == 0)
                    {
                        if (dynamic_cast<GButton*>(ii2.obj))
                            ii2.selected = ((GButton*)ii2.obj)->isSelected();
                        ii.obj = ii2.obj;
                        ii2.obj = nullptr;
                        if (j == reuseIndex)
                            reuseIndex++;
                        break;
                    }
                }
            }

            if (ii.obj != nullptr)
            {
                setChildIndex(ii.obj, forward ? curIndex - newFirstIndex : numChildren());
            }
            else
            {
                ii.obj = _pool->getObject(url);
                if (forward)
                    addChildAt(ii.obj, curIndex - newFirstIndex);
                else
                    addChild(ii.obj);
            }
            if (dynamic_cast<GButton*>(ii.obj))
                ((GButton*)ii.obj)->setSelected(ii.selected);

            needRender = true;
        }
        else
            needRender = forceUpdate;

        if (needRender)
        {
            if (_autoResizeItem && (_layout == ListLayoutType::SINGLE_COLUMN || _columnCount > 0))
                ii.obj->setSize(partSize, ii.obj->getHeight(), true);

            itemRenderer(curIndex % _numItems, ii.obj);
            if (curIndex % _curLineItemCount == 0)
            {
                deltaSize += ceil(ii.obj->getHeight()) - ii.size.y;
                if (curIndex == newFirstIndex && oldFirstIndex > newFirstIndex)
                {
                    firstItemDeltaSize = ceil(ii.obj->getHeight()) - ii.size.y;
                }
            }
            ii.size.x = ceil(ii.obj->getWidth());
            ii.size.y = ceil(ii.obj->getHeight());
        }

        ii.updateFlag = _itemInfoVer;
        ii.obj->setPosition(curX, curY);
        if (curIndex == newFirstIndex)
            max += ii.size.y;

        curX += ii.size.x + _columnGap;

        if (curIndex % _curLineItemCount == _curLineItemCount - 1)
        {
            curX = 0;
            curY += ii.size.y + _lineGap;
        }
        curIndex++;
    }

    for (int i = 0; i < childCount; i++)
    {
        ItemInfo& ii = _virtualItems[oldFirstIndex + i];
        if (ii.updateFlag != _itemInfoVer && ii.obj != nullptr)
        {
            if (dynamic_cast<GButton*>(ii.obj))
                ii.selected = ((GButton*)ii.obj)->isSelected();
            removeChildToPool(ii.obj);
            ii.obj = nullptr;
        }
    }

    childCount = (int)_children.size();
    for (int i = 0; i < childCount; i++)
    {
        GObject* obj = _virtualItems[newFirstIndex + i].obj;
        if (_children.at(i) != obj)
            setChildIndex(obj, i);
    }

    if (deltaSize != 0 || firstItemDeltaSize != 0)
        _scrollPane->changeContentSizeOnScrolling(0, deltaSize, 0, firstItemDeltaSize);

    if (curIndex > 0 && numChildren() > 0 && _container->getPositionY2() <= 0 && getChildAt(0)->getY() > -_container->getPositionY2())
        return true;
    else
        return false;
}

bool GList::handleScroll2(bool forceUpdate)
{
    float pos = _scrollPane->getScrollingPosX();
    float max = pos + _scrollPane->getViewSize().width;
    bool end = pos == _scrollPane->getContentSize().width;

    int newFirstIndex = getIndexOnPos2(pos, forceUpdate);
    if (newFirstIndex == _firstIndex && !forceUpdate)
        return false;

    int oldFirstIndex = _firstIndex;
    _firstIndex = newFirstIndex;
    int curIndex = newFirstIndex;
    bool forward = oldFirstIndex > newFirstIndex;
    int childCount = numChildren();
    int lastIndex = oldFirstIndex + childCount - 1;
    int reuseIndex = forward ? lastIndex : oldFirstIndex;
    float curX = pos, curY = 0;
    bool needRender;
    float deltaSize = 0;
    float firstItemDeltaSize = 0;
    string url = _defaultItem;
    int partSize = (int)((_scrollPane->getViewSize().height - _lineGap * (_curLineItemCount - 1)) / _curLineItemCount);

    _itemInfoVer++;
    while (curIndex < _realNumItems && (end || curX < max))
    {
        ItemInfo& ii = _virtualItems[curIndex];

        if (ii.obj == nullptr || forceUpdate)
        {
            if (itemProvider != nullptr)
            {
                url = itemProvider(curIndex % _numItems);
                if (url.size() == 0)
                    url = _defaultItem;
                url = UIPackage::normalizeURL(url);
            }

            if (ii.obj != nullptr && ii.obj->getResourceURL().compare(url) != 0)
            {
                if (dynamic_cast<GButton*>(ii.obj))
                    ii.selected = ((GButton*)ii.obj)->isSelected();
                removeChildToPool(ii.obj);
                ii.obj = nullptr;
            }
        }

        if (ii.obj == nullptr)
        {
            if (forward)
            {
                for (int j = reuseIndex; j >= oldFirstIndex; j--)
                {
                    ItemInfo& ii2 = _virtualItems[j];
                    if (ii2.obj != nullptr && ii2.updateFlag != _itemInfoVer && ii2.obj->getResourceURL().compare(url) == 0)
                    {
                        if (dynamic_cast<GButton*>(ii2.obj))
                            ii2.selected = ((GButton*)ii2.obj)->isSelected();
                        ii.obj = ii2.obj;
                        ii2.obj = nullptr;
                        if (j == reuseIndex)
                            reuseIndex--;
                        break;
                    }
                }
            }
            else
            {
                for (int j = reuseIndex; j <= lastIndex; j++)
                {
                    ItemInfo& ii2 = _virtualItems[j];
                    if (ii2.obj != nullptr && ii2.updateFlag != _itemInfoVer && ii2.obj->getResourceURL().compare(url) == 0)
                    {
                        if (dynamic_cast<GButton*>(ii2.obj))
                            ii2.selected = ((GButton*)ii2.obj)->isSelected();
                        ii.obj = ii2.obj;
                        ii2.obj = nullptr;
                        if (j == reuseIndex)
                            reuseIndex++;
                        break;
                    }
                }
            }

            if (ii.obj != nullptr)
            {
                setChildIndex(ii.obj, forward ? curIndex - newFirstIndex : numChildren());
            }
            else
            {
                ii.obj = _pool->getObject(url);
                if (forward)
                    addChildAt(ii.obj, curIndex - newFirstIndex);
                else
                    addChild(ii.obj);
            }
            if (dynamic_cast<GButton*>(ii.obj))
                ((GButton*)ii.obj)->setSelected(ii.selected);

            needRender = true;
        }
        else
            needRender = forceUpdate;

        if (needRender)
        {
            if (_autoResizeItem && (_layout == ListLayoutType::SINGLE_ROW || _lineCount > 0))
                ii.obj->setSize(ii.obj->getWidth(), partSize, true);

            itemRenderer(curIndex % _numItems, ii.obj);
            if (curIndex % _curLineItemCount == 0)
            {
                deltaSize += ceil(ii.obj->getWidth()) - ii.size.x;
                if (curIndex == newFirstIndex && oldFirstIndex > newFirstIndex)
                {
                    firstItemDeltaSize = ceil(ii.obj->getWidth()) - ii.size.x;
                }
            }
            ii.size.x = ceil(ii.obj->getWidth());
            ii.size.y = ceil(ii.obj->getHeight());
        }

        ii.updateFlag = _itemInfoVer;
        ii.obj->setPosition(curX, curY);
        if (curIndex == newFirstIndex)
            max += ii.size.x;

        curY += ii.size.y + _lineGap;

        if (curIndex % _curLineItemCount == _curLineItemCount - 1)
        {
            curY = 0;
            curX += ii.size.x + _columnGap;
        }
        curIndex++;
    }

    for (int i = 0; i < childCount; i++)
    {
        ItemInfo& ii = _virtualItems[oldFirstIndex + i];
        if (ii.updateFlag != _itemInfoVer && ii.obj != nullptr)
        {
            if (dynamic_cast<GButton*>(ii.obj))
                ii.selected = ((GButton*)ii.obj)->isSelected();
            removeChildToPool(ii.obj);
            ii.obj = nullptr;
        }
    }

    childCount = (int)_children.size();
    for (int i = 0; i < childCount; i++)
    {
        GObject* obj = _virtualItems[newFirstIndex + i].obj;
        if (_children.at(i) != obj)
            setChildIndex(obj, i);
    }

    if (deltaSize != 0 || firstItemDeltaSize != 0)
        _scrollPane->changeContentSizeOnScrolling(deltaSize, 0, firstItemDeltaSize, 0);

    if (curIndex > 0 && numChildren() > 0 && _container->getPositionX() <= 0 && getChildAt(0)->getX() > -_container->getPositionX())
        return true;
    else
        return false;
}

void GList::handleScroll3(bool forceUpdate)
{
    float pos = _scrollPane->getScrollingPosX();

    int newFirstIndex = getIndexOnPos3(pos, forceUpdate);
    if (newFirstIndex == _firstIndex && !forceUpdate)
        return;

    int oldFirstIndex = _firstIndex;
    _firstIndex = newFirstIndex;

    int reuseIndex = oldFirstIndex;
    int virtualItemCount = (int)_virtualItems.size();
    int pageSize = _curLineItemCount * _curLineItemCount2;
    int startCol = newFirstIndex % _curLineItemCount;
    float viewWidth = getViewWidth();
    int page = (int)(newFirstIndex / pageSize);
    int startIndex = page * pageSize;
    int lastIndex = startIndex + pageSize * 2;
    bool needRender;
    string url = _defaultItem;
    int partWidth = (int)((_scrollPane->getViewSize().width - _columnGap * (_curLineItemCount - 1)) / _curLineItemCount);
    int partHeight = (int)((_scrollPane->getViewSize().height - _lineGap * (_curLineItemCount2 - 1)) / _curLineItemCount2);
    _itemInfoVer++;

    for (int i = startIndex; i < lastIndex; i++)
    {
        if (i >= _realNumItems)
            continue;

        int col = i % _curLineItemCount;
        if (i - startIndex < pageSize)
        {
            if (col < startCol)
                continue;
        }
        else
        {
            if (col > startCol)
                continue;
        }

        ItemInfo& ii = _virtualItems[i];
        ii.updateFlag = _itemInfoVer;
    }

    GObject* lastObj = nullptr;
    int insertIndex = 0;
    for (int i = startIndex; i < lastIndex; i++)
    {
        if (i >= _realNumItems)
            continue;

        ItemInfo& ii = _virtualItems[i];
        if (ii.updateFlag != _itemInfoVer)
            continue;

        if (ii.obj == nullptr)
        {
            reuseIndex = reuseIndex < 0 ? 0 : reuseIndex;
            while (reuseIndex < virtualItemCount)
            {
                ItemInfo& ii2 = _virtualItems[reuseIndex];
                if (ii2.obj != nullptr && ii2.updateFlag != _itemInfoVer)
                {
                    if (dynamic_cast<GButton*>(ii2.obj))
                        ii2.selected = ((GButton*)ii2.obj)->isSelected();
                    ii.obj = ii2.obj;
                    ii2.obj = nullptr;
                    break;
                }
                reuseIndex++;
            }

            if (insertIndex == -1)
                insertIndex = getChildIndex(lastObj) + 1;

            if (ii.obj == nullptr)
            {
                if (itemProvider != nullptr)
                {
                    url = itemProvider(i % _numItems);
                    if (url.size() == 0)
                        url = _defaultItem;
                    url = UIPackage::normalizeURL(url);
                }

                ii.obj = _pool->getObject(url);
                addChildAt(ii.obj, insertIndex);
            }
            else
            {
                insertIndex = setChildIndexBefore(ii.obj, insertIndex);
            }
            insertIndex++;

            if (dynamic_cast<GButton*>(ii.obj))
                ((GButton*)ii.obj)->setSelected(ii.selected);

            needRender = true;
        }
        else
        {
            needRender = forceUpdate;
            insertIndex = -1;
            lastObj = ii.obj;
        }

        if (needRender)
        {
            if (_autoResizeItem)
            {
                if (_curLineItemCount == _columnCount && _curLineItemCount2 == _lineCount)
                    ii.obj->setSize(partWidth, partHeight, true);
                else if (_curLineItemCount == _columnCount)
                    ii.obj->setSize(partWidth, ii.obj->getHeight(), true);
                else if (_curLineItemCount2 == _lineCount)
                    ii.obj->setSize(ii.obj->getWidth(), partHeight, true);
            }

            itemRenderer(i % _numItems, ii.obj);
            ii.size.x = ceil(ii.obj->getWidth());
            ii.size.y = ceil(ii.obj->getHeight());
        }
    }

    float borderX = (startIndex / pageSize) * viewWidth;
    float xx = borderX;
    float yy = 0;
    float lineHeight = 0;
    for (int i = startIndex; i < lastIndex; i++)
    {
        if (i >= _realNumItems)
            continue;

        ItemInfo& ii = _virtualItems[i];
        if (ii.updateFlag == _itemInfoVer)
            ii.obj->setPosition(xx, yy);

        if (ii.size.y > lineHeight)
            lineHeight = ii.size.y;
        if (i % _curLineItemCount == _curLineItemCount - 1)
        {
            xx = borderX;
            yy += lineHeight + _lineGap;
            lineHeight = 0;

            if (i == startIndex + pageSize - 1)
            {
                borderX += viewWidth;
                xx = borderX;
                yy = 0;
            }
        }
        else
            xx += ii.size.x + _columnGap;
    }

    for (int i = reuseIndex; i < virtualItemCount; i++)
    {
        ItemInfo& ii = _virtualItems[i];
        if (ii.updateFlag != _itemInfoVer && ii.obj != nullptr)
        {
            if (dynamic_cast<GButton*>(ii.obj))
                ii.selected = ((GButton*)ii.obj)->isSelected();
            removeChildToPool(ii.obj);
            ii.obj = nullptr;
        }
    }
}

void GList::handleArchOrder1()
{
    if (_childrenRenderOrder == ChildrenRenderOrder::ARCH)
    {
        float mid = _scrollPane->getPosY() + getViewHeight() / 2;
        float minDist = FLT_MAX, dist;
        int apexIndex = 0;
        int cnt = numChildren();
        for (int i = 0; i < cnt; i++)
        {
            GObject* obj = getChildAt(i);
            if (!foldInvisibleItems || obj->isVisible())
            {
                dist = abs(mid - obj->getY() - obj->getHeight() / 2);
                if (dist < minDist)
                {
                    minDist = dist;
                    apexIndex = i;
                }
            }
        }
        setApexIndex(apexIndex);
    }
}

void GList::handleArchOrder2()
{
    if (_childrenRenderOrder == ChildrenRenderOrder::ARCH)
    {
        float mid = _scrollPane->getPosX() + getViewWidth() / 2;
        float minDist = FLT_MAX, dist;
        int apexIndex = 0;
        int cnt = numChildren();
        for (int i = 0; i < cnt; i++)
        {
            GObject* obj = getChildAt(i);
            if (!foldInvisibleItems || obj->isVisible())
            {
                dist = abs(mid - obj->getX() - obj->getWidth() / 2);
                if (dist < minDist)
                {
                    minDist = dist;
                    apexIndex = i;
                }
            }
        }
        setApexIndex(apexIndex);
    }
}

void GList::handleAlign(float contentWidth, float contentHeight)
{
    Vec2 newOffset(0, 0);

    float viewHeight = getViewHeight();
    float viewWidth = getViewWidth();
    if (contentHeight < viewHeight)
    {
        if (_verticalAlign == TextVAlignment::CENTER)
            newOffset.y = (int)((viewHeight - contentHeight) / 2);
        else if (_verticalAlign == TextVAlignment::BOTTOM)
            newOffset.y = viewHeight - contentHeight;
    }

    if (contentWidth < viewWidth)
    {
        if (_align == TextHAlignment::CENTER)
            newOffset.x = (int)((viewWidth - contentWidth) / 2);
        else if (_align == TextHAlignment::RIGHT)
            newOffset.x = viewWidth - contentWidth;
    }

    if (!newOffset.equals(_alignOffset))
    {
        _alignOffset = newOffset;
        if (_scrollPane != nullptr)
            _scrollPane->adjustMaskContainer();
        else
            _container->setPosition2(_margin.left + _alignOffset.x, _margin.top + _alignOffset.y);
    }
}

void GList::updateBounds()
{
    if (_virtual)
        return;

    int cnt = (int)_children.size();
    int i;
    int j = 0;
    GObject* child;
    float curX = 0;
    float curY = 0;
    float cw, ch;
    float maxWidth = 0;
    float maxHeight = 0;
    float viewWidth = getViewWidth();
    float viewHeight = getViewHeight();

    if (_layout == ListLayoutType::SINGLE_COLUMN)
    {
        for (i = 0; i < cnt; i++)
        {
            child = getChildAt(i);
            if (foldInvisibleItems && !child->isVisible())
                continue;

            if (curY != 0)
                curY += _lineGap;
            child->setY(curY);
            if (_autoResizeItem)
                child->setSize(viewWidth, child->getHeight(), true);
            curY += ceil(child->getHeight());
            if (child->getWidth() > maxWidth)
                maxWidth = child->getWidth();
        }
        ch = curY;
        if (ch <= viewHeight && _autoResizeItem && _scrollPane != nullptr && _scrollPane->_displayInDemand && _scrollPane->_vtScrollBar != nullptr)
        {
            viewWidth += _scrollPane->_vtScrollBar->getWidth();
            for (i = 0; i < cnt; i++)
            {
                child = getChildAt(i);
                if (foldInvisibleItems && !child->isVisible())
                    continue;

                child->setSize(viewWidth, child->getHeight(), true);
                if (child->getWidth() > maxWidth)
                    maxWidth = child->getWidth();
            }
        }
        cw = ceil(maxWidth);
    }
    else if (_layout == ListLayoutType::SINGLE_ROW)
    {
        for (i = 0; i < cnt; i++)
        {
            child = getChildAt(i);
            if (foldInvisibleItems && !child->isVisible())
                continue;

            if (curX != 0)
                curX += _columnGap;
            child->setX(curX);
            if (_autoResizeItem)
                child->setSize(child->getWidth(), viewHeight, true);
            curX += ceil(child->getWidth());
            if (child->getHeight() > maxHeight)
                maxHeight = child->getHeight();
        }
        cw = curX;
        if (cw <= viewWidth && _autoResizeItem && _scrollPane != nullptr && _scrollPane->_displayInDemand && _scrollPane->_hzScrollBar != nullptr)
        {
            viewHeight += _scrollPane->_hzScrollBar->getHeight();
            for (i = 0; i < cnt; i++)
            {
                child = getChildAt(i);
                if (foldInvisibleItems && !child->isVisible())
                    continue;

                child->setSize(child->getWidth(), viewHeight, true);
                if (child->getHeight() > maxHeight)
                    maxHeight = child->getHeight();
            }
        }
        ch = ceil(maxHeight);
    }
    else if (_layout == ListLayoutType::FLOW_HORIZONTAL)
    {
        if (_autoResizeItem && _columnCount > 0)
        {
            float lineSize = 0;
            int lineStart = 0;
            float ratio;

            for (i = 0; i < cnt; i++)
            {
                child = getChildAt(i);
                if (foldInvisibleItems && !child->isVisible())
                    continue;

                lineSize += child->sourceSize.width;
                j++;
                if (j == _columnCount || i == cnt - 1)
                {
                    ratio = (viewWidth - lineSize - (j - 1) * _columnGap) / lineSize;
                    curX = 0;
                    for (j = lineStart; j <= i; j++)
                    {
                        child = getChildAt(j);
                        if (foldInvisibleItems && !child->isVisible())
                            continue;

                        child->setPosition(curX, curY);

                        if (j < i)
                        {
                            child->setSize(child->sourceSize.width + round(child->sourceSize.width * ratio), child->getHeight(), true);
                            curX += ceil(child->getWidth()) + _columnGap;
                        }
                        else
                        {
                            child->setSize(viewWidth - curX, child->getHeight(), true);
                        }
                        if (child->getHeight() > maxHeight)
                            maxHeight = child->getHeight();
                    }
                    //new line
                    curY += ceil(maxHeight) + _lineGap;
                    maxHeight = 0;
                    j = 0;
                    lineStart = i + 1;
                    lineSize = 0;
                }
            }
            ch = curY + ceil(maxHeight);
            cw = viewWidth;
        }
        else
        {
            for (i = 0; i < cnt; i++)
            {
                child = getChildAt(i);
                if (foldInvisibleItems && !child->isVisible())
                    continue;

                if (curX != 0)
                    curX += _columnGap;

                if ((_columnCount != 0 && j >= _columnCount) || (_columnCount == 0 && curX + child->getWidth() > viewWidth && maxHeight != 0))
                {
                    //new line
                    curX = 0;
                    curY += ceil(maxHeight) + _lineGap;
                    maxHeight = 0;
                    j = 0;
                }
                child->setPosition(curX, curY);
                curX += ceil(child->getWidth());
                if (curX > maxWidth)
                    maxWidth = curX;
                if (child->getHeight() > maxHeight)
                    maxHeight = child->getHeight();
                j++;
            }
            ch = curY + ceil(maxHeight);
            cw = ceil(maxWidth);
        }
    }
    else if (_layout == ListLayoutType::FLOW_VERTICAL)
    {
        if (_autoResizeItem && _lineCount > 0)
        {
            float lineSize = 0;
            int lineStart = 0;
            float ratio;

            for (i = 0; i < cnt; i++)
            {
                child = getChildAt(i);
                if (foldInvisibleItems && !child->isVisible())
                    continue;

                lineSize += child->sourceSize.height;
                j++;
                if (j == _lineCount || i == cnt - 1)
                {
                    ratio = (viewHeight - lineSize - (j - 1) * _lineGap) / lineSize;
                    curY = 0;
                    for (j = lineStart; j <= i; j++)
                    {
                        child = getChildAt(j);
                        if (foldInvisibleItems && !child->isVisible())
                            continue;

                        child->setPosition(curX, curY);

                        if (j < i)
                        {
                            child->setSize(child->getWidth(), child->sourceSize.height + round(child->sourceSize.height * ratio), true);
                            curY += ceil(child->getHeight()) + _lineGap;
                        }
                        else
                        {
                            child->setSize(child->getWidth(), viewHeight - curY, true);
                        }
                        if (child->getWidth() > maxWidth)
                            maxWidth = child->getWidth();
                    }
                    //new line
                    curX += ceil(maxWidth) + _columnGap;
                    maxWidth = 0;
                    j = 0;
                    lineStart = i + 1;
                    lineSize = 0;
                }
            }
            cw = curX + ceil(maxWidth);
            ch = viewHeight;
        }
        else
        {
            for (i = 0; i < cnt; i++)
            {
                child = getChildAt(i);
                if (foldInvisibleItems && !child->isVisible())
                    continue;

                if (curY != 0)
                    curY += _lineGap;

                if ((_lineCount != 0 && j >= _lineCount) || (_lineCount == 0 && curY + child->getHeight() > viewHeight && maxWidth != 0))
                {
                    curY = 0;
                    curX += ceil(maxWidth) + _columnGap;
                    maxWidth = 0;
                    j = 0;
                }
                child->setPosition(curX, curY);
                curY += child->getHeight();
                if (curY > maxHeight)
                    maxHeight = curY;
                if (child->getWidth() > maxWidth)
                    maxWidth = child->getWidth();
                j++;
            }
            cw = curX + ceil(maxWidth);
            ch = ceil(maxHeight);
        }
    }
    else //pagination
    {
        int page = 0;
        int k = 0;
        float eachHeight = 0;
        if (_autoResizeItem && _lineCount > 0)
            eachHeight = floor((viewHeight - (_lineCount - 1) * _lineGap) / _lineCount);

        if (_autoResizeItem && _columnCount > 0)
        {
            float lineSize = 0;
            int lineStart = 0;
            float ratio;

            for (i = 0; i < cnt; i++)
            {
                child = getChildAt(i);
                if (foldInvisibleItems && !child->isVisible())
                    continue;

                if (j == 0 && ((_lineCount != 0 && k >= _lineCount) || (_lineCount == 0 && curY + (_lineCount > 0 ? eachHeight : child->getHeight()) > viewHeight)))
                {
                    //new page
                    page++;
                    curY = 0;
                    k = 0;
                }

                lineSize += child->sourceSize.width;
                j++;
                if (j == _columnCount || i == cnt - 1)
                {
                    ratio = (viewWidth - lineSize - (j - 1) * _columnGap) / lineSize;
                    curX = 0;
                    for (j = lineStart; j <= i; j++)
                    {
                        child = getChildAt(j);
                        if (foldInvisibleItems && !child->isVisible())
                            continue;

                        child->setPosition(page * viewWidth + curX, curY);

                        if (j < i)
                        {
                            child->setSize(child->sourceSize.width + round(child->sourceSize.width * ratio),
                                           _lineCount > 0 ? eachHeight : child->getHeight(), true);
                            curX += ceil(child->getWidth()) + _columnGap;
                        }
                        else
                        {
                            child->setSize(viewWidth - curX, _lineCount > 0 ? eachHeight : child->getHeight(), true);
                        }
                        if (child->getHeight() > maxHeight)
                            maxHeight = child->getHeight();
                    }
                    //new line
                    curY += ceil(maxHeight) + _lineGap;
                    maxHeight = 0;
                    j = 0;
                    lineStart = i + 1;
                    lineSize = 0;

                    k++;
                }
            }
        }
        else
        {
            for (i = 0; i < cnt; i++)
            {
                child = getChildAt(i);
                if (foldInvisibleItems && !child->isVisible())
                    continue;

                if (curX != 0)
                    curX += _columnGap;

                if (_autoResizeItem && _lineCount > 0)
                    child->setSize(child->getWidth(), eachHeight, true);

                if ((_columnCount != 0 && j >= _columnCount) || (_columnCount == 0 && curX + child->getWidth() > viewWidth && maxHeight != 0))
                {
                    curX = 0;
                    curY += maxHeight + _lineGap;
                    maxHeight = 0;
                    j = 0;
                    k++;

                    if ((_lineCount != 0 && k >= _lineCount) || (_lineCount == 0 && curY + child->getHeight() > viewHeight && maxWidth != 0)) //new page
                    {
                        page++;
                        curY = 0;
                        k = 0;
                    }
                }
                child->setPosition(page * viewWidth + curX, curY);
                curX += ceil(child->getWidth());
                if (curX > maxWidth)
                    maxWidth = curX;
                if (child->getHeight() > maxHeight)
                    maxHeight = child->getHeight();
                j++;
            }
        }
        ch = page > 0 ? viewHeight : (curY + ceil(maxHeight));
        cw = (page + 1) * viewWidth;
    }

    handleAlign(cw, ch);
    setBounds(0, 0, cw, ch);
}

void GList::setup_beforeAdd(ByteBuffer* buffer, int beginPos)
{
    GComponent::setup_beforeAdd(buffer, beginPos);

    buffer->seek(beginPos, 5);

    _layout = (ListLayoutType)buffer->readByte();
    _selectionMode = (ListSelectionMode)buffer->readByte();
    _align = (TextHAlignment)buffer->readByte();
    _verticalAlign = (TextVAlignment)buffer->readByte();
    _lineGap = buffer->readShort();
    _columnGap = buffer->readShort();
    _lineCount = buffer->readShort();
    _columnCount = buffer->readShort();
    _autoResizeItem = buffer->readBool();
    _childrenRenderOrder = (ChildrenRenderOrder)buffer->readByte();
    _apexIndex = buffer->readShort();

    if (buffer->readBool())
    {
        _margin.top = buffer->readInt();
        _margin.bottom = buffer->readInt();
        _margin.left = buffer->readInt();
        _margin.right = buffer->readInt();
    }

    OverflowType overflow = (OverflowType)buffer->readByte();
    if (overflow == OverflowType::SCROLL)
    {
        int savedPos = buffer->getPos();
        buffer->seek(beginPos, 7);
        setupScroll(buffer);
        buffer->setPos(savedPos);
    }
    else
        setupOverflow(overflow);

    if (buffer->readBool()) //clipSoftness
        buffer->skip(8);

    if (buffer->version >= 2)
    {
        scrollItemToViewOnClick = buffer->readBool();
        foldInvisibleItems = buffer->readBool();
    }

    buffer->seek(beginPos, 8);

    _defaultItem = buffer->readS();
    readItems(buffer);
}

void GList::readItems(ByteBuffer* buffer)
{
    const string* str;

    int itemCount = buffer->readShort();
    for (int i = 0; i < itemCount; i++)
    {
        int nextPos = buffer->readUshort();
        nextPos += buffer->getPos();

        str = buffer->readSP();
        if (!str || (*str).empty())
        {
            str = &_defaultItem;
            if ((*str).empty())
            {
                buffer->setPos(nextPos);
                continue;
            }
        }

        GObject* obj = getFromPool(*str);
        if (obj != nullptr)
        {
            addChild(obj);
            setupItem(buffer, obj);
        }

        buffer->setPos(nextPos);
    }
}

void GList::setupItem(ByteBuffer* buffer, GObject* obj)
{
    const string* str;
    GButton* btn = dynamic_cast<GButton*>(obj);

    if ((str = buffer->readSP()))
        obj->setText(*str);
    if ((str = buffer->readSP()) && btn)
        btn->setSelectedTitle(*str);
    if ((str = buffer->readSP()))
        obj->setIcon(*str);
    if ((str = buffer->readSP()) && btn)
        btn->setSelectedIcon(*str);
    if ((str = buffer->readSP()))
        obj->name = *str;

    GComponent* gcom = dynamic_cast<GComponent*>(obj);
    if (gcom != nullptr)
    {
        int cnt = buffer->readShort();
        for (int i = 0; i < cnt; i++)
        {
            GController* cc = gcom->getController(buffer->readS());
            const std::string& pageId = buffer->readS();
            cc->setSelectedPageId(pageId);
        }

        if (buffer->version >= 2)
        {
            cnt = buffer->readShort();
            for (int i = 0; i < cnt; i++)
            {
                std::string target = buffer->readS();
                ObjectPropID propId = (ObjectPropID)buffer->readShort();
                std::string value = buffer->readS();
                GObject* obj2 = gcom->getChildByPath(target);
                if (obj2 != nullptr)
                    obj2->setProp(propId, Value(value));
            }
        }
    }
}

void GList::setup_afterAdd(ByteBuffer* buffer, int beginPos)
{
    GComponent::setup_afterAdd(buffer, beginPos);

    buffer->seek(beginPos, 6);

    int i = buffer->readShort();
    if (i != -1)
        _selectionController = _parent->getControllerAt(i);
}

NS_FGUI_END