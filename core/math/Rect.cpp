/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2017 Chukong Technologies
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#include <algorithm>
#include <cmath>
#include "base/Macros.h"

// implementation of Vec2
NS_AX_BEGIN

// implementation of Rect

Rect::Rect()
{
    setRect(0.0f, 0.0f, 0.0f, 0.0f);
}

Rect::Rect(float x, float y, float width, float height)
{
    setRect(x, y, width, height);
}
Rect::Rect(const Vec2& pos, const Vec2& dimension)
{
    setRect(pos.x, pos.y, dimension.x, dimension.y);
}

Rect::Rect(const Rect& other)
{
    setRect(other.origin.x, other.origin.y, other.size.x, other.size.y);
}

Rect& Rect::operator=(const Rect& other)
{
    setRect(other.origin.x, other.origin.y, other.size.x, other.size.y);
    return *this;
}

void Rect::setRect(float x, float y, float width, float height)
{
    // CGRect can support width<0 or height<0
    // AXASSERT(width >= 0.0f && height >= 0.0f, "width and height of Rect must not less than 0.");

    origin.x = x;
    origin.y = y;

    size.x = width;
    size.y = height;
}

bool Rect::equals(const Rect& rect) const
{
    return (origin.equals(rect.origin) && size.equals(rect.size));
}

float Rect::getMaxX() const
{
    return origin.x + size.x;
}

float Rect::getMidX() const
{
    return origin.x + size.x / 2.0f;
}

float Rect::getMinX() const
{
    return origin.x;
}

float Rect::getMaxY() const
{
    return origin.y + size.y;
}

float Rect::getMidY() const
{
    return origin.y + size.y / 2.0f;
}

float Rect::getMinY() const
{
    return origin.y;
}

bool Rect::containsPoint(const Vec2& point) const
{
    bool bRet = false;

    if (point.x >= getMinX() && point.x <= getMaxX() && point.y >= getMinY() && point.y <= getMaxY())
    {
        bRet = true;
    }

    return bRet;
}

bool Rect::intersectsRect(const Rect& rect) const
{
    return !(getMaxX() < rect.getMinX() || rect.getMaxX() < getMinX() || getMaxY() < rect.getMinY() ||
             rect.getMaxY() < getMinY());
}

bool Rect::intersectsCircle(const Vec2& center, float radius) const
{
    Vec2 rectangleCenter((origin.x + size.x / 2), (origin.y + size.y / 2));

    float w = size.x / 2;
    float h = size.y / 2;

    float dx = std::abs(center.x - rectangleCenter.x);
    float dy = std::abs(center.y - rectangleCenter.y);

    if (dx > (radius + w) || dy > (radius + h))
    {
        return false;
    }

    Vec2 circleDistance(std::abs(center.x - origin.x - w), std::abs(center.y - origin.y - h));

    if (circleDistance.x <= (w))
    {
        return true;
    }

    if (circleDistance.y <= (h))
    {
        return true;
    }

    float cornerDistanceSq = powf(circleDistance.x - w, 2) + powf(circleDistance.y - h, 2);

    return (cornerDistanceSq <= (powf(radius, 2)));
}

void Rect::merge(const Rect& rect)
{
    float minX = std::min(getMinX(), rect.getMinX());
    float minY = std::min(getMinY(), rect.getMinY());
    float maxX = std::max(getMaxX(), rect.getMaxX());
    float maxY = std::max(getMaxY(), rect.getMaxY());
    setRect(minX, minY, maxX - minX, maxY - minY);
}

Rect Rect::unionWithRect(const Rect& rect) const
{
    float thisLeftX   = origin.x;
    float thisRightX  = origin.x + size.x;
    float thisTopY    = origin.y + size.y;
    float thisBottomY = origin.y;

    if (thisRightX < thisLeftX)
    {
        std::swap(thisRightX, thisLeftX);  // This rect has negative width
    }

    if (thisTopY < thisBottomY)
    {
        std::swap(thisTopY, thisBottomY);  // This rect has negative height
    }

    float otherLeftX   = rect.origin.x;
    float otherRightX  = rect.origin.x + rect.size.x;
    float otherTopY    = rect.origin.y + rect.size.y;
    float otherBottomY = rect.origin.y;

    if (otherRightX < otherLeftX)
    {
        std::swap(otherRightX, otherLeftX);  // Other rect has negative width
    }

    if (otherTopY < otherBottomY)
    {
        std::swap(otherTopY, otherBottomY);  // Other rect has negative height
    }

    float combinedLeftX   = std::min(thisLeftX, otherLeftX);
    float combinedRightX  = std::max(thisRightX, otherRightX);
    float combinedTopY    = std::max(thisTopY, otherTopY);
    float combinedBottomY = std::min(thisBottomY, otherBottomY);

    return Rect(combinedLeftX, combinedBottomY, combinedRightX - combinedLeftX, combinedTopY - combinedBottomY);
}

const Rect Rect::ZERO = Rect(0, 0, 0, 0);

NS_AX_END
