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

namespace ax
{

#if defined(AX_DLLEXPORT) || defined(AX_DLLIMPORT)
    const Rect Rect::ZERO = Rect(0, 0, 0, 0);
#endif


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

}
