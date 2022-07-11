#include "GTween.h"
#include "TweenManager.h"

NS_FGUI_BEGIN
USING_NS_AX;

GTweener* GTween::to(float startValue, float endValue, float duration)
{
    return TweenManager::createTween()->_to(startValue, endValue, duration);
}

GTweener* GTween::to(const axis::Vec2& startValue, const axis::Vec2 & endValue, float duration)
{
    return TweenManager::createTween()->_to(startValue, endValue, duration);
}

GTweener* GTween::to(const axis::Vec3& startValue, const axis::Vec3 & endValue, float duration)
{
    return TweenManager::createTween()->_to(startValue, endValue, duration);
}

GTweener* GTween::to(const axis::Vec4& startValue, const axis::Vec4 & endValue, float duration)
{
    return TweenManager::createTween()->_to(startValue, endValue, duration);
}

GTweener* GTween::to(const axis::Color4B& startValue, const axis::Color4B & endValue, float duration)
{
    return TweenManager::createTween()->_to(startValue, endValue, duration);
}

GTweener* GTween::toDouble(double startValue, double endValue, float duration)
{
    return TweenManager::createTween()->_to(startValue, endValue, duration);
}

GTweener* GTween::delayedCall(float delay)
{
    return TweenManager::createTween()->setDelay(delay);
}

GTweener* GTween::shake(const axis::Vec2& startValue, float amplitude, float duration)
{
    return TweenManager::createTween()->_shake(startValue, amplitude, duration);
}

bool GTween::isTweening(axis::Ref * target)
{
    return TweenManager::isTweening(target, TweenPropType::None);
}

bool GTween::isTweening(axis::Ref * target, TweenPropType propType)
{
    return TweenManager::isTweening(target, propType);
}

void GTween::kill(axis::Ref * target)
{
    TweenManager::killTweens(target, TweenPropType::None, false);
}

void GTween::kill(axis::Ref * target, bool complete)
{
    TweenManager::killTweens(target, TweenPropType::None, complete);
}

void GTween::kill(axis::Ref * target, TweenPropType propType, bool complete)
{
    TweenManager::killTweens(target, propType, complete);
}

GTweener* GTween::getTween(axis::Ref * target)
{
    return TweenManager::getTween(target, TweenPropType::None);
}

GTweener* GTween::getTween(axis::Ref * target, TweenPropType propType)
{
    return TweenManager::getTween(target, propType);
}

void GTween::clean()
{
    TweenManager::clean();
}

NS_FGUI_END