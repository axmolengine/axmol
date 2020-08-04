#ifndef __GCOMBOBOX_H__
#define __GCOMBOBOX_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GComponent.h"
#include "GList.h"

NS_FGUI_BEGIN

class GTextField;

class GComboBox : public GComponent
{
public:
    GComboBox();
    virtual ~GComboBox();

    CREATE_FUNC(GComboBox);

    const std::string& getTitle() const;
    void setTitle(const std::string& value);

    virtual const std::string& getText() const override { return getTitle(); }
    virtual void setText(const std::string& value) override { setTitle(value); }

    const cocos2d::Color3B getTitleColor() const;
    void setTitleColor(const cocos2d::Color3B& value);

    int getTitleFontSize() const;
    void setTitleFontSize(int value);

    virtual const std::string& getIcon() const override;
    virtual void setIcon(const std::string& value) override;

    const std::string& getValue() const;
    void setValue(const std::string& value);

    int getSelectedIndex() const { return _selectedIndex; }
    void setSelectedIndex(int value);

    GController* getSelectionController() const { return _selectionController; }
    void setSelectionController(GController* value) { _selectionController = value; }

    std::vector<std::string>& getItems() { return _items; }
    std::vector<std::string>& getIcons() { return _icons; }
    std::vector<std::string>& getValues() { return _values; }
    
    GObject* getDropdown() const { return _dropdown; }

    void refresh();

    int visibleItemCount;
    PopupDirection popupDirection;

    GTextField* getTextField() const;

    virtual cocos2d::Value getProp(ObjectPropID propId) override;
    virtual void setProp(ObjectPropID propId, const cocos2d::Value& value) override;

protected:
    virtual void constructExtension(ByteBuffer* buffer) override;
    virtual void setup_afterAdd(ByteBuffer* buffer, int beginPos) override;
    virtual void handleControllerChanged(GController* c) override;
    virtual void handleGrayedChanged() override;

    void setState(const std::string& value);
    void setCurrentState();
    void updateSelectionController();
    void updateDropdownList();
    void showDropdown();
    void renderDropdownList();

    GComponent* _dropdown;
    GObject* _titleObject;
    GObject* _iconObject;
    GList* _list;
    GController* _selectionController;

    std::vector<std::string> _items;
    std::vector<std::string> _icons;
    std::vector<std::string> _values;

private:

    void onClickItem(EventContext* context);
    void onRollover(EventContext* context);
    void onRollout(EventContext* context);
    void onTouchBegin(EventContext* context);
    void onTouchEnd(EventContext* context);
    void onPopupWinClosed(EventContext* context);

    bool _itemsUpdated;
    int _selectedIndex;
    GController* _buttonController;
    bool _down;
    bool _over;
};

NS_FGUI_END

#endif
