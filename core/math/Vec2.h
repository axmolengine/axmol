/**
 Copyright 2013 BlackBerry Inc.
 Copyright (c) 2014-2017 Chukong Technologies
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 Original file from GamePlay3D: http://gameplay3d.org

 This file was modified to fit the cocos2d-x project
 */

#ifndef MATH_VEC2_H
#define MATH_VEC2_H

#include <algorithm>
#include <functional>
#include <cmath>

#include "base/Macros.h"
#include "math/MathBase.h"

/**
 * @addtogroup base
 * @{
 */

NS_AX_MATH_BEGIN

/** Clamp a value between from and to.
 */

inline float clampf(float value, float min_inclusive, float max_inclusive)
{
    if (min_inclusive > max_inclusive)
    {
        std::swap(min_inclusive, max_inclusive);
    }
    return value < min_inclusive ? min_inclusive : value < max_inclusive ? value : max_inclusive;
}

class Mat4;

/**
 * Defines a 2-element floating point vector.
 */
class AX_DLL Vec2
{
public:
    union
    {
        float comps[2];

        // The coord alias
        struct
        {
            float x;
            float y;
        };

        // The size alias
        struct
        {
            float width;
            float height;
        };

        // The tex coord alias
        struct
        {
            float u;
            float v;
        };
    };
    /**
     * Constructs a new vector initialized to all zeros.
     */
    constexpr Vec2() : x(0.0f), y(0.0f) {}

    /**
     * Constructs a new vector initialized to the specified values.
     *
     * @param xx The x coordinate.
     * @param yy The y coordinate.
     */
    constexpr Vec2(float xx, float yy) : x(xx), y(yy) { }

    /**
     * Constructs a new vector from the values in the specified array.
     *
     * @param array An array containing the elements of the vector in the order x, y.
     */
    constexpr Vec2(const float* array) { set(array); }

    /**
     * Constructs a vector that describes the direction between the specified points.
     *
     * @param p1 The first point.
     * @param p2 The second point.
     */
    constexpr Vec2(const Vec2& p1, const Vec2& p2) { set(p1, p2); }

    /**
     * Indicates whether this vector contains all zeros.
     *
     * @return true if this vector contains all zeros, false otherwise.
     */
    bool isZero() const { return x == 0.0f && y == 0.0f; }

    /**
     * Indicates whether this vector contains all ones.
     *
     * @return true if this vector contains all ones, false otherwise.
     */
    bool isOne() const { return x == 1.0f && y == 1.0f; }

    /**
     * Returns the angle (in radians) between the specified vectors.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     *
     * @return The angle between the two vectors (in radians).
     */
    static float angle(const Vec2& v1, const Vec2& v2);

    /**
     * Adds the elements of the specified vector to this one.
     *
     * @param v The vector to add.
     */
    void add(const Vec2& v) { x += v.x; y += v.y; }

    /**
     * Adds the specified vectors and stores the result in dst.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst A vector to store the result in.
     */
    static void add(const Vec2& v1, const Vec2& v2, Vec2* dst);

    /**
     * Clamps this vector within the specified range.
     *
     * @param min The minimum value.
     * @param max The maximum value.
     */
    void clamp(const Vec2& min, const Vec2& max);

    /**
     * Clamps the specified vector within the specified range and returns it in dst.
     *
     * @param v The vector to clamp.
     * @param min The minimum value.
     * @param max The maximum value.
     * @param dst A vector to store the result in.
     */
    static void clamp(const Vec2& v, const Vec2& min, const Vec2& max, Vec2* dst);

    /**
     * Returns the distance between this vector and v.
     *
     * @param v The other vector.
     *
     * @return The distance between this vector and v.
     *
     * @see distanceSquared
     */
    float distance(const Vec2& v) const;

    /**
     * Returns the squared distance between this vector and v.
     *
     * When it is not necessary to get the exact distance between
     * two vectors (for example, when simply comparing the
     * distance between different vectors), it is advised to use
     * this method instead of distance.
     *
     * @param v The other vector.
     *
     * @return The squared distance between this vector and v.
     *
     * @see distance
     */
    float distanceSquared(const Vec2& v) const
    {
        float dx = v.x - x;
        float dy = v.y - y;
        return (dx * dx + dy * dy);
    }

    /**
     * Returns the dot product of this vector and the specified vector.
     *
     * @param v The vector to compute the dot product with.
     *
     * @return The dot product.
     */
    float dot(const Vec2& v) const { return (x * v.x + y * v.y); }

