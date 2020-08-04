#ifndef __LOOPLIST_SCENE_H__
#define __LOOPLIST_SCENE_H__

#include "cocos2d.h"
#include "DemoScene.h"

USING_NS_FGUI;

class LoopListScene : public DemoScene
{
public:
    // implement the "static create()" method manually
    CREATE_FUNC(LoopListScene);

protected:
    virtual void continueInit() override;

private:
    void renderListItem(int index, GObject* obj);
    void doSpecialEffect(EventContext*);

    GComponent* _view;
    GList* _list;
};

#endif
