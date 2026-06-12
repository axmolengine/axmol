/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "axmol/base/PointerEvent.h"

#include <utility>

namespace ax
{

PointerEvent::PointerEvent() : Event(Type::POINTER) {}

void PointerEvent::setPointerInfo(InputPhase phase, Vec2 point, const PointerInputState& state)
{
    _isStopped     = false;
    _currentTarget = nullptr;
    setCaptureBits(CAPTURE_NONE);
    _prevPoint = _point;

    _phase = phase;

    _pointerType    = state.type;
    _button         = state.button;
    _pointerId      = state.id;
    _pressedButtons = state.pressedButtons;

    _point    = point;
    _pressure = state.pressure;
    if (!_startPointCaptured)
    {
        _startPoint         = _point;
        _startPointCaptured = true;
        _prevPoint          = _point;
    }
}

Vec2 PointerEvent::getScreenLocation() const
{
    return _point;
}

Vec2 PointerEvent::getPreviousScreenLocation() const
{
    return _prevPoint;
}

Vec2 PointerEvent::getStartScreenLocation() const
{
    return _startPoint;
}

bool PointerEvent::isButtonPressed(int buttonIndex) const
{
    return buttonIndex >= 0 ? ((1u << buttonIndex) & _pressedButtons) != 0 : false;
}

bool PointerEvent::isPrimaryPressed() const
{
    return _phase != InputPhase::PointerUp &&
           ((_pointerType == PointerType::Touch && _primary) || isButtonPressed(InputButton::Primary));
}

Vec2 PointerEvent::getLocation() const
{
    return Director::getInstance()->screenToWorld(_point);
}

Vec2 PointerEvent::getPreviousLocation() const
{
    return Director::getInstance()->screenToWorld(_prevPoint);
}

Vec2 PointerEvent::getStartLocation() const
{
    return Director::getInstance()->screenToWorld(_startPoint);
}

Vec2 PointerEvent::getDelta() const
{
    return getLocation() - getPreviousLocation();
}

}  // namespace ax
