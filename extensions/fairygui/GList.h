#ifndef __GLIST_H__
#define __GLIST_H__

#include "FairyGUIMacros.h"
#include "GComponent.h"
#include "GObjectPool.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GList : public GComponent
{
public:
    typedef std::function<void(int, GObject*)> ListItemRenderer;
    typedef std::function<std::string(int)> ListItemProvider;

    GList();
    virtual ~GList();

    CREATE_FUNC(GList);

    const std::string& getDefaultItem() const { return _defaultItem; }
    void setDefaultItem(const std::string& value);

    ListLayoutType getLayout() const { return _layout; }
    void setLayout(ListLayoutType value);

    int getLineCount() const { return _lineCount; }
    void setLineCount(int value);

    int getColumnCount() { return _columnCount; }
    void setColumnCount(int value);

    int getColumnGap() const { return _columnGap; }
    void setColumnGap(int value);

    int getLineGap() const { return _lineGap; }
    void setLineGap(int value);

    cocos2d::TextHAlignment getAlign() const { return _align; }
    void setAlign(cocos2d::TextHAlignment value);

    cocos2d::TextVAlignment getVerticalAlign() const { return _verticalAlign; }
    void setVerticalAlign(cocos2d::TextVAlignment value);

    bool getAutoResizeItem() const { return _autoResizeItem; }
    void setAutoResizeItem(bool value);

    ListSelectionMode getSelectionMode() const { return _selectionMode; }
    void setSelectionMode(ListSelectionMode value) { _selectionMode = value; }

    GObjectPool* getItemPool() const { return _pool; }
    GObject* getFromPool() { return getFromPool(cocos2d::STD_STRING_EMPTY); }
    GObject* getFromPool(const std::string& url);
    void returnToPool(GObject* obj);
    GObject* addItemFromPool() { return addItemFromPool(cocos2d::STD_STRING_EMPTY); }
    GObject* addItemFromPool(const std::string& url);

    GObject* addChildAt(GObject* child, int index) override;
    void removeChildAt(int index) override;
    void removeChildToPoolAt(int index);
    void removeChildToPool(GObject* child);
    void removeChildrenToPool();
    void removeChildrenToPool(int beginIndex, int endIndex);

    int getSelectedIndex() const;
    void setSelectedIndex(int value);

    void getSelection(std::vector<int>& result) const;
    void addSelection(int index, bool scrollItToView);
    void removeSelection(int index);
    void clearSelection();
    void selectAll();
    void selectReverse();

    void handleArrowKey(int dir);

    void resizeToFit(int itemCount) { resizeToFit(itemCount, 0); }
    void resizeToFit(int itemCount, int minSize);

    virtual int getFirstChildInView() override;

    void scrollToView(int index, bool ani = false, bool setFirst = false);

    GController* getSelectionController() const { return _selectionController; }
    void setSelectionController(GController* value);

    void setVirtual();
    void setVirtualAndLoop();
    bool isVirtual() { return _virtual; }
    void refreshVirtualList();

    int getNumItems();
    void setNumItems(int value);

    int childIndexToItemIndex(int index);
    int itemIndexToChildIndex(int index);

    virtual cocos2d::Vec2 getSnappingPosition(const cocos2d::Vec2& pt) override;

    ListItemRenderer itemRenderer;
    ListItemProvider itemProvider;
    bool scrollItemToViewOnClick;
    bool foldInvisibleItems;

protected:
    virtual void handleControllerChanged(GController* c) override;
    virtual void handleSizeChanged() override;
    virtual void updateBounds() override;
    virtual void setup_beforeAdd(ByteBuffer* buffer, int beginPos) override;
    virtual void setup_afterAdd(ByteBuffer* buffer, int beginPos) override;

    virtual void dispatchItemEvent(GObject* item, EventContext* context);
    virtual void readItems(ByteBuffer* buffer);
    virtual void setupItem(ByteBuffer* buffer, GObject* obj);

private:
    void clearSelectionExcept(GObject* g);
    void setSelectionOnEvent(GObject* item, InputEvent* evt);

    void onItemTouchBegin(EventContext* context);
    void onClickItem(EventContext* context);

    void updateSelectionController(int index);

    void setVirtual(bool loop);
    void checkVirtualList();
    void setVirtualListChangedFlag(bool layoutChanged);
    CALL_LATER_FUNC(GList, doRefreshVirtualList);

    void onScroll(EventContext* context);

    int getIndexOnPos1(float& pos, bool forceUpdate);
    int getIndexOnPos2(float& pos, bool forceUpdate);
    int getIndexOnPos3(float& pos, bool forceUpdate);

    void handleScroll(bool forceUpdate);
    bool handleScroll1(bool forceUpdate);
    bool handleScroll2(bool forceUpdate);
    void handleScroll3(bool forceUpdate);

    void handleArchOrder1();
    void handleArchOrder2();

    void handleAlign(float contentWidth, float contentHeight);

    ListLayoutType _layout;
    int _lineCount;
    int _columnCount;
    int _lineGap;
    int _columnGap;
    cocos2d::TextHAlignment _align;
    cocos2d::TextVAlignment _verticalAlign;
    bool _autoResizeItem;
    ListSelectionMode _selectionMode;
    std::string _defaultItem;
    GController* _selectionController;

    GObjectPool* _pool;
    bool _selectionHandled;
    int _lastSelectedIndex;

    //Virtual List support
    bool _virtual;
    bool _loop;
    int _numItems;
    int _realNumItems;
    int _firstIndex;        //the top left index
    int _curLineItemCount;  //item count in one line
    int _curLineItemCount2; //item count in vertical direction,only pagination layout
    cocos2d::Vec2 _itemSize;
    int _virtualListChanged; //1-content changed, 2-size changed
    bool _eventLocked;
    uint32_t _itemInfoVer;

    struct ItemInfo
    {
        cocos2d::Vec2 size;
        GObject* obj;
        uint32_t updateFlag;
        bool selected;

        ItemInfo();
    };
    std::vector<ItemInfo> _virtualItems;
};

NS_FGUI_END

#endif
