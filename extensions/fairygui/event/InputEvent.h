#ifndef __INPUTEVENT_H__
#define __INPUTEVENT_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GObject;
class InputProcessor;

class InputEvent
{
public:
    InputEvent();
    ~InputEvent();

    GObject* getTarget() const { return _target; }
    const int getX() const { return _pos.x; }
    const int getY() const { return _pos.y; }
    const cocos2d::Vec2& getPosition() const { return _pos; }
    cocos2d::Touch* getTouch()const { return _touch; }
    int getTouchId()const { return _touchId; }
    int isDoubleClick()const { return _clickCount == 2; }
    cocos2d::EventMouse::MouseButton getButton() const { return _button; }
    cocos2d::EventKeyboard::KeyCode getKeyCode() const { return _keyCode; }
    bool isCtrlDown() const { return (_keyModifiers & 1)!=0; }
    bool isAltDown() const  { return (_keyModifiers & 2) != 0; }
    bool isShiftDown() const { return (_keyModifiers & 4) != 0; }
    int getMouseWheelDelta() const { return _mouseWheelDelta; }

    InputProcessor* getProcessor() const { return _inputProcessor; }

private:
    GObject* _target;
    cocos2d::Touch* _touch;
    cocos2d::Vec2 _pos;
    int _touchId;
    int _clickCount;
    int _mouseWheelDelta;
    cocos2d::EventMouse::MouseButton _button;
    cocos2d::EventKeyboard::KeyCode _keyCode;
    uint16_t _keyModifiers;
    InputProcessor* _inputProcessor;

    friend class InputProcessor;
};

NS_FGUI_END

#endif
