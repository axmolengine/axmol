#include "GTweener.h"
#include "EaseManager.h"
#include "GObject.h"
#include "GPath.h"

NS_FGUI_BEGIN
using namespace ax;

GTweener::GTweener() : _target(nullptr),
                       _refTarget(nullptr),
                       _userData(nullptr),
                       _onStart(nullptr),
                       _onUpdate(nullptr),
                       _onComplete(nullptr),
                       _onComplete0(nullptr),
                       _path(nullptr)
{
}

GTweener::~GTweener()
{
}

GTweener* GTweener::setDelay(float value)
{
    _delay = value;
    return this;
}

GTweener* GTweener::setDuration(float value)
{
    _duration = value;
    return this;
}

GTweener* GTweener::setBreakpoint(float value)
{
    _breakpoint = value;
    return this;
}

GTweener* GTweener::setEase(EaseType value)
{
    _easeType = value;
    return this;
}

GTweener* GTweener::setEasePeriod(float value)
{
    _easePeriod = value;
    return this;
}

GTweener* GTweener::setEaseOvershootOrAmplitude(float value)
{
    _easeOvershootOrAmplitude = value;
    return this;
}

GTweener* GTweener::setRepeat(int repeat, bool yoyo)
{
    _repeat = repeat;
    _yoyo = yoyo;
    return this;
}

GTweener* GTweener::setTimeScale(float value)
{
    _timeScale = value;
    return this;
}

GTweener* GTweener::setSnapping(bool value)
{
    _snapping = value;
    return this;
}

GTweener* GTweener::setTargetAny(void* value)
{
    AX_SAFE_RELEASE(_refTarget);
    _refTarget = nullptr;
    _target = value;
    return this;
}

GTweener* GTweener::setTarget(ax::Object* target, TweenPropType propType/* = TweenPropType::None*/)
{
    AX_SAFE_RELEASE(_refTarget);
    _target = _refTarget = target;
    _propType = propType;
    AX_SAFE_RETAIN(_refTarget);
    return this;
}

GTweener* GTweener::setUserData(const Value& value)
{
    _userData = value;
    return this;
}

GTweener* GTweener::setPath(GPath* path)
{
    _path = path;
    return this;
}

GTweener* GTweener::onUpdate(GTweenCallback callback)
{
    _onUpdate = callback;
    return this;
}

GTweener* GTweener::onStart(GTweenCallback callback)
{
    _onStart = callback;
    return this;
}

GTweener* GTweener::onComplete(GTweenCallback0 callback)
{
    _onComplete0 = callback;
    return this;
}

GTweener* GTweener::onComplete1(GTweenCallback callback)
{
    _onComplete = callback;
    return this;
}

GTweener* GTweener::setPaused(bool paused)
{
    _paused = paused;
    return this;
}

void GTweener::seek(float time)
{
    if (_killed)
        return;

    _elapsedTime = time;
    if (_elapsedTime < _delay)
    {
        if (_started)
            _elapsedTime = _delay;
        else
            return;
    }

    update();
}

void GTweener::kill(bool complete)
{
    if (_killed)
        return;

    if (complete)
    {
        if (_ended == 0)
        {
            if (_breakpoint >= 0)
                _elapsedTime = _delay + _breakpoint;
            else if (_repeat >= 0)
                _elapsedTime = _delay + _duration * (_repeat + 1);
            else
                _elapsedTime = _delay + _duration * 2;
            update();
        }

        callCompleteCallback();
    }

    _killed = true;
}

GTweener* GTweener::_to(float start, float end, float duration)
{
    _valueSize = 1;
    startValue.x = start;
    endValue.x = end;
    value.x = start;
    _duration = duration;
    return this;
}

GTweener* GTweener::_to(const ax::Vec2& start, const ax::Vec2& end, float duration)
{
    _valueSize = 2;
    startValue.setVec2(start);
    endValue.setVec2(end);
    value.setVec2(start);
    _duration = duration;
    return this;
}

GTweener* GTweener::_to(const ax::Vec3& start, const ax::Vec3& end, float duration)
{
    _valueSize = 3;
    startValue.setVec3(start);
    endValue.setVec3(end);
    value.setVec3(start);
    _duration = duration;
    return this;
}

GTweener* GTweener::_to(const ax::Vec4& start, const ax::Vec4& end, float duration)
{
    _valueSize = 4;
    startValue.setVec4(start);
    endValue.setVec4(end);
    value.setVec4(start);
    _duration = duration;
    return this;
}

GTweener* GTweener::_to(const ax::Color4B& start, const ax::Color4B& end, float duration)
{
    _valueSize = 4;
    startValue.setColor(start);
    endValue.setColor(end);
    value.setColor(start);
    _duration = duration;
    return this;
}

GTweener* GTweener::_to(double start, double end, float duration)
{
    _valueSize = 5;
    startValue.d = start;
    endValue.d = end;
    value.d = start;
    _duration = duration;
    return this;
}

GTweener* GTweener::_shake(const ax::Vec2& start, float amplitude, float duration)
{
    _valueSize = 6;
    startValue.setVec2(start);
    startValue.w = amplitude;
    _duration = duration;
    _easeType = EaseType::Linear;
    return this;
}

