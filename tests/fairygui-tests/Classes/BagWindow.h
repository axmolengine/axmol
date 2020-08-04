#ifndef __BAG_WINDOW_H__
#define __BAG_WINDOW_H__

#include "FairyGUI.h"

USING_NS_FGUI;

class BagWindow : public Window
{
public:
    CREATE_FUNC(BagWindow);

protected:
    virtual void onInit() override;
    virtual void doShowAnimation() override;
    virtual void doHideAnimation() override;

private:
    void onClickItem(EventContext* context);
    void renderListItem(int index, GObject* obj);

    GList* _list;
};

#endif