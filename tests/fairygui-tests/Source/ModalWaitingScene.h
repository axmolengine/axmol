#ifndef __MODALWAITING_SCENE_H__
#define __MODALWAITING_SCENE_H__

#include "cocos2d.h"
#include "DemoScene.h"

USING_NS_FGUI;

class ModalWaitingScene : public DemoScene
{
public:
    ModalWaitingScene();
    ~ModalWaitingScene();

    // implement the "static create()" method manually
    CREATE_FUNC(ModalWaitingScene);

protected:
    virtual void continueInit() override;

private:
    GComponent* _view;
    Window* _testWin;
};

#endif
