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
    static GTweener* to(const ax::Vec2& startValue, const ax::Vec2& endValue, float duration);
    static GTweener* to(const ax::Vec3& startValue, const ax::Vec3& endValue, float duration);
    static GTweener* to(const ax::Vec4& startValue, const ax::Vec4& endValue, float duration);
    static GTweener* to(const ax::Color4B& startValue, const ax::Color4B& endValue, float duration);
    static GTweener* toDouble(double startValue, double endValue, float duration);
    static GTweener* delayedCall(float delay);
    static GTweener* shake(const ax::Vec2& startValue, float amplitude, float duration);
    static bool isTweening(ax::Ref* target);
    static bool isTweening(ax::Ref* target, TweenPropType propType);
    static void kill(ax::Ref* target);
    static void kill(ax::Ref* target, bool complete);
    static void kill(ax::Ref* target, TweenPropType propType, bool complete);
    static GTweener* getTween(ax::Ref* target);
    static GTweener* getTween(ax::Ref* target, TweenPropType propType);
    static void clean();
};

NS_FGUI_END

#endif
