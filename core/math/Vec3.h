/**
 Copyright 2013 BlackBerry Inc.
 Copyright (c) 2014-2017 Chukong Technologies
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#ifndef MATH_VEC3_H
#define MATH_VEC3_H

#include <cmath>
#include "base/Macros.h"
#include "math/MathBase.h"

/**
 * @addtogroup base
 * @{
 */

NS_AX_MATH_BEGIN

class Mat4;
class Quaternion;

/**
 * Defines a 3-element floating point vector.
 *
 * When using a vector to represent a surface normal,
 * the vector should typically be normalized.
 * Other uses of directional vectors may wish to leave
 * the magnitude of the vector intact. When used as a point,
 * the elements of the vector represent a position in 3D space.
 */
class AX_DLL Vec3
{
public:
    /**
     * The x-coordinate.
     */
    float x;

    /**
     * The y-coordinate.
     */
    float y;

    /**
     * The z-coordinate.
     */
    float z;

    /**
     * Constructs a new vector initialized to all zeros.
     */
    constexpr Vec3() : x(0.0f), y(0.0f), z(0.0f) {}

    /**
     * Constructs a new vector initialized to the specified values.
     *
     * @param xx The x coordinate.
     * @param yy The y coordinate.
     * @param zz The z coordinate.
     */
    constexpr Vec3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}

    /**
     * Constructs a new vector from the values in the specified array.
     *
     * @param array An array containing the elements of the vector in the order x, y, z.
     */
    constexpr Vec3(const float* array) { set(array); }

    /**
     * Constructs a vector that describes the direction between the specified points.
     *
     * @param p1 The first point.
     * @param p2 The second point.
     */
    constexpr Vec3(const Vec3& p1, const Vec3& p2) { set(p1, p2); }

    /**
     * Creates a new vector from an integer interpreted as an RGB value.
     * E.g. 0xff0000 represents red or the vector (1, 0, 0).
     *
     * @param color The integer to interpret as an RGB value.
     *
     * @return A vector corresponding to the interpreted RGB color.
     */
    static Vec3 fromColor(unsigned int color);

    /**
     * Indicates whether this vector contains all zeros.
     *
     * @return true if this vector contains all zeros, false otherwise.
     */
    bool isZero() const { return x == 0.0f && y == 0.0f && z == 0.0f; }

    /**
     * Indicates whether this vector contains all ones.
     *
     * @return true if this vector contains all ones, false otherwise.
     */
    bool isOne() const { return x == 1.0f && y == 1.0f && z == 1.0f; }

    /**
     * Returns the angle (in radians) between the specified vectors.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     *
     * @return The angle between the two vectors (in radians).
     */
    static float angle(const Vec3& v1, const Vec3& v2);

    /**
     * Adds the elements of the specified vector to this one.
     *
     * @param v The vector to add.
     */
    void add(const Vec3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    /**
     * Adds the elements of this vector to the specified values.
     *
     * @param xx The add x coordinate.
     * @param yy The add y coordinate.
     * @param zz The add z coordinate.
     */
    void add(float xx, float yy, float zz)
    {
        x += xx;
        y += yy;
        z += zz;
    }

    /**
     * Adds the specified vectors and stores the result in dst.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst A vector to store the result in.
     */
    static void add(const Vec3& v1, const Vec3& v2, Vec3* dst);

    /**
     * Clamps this vector within the specified range.
     *
     * @param min The minimum value.
     * @param max The maximum value.
     */
    void clamp(const Vec3& min, const Vec3& max);

    /**
     * Clamps the specified vector within the specified range and returns it in dst.
     *
     * @param v The vector to clamp.
     * @param min The minimum value.
     * @param max The maximum value.
     * @param dst A vector to store the result in.
     */
    static void clamp(const Vec3& v, const Vec3& min, const Vec3& max, Vec3* dst);

    /**
     * Sets this vector to the cross product between itself and the specified vector.
     *
     * @param v The vector to compute the cross product with.
     */
    void cross(const Vec3& v);

    /**
     * Computes the cross product of the specified vectors and stores the result in dst.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst A vector to store the result in.
     */
    static void cross(const Vec3& v1, const Vec3& v2, Vec3* dst);

    /**
     * Returns the distance between this vector and v.
     *
     * @param v The other vector.
     *
     * @return The distance between this vector and v.
     *
     * @see distanceSquared
     */
    float distance(const Vec3& v) const;

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
    float distanceSquared(const Vec3& v) const;

    /**
     * Returns the dot product of this vector and the specified vector.
     *
     * @param v The vector to compute the dot product with.
     *
     * @return The dot product.
     */
    float dot(const Vec3& v) const;

    /**
     * Returns the dot product between the specified vectors.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     *
     * @return The dot product between the vectors.
     */
    static float dot(const Vec3& v1, const Vec3& v2);

    /**
     * Computes the length of this vector.
     *
     * @return The length of the vector.
     *
     * @see lengthSquared
     */
    float length() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }

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
    float lengthSquared() const { return (x * x + y * y + z * z); }

    /**
     * Negates this vector.
     */
    void negate()
    {
        x = -x;
        y = -y;
        z = -z;
    }

    /**
     * Normalizes this vector.
     *
     * This method normalizes this Vec3 so that it is of
     * unit length (in other words, the length of the vector
     * after calling this method will be 1.0f). If the vector
     * already has unit length or if the length of the vector
     * is zero, this method does nothing.
     *
     * @return This vector, after the normalization occurs.
     */
    void normalize();

    /**
     * Get the normalized vector.
     */
    Vec3 getNormalized() const;

    /**
     * Scales all elements of this vector by the specified value.
     *
     * @param scalar The scalar value.
     */
    void scale(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
    }

    /**
     * Sets the elements of this vector to the specified values.
     *
     * @param xx The new x coordinate.
     * @param yy The new y coordinate.
     * @param zz The new z coordinate.
     */
    constexpr void set(float xx, float yy, float zz)
    {
        this->x = xx;
        this->y = yy;
        this->z = zz;
    }

    /**
     * Sets the elements of this vector from the values in the specified array.
     *
     * @param array An array containing the elements of the vector in the order x, y, z.
     */
    constexpr void set(const float* array)
    {
        AX_ASSERT(array);

        x = array[0];
        y = array[1];
        z = array[2];
    }

    /**
     * Sets the elements of this vector to those in the specified vector.
     *
     * @param v The vector to copy.
     */
    constexpr void set(const Vec3& v)
    {
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
    }

    /**
     * Sets this vector to the directional vector between the specified points.
     */
    constexpr void set(const Vec3& p1, const Vec3& p2)
    {
        x = p2.x - p1.x;
        y = p2.y - p1.y;
        z = p2.z - p1.z;
    }

    /**
     * Sets the elements of this vector to zero.
     */
    void setZero()
    {
        x = y = z = 0.0f;
    }

    /**
     * Subtracts this vector and the specified vector as (this - v)
     * and stores the result in this vector.
     *
     * @param v The vector to subtract.
     */
    void subtract(const Vec3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }

    /**
     * Subtracts the specified vectors and stores the result in dst.
     * The resulting vector is computed as (v1 - v2).
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst The destination vector.
     */
    static void subtract(const Vec3& v1, const Vec3& v2, Vec3* dst);

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
    void smooth(const Vec3& target, float elapsedTime, float responseTime);

    /**
     * Linear interpolation between two vectors A and B by alpha which
     * is in the range [0,1]
     */
    Vec3 lerp(const Vec3& target, float alpha) const
    {
        return *this * (1.f - alpha) + target * alpha;
    }

    /**
     * Calculates the sum of this vector with the given vector.
     *
     * Note: this does not modify this vector.
     *
     * @param v The vector to add.
     * @return The vector sum.
     */
    Vec3 operator+(const Vec3& v) const
    {
        Vec3 result(*this);
        result.add(v);
        return result;
    }

    /**
     * Adds the given vector to this vector.
     *
     * @param v The vector to add.
     * @return This vector, after the addition occurs.
     */
    Vec3& operator+=(const Vec3& v)
    {
        add(v);
        return *this;
    }

    /**
     * Calculates the difference of this vector with the given vector.
     *
     * Note: this does not modify this vector.
     *
     * @param v The vector to subtract.
     * @return The vector difference.
     */
    Vec3 operator-(const Vec3& v) const
    {
        Vec3 result(*this);
        result.subtract(v);
        return result;
    }

    /**
     * Subtracts the given vector from this vector.
     *
     * @param v The vector to subtract.
     * @return This vector, after the subtraction occurs.
     */
    Vec3& operator-=(const Vec3& v)
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
    Vec3 operator-() const
    {
        Vec3 result(*this);
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
    Vec3 operator*(float s) const
    {
        Vec3 result(*this);
        result.scale(s);
        return result;
    }

    /**
     * Scales this vector by the given value.
     *
     * @param s The value to scale by.
     * @return This vector, after the scale occurs.
     */
    Vec3& operator*=(float s)
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
    Vec3 operator/(float s) const { return Vec3(this->x / s, this->y / s, this->z / s); }

    /** Returns true if the vector's scalar components are all greater
     that the ones of the vector it is compared against.
     */
    bool operator<(const Vec3& rhs) const
    {
        if (x < rhs.x && y < rhs.y && z < rhs.z)
            return true;
        return false;
    }

    /** Returns true if the vector's scalar components are all smaller
     that the ones of the vector it is compared against.
     */
    bool operator>(const Vec3& rhs) const
    {
        if (x > rhs.x && y > rhs.y && z > rhs.z)
            return true;
        return false;
    }

    /**
     * Determines if this vector is equal to the given vector.
     *
     * @param v The vector to compare against.
     *
     * @return True if this vector is equal to the given vector, false otherwise.
     */
    bool operator==(const Vec3& v) const { return x==v.x && y==v.y && z==v.z; }

    /**
     * Determines if this vector is not equal to the given vector.
     *
     * @param v The vector to compare against.
     *
     * @return True if this vector is not equal to the given vector, false otherwise.
     */
    bool operator!=(const Vec3& v) const { return x!=v.x || y!=v.y || z!=v.z; }

    /** equals to Vec3(0,0,0) */
    static const Vec3 ZERO;
    /** equals to Vec3(1,1,1) */
    static const Vec3 ONE;
    /** equals to Vec3(1,0,0) */
    static const Vec3 UNIT_X;
    /** equals to Vec3(0,1,0) */
    static const Vec3 UNIT_Y;
    /** equals to Vec3(0,0,1) */
    static const Vec3 UNIT_Z;
};

/**
 * Calculates the scalar product of the given vector with the given value.
 *
 * @param x The value to scale by.
 * @param v The vector to scale.
 * @return The scaled vector.
 */
inline Vec3 operator*(float x, const Vec3& v)
{
    Vec3 result(v);
    result.scale(x);
    return result;
}

#if !(defined(AX_DLLEXPORT) || defined(AX_DLLIMPORT))
    inline constexpr Vec3 Vec3::ZERO(0.0f, 0.0f, 0.0f);
    inline constexpr Vec3 Vec3::ONE(1.0f, 1.0f, 1.0f);
    inline constexpr Vec3 Vec3::UNIT_X(1.0f, 0.0f, 0.0f);
    inline constexpr Vec3 Vec3::UNIT_Y(0.0f, 1.0f, 0.0f);
    inline constexpr Vec3 Vec3::UNIT_Z(0.0f, 0.0f, 1.0f);
#endif

NS_AX_MATH_END
/**
 end of base group
 @}
 */

#endif  // MATH_VEC3_H
