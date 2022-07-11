#ifndef __GTWEEN_H__
#define __GTWEEN_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "GTweener.h"
#include "EaseType.h"
#include "TweenValue.h"
#include "TweenPropType.h"

NS_FGUI_BEGIN

class GTween
{
public:
    static GTweener* to(float startValue, float endValue, float duration);
    static GTweener* to(const axis::Vec2& startValue, const axis::Vec2& endValue, float duration);
    static GTweener* to(const axis::Vec3& startValue, const axis::Vec3& endValue, float duration);
    static GTweener* to(const axis::Vec4& startValue, const axis::Vec4& endValue, float duration);
    static GTweener* to(const axis::Color4B& startValue, const axis::Color4B& endValue, float duration);
    static GTweener* toDouble(double startValue, double endValue, float duration);
    static GTweener* delayedCall(float delay);
    static GTweener* shake(const axis::Vec2& startValue, float amplitude, float duration);
    static bool isTweening(axis::Ref* target);
    static bool isTweening(axis::Ref* target, TweenPropType propType);
    static void kill(axis::Ref* target);
    static void kill(axis::Ref* target, bool complete);
    static void kill(axis::Ref* target, TweenPropType propType, bool complete);
    static GTweener* getTween(axis::Ref* target);
    static GTweener* getTween(axis::Ref* target, TweenPropType propType);
    static void clean();
};

NS_FGUI_END

#endif
