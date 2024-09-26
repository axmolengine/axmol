#include "TweenValue.h"
#include "base/format.h"

NS_FGUI_BEGIN
using namespace ax;

TweenValue::TweenValue():x(0),y(0),z(0),w(0),d(0)
{
}

ax::Vec2 TweenValue::getVec2() const
{
    return ax::Vec2(x, y);
}

void TweenValue::setVec2(const ax::Vec2 & value)
{
    x = value.x;
    y = value.y;
}

ax::Vec3 TweenValue::getVec3() const
{
    return ax::Vec3(x, y, z);
}

void TweenValue::setVec3(const ax::Vec3 & value)
{
    x = value.x;
    y = value.y;
    z = value.z;
}

ax::Vec4 TweenValue::getVec4() const
{
    return ax::Vec4(x, y, z, w);
}

void TweenValue::setVec4(const ax::Vec4 & value)
{
    x = value.x;
    y = value.y;
    z = value.z;
    w = value.w;
}

ax::Color4B TweenValue::getColor() const
{
    return ax::Color4B(x, y, z, w);
}

void TweenValue::setColor(const ax::Color4B & value)
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