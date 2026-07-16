/**
 Copyright 2013 BlackBerry Inc.
 Copyright (c) 2014-2017 Chukong Technologies
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

#pragma once

#include "axmol/base/Macros.h"
#include "axmol/math/MathBase.h"

/**
 * @addtogroup base
 * @{
 */

NS_AX_MATH_BEGIN

class Mat4;

struct AX_DLL Vec4Base
{
    constexpr Vec4Base() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    constexpr Vec4Base(float xx, float yy, float zz, float ww) : x(xx), y(yy), z(zz), w(ww) {}
    constexpr explicit Vec4Base(const float* src) : x(src[0]), y(src[1]), z(src[2]), w(src[3]) {}

    /**
     * Sets the elements of this vector to those in the specified vector.
     *
     * @param val The vector to copy.
     */
    void set(const Vec4Base& val)
    {
        this->x = val.x;
        this->y = val.y;
        this->z = val.z;
        this->w = val.w;
    }

    /**
     * Sets the elements of this vector to the specified values.
     *
     * @param xx The new x coordinate.
     * @param yy The new y coordinate.
     * @param zz The new z coordinate.
     * @param ww The new w coordinate.
     */
    constexpr void set(float xx, float yy, float zz, float ww)
    {
        this->x = xx;
        this->y = yy;
        this->z = zz;
        this->w = ww;
    }

    /**
     * Sets the elements of this vector from the values in the specified array.
     *
     * @param array An array containing the elements of the vector in the order x, y, z, w.
     */
    constexpr void set(const float* array)
    {
        this->x = array[0];
        this->y = array[1];
        this->z = array[2];
        this->w = array[3];
    }

    /**
     * Clamps this vector within the specified range.
     *
     * @param min The minimum value.
     * @param max The maximum value.
     */
    void clamp(const Vec4Base& min, const Vec4Base& max);

    /**
     * Clamps the specified vector within the specified range and returns it in dst.
     *
     * @param v The vector to clamp.
     * @param min The minimum value.
     * @param max The maximum value.
     * @param dst A vector to store the result in.
     */
    static void clamp(const Vec4Base& v, const Vec4Base& min, const Vec4Base& max, Vec4Base* dst);

    union
    {
        struct
        {
            float x, y, z, w;
        };
        struct
        {
            float r, g, b, a;
        };
        struct
        {
            float h, s;
            union
            {
                float v, l;
            };
        };
        float comps[4];
    };
};

/*
 * @brief A 4-floats class template with base math operators
 */
template <typename _ImplType>
struct Vec4Adapter : public Vec4Base
{
public:
    using impl_type = _ImplType;

    constexpr Vec4Adapter() = default;
    constexpr Vec4Adapter(float xx, float yy, float zz, float ww) : Vec4Base(xx, yy, zz, ww) {}
    constexpr explicit Vec4Adapter(const float* src) : Vec4Base(src) {}

    impl_type& negate()
    {
        x = -x;
        y = -y;
        z = -z;
        w = -w;
        return *static_cast<impl_type*>(this);
    }

    impl_type& add(const impl_type& val)
    {
        x += val.x;
        y += val.y;
        z += val.z;
        w += val.w;
        return *static_cast<impl_type*>(this);
    }

    /**
     * Subtracts this vector and the specified vector as (this - v)
     * and stores the result in this vector.
     *
     * @param val The vector to subtract.
     */
    impl_type& subtract(const impl_type& val)
    {
        x -= val.x;
        y -= val.y;
        z -= val.z;
        w -= val.w;
        return *static_cast<impl_type*>(this);
    }

    impl_type& scale(float factor)
    {
        x *= factor;
        y *= factor;
        z *= factor;
        w *= factor;
        return *static_cast<impl_type*>(this);
    }

    inline impl_type& operator-() { return impl_type{*static_cast<impl_type*>(this)}.negate(); }

    /**
     * Adds the given vector to this vector.
     *
     * @param val The vector to add.
     * @return This vector, after the addition occurs.
     */
    inline impl_type& operator+=(const impl_type& val) { return this->add(val); }

    /**
     * Subtracts the given vector from this vector.
     *
     * @param val The vector to subtract.
     * @return This vector, after the subtraction occurs.
     */
    inline impl_type& operator-=(const impl_type& val) { return this->subtract(val); }

    /**
     * Scales this vector by the given value.
     *
     * @param factor The value to scale by.
     * @return This vector, after the scale occurs.
     */
    inline impl_type& operator*=(float factor)
    {
        scale(factor);
        return *static_cast<impl_type*>(this);
    }

    /**
     * Scales this vector by the given value.
     *
     * @param factor The value to scale by.
     * @return This vector, after the scale occurs.
     */
    inline impl_type& operator*=(const impl_type& factor)
    {
        this->x *= factor.x;
        this->y *= factor.y;
        this->z *= factor.z;
        this->w *= factor.w;
        return *static_cast<impl_type*>(this);
    }

    /**
     * Divide this vector by the given value.
     *
     * @param factor The value to divide by.
     * @return This vector, after the scale occurs.
     */
    inline impl_type& operator/=(float factor)
    {
        this->x /= factor;
        this->y /= factor;
        this->z /= factor;
        this->w /= factor;
        return *static_cast<impl_type*>(this);
    }

