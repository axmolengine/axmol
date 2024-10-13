#pragma once

#include "math/Math.h"
#include "box2d/box2d.h"

namespace ax
{
struct PhysicsHelper
{
    static Vec2 toVec2(const b2Vec2& v) { return Vec2{v.x, v.y}; }

    static b2Vec2 tob2Vec2(const Vec2& v) { return b2Vec2{v.x, v.y}; }

    static Color toColor(b2HexColor color)
    {
        unsigned int r = ((unsigned int)color >> 16) & 0xff;
        unsigned int g = ((unsigned int)color >> 8) & 0xff;
        unsigned int b = ((unsigned int)color) & 0xff;
        return ax::Color{r / 255.f, g / 255.f, b / 255.f, 1.0f};
    }

    static b2HexColor tob2HexColor(Color color)
    {
        Color3B ret(color);
        return (b2HexColor)(static_cast<uint32_t>(ret.r) << 16 | static_cast<uint32_t>(ret.g) << 8 |
                            static_cast<uint32_t>(ret.b));
    }
};

}  // namespace ax
