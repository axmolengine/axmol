#ifndef __VIRTUALLIST_SCENE_H__
#define __VIRTUALLIST_SCENE_H__

#include "cocos2d.h"
#include "DemoScene.h"

USING_NS_FGUI;

class VirtualListScene : public DemoScene
{
public:
    // implement the "static create()" method manually
    CREATE_FUNC(VirtualListScene);

protected:
    virtual void continueInit() override;

private:
    void renderListItem(int index, GObject* obj);

    GComponent* _view;
    GList* _list;
};

#endif
