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
    const int getX() const { return static_cast<int>(_pos.x); }
    const int getY() const {return static_cast<int>(_pos.y); }
    const ax::Vec2& getPosition() const { return _pos; }
    ax::Touch* getTouch()const { return _touch; }
    int getTouchId()const { return _touchId; }
    int isDoubleClick()const { return _clickCount == 2; }
    ax::EventMouse::MouseButton getButton() const { return _button; }
    ax::EventKeyboard::KeyCode getKeyCode() const { return _keyCode; }
    bool isCtrlDown() const { return (_keyModifiers & 1)!=0; }
    bool isAltDown() const  { return (_keyModifiers & 2) != 0; }
    bool isShiftDown() const { return (_keyModifiers & 4) != 0; }
    int getMouseWheelDelta() const { return _mouseWheelDelta; }

    InputProcessor* getProcessor() const { return _inputProcessor; }

private:
    GObject* _target;
    ax::Touch* _touch;
    ax::Vec2 _pos;
    int _touchId;
    int _clickCount;
    int _mouseWheelDelta;
    ax::EventMouse::MouseButton _button;
    ax::EventKeyboard::KeyCode _keyCode;
    uint16_t _keyModifiers;
    InputProcessor* _inputProcessor;

    friend class InputProcessor;
};

NS_FGUI_END

#endif