    /**
     * Returns the dot product between the specified vectors.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     *
     * @return The dot product between the vectors.
     */
    static float dot(const Vec2& v1, const Vec2& v2);

    /**
     * Computes the length of this vector.
     *
     * @return The length of the vector.
     *
     * @see lengthSquared
     */
    float length() const;

    /**
     * Returns the squared length of this vector.
     *
     * When it is not necessary to get the exact length of a
     * vector (for example, when simply comparing the lengths of
     * different vectors), it is advised to use this method
     * instead of length.
     *
     * @return The squared length of the vector.
     *
     * @see length
     */
    float lengthSquared() const { return (x * x + y * y); }

    /**
     * Negates this vector.
     */
    void negate()
    {
        x = -x;
        y = -y;
    }

    /**
     * Normalizes this vector.
     *
     * This method normalizes this Vec2 so that it is of
     * unit length (in other words, the length of the vector
     * after calling this method will be 1.0f). If the vector
     * already has unit length or if the length of the vector
     * is zero, this method does nothing.
     *
     * @return This vector, after the normalization occurs.
     */
    void normalize();

    /**
     Get the normalized vector.
     */
    Vec2 getNormalized() const;

    /**
     * Scales all elements of this vector by the specified value.
     *
     * @param scalar The scalar value.
     */
    void scale(float scalar)
    {
        x *= scalar;
        y *= scalar;
    }

    /**
     * Scales each element of this vector by the matching component of scale.
     *
     * @param scale The vector to scale by.
     */
    void scale(const Vec2& scale)
    {
        x *= scale.x;
        y *= scale.y;
    }

    /**
     * Rotates this vector by angle (specified in radians) around the given point.
     *
     * @param point The point to rotate around.
     * @param angle The angle to rotate by (in radians).
     */
    void rotate(const Vec2& point, float angle);

    /**
     * Sets the elements of this vector to the specified values.
     *
     * @param xx The new x coordinate.
     * @param yy The new y coordinate.
     */
    constexpr void set(float xx, float yy)
    {
        this->x = xx;
        this->y = yy;
    }

    /**
     * [DEPRECATED] use Vec2::set instead
     */
    constexpr void setSize(float w, float h) { this->set(w, h); }

    /**
     * Sets the elements of this vector from the values in the specified array.
     *
     * @param array An array containing the elements of the vector in the order x, y.
     */
    constexpr void set(const float* array)
    {
        AX_ASSERT(array);

        x = array[0];
        y = array[1];
    }

    /**
     * Sets the elements of this vector to those in the specified vector.
     *
     * @param v The vector to copy.
     */
    constexpr void set(const Vec2& v)
    {
        this->x = v.x;
        this->y = v.y;
    }

    /**
     * Sets this vector to the directional vector between the specified points.
     *
     * @param p1 The first point.
     * @param p2 The second point.
     */
    constexpr void set(const Vec2& p1, const Vec2& p2)
    {
        x = p2.x - p1.x;
        y = p2.y - p1.y;
    }

    /**
     * Sets the elements of this vector to zero.
     */
    void setZero() { x = y = 0.0f; }

    /**
     * Subtracts this vector and the specified vector as (this - v)
     * and stores the result in this vector.
     *
     * @param v The vector to subtract.
     */
    void subtract(const Vec2& v)
    {
        x -= v.x;
        y -= v.y;
    }

    /**
     * Subtracts the specified vectors and stores the result in dst.
     * The resulting vector is computed as (v1 - v2).
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst The destination vector.
     */
    static void subtract(const Vec2& v1, const Vec2& v2, Vec2* dst);

    /**
     * Updates this vector towards the given target using a smoothing function.
     * The given response time determines the amount of smoothing (lag). A longer
     * response time yields a smoother result and more lag. To force this vector to
     * follow the target closely, provide a response time that is very small relative
     * to the given elapsed time.
     *
     * @param target target value.
     * @param elapsedTime elapsed time between calls.
     * @param responseTime response time (in the same units as elapsedTime).
     */
    void smooth(const Vec2& target, float elapsedTime, float responseTime)
    {
        if (elapsedTime > 0)
        {
            *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
        }
    }

    /**
     * Calculates the sum of this vector with the given vector.
     *
     * Note: this does not modify this vector.
     *
     * @param v The vector to add.
     * @return The vector sum.
     */
    Vec2 operator+(const Vec2& v) const
    {
        Vec2 result(*this);
        result.add(v);
        return result;
    }

