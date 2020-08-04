#include "GTween.h"
#include "TweenManager.h"

NS_FGUI_BEGIN
USING_NS_CC;

GTweener* GTween::to(float startValue, float endValue, float duration)
{
    return TweenManager::createTween()->_to(startValue, endValue, duration);
}

GTweener* GTween::to(const cocos2d::Vec2& startValue, const cocos2d::Vec2 & endValue, float duration)
{
    return TweenManager::createTween()->_to(startValue, endValue, duration);
}

GTweener* GTween::to(const cocos2d::Vec3& startValue, const cocos2d::Vec3 & endValue, float duration)
{
    return TweenManager::createTween()->_to(startValue, endValue, duration);
}

GTweener* GTween::to(const cocos2d::Vec4& startValue, const cocos2d::Vec4 & endValue, float duration)
{
    return TweenManager::createTween()->_to(startValue, endValue, duration);
}

GTweener* GTween::to(const cocos2d::Color4B& startValue, const cocos2d::Color4B & endValue, float duration)
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

GTweener* GTween::shake(const cocos2d::Vec2& startValue, float amplitude, float duration)
{
    return TweenManager::createTween()->_shake(startValue, amplitude, duration);
}

bool GTween::isTweening(cocos2d::Ref * target)
{
    return TweenManager::isTweening(target, TweenPropType::None);
}

bool GTween::isTweening(cocos2d::Ref * target, TweenPropType propType)
{
    return TweenManager::isTweening(target, propType);
}

void GTween::kill(cocos2d::Ref * target)
{
    TweenManager::killTweens(target, TweenPropType::None, false);
}

void GTween::kill(cocos2d::Ref * target, bool complete)
{
    TweenManager::killTweens(target, TweenPropType::None, complete);
}

void GTween::kill(cocos2d::Ref * target, TweenPropType propType, bool complete)
{
    TweenManager::killTweens(target, propType, complete);
}

GTweener* GTween::getTween(cocos2d::Ref * target)
{
    return TweenManager::getTween(target, TweenPropType::None);
}

GTweener* GTween::getTween(cocos2d::Ref * target, TweenPropType propType)
{
    return TweenManager::getTween(target, propType);
}

void GTween::clean()
{
    TweenManager::clean();
}

NS_FGUI_END