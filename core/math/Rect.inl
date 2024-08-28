/****************************************************************************

Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

https://axmol.dev/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "math/Rect.h"

namespace ax
{

inline constexpr Rect::Rect()
{
    setRect(0.0f, 0.0f, 0.0f, 0.0f);
}

inline constexpr Rect::Rect(float x, float y, float width, float height)
{
    setRect(x, y, width, height);
}

inline constexpr Rect::Rect(const Vec2& pos, const Vec2& dimension)
{
    setRect(pos.x, pos.y, dimension.x, dimension.y);
}

inline constexpr Rect::Rect(const Rect& other)
{
    setRect(other.origin.x, other.origin.y, other.size.x, other.size.y);
}

inline Rect& Rect::operator=(const Rect& other)
{
    setRect(other.origin.x, other.origin.y, other.size.x, other.size.y);
    return *this;
}

inline constexpr void Rect::setRect(float x, float y, float width, float height)
{
    origin.x = x;
    origin.y = y;

    size.x = width;
    size.y = height;
}

inline bool Rect::equals(const Rect& rect) const
{
    return (origin.equals(rect.origin) && size.equals(rect.size));
}

inline float Rect::getMaxX() const
{
    return origin.x + size.x;
}

inline float Rect::getMidX() const
{
    return origin.x + size.x / 2.0f;
}

inline float Rect::getMinX() const
{
    return origin.x;
}

inline float Rect::getMaxY() const
{
    return origin.y + size.y;
}

inline float Rect::getMidY() const
{
    return origin.y + size.y / 2.0f;
}

inline float Rect::getMinY() const
{
    return origin.y;
}

inline bool Rect::containsPoint(const Vec2& point) const
{
    bool bRet = false;

    if (point.x >= getMinX() && point.x <= getMaxX() && point.y >= getMinY() && point.y <= getMaxY())
    {
        bRet = true;
    }

    return bRet;
}

inline bool Rect::intersectsRect(const Rect& rect) const
{
    return !(getMaxX() < rect.getMinX() || rect.getMaxX() < getMinX() || getMaxY() < rect.getMinY() ||
             rect.getMaxY() < getMinY());
}

constexpr Rect Rect::ZERO(0, 0, 0, 0);

}
