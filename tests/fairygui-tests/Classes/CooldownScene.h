#ifndef __COOLDOWN_SCENE_H__
#define __COOLDOWN_SCENE_H__

#include "cocos2d.h"
#include "DemoScene.h"

USING_NS_FGUI;

class CooldownScene : public DemoScene
{
public:
    // implement the "static create()" method manually
    CREATE_FUNC(CooldownScene);

protected:
    virtual void continueInit() override;

private:
    GComponent* _view;
};

#endif