    /**
     * Divide this vector by the given value.
     *
     * @param factor The value to divide by.
     * @return This vector, after the divide occurs.
     */
    inline impl_type& operator/=(const impl_type& factor)
    {
        this->x /= factor.x;
        this->y /= factor.y;
        this->z /= factor.z;
        this->w /= factor.w;
        return *static_cast<impl_type*>(this);
    }

    /**
     * Determines if this vector is equal to the given vector.
     *
     * @param v The vector to compare against.
     *
     * @return True if this vector is equal to the given vector, false otherwise.
     */
    friend inline bool operator==(const impl_type& lhs, const impl_type& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
    }

    /**
     * Determines if this vector is not equal to the given vector.
     *
     * @param v The vector to compare against.
     *
     * @return True if this vector is not equal to the given vector, false otherwise.
     */
    friend inline bool operator!=(const impl_type& lhs, const impl_type& rhs)
    {
        return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z || lhs.w != rhs.w;
    }

    friend inline impl_type operator+(const impl_type& lhs, const impl_type& rhs) { return impl_type{lhs} += rhs; }

    friend inline impl_type operator-(const impl_type& lhs, const impl_type& rhs) { return impl_type{lhs} -= rhs; }

    friend inline impl_type operator*(const impl_type& lhs, float s) { return impl_type{lhs} *= s; }

    friend inline impl_type operator*(float x, const impl_type& v) { return impl_type{v} *= x; }

    friend inline impl_type operator/(const impl_type& lhs, const float s) { return impl_type{lhs} /= s; }
};

/**
 * Defines 4-element floating point vector.
 */
class AX_DLL Vec4 : public Vec4Adapter<Vec4>
{
    using MyBase = Vec4Adapter<Vec4>;

public:
    /**
     * Constructs a new vector initialized to all zeros.
     */
    constexpr Vec4() {}

    /**
     * Constructs a new vector initialized to the specified values.
     *
     * @param xx The x coordinate.
     * @param yy The y coordinate.
     * @param zz The z coordinate.
     * @param ww The w coordinate.
     */
    constexpr Vec4(float xx, float yy, float zz, float ww) : MyBase(xx, yy, zz, ww) {}

    /**
     * Constructs a new vector from the values in the specified array.
     *
     * @param array An array containing the elements of the vector in the order x, y, z, w.
     */
    constexpr explicit Vec4(const float* array) : MyBase(array) {}

    /**
     * Constructs a vector that describes the direction between the specified points.
     *
     * @param p1 The first point.
     * @param p2 The second point.
     */
    constexpr Vec4(const Vec4& p1, const Vec4& p2) { setDirection(p1, p2); }

    /**
     * Indicates whether this vector contains all zeros.
     *
     * @return true if this vector contains all zeros, false otherwise.
     */
    bool isZero() const;

    /**
     * Indicates whether this vector contains all ones.
     *
     * @return true if this vector contains all ones, false otherwise.
     */
    bool isOne() const;

    /**
     * Returns the angle (in radians) between the specified vectors.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     *
     * @return The angle between the two vectors (in radians).
     */
    static float angle(const Vec4& v1, const Vec4& v2);

    /**
     * Adds the specified vectors and stores the result in dst.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst A vector to store the result in.
     */
    static void add(const Vec4& v1, const Vec4& v2, Vec4* dst);

    /**
     * Returns the distance between this vector and val.
     *
     * @param val The other vector.
     *
     * @return The distance between this vector and val.
     *
     * @see distanceSquared
     */
    float distance(const Vec4& val) const;

    /**
     * Returns the squared distance between this vector and val.
     *
     * When it is not necessary to get the exact distance between
     * two vectors (for example, when simply comparing the
     * distance between different vectors), it is advised to use
     * this method instead of distance.
     *
     * @param val The other vector.
     *
     * @return The squared distance between this vector and val.
     *
     * @see distance
     */
    float distanceSquared(const Vec4& val) const;

    /**
     * Returns the dot product of this vector and the specified vector.
     *
     * @param val The vector to compute the dot product with.
     *
     * @return The dot product.
     */
    float dot(const Vec4& val) const;

    /**
     * Returns the dot product between the specified vectors.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     *
     * @return The dot product between the vectors.
     */
    static float dot(const Vec4& v1, const Vec4& v2);

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
    float lengthSquared() const;

    /**
     * Normalizes this vector.
     *
     * This method normalizes this Vec4 so that it is of
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
    Vec4 getNormalized() const;

    /**
     * Sets this vector to the directional vector between the specified points.
     *
     * @param p1 The first point.
     * @param p2 The second point.
     */
    constexpr void setDirection(const Vec4& p1, const Vec4& p2)
    {
        x = p2.x - p1.x;
        y = p2.y - p1.y;
        z = p2.z - p1.z;
        w = p2.w - p1.w;
    }

    /**
     * Subtracts the specified vectors and stores the result in dst.
     * The resulting vector is computed as (v1 - v2).
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst The destination vector.
     */
    static void subtract(const Vec4& v1, const Vec4& v2, Vec4* dst);

    /** equals to Vec4(0,0,0,0) */
    static const Vec4 zero;
    /** equals to Vec4(1,1,1,1) */
    static const Vec4 one;
};

#if !(defined(AX_DLLEXPORT) || defined(AX_DLLIMPORT))
inline constexpr Vec4 Vec4::zero{0.0f, 0.0f, 0.0f, 0.0f};
inline constexpr Vec4 Vec4::one{1.0f, 1.0f, 1.0f, 1.0f};
#endif

NS_AX_MATH_END
/**
 end of base group
 @}
 */
