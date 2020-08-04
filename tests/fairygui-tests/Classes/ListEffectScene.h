#ifndef __LISTEFFECT_SCENE_H__
#define __LISTEFFECT_SCENE_H__

#include "cocos2d.h"
#include "DemoScene.h"

USING_NS_FGUI;

class ListEffectScene : public DemoScene
{
public:
    // implement the "static create()" method manually
    CREATE_FUNC(ListEffectScene);

protected:
    virtual void continueInit() override;

private:

    GComponent* _view;
    GList* _list;
};

#endif
