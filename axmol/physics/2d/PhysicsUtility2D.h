#pragma once

#include "axmol/math/Math.h"
#include "box2d/box2d.h"

namespace ax
{
namespace b2util
{
inline Vec2 cast(const b2Vec2& v)
{
    return Vec2{v.x, v.y};
}

inline b2Vec2 cast(const Vec2& v)
{
    return b2Vec2{v.x, v.y};
}

inline b2AABB cast(const Rect& v)
{
    return b2AABB{b2Vec2{v.getMinX(), v.getMinY()}, b2Vec2{v.getMaxX(), v.getMaxY()}};
}

inline Color cast(b2HexColor color)
{
    unsigned int r = ((unsigned int)color >> 16) & 0xff;
    unsigned int g = ((unsigned int)color >> 8) & 0xff;
    unsigned int b = ((unsigned int)color) & 0xff;
    return ax::Color{r / 255.f, g / 255.f, b / 255.f, 1.0f};
}

inline b2HexColor cast(Color color)
{
    Color32 ret{color};
    return (b2HexColor)(static_cast<uint32_t>(ret.r) << 16 | static_cast<uint32_t>(ret.g) << 8 |
                        static_cast<uint32_t>(ret.b));
}
};  // namespace b2util

}  // namespace ax
