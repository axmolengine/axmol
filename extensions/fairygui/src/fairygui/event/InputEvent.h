#ifndef __INPUTEVENT_H__
#define __INPUTEVENT_H__

#include "FairyGUIMacros.h"
#include "axmol/cocos2d.h"

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
    intptr_t getPointerId() const { return _pointerId; }
    intptr_t getTouchId() const { return _pointerId; }
    int isDoubleClick()const { return _clickCount == 2; }
    int getButton() const { return _button; }
    ax::KeyboardEvent::KeyCode getKeyCode() const { return _keyCode; }
    bool isCtrlDown() const { return (_keyModifiers & 1)!=0; }
    bool isAltDown() const  { return (_keyModifiers & 2) != 0; }
    bool isShiftDown() const { return (_keyModifiers & 4) != 0; }
    int getMouseWheelDelta() const { return _mouseWheelDelta; }

    InputProcessor* getProcessor() const { return _inputProcessor; }

private:
    GObject* _target;
    ax::Vec2 _pos;
    intptr_t _pointerId{0};
    ax::PointerEvent* _pointerEvent{nullptr};
    int _clickCount;
    int _mouseWheelDelta;
    int _button{ax::InputButton::None};
    ax::KeyboardEvent::KeyCode _keyCode;
    uint16_t _keyModifiers;
    InputProcessor* _inputProcessor;

    friend class InputProcessor;
};

NS_FGUI_END

#endif
