#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__

#include "cocos2d.h"
#include "DemoScene.h"

USING_NS_FGUI;

class MenuScene : public DemoScene
{
public:
    // implement the "static create()" method manually
    CREATE_FUNC(MenuScene);

protected:
    virtual void continueInit() override;

private:
    GComponent* _view;

};

#endif // __MENU_SCENE_H__
