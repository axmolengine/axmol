#include "TweenValue.h"
#include "base/format.h"

NS_FGUI_BEGIN
USING_NS_AX;

TweenValue::TweenValue():x(0),y(0),z(0),w(0),d(0)
{
}

axis::Vec2 TweenValue::getVec2() const
{
    return axis::Vec2(x, y);
}

void TweenValue::setVec2(const axis::Vec2 & value)
{
    x = value.x;
    y = value.y;
}

axis::Vec3 TweenValue::getVec3() const
{
    return axis::Vec3(x, y, z);
}

void TweenValue::setVec3(const axis::Vec3 & value)
{
    x = value.x;
    y = value.y;
    z = value.z;
}

axis::Vec4 TweenValue::getVec4() const
{
    return axis::Vec4(x, y, z, w);
}

void TweenValue::setVec4(const axis::Vec4 & value)
{
    x = value.x;
    y = value.y;
    z = value.z;
    w = value.w;
}

axis::Color4B TweenValue::getColor() const
{
    return axis::Color4B(x, y, z, w);
}

void TweenValue::setColor(const axis::Color4B & value)
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
        throw fmt::format("Index out of bounds: {}", index);
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
        throw fmt::format("Index out of bounds: {}", index);
    }
}

void TweenValue::setZero()
{
    x = y = z = w = d = 0;
}

NS_FGUI_END