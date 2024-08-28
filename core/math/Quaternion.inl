/**
 Copyright 2013 BlackBerry Inc.

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

#include "base/Macros.h"
#include "math/Quaternion.h"

NS_AX_MATH_BEGIN

inline constexpr Quaternion::Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}

inline constexpr Quaternion::Quaternion(float xx, float yy, float zz, float ww) : x(xx), y(yy), z(zz), w(ww) {}

inline constexpr Quaternion::Quaternion(float* array)
{
    set(array);
}

inline Quaternion::Quaternion(const Vec3& axis, float angle)
{
    set(axis, angle);
}

inline const Quaternion& Quaternion::identity()
{
    static Quaternion value(0.0f, 0.0f, 0.0f, 1.0f);
    return value;
}

inline const Quaternion& Quaternion::zero()
{
    static Quaternion value(0.0f, 0.0f, 0.0f, 0.0f);
    return value;
}

inline void Quaternion::createFromAxisAngle(const Vec3& axis, float angle, Quaternion* dst)
{
    GP_ASSERT(dst);

    float halfAngle    = angle * 0.5f;
    float sinHalfAngle = sinf(halfAngle);

    Vec3 normal(axis);
    normal.normalize();
    dst->x = normal.x * sinHalfAngle;
    dst->y = normal.y * sinHalfAngle;
    dst->z = normal.z * sinHalfAngle;
    dst->w = cosf(halfAngle);
}

inline constexpr void Quaternion::set(float xx, float yy, float zz, float ww)
{
    this->x = xx;
    this->y = yy;
    this->z = zz;
    this->w = ww;
}

inline constexpr void Quaternion::set(float* array)
{
    GP_ASSERT(array);

    x = array[0];
    y = array[1];
    z = array[2];
    w = array[3];
}

inline constexpr void Quaternion::set(const Quaternion& q)
{
    this->x = q.x;
    this->y = q.y;
    this->z = q.z;
    this->w = q.w;
}

inline void Quaternion::set(const Vec3& axis, float angle)
{
    Quaternion::createFromAxisAngle(axis, angle, this);
}

inline void Quaternion::setIdentity()
{
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    w = 1.0f;
}

inline Quaternion Quaternion::operator*(const Quaternion& q) const
{
    Quaternion result(*this);
    result.multiply(q);
    return result;
}

inline Quaternion& Quaternion::operator*=(const Quaternion& q)
{
    multiply(q);
    return *this;
}

inline Vec3 Quaternion::operator*(const Vec3& v) const
{
    Vec3 uv, uuv;
    Vec3 qvec(x, y, z);
    Vec3::cross(qvec, v, &uv);
    Vec3::cross(qvec, uv, &uuv);

    uv *= (2.0f * w);
    uuv *= 2.0f;

    return v + uv + uuv;
}

inline constexpr Quaternion Quaternion::ZERO(0.0f, 0.0f, 0.0f, 0.0f);

NS_AX_MATH_END
