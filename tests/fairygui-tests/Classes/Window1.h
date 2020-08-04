#ifndef __WINDOW1_H__
#define __WINDOW1_H__

#include "FairyGUI.h"

USING_NS_FGUI;

class Window1 : public Window
{
public:
    CREATE_FUNC(Window1);

protected:
    virtual void onInit() override;
    virtual void onShown() override;
};

#endif