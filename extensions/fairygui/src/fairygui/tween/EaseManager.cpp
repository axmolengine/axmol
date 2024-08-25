#include "EaseManager.h"
#include "cocos2d.h"

NS_FGUI_BEGIN
using namespace ax;

static const float _PiOver2 = (float)(M_PI * 0.5f);
static const float _TwoPi = (float)(M_PI * 2);

class Bounce
{
public:
    static float easeIn(float time, float duration);
    static float easeOut(float time, float duration);
    static float easeInOut(float time, float duration);
};

float EaseManager::evaluate(EaseType easeType, float time, float duration, float overshootOrAmplitude, float period)
{
    switch (easeType)
    {
    case EaseType::Linear:
        return time / duration;
    case EaseType::SineIn:
        return -(float)cos(time / duration * _PiOver2) + 1;
    case EaseType::SineOut:
        return (float)sin(time / duration * _PiOver2);
    case EaseType::SineInOut:
        return -0.5f * ((float)cos(M_PI * time / duration) - 1);
    case EaseType::QuadIn:
        time /= duration;
        return time * time;
    case EaseType::QuadOut:
        time /= duration;
        return -time * (time - 2);
    case EaseType::QuadInOut:
        time /= duration * 0.5f;
        if (time < 1) return 0.5f * time * time;
        --time;
        return -0.5f * (time * (time - 2) - 1);
    case EaseType::CubicIn:
        time /= duration;
        return time * time * time;
    case EaseType::CubicOut:
        time = time / duration - 1;
        return time * time * time + 1;
    case EaseType::CubicInOut:
        time /= duration * 0.5f;
        if (time < 1) return 0.5f * time * time * time;
        time -= 2;
        return 0.5f * (time * time * time + 2);
    case EaseType::QuartIn:
        time /= duration;
        return time * time * time * time;
    case EaseType::QuartOut:
        time = time / duration - 1;
        return -(time * time * time * time - 1);
    case EaseType::QuartInOut:
        time /= duration * 0.5f;
        if (time < 1) return 0.5f * time * time * time * time;
        time -= 2;
        return -0.5f * (time * time * time * time - 2);
    case EaseType::QuintIn:
        time /= duration;
        return time * time * time * time * time;
    case EaseType::QuintOut:
        time = time / duration - 1;
        return (time * time * time * time * time + 1);
    case EaseType::QuintInOut:
        time /= duration * 0.5f;
        if (time < 1) return 0.5f * time * time * time * time * time;
        time -= 2;
        return 0.5f * (time * time * time * time * time + 2);
    case EaseType::ExpoIn:
        return (time == 0) ? 0 : (float)pow(2, 10 * (time / duration - 1));
    case EaseType::ExpoOut:
        if (time == duration) return 1;
        return (-(float)pow(2, -10 * time / duration) + 1);
    case EaseType::ExpoInOut:
        if (time == 0) return 0;
        if (time == duration) return 1;
        if ((time /= duration * 0.5f) < 1) return 0.5f * (float)pow(2, 10 * (time - 1));
        return 0.5f * (-(float)pow(2, -10 * --time) + 2);
    case EaseType::CircIn:
        time /= duration;
        return -((float)sqrt(1 - time * time) - 1);
    case EaseType::CircOut:
        time = time / duration - 1;
        return (float)sqrt(1 - time * time);
    case EaseType::CircInOut:
        time /= duration * 0.5f;
        if (time < 1) return -0.5f * ((float)sqrt(1 - time * time) - 1);
        time -= 2;
        return 0.5f * ((float)sqrt(1 - time * time) + 1);
    case EaseType::ElasticIn:
        float s0;
        if (time == 0) return 0;
        if ((time /= duration) == 1) return 1;
        if (period == 0) period = duration * 0.3f;
        if (overshootOrAmplitude < 1)
        {
            overshootOrAmplitude = 1;
            s0 = period / 4;
        }
        else s0 = period / _TwoPi * (float)asin(1 / overshootOrAmplitude);
        time -= 1;
        return -(overshootOrAmplitude * (float)pow(2, 10 * time) * (float)sin((time * duration - s0) * _TwoPi / period));
    case EaseType::ElasticOut:
        float s1;
        if (time == 0) return 0;
        if ((time /= duration) == 1) return 1;
        if (period == 0) period = duration * 0.3f;
        if (overshootOrAmplitude < 1)
        {
            overshootOrAmplitude = 1;
            s1 = period / 4;
        }
        else s1 = period / _TwoPi * (float)asin(1 / overshootOrAmplitude);
        return (overshootOrAmplitude * (float)pow(2, -10 * time) * (float)sin((time * duration - s1) * _TwoPi / period) + 1);
    case EaseType::ElasticInOut:
        float s;
        if (time == 0) return 0;
        if ((time /= duration * 0.5f) == 2) return 1;
        if (period == 0) period = duration * (0.3f * 1.5f);
        if (overshootOrAmplitude < 1)
        {
            overshootOrAmplitude = 1;
            s = period / 4;
        }
        else s = period / _TwoPi * (float)asin(1 / overshootOrAmplitude);
        if (time < 1)
        {
            time -= 1;
            return -0.5f * (overshootOrAmplitude * (float)pow(2, 10 * time) * (float)sin((time * duration - s) * _TwoPi / period));
        }
            
        time -= 1;
        return overshootOrAmplitude * (float)pow(2, -10 * time) * (float)sin((time * duration - s) * _TwoPi / period) * 0.5f + 1;
    case EaseType::BackIn:
        time /= duration;
        return time * time * ((overshootOrAmplitude + 1) * time - overshootOrAmplitude);
    case EaseType::BackOut:
        time = time / duration - 1;
        return (time * time * ((overshootOrAmplitude + 1) * time + overshootOrAmplitude) + 1);
    case EaseType::BackInOut:
        time /= duration * 0.5f;
        overshootOrAmplitude *= (1.525f);
        if (time < 1) return 0.5f * (time * time * ((overshootOrAmplitude + 1) * time - overshootOrAmplitude));
        time -= 2;
        return 0.5f * (time * time * ((overshootOrAmplitude + 1) * time + overshootOrAmplitude) + 2);
    case EaseType::BounceIn:
        return Bounce::easeIn(time, duration);
    case EaseType::BounceOut:
        return Bounce::easeOut(time, duration);
    case EaseType::BounceInOut:
        return Bounce::easeInOut(time, duration);

    default:
        time /= duration;
        return -time * (time - 2);
    }
}

float Bounce::easeIn(float time, float duration)
{
    return 1 - easeOut(duration - time, duration);
}

float Bounce::easeOut(float time, float duration)
{
    time /= duration;
    if (time < (1 / 2.75f))
    {
        return (7.5625f * time * time);
    }
    if (time < (2 / 2.75f))
    {
        time -= (1.5f / 2.75f);
        return (7.5625f * time * time + 0.75f);
    }
    if (time < (2.5f / 2.75f))
    {
        time -= (2.25f / 2.75f);
        return (7.5625f * time * time + 0.9375f);
    }
    time -= (2.625f / 2.75f);
    return (7.5625f * time * time + 0.984375f);
}

float Bounce::easeInOut(float time, float duration)
{
    if (time < duration * 0.5f)
    {
        return easeIn(time * 2, duration) * 0.5f;
    }
    return easeOut(time * 2 - duration, duration) * 0.5f + 0.5f;
}

NS_FGUI_END

