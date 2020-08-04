#ifndef __PULLTOREFRESH_SCENE_H__
#define __PULLTOREFRESH_SCENE_H__

#include "cocos2d.h"
#include "DemoScene.h"

USING_NS_FGUI;

class PullToRefreshScene : public DemoScene
{
public:
    // implement the "static create()" method manually
    CREATE_FUNC(PullToRefreshScene);

protected:
    virtual void continueInit() override;

private:
    void onPullDownToRefresh(EventContext*);
    void onPullUpToRefresh(EventContext*);
    void renderListItem1(int index, GObject* obj);
    void renderListItem2(int index, GObject* obj);

    GComponent* _view;
    GList* _list1;
    GList* _list2;

};

#endif
