#ifndef __DEMO_SCENE_H__
#define __DEMO_SCENE_H__

#include "axmol/cocos2d.h"
#include "FairyGUI.h"

USING_NS_FGUI;

class DemoScene : public ax::Scene
{
public:
    bool init();

    DemoScene();
    virtual ~DemoScene();

protected:
    virtual void continueInit();

    GRoot* _groot;

private:
    void onClose(EventContext* context);
};

#endif
