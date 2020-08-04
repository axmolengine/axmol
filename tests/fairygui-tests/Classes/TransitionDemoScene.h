#ifndef __TRANSITION_DEMO_SCENE_H__
#define __TRANSITION_DEMO_SCENE_H__

#include "cocos2d.h"
#include "DemoScene.h"

USING_NS_FGUI;

class TransitionDemoScene : public DemoScene
{
public:
    // implement the "static create()" method manually
    CREATE_FUNC(TransitionDemoScene);

    ~TransitionDemoScene();

protected:
    virtual void continueInit() override;

private:
    void playNum();
    void __play(GComponent* target);
    void __play4(EventContext* context);
    void __play5(EventContext* context);

    GComponent* _view;
    GGroup* _btnGroup;
    GComponent* _g1;
    GComponent* _g2;
    GComponent* _g3;
    GComponent* _g4;
    GComponent* _g5;
    GComponent* _g6;

    int _startValue;
    int _endValue;

};

#endif
