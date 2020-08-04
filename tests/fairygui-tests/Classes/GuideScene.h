#ifndef __GUIDE_SCENE_H__
#define __GUIDE_SCENE_H__

#include "cocos2d.h"
#include "DemoScene.h"

USING_NS_FGUI;

class GuideScene : public DemoScene
{
public:
    GuideScene();
    ~GuideScene();

    // implement the "static create()" method manually
    CREATE_FUNC(GuideScene);

protected:
    virtual void continueInit() override;

private:
    GComponent* _view;
    GComponent* _guideLayer;
};

#endif