    /**
     * Adds the given vector to this vector.
     *
     * @param v The vector to add.
     * @return This vector, after the addition occurs.
     */
    Vec2& operator+=(const Vec2& v)
    {
        add(v);
        return *this;
    }

    /**
     * Calculates the sum of this vector with the given vector.
     *
     * Note: this does not modify this vector.
     *
     * @param v The vector to add.
     * @return The vector sum.
     */
    Vec2 operator-(const Vec2& v) const
    {
        Vec2 result(*this);
        result.subtract(v);
        return result;
    }

    /**
     * Subtracts the given vector from this vector.
     *
     * @param v The vector to subtract.
     * @return This vector, after the subtraction occurs.
     */
    Vec2& operator-=(const Vec2& v)
    {
        subtract(v);
        return *this;
    }

    /**
     * Calculates the negation of this vector.
     *
     * Note: this does not modify this vector.
     *
     * @return The negation of this vector.
     */
    Vec2 operator-() const
    {
        Vec2 result(*this);
        result.negate();
        return result;
    }

    /**
     * Calculates the scalar product of this vector with the given value.
     *
     * Note: this does not modify this vector.
     *
     * @param s The value to scale by.
     * @return The scaled vector.
     */
    Vec2 operator*(float s) const
    {
        Vec2 result(*this);
        result.scale(s);
        return result;
    }

    /**
     * Scales this vector by the given value.
     *
     * @param s The value to scale by.
     * @return This vector, after the scale occurs.
     */
    Vec2& operator*=(float s)
    {
        scale(s);
        return *this;
    }

    /**
     * Returns the components of this vector divided by the given constant
     *
     * Note: this does not modify this vector.
     *
     * @param s the constant to divide this vector with
     * @return a smaller vector
     */
    Vec2 operator/(float s) const
    {
        return Vec2(this->x / s, this->y / s);
    }

    /**
     * Determines if this vector is less than the given vector.
     *
     * @param v The vector to compare against.
     *
     * @return True if this vector is less than the given vector, false otherwise.
     */
    bool operator<(const Vec2& v) const
    {
        if (x == v.x)
        {
            return y < v.y;
        }
        return x < v.x;
    }

    /**
     * Determines if this vector is greater than the given vector.
     *
     * @param v The vector to compare against.
     *
     * @return True if this vector is greater than the given vector, false otherwise.
     */
    bool operator>(const Vec2& v) const
    {
        if (x == v.x)
        {
            return y > v.y;
        }
        return x > v.x;
    }

    /**
     * Determines if this vector is equal to the given vector.
     *
     * @param v The vector to compare against.
     *
     * @return True if this vector is equal to the given vector, false otherwise.
     */
    bool operator==(const Vec2& v) const { return x==v.x && y==v.y; }

    /**
     * Determines if this vector is not equal to the given vector.
     *
     * @param v The vector to compare against.
     *
     * @return True if this vector is not equal to the given vector, false otherwise.
     */
    bool operator!=(const Vec2& v) const { return x != v.x || y != v.y; }

    // code added compatible for Point
public:
    /**
     * @js NA
     * @lua NA
     */
    void setPoint(float xx, float yy)
    {
        this->x = xx;
        this->y = yy;
    }
    /**
     * @js NA
     */
    bool equals(const Vec2& target) const
    {
        return (std::abs(this->x - target.x) < FLT_EPSILON) && (std::abs(this->y - target.y) < FLT_EPSILON);
    }

    /** @returns if points have fuzzy equality which means equal with some degree of variance.
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    bool fuzzyEquals(const Vec2& target, float variance) const;

    /** Calculates distance between point an origin
     @return float
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    float getLength() const { return sqrtf(x * x + y * y); }

    /** Calculates the square length of a Vec2 (not calling sqrt() )
     @return float
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    float getLengthSq() const
    {
        return dot(*this);  // x*x + y*y;
    }

    /** Calculates the square distance between two points (not calling sqrt() )
     @return float
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    float getDistanceSq(const Vec2& other) const { return (*this - other).getLengthSq(); }

    /** Calculates the distance between two points
     @return float
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    float getDistance(const Vec2& other) const { return (*this - other).getLength(); }

    /** @returns the angle in radians between this vector and the x axis
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    float getAngle() const { return atan2f(y, x); }

    /** @returns the angle in radians between two vector directions
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    float getAngle(const Vec2& other) const;

    /** Calculates cross product of two points.
     @return float
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    float cross(const Vec2& other) const { return x * other.y - y * other.x; }

    /** Calculates perpendicular of v, rotated 90 degrees counter-clockwise -- cross(v, perp(v)) >= 0
     @return Vec2
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    Vec2 getPerp() const { return Vec2(-y, x); }

    /** Calculates midpoint between two points.
     @return Vec2
     @since v3.0
     * @js NA
     * @lua NA
     */
    Vec2 getMidpoint(const Vec2& other) const { return Vec2((x + other.x) / 2.0f, (y + other.y) / 2.0f); }

    /** Clamp a point between from and to.
     @since v3.0
     * @js NA
     * @lua NA
     */
    Vec2 getClampPoint(const Vec2& min_inclusive, const Vec2& max_inclusive) const
    {
        return Vec2(clampf(x, min_inclusive.x, max_inclusive.x), clampf(y, min_inclusive.y, max_inclusive.y));
    }

    /** Run a math operation function on each point component
     * absf, floorf, ceilf, roundf
     * any function that has the signature: float func(float);
     * For example: let's try to take the floor of x,y
     * p.compOp(floorf);
     @since v3.0
     * @js NA
     * @lua NA
     */
    Vec2 compOp(std::function<float(float)> function) const { return Vec2(function(x), function(y)); }

    /** Calculates perpendicular of v, rotated 90 degrees clockwise -- cross(v, rperp(v)) <= 0
     @return Vec2
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    Vec2 getRPerp() const { return Vec2(y, -x); }

    /** Calculates the projection of this over other.
     @return Vec2
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    Vec2 project(const Vec2& other) const { return other * (dot(other) / other.dot(other)); }

    /** Complex multiplication of two points ("rotates" two points).
     @return Vec2 vector with an angle of this.getAngle() + other.getAngle(),
     and a length of this.getLength() * other.getLength().
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    Vec2 rotate(const Vec2& other) const { return Vec2(x * other.x - y * other.y, x * other.y + y * other.x); }

    /** Unrotates two points.
     @return Vec2 vector with an angle of this.getAngle() - other.getAngle(),
     and a length of this.getLength() * other.getLength().
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    Vec2 unrotate(const Vec2& other) const { return Vec2(x * other.x + y * other.y, y * other.x - x * other.y); }

    /** Linear Interpolation between two points a and b
     @returns
        alpha == 0 ? a
        alpha == 1 ? b
        otherwise a value between a..b
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    Vec2 lerp(const Vec2& other, float alpha) const { return *this * (1.f - alpha) + other * alpha; }

    /** Rotates a point counter clockwise by the angle around a pivot
     @param pivot is the pivot, naturally
     @param angle is the angle of rotation ccw in radians
     @returns the rotated point
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    Vec2 rotateByAngle(const Vec2& pivot, float angle) const;

    /**
     * @js NA
     * @lua NA
     */
    static Vec2 forAngle(const float a) { return Vec2(cosf(a), sinf(a)); }

    /** A general line-line intersection test
     @param A   the startpoint for the first line L1 = (A - B)
     @param B   the endpoint for the first line L1 = (A - B)
     @param C   the startpoint for the second line L2 = (C - D)
     @param D   the endpoint for the second line L2 = (C - D)
     @param S   the range for a hitpoint in L1 (p = A + S*(B - A))
     @param T   the range for a hitpoint in L2 (p = C + T*(D - C))
     @return    whether these two lines intersects.

     Note that to truly test intersection for segments we have to make
     sure that S & T lie within [0..1] and for rays, make sure S & T > 0
     the hit point is        C + T * (D - C);
     the hit point also is   A + S * (B - A);
     @since 3.0
     * @js NA
     * @lua NA
     */
    static bool isLineIntersect(const Vec2& A,
                                const Vec2& B,
                                const Vec2& C,
                                const Vec2& D,
                                float* S = nullptr,
                                float* T = nullptr);

    /**
     returns true if Line A-B overlap with segment C-D
     @since v3.0
     * @js NA
     * @lua NA
     */
    static bool isLineOverlap(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D);

    /**
     returns true if Line A-B parallel with segment C-D
     @since v3.0
     * @js NA
     * @lua NA
     */
    static bool isLineParallel(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D);

    /**
     returns true if Segment A-B overlap with segment C-D
     @since v3.0
     * @js NA
     * @lua NA
     */
    static bool isSegmentOverlap(const Vec2& A,
                                 const Vec2& B,
                                 const Vec2& C,
                                 const Vec2& D,
                                 Vec2* S = nullptr,
                                 Vec2* E = nullptr);

    /**
     returns true if Segment A-B intersects with segment C-D
     @since v3.0
     * @js NA
     * @lua NA
     */
    static bool isSegmentIntersect(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D);

    /**
     returns the intersection point of line A-B, C-D
     @since v3.0
     * @js NA
     * @lua NA
     */
    static Vec2 getIntersectPoint(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D);

    /** equals to Vec2(0,0) */
    static const Vec2 ZERO;
    /** equals to Vec2(1,1) */
    static const Vec2 ONE;
    /** equals to Vec2(1,0) */
    static const Vec2 UNIT_X;
    /** equals to Vec2(0,1) */
    static const Vec2 UNIT_Y;
    /** equals to Vec2(0.5, 0.5) */
    static const Vec2 ANCHOR_MIDDLE;
    /** equals to Vec2(0, 0) */
    static const Vec2 ANCHOR_BOTTOM_LEFT;
    /** equals to Vec2(0, 1) */
    static const Vec2 ANCHOR_TOP_LEFT;
    /** equals to Vec2(1, 0) */
    static const Vec2 ANCHOR_BOTTOM_RIGHT;
    /** equals to Vec2(1, 1) */
    static const Vec2 ANCHOR_TOP_RIGHT;
    /** equals to Vec2(1, 0.5) */
    static const Vec2 ANCHOR_MIDDLE_RIGHT;
    /** equals to Vec2(0, 0.5) */
    static const Vec2 ANCHOR_MIDDLE_LEFT;
    /** equals to Vec2(0.5, 1) */
    static const Vec2 ANCHOR_MIDDLE_TOP;
    /** equals to Vec2(0.5, 0) */
    static const Vec2 ANCHOR_MIDDLE_BOTTOM;
};

/**
 * Calculates the scalar product of the given vector with the given value.
 *
 * @param x The value to scale by.
 * @param v The vector to scale.
 * @return The scaled vector.
 */
inline Vec2 operator*(float x, const Vec2& v)
{
    Vec2 result(v);
    result.scale(x);
    return result;
}

inline Vec2 operator*(const Vec2& left, const Vec2& right)
{
    return Vec2(left.x * right.x, left.y * right.y);
}

inline Vec2 operator/(const Vec2& left, const Vec2& right)
{
    return Vec2(left.x / right.x, left.y / right.y);
}

#if !(defined(AX_DLLEXPORT) || defined(AX_DLLIMPORT))
    inline constexpr Vec2 Vec2::ZERO(0.0f, 0.0f);
    inline constexpr Vec2 Vec2::ONE(1.0f, 1.0f);
    inline constexpr Vec2 Vec2::UNIT_X(1.0f, 0.0f);
    inline constexpr Vec2 Vec2::UNIT_Y(0.0f, 1.0f);
    inline constexpr Vec2 Vec2::ANCHOR_MIDDLE(0.5f, 0.5f);
    inline constexpr Vec2 Vec2::ANCHOR_BOTTOM_LEFT(0.0f, 0.0f);
    inline constexpr Vec2 Vec2::ANCHOR_TOP_LEFT(0.0f, 1.0f);
    inline constexpr Vec2 Vec2::ANCHOR_BOTTOM_RIGHT(1.0f, 0.0f);
    inline constexpr Vec2 Vec2::ANCHOR_TOP_RIGHT(1.0f, 1.0f);
    inline constexpr Vec2 Vec2::ANCHOR_MIDDLE_RIGHT(1.0f, 0.5f);
    inline constexpr Vec2 Vec2::ANCHOR_MIDDLE_LEFT(0.0f, 0.5f);
    inline constexpr Vec2 Vec2::ANCHOR_MIDDLE_TOP(0.5f, 1.0f);
    inline constexpr Vec2 Vec2::ANCHOR_MIDDLE_BOTTOM(0.5f, 0.0f);
#endif

using Point = Vec2;

// [DEPRECATED] compatible only
using Size = Vec2;  // typedef Vec2 Size;

NS_AX_MATH_END

/**
 end of base group
 @}
 */

#endif  // MATH_VEC2_H