void GTweener::_init()
{
    _delay = 0;
    _duration = 0;
    _breakpoint = -1;
    _easeType = EaseType::QuadOut;
    _timeScale = 1;
    _easePeriod = 0;
    _easeOvershootOrAmplitude = 1.70158f;
    _snapping = false;
    _repeat = 0;
    _yoyo = false;
    _valueSize = 0;
    _started = false;
    _paused = false;
    _killed = false;
    _elapsedTime = 0;
    _normalizedTime = 0;
    _ended = 0;
    startValue.setZero();
    endValue.setZero();
    value.setZero();
    deltaValue.setZero();
}

void GTweener::_reset()
{
    AX_SAFE_RELEASE(_refTarget);
    _target = nullptr;
    _refTarget = nullptr;
    _userData = nullptr;
    _path = nullptr;
    _onStart = _onUpdate = _onComplete = nullptr;
    _onComplete0 = nullptr;
}

void GTweener::_update(float dt)
{
    if (_ended != 0) //Maybe completed by seek
    {
        callCompleteCallback();
        _killed = true;
        return;
    }

    if (_timeScale != 1)
        dt *= _timeScale;
    if (dt == 0)
        return;

    _elapsedTime += dt;
    update();

    if (_ended != 0)
    {
        if (!_killed)
        {
            callCompleteCallback();
            _killed = true;
        }
    }
}

void GTweener::update()
{
    _ended = 0;

    if (_valueSize == 0) //DelayedCall
    {
        if (_elapsedTime >= _delay + _duration)
            _ended = 1;

        return;
    }

    if (!_started)
    {
        if (_elapsedTime < _delay)
            return;

        _started = true;
        callStartCallback();
        if (_killed)
            return;
    }

    bool reversed = false;
    float tt = _elapsedTime - _delay;
    if (_breakpoint >= 0 && tt >= _breakpoint)
    {
        tt = _breakpoint;
        _ended = 2;
    }

    if (_repeat != 0)
    {
        int round = (int)floor(tt / _duration);
        tt -= _duration * round;
        if (_yoyo)
            reversed = round % 2 == 1;

        if (_repeat > 0 && _repeat - round < 0)
        {
            if (_yoyo)
                reversed = _repeat % 2 == 1;
            tt = _duration;
            _ended = 1;
        }
    }
    else if (tt >= _duration)
    {
        tt = _duration;
        _ended = 1;
    }

    _normalizedTime = EaseManager::evaluate(_easeType, reversed ? (_duration - tt) : tt, _duration,
                                            _easeOvershootOrAmplitude, _easePeriod);

    value.setZero();
    deltaValue.setZero();

    if (_valueSize == 5)
    {
        double d = startValue.d + (endValue.d - startValue.d) * _normalizedTime;
        if (_snapping)
            d = round(d);
        deltaValue.d = d - value.d;
        value.d = d;
        value.x = (float)d;
    }
    else if (_valueSize == 6)
    {
        if (_ended == 0)
        {
            float r = startValue.w * (1 - _normalizedTime);
            float rx = (rand_0_1() * 2 - 1) * r;
            float ry = (rand_0_1() * 2 - 1) * r;
            rx = rx > 0 ? ceil(rx) : floor(rx);
            ry = ry > 0 ? ceil(ry) : floor(ry);

            deltaValue.x = rx;
            deltaValue.y = ry;
            value.x = startValue.x + rx;
            value.y = startValue.y + ry;
        }
        else
            value.setVec3(startValue.getVec3());
    }
    else if (_path != nullptr)
    {
        Vec3 vec3 = _path->getPointAt(_normalizedTime);
        if (_snapping)
        {
            vec3.x = round(vec3.x);
            vec3.y = round(vec3.y);
            vec3.z = round(vec3.z);
        }
        deltaValue.setVec3(vec3 - value.getVec3());
        value.setVec3(vec3);
    }
    else
    {
        for (int i = 0; i < _valueSize; i++)
        {
            float n1 = startValue[i];
            float n2 = endValue[i];
            float f = n1 + (n2 - n1) * _normalizedTime;
            if (_snapping)
                f = round(f);
            deltaValue[i] = f - value[i];
            value[i] = f;
        }
        value.d = value.x;
    }

    if (_refTarget != nullptr && _propType != TweenPropType::None)
    {
        GObject* gobj = dynamic_cast<GObject*>(_refTarget);
        if (gobj != nullptr)
            TweenPropTypeUtils::setProps(gobj, _propType, value);
        else
        {
            Node* node = dynamic_cast<Node*>(_refTarget);
            if (node != nullptr)
                TweenPropTypeUtils::setProps(node, _propType, value);
        }
    }

    callUpdateCallback();
}

void GTweener::callStartCallback()
{
    if (_onStart != nullptr)
        _onStart(this);
}

void GTweener::callUpdateCallback()
{
    if (_onUpdate != nullptr)
        _onUpdate(this);
}

void GTweener::callCompleteCallback()
{
    if (_onComplete != nullptr)
        _onComplete(this);
    if (_onComplete0 != nullptr)
        _onComplete0();
}

NS_FGUI_END