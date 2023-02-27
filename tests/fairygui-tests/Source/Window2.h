#ifndef __WINDOW2_H__
#define __WINDOW2_H__

#include "FairyGUI.h"

USING_NS_FGUI;

class Window2 : public Window
{
public:
    CREATE_FUNC(Window2);

protected:
    virtual void onInit() override;
    virtual void doShowAnimation() override;
    virtual void doHideAnimation() override;
    virtual void onShown() override;
    virtual void onHide() override;
};

#endif