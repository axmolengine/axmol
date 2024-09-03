/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2017 Chukong Technologies
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

#ifndef __MATH_RECT_H__
#define __MATH_RECT_H__

#include "math/Vec2.h"

/**
 * @addtogroup base
 * @{
 */

namespace ax
{

/**Rectangle area.*/
class AX_DLL Rect
{
public:
    /**Low left point of rect.*/
    Vec2 origin;
    /**Width and height of the rect.*/
    Vec2 size;

public:
    /**
    Constructor an empty Rect.
     * @js NA
     */
    constexpr Rect() { setRect(0.0f, 0.0f, 0.0f, 0.0f); }
    /**
    Constructor a rect.
     * @js NA
     */
    constexpr Rect(float x, float y, float width, float height) { setRect(x, y, width, height); }
    /**
     Constructor a rect.
     * @js NA
     */
    constexpr Rect(const Vec2& pos, const Vec2& dimension) { setRect(pos.x, pos.y, dimension.x, dimension.y); }
    /**
    Copy constructor.
     * @js NA
     * @lua NA
     */
    constexpr Rect(const Rect& other) { setRect(other.origin.x, other.origin.y, other.size.x, other.size.y); }
    /**
     * @js NA
     * @lua NA
     */
    Rect& operator=(const Rect& other)
    {
        setRect(other.origin.x, other.origin.y, other.size.x, other.size.y);
        return *this;
    }
    /**
    Set the x, y, width and height of Rect.
     * @js NA
     * @lua NA
     */
    constexpr void setRect(float x, float y, float width, float height)
    {
        origin.x = x;
        origin.y = y;

        size.x = width;
        size.y = height;
    }
    /**
    Get the left of the rect.
     * @js NA
     */
    float getMinX() const  /// return the leftmost x-value of current rect
    {
        return origin.x;
    }
    /**
    Get the X coordinate of center point.
     * @js NA
     */
    float getMidX() const  /// return the midpoint x-value of current rect
    {
        return origin.x + size.x / 2.0f;
    }
    /**
    Get the right of rect.
     * @js NA
     */
    float getMaxX() const  /// return the rightmost x-value of current rect
    {
        return origin.x + size.x;
    }
    /**
    Get the bottom of rect.
     * @js NA
     */
    float getMinY() const  /// return the bottommost y-value of current rect
    {
        return origin.y;
    }
    /**
    Get the Y coordinate of center point.
     * @js NA
     */
    float getMidY() const  /// return the midpoint y-value of current rect
    {
        return origin.y + size.y / 2.0f;
    }
    /**
    Get top of rect.
     * @js NA
     */
    float getMaxY() const  /// return the topmost y-value of current rect
    {
        return origin.y + size.y;
    }
    /**
    Compare two rects.
     * @js NA
     */
    bool equals(const Rect& rect) const { return (origin.equals(rect.origin) && size.equals(rect.size)); }
    /**
    Check if the points is contained in the rect.
     * @js NA
     */
    bool containsPoint(const Vec2& point) const
    {
        bool bRet = false;

        if (point.x >= getMinX() && point.x <= getMaxX() && point.y >= getMinY() && point.y <= getMaxY())
        {
            bRet = true;
        }

        return bRet;
    }
    /**
    Check the intersect status of two rects.
     * @js NA
     */
    bool intersectsRect(const Rect& rect) const
    {
        return !(getMaxX() < rect.getMinX() || rect.getMaxX() < getMinX() || getMaxY() < rect.getMinY() ||
                rect.getMaxY() < getMinY());
    }
    /**
    Check the intersect status of the rect and a circle.
     * @js NA
     */
    bool intersectsCircle(const Vec2& center, float radius) const;
    /**
    Get the min rect which can contain this and rect.
     * @js NA
     * @lua NA
     */
    Rect unionWithRect(const Rect& rect) const;
    /**Compute the min rect which can contain this and rect, assign it to this.*/
    void merge(const Rect& rect);
    /**An empty Rect.*/
    static const Rect ZERO;
};

#if !(defined(AX_DLLEXPORT) || defined(AX_DLLIMPORT))
    inline constexpr Rect Rect::ZERO(0, 0, 0, 0);
#endif

}

// end of base group
/// @}

#endif  // __MATH_RECT_H__
