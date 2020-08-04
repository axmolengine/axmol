#ifndef __GCONTROLLER_H__
#define __GCONTROLLER_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "event/UIEventDispatcher.h"

NS_FGUI_BEGIN

class GComponent;
class ControllerAction;
class ByteBuffer;

class GController : public UIEventDispatcher
{
public:
    GController();
    virtual ~GController();

    GComponent* getParent() const { return _parent; }
    void setParent(GComponent* value) { _parent = value; }

    int getSelectedIndex() const { return _selectedIndex; }
    void setSelectedIndex(int value, bool triggerEvent = true);

    const std::string& getSelectedPage() const;
    void setSelectedPage(const std::string& value, bool triggerEvent = true);

    const std::string& getSelectedPageId() const;
    void setSelectedPageId(const std::string& value, bool triggerEvent = true);

    int getPrevisousIndex() const { return _previousIndex; }
    const std::string& getPreviousPage() const;
    const std::string& getPreviousPageId() const;

    int getPageCount() const;
    bool hasPage(const std::string& aName) const;
    int getPageIndexById(const std::string& value) const;
    const std::string& getPageNameById(const std::string& value) const;
    const std::string& getPageId(int index) const;
    void setOppositePageId(const std::string& value);
    void runActions();

    void setup(ByteBuffer* buffer);

    std::string name;
    bool changing;
    bool autoRadioGroupDepth;

private:
    GComponent* _parent;
    int _selectedIndex;
    int _previousIndex;
    std::vector<std::string> _pageIds;
    std::vector<std::string> _pageNames;
    std::vector<ControllerAction*> _actions;
};

NS_FGUI_END

#endif
