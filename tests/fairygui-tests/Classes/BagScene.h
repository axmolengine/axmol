#ifndef __BAG_SCENE_H__
#define __BAG_SCENE_H__

#include "cocos2d.h"
#include "DemoScene.h"
#include "BagWindow.h"

USING_NS_FGUI;

class BagScene : public DemoScene
{
public:
    BagScene();
    ~BagScene();

    // implement the "static create()" method manually
    CREATE_FUNC(BagScene);

protected:
    virtual void continueInit() override;

private:
    GComponent* _view;
    BagWindow* _bagWindow;
};

#endif
