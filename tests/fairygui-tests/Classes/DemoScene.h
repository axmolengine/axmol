#ifndef __DEMO_SCENE_H__
#define __DEMO_SCENE_H__

#include "cocos2d.h"
#include "FairyGUI.h"

USING_NS_FGUI;

class DemoScene : public cocos2d::Scene
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
