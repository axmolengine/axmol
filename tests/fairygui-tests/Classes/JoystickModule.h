#ifndef __JOYSTICK_MODULE_H__
#define __JOYSTICK_MODULE_H__

#include "cocos2d.h"
#include "FairyGUI.h"

USING_NS_FGUI;

class JoystickModule : public UIEventDispatcher
{
public:
    static const int MOVE = 100;
    static const int END = 101;

    static JoystickModule* create(GComponent* mainView);

private:
    bool init(GComponent* mainView);

    void onTouchBegin(EventContext* context);
    void onTouchMove(EventContext* context);
    void onTouchEnd(EventContext* context);

    float _InitX;
    float _InitY;
    float _startStageX;
    float _startStageY;
    float _lastStageX;
    float _lastStageY;
    int _radius;

    GButton* _button;
    GObject* _touchArea;
    GObject* _thumb;
    GObject* _center;
    GTweener* _tweener;
    int touchId;
};

#endif
