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

    _point            = point;
    _pressure         = state.pressure;
    _previousRay      = _ray;
    _ray              = Ray{};
    _previousHitPoint = _hitResult.hit ? std::optional<Vec3>(_hitResult.worldPoint) : std::nullopt;
    _hitResult        = {};
    if (!_startPointCaptured)
    {
        _startPoint         = _point;
        _startPointCaptured = true;
        _prevPoint          = _point;
        _previousRay        = Ray{};
        _previousHitPoint.reset();
        _startHitPoint.reset();
    }
}

Vec2 PointerEvent::getPoint() const
{
    return _point;
}

Vec2 PointerEvent::getPrevPoint() const
{
    return _prevPoint;
}

Vec2 PointerEvent::getStartPoint() const
{
    return _startPoint;
}

Vec2 PointerEvent::getWorldPoint() const
{
    if (_hitResult.hit)
        return Vec2(_hitResult.worldPoint.x, _hitResult.worldPoint.y);

    if (_ray.direction.z == 0.0f)
        return Vec2();
    float t = -_ray.origin.z / _ray.direction.z;
    return Vec2(_ray.origin.x + t * _ray.direction.x, _ray.origin.y + t * _ray.direction.y);
}

Vec2 PointerEvent::getPrevWorldPoint() const
{
    if (_previousHitPoint.has_value())
        return Vec2(_previousHitPoint->x, _previousHitPoint->y);

    if (_previousRay.direction.z == 0.0f)
        return Vec2();
    float t = -_previousRay.origin.z / _previousRay.direction.z;
    return Vec2(_previousRay.origin.x + t * _previousRay.direction.x,
                _previousRay.origin.y + t * _previousRay.direction.y);
}

Vec2 PointerEvent::getStartWorldPoint() const
{
    if (_startHitPoint.has_value())
        return Vec2(_startHitPoint->x, _startHitPoint->y);

    // startHitPoint is set by setHitResult during PointerDown dispatch
    return getWorldPoint();
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

void PointerEvent::setHitResult(const Vec3& worldPoint, const Camera* camera, const Node* target)
{
    _hitResult.hit        = true;
    _hitResult.worldPoint = worldPoint;
    _hitResult.camera     = camera;
    _hitResult.target     = target;

    if (!_startHitPoint.has_value())
        _startHitPoint = worldPoint;
}

void PointerEvent::clearHitResult()
{
    _hitResult = {};
}

}  // namespace ax
