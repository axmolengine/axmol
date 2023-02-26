#ifndef __HITEST_SCENE_H__
#define __HITEST_SCENE_H__

#include "cocos2d.h"
#include "DemoScene.h"

USING_NS_FGUI;

class HitTestScene : public DemoScene
{
public:
    // implement the "static create()" method manually
    CREATE_FUNC(HitTestScene);

protected:
    virtual void continueInit() override;

private:
    GComponent* _view;
};

#endif
