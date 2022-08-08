#include "GTween.h"
#include "TweenManager.h"

NS_FGUI_BEGIN
USING_NS_AX;

GTweener* GTween::to(float startValue, float endValue, float duration)
{
    return TweenManager::createTween()->_to(startValue, endValue, duration);
}

GTweener* GTween::to(const ax::Vec2& startValue, const ax::Vec2 & endValue, float duration)
{
    return TweenManager::createTween()->_to(startValue, endValue, duration);
}

GTweener* GTween::to(const ax::Vec3& startValue, const ax::Vec3 & endValue, float duration)
{
    return TweenManager::createTween()->_to(startValue, endValue, duration);
}

GTweener* GTween::to(const ax::Vec4& startValue, const ax::Vec4 & endValue, float duration)
{
    return TweenManager::createTween()->_to(startValue, endValue, duration);
}

GTweener* GTween::to(const ax::Color4B& startValue, const ax::Color4B & endValue, float duration)
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

GTweener* GTween::shake(const ax::Vec2& startValue, float amplitude, float duration)
{
    return TweenManager::createTween()->_shake(startValue, amplitude, duration);
}

bool GTween::isTweening(ax::Ref * target)
{
    return TweenManager::isTweening(target, TweenPropType::None);
}

bool GTween::isTweening(ax::Ref * target, TweenPropType propType)
{
    return TweenManager::isTweening(target, propType);
}

void GTween::kill(ax::Ref * target)
{
    TweenManager::killTweens(target, TweenPropType::None, false);
}

void GTween::kill(ax::Ref * target, bool complete)
{
    TweenManager::killTweens(target, TweenPropType::None, complete);
}

void GTween::kill(ax::Ref * target, TweenPropType propType, bool complete)
{
    TweenManager::killTweens(target, propType, complete);
}

GTweener* GTween::getTween(ax::Ref * target)
{
    return TweenManager::getTween(target, TweenPropType::None);
}

GTweener* GTween::getTween(ax::Ref * target, TweenPropType propType)
{
    return TweenManager::getTween(target, propType);
}

void GTween::clean()
{
    TweenManager::clean();
}

NS_FGUI_END