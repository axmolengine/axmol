#ifndef __JOYSTICK_SCENE_H__
#define __JOYSTICK_SCENE_H__

#include "cocos2d.h"
#include "DemoScene.h"
#include "JoystickModule.h"

USING_NS_FGUI;

class JoystickScene : public DemoScene
{
public:
    // implement the "static create()" method manually
    CREATE_FUNC(JoystickScene);

    ~JoystickScene();

protected:
    virtual void continueInit() override;

private:
    GComponent* _view;
    JoystickModule* _joystick;
};

#endif
