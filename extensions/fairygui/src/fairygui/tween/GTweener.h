#ifndef __GTWEENER_H__
#define __GTWEENER_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "EaseType.h"
#include "TweenValue.h"
#include "TweenPropType.h"

NS_FGUI_BEGIN

class GPath;

class GTweener : public ax::Object
{
public:
    typedef std::function<void(GTweener* tweener)> GTweenCallback;
    typedef std::function<void()> GTweenCallback0;

    GTweener();
    ~GTweener();
    GTweener* setDelay(float value);
    float getDelay() const { return _delay; }
    GTweener* setDuration(float value);
    float getDuration() const { return _duration; }
    GTweener* setBreakpoint(float value);
    GTweener* setEase(EaseType value);
    GTweener* setEasePeriod(float value);
    GTweener* setEaseOvershootOrAmplitude(float value);
    GTweener* setRepeat(int repeat, bool yoyo = false);
    int getRepeat() const { return _repeat; }
    GTweener* setTimeScale(float value);
    GTweener* setSnapping(bool value);
    GTweener* setTargetAny(void* value);
    GTweener* setTarget(ax::Object* target, TweenPropType propType = TweenPropType::None);
    void* getTarget() const { return _target; }
    GTweener* setUserData(const ax::Value& value);
    GTweener* setPath(GPath* path);
    const ax::Value& getUserData() const { return _userData; }
    GTweener* onUpdate(GTweenCallback callback);
    GTweener* onStart(GTweenCallback callback);
    GTweener* onComplete(GTweenCallback0 callback);
    GTweener* onComplete1(GTweenCallback callback);

    float getNormalizedTime() const { return _normalizedTime; }
    bool isCompleted() const { return _ended != 0; }
    bool allCompleted() const { return _ended == 1; }
    GTweener* setPaused(bool paused);
    void seek(float time);
    void kill(bool complete = false);

    TweenValue startValue;
    TweenValue endValue;
    TweenValue value;
    TweenValue deltaValue;

private:
    GTweener* _to(float start, float end, float duration);
    GTweener* _to(const ax::Vec2& start, const ax::Vec2& end, float duration);
    GTweener* _to(const ax::Vec3& start, const ax::Vec3& end, float duration);
    GTweener* _to(const ax::Vec4& start, const ax::Vec4& end, float duration);
    GTweener* _to(const ax::Color4B& start, const ax::Color4B& end, float duration);
    GTweener* _to(double start, double end, float duration);
    GTweener* _shake(const ax::Vec2& start, float amplitude, float duration);
    void _init();
    void _reset();
    void _update(float dt);
    void update();
    void callStartCallback();
    void callUpdateCallback();
    void callCompleteCallback();

private:
    void* _target;
    ax::Object* _refTarget;
    TweenPropType _propType;
    bool _killed;
    bool _paused;

    float _delay;
    float _duration;
    float _breakpoint;
    EaseType _easeType;
    float _easeOvershootOrAmplitude;
    float _easePeriod;
    int _repeat;
    bool _yoyo;
    float _timeScale;
    bool _snapping;
    ax::Value _userData;
    int _valueSize;
    GPath* _path;

    GTweenCallback _onUpdate;
    GTweenCallback _onStart;
    GTweenCallback _onComplete;
    GTweenCallback0 _onComplete0;

    bool _started;
    int _ended;
    float _elapsedTime;
    float _normalizedTime;

    friend class GTween;
    friend class TweenManager;
};

NS_FGUI_END

#endif
