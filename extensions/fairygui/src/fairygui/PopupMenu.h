#ifndef __POPUPMENU_H__
#define __POPUPMENU_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "event/UIEventDispatcher.h"

NS_FGUI_BEGIN

class GObject;
class GComponent;
class GButton;
class GList;

class PopupMenu : public ax::Object
{
public:
    static PopupMenu* create(const std::string& resourceURL);
    static PopupMenu* create() { return create(""); }

    PopupMenu();
    virtual ~PopupMenu();

    GButton* addItem(const std::string& caption, EventCallback callback);
    GButton* addItemAt(const std::string& caption, int index, EventCallback callback);
    void addSeperator();
    const std::string& getItemName(int index) const;
    void setItemText(const std::string& name, const std::string& caption);
    void setItemVisible(const std::string& name, bool visible);
    void setItemGrayed(const std::string& name, bool grayed);
    void setItemCheckable(const std::string& name, bool checkable);
    void setItemChecked(const std::string& name, bool check);
    bool isItemChecked(const std::string& name) const;
    bool removeItem(const std::string& name);
    void clearItems();
    int getItemCount() const;
    GComponent* getContentPane() const { return _contentPane; }
    GList* getList() const { return _list; }
    void show() { show(nullptr, PopupDirection::AUTO); }
    void show(GObject* target, PopupDirection dir);

protected:
    bool init(const std::string& resourceURL);

    GComponent* _contentPane;
    GList* _list;

private:
    void onClickItem(EventContext* context);
    void onEnter(EventContext* context);
};

NS_FGUI_END

#endif
