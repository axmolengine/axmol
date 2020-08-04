#include "TweenValue.h"

NS_FGUI_BEGIN
USING_NS_CC;

TweenValue::TweenValue():x(0),y(0),z(0),w(0),d(0)
{
}

cocos2d::Vec2 TweenValue::getVec2() const
{
    return cocos2d::Vec2(x, y);
}

void TweenValue::setVec2(const cocos2d::Vec2 & value)
{
    x = value.x;
    y = value.y;
}

cocos2d::Vec3 TweenValue::getVec3() const
{
    return cocos2d::Vec3(x, y, z);
}

void TweenValue::setVec3(const cocos2d::Vec3 & value)
{
    x = value.x;
    y = value.y;
    z = value.z;
}

cocos2d::Vec4 TweenValue::getVec4() const
{
    return cocos2d::Vec4(x, y, z, w);
}

void TweenValue::setVec4(const cocos2d::Vec4 & value)
{
    x = value.x;
    y = value.y;
    z = value.z;
    w = value.w;
}

cocos2d::Color4B TweenValue::getColor() const
{
    return cocos2d::Color4B(x, y, z, w);
}

void TweenValue::setColor(const cocos2d::Color4B & value)
{
    x = value.r;
    y = value.g;
    z = value.b;
    w = value.a;
}

float TweenValue::operator[](int index) const
{
    switch (index)
    {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    case 3:
        return w;
    default:
        throw "Index out of bounds: " + std::to_string(index);
    }
}

float & TweenValue::operator[](int index)
{
    switch (index)
    {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    case 3:
        return w;
    default:
        throw "Index out of bounds: " + std::to_string(index);
    }
}

void TweenValue::setZero()
{
    x = y = z = w = d = 0;
}

NS_FGUI_END