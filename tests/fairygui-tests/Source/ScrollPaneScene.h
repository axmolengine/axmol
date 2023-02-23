#ifndef __SCROLLPANE_SCENE_H__
#define __SCROLLPANE_SCENE_H__

#include "cocos2d.h"
#include "DemoScene.h"

USING_NS_FGUI;

class ScrollPaneScene : public DemoScene
{
public:
    // implement the "static create()" method manually
    CREATE_FUNC(ScrollPaneScene);

protected:
    virtual void continueInit() override;

private:
    void renderListItem(int index, GObject* obj);
    void onClickStick(EventContext* context);
    void onClickDelete(EventContext* context);
    void onClickList(EventContext* context);

    GComponent* _view;
    GList* _list;
};

#endif
