#include "InputEvent.h"
#include "GObject.h"

NS_FGUI_BEGIN

InputEvent::InputEvent() :
    _target(nullptr),
    _pointerEvent(nullptr),
    _inputProcessor(nullptr),
    _pointerId(-1),
    _clickCount(0),
    _mouseWheelDelta(0),
    _button(-1),
    _keyCode(ax::KeyboardEvent::KeyCode::KEY_0),
    _keyModifiers(0)
{
}

InputEvent::~InputEvent()
{

}


NS_FGUI_END
