/**
 Copyright 2013 BlackBerry Inc.
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

 This file was modified to fit the axmol project
 */

#include "math/Mat4.h"

NS_AX_MATH_BEGIN

constexpr inline Mat4::Mat4()
{
    setIdentity();
}

constexpr inline Mat4::Mat4(float m11,
                     float m12,
                     float m13,
                     float m14,
                     float m21,
                     float m22,
                     float m23,
                     float m24,
                     float m31,
                     float m32,
                     float m33,
                     float m34,
                     float m41,
                     float m42,
                     float m43,
                     float m44)
{
    set(m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44);
}

inline Mat4::Mat4(const float* mat)
{
    set(mat);
}

inline Mat4::Mat4(const Mat4& copy)
{
    memcpy(m, copy.m, sizeof(m));
}

constexpr inline void Mat4::set(float m11,
                         float m12,
                         float m13,
                         float m14,
                         float m21,
                         float m22,
                         float m23,
                         float m24,
                         float m31,
                         float m32,
                         float m33,
                         float m34,
                         float m41,
                         float m42,
                         float m43,
                         float m44)
{
    m[0]  = m11;
    m[1]  = m21;
    m[2]  = m31;
    m[3]  = m41;
    m[4]  = m12;
    m[5]  = m22;
    m[6]  = m32;
    m[7]  = m42;
    m[8]  = m13;
    m[9]  = m23;
    m[10] = m33;
    m[11] = m43;
    m[12] = m14;
    m[13] = m24;
    m[14] = m34;
    m[15] = m44;
}

inline void Mat4::set(const float* mat)
{
    GP_ASSERT(mat);
    memcpy(m, mat, sizeof(m));
}

inline void Mat4::set(const Mat4& mat)
{
    memcpy(m, mat.m, sizeof(m));
}

inline constexpr void Mat4::setIdentity()
{
    set(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

inline void Mat4::setZero()
{
    memset(m, 0, sizeof(m));
}

inline Mat4 Mat4::operator+(const Mat4& mat) const
{
    Mat4 result(*this);
    result.add(mat);
    return result;
}

inline Mat4& Mat4::operator+=(const Mat4& mat)
{
    add(mat);
    return *this;
}

inline Mat4 Mat4::operator-(const Mat4& mat) const
{
    Mat4 result(*this);
    result.subtract(mat);
    return result;
}

inline Mat4& Mat4::operator-=(const Mat4& mat)
{
    subtract(mat);
    return *this;
}

inline Mat4 Mat4::operator-() const
{
    Mat4 mat(*this);
    mat.negate();
    return mat;
}

inline Mat4 Mat4::operator*(const Mat4& mat) const
{
    Mat4 result(*this);
    result.multiply(mat);
    return result;
}

inline Mat4& Mat4::operator*=(const Mat4& mat)
{
    multiply(mat);
    return *this;
}

inline Vec3& operator*=(Vec3& v, const Mat4& m)
{
    m.transformVector(&v);
    return v;
}

inline Vec3 operator*(const Mat4& m, const Vec3& v)
{
    Vec3 x;
    m.transformVector(v, &x);
    return x;
}

inline Vec4& operator*=(Vec4& v, const Mat4& m)
{
    m.transformVector(&v);
    return v;
}

inline Vec4 operator*(const Mat4& m, const Vec4& v)
{
    Vec4 x;
    m.transformVector(v, &x);
    return x;
}

inline constexpr Mat4 Mat4::IDENTITY(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
inline constexpr Mat4 Mat4::ZERO(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

NS_AX_MATH_END
