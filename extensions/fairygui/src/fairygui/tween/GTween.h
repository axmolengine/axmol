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
    static GTweener* toFloat(float startValue, float endValue, float duration) { return to(startValue, endValue, duration); }
    static GTweener* toVec2(const ax::Vec2& startValue, const ax::Vec2& endValue, float duration) { return to(startValue, endValue, duration); }
    static GTweener* toVec3(const ax::Vec3& startValue, const ax::Vec3& endValue, float duration) { return to(startValue, endValue, duration); }
    static GTweener* toVec4(const ax::Vec4& startValue, const ax::Vec4& endValue, float duration) { return to(startValue, endValue, duration); }
    static GTweener* toColor4B(const ax::Color4B& startValue, const ax::Color4B& endValue, float duration) { return to(startValue, endValue, duration); }
    static GTweener* toDouble(double startValue, double endValue, float duration);
    static GTweener* delayedCall(float delay);
    static GTweener* shake(const ax::Vec2& startValue, float amplitude, float duration);
    static bool isTweening(ax::Object* target, TweenPropType propType = TweenPropType::None);
    static void kill(ax::Object* target, TweenPropType propType = TweenPropType::None, bool complete = false);
    static GTweener* getTween(ax::Object* target, TweenPropType propType = TweenPropType::None);
    static void clean();
};

NS_FGUI_END

#endif
