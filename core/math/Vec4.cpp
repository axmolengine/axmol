/**
 Copyright 2013 BlackBerry Inc.
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

#include "math/Vec4.h"

#include <cmath>
#include "math/MathUtil.h"
#include "base/Macros.h"

NS_AX_MATH_BEGIN

#if defined(AX_DLLEXPORT) || defined(AX_DLLIMPORT)
    const Vec4 Vec4::ZERO   = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
    const Vec4 Vec4::ONE    = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    const Vec4 Vec4::UNIT_X = Vec4(1.0f, 0.0f, 0.0f, 0.0f);
    const Vec4 Vec4::UNIT_Y = Vec4(0.0f, 1.0f, 0.0f, 0.0f);
    const Vec4 Vec4::UNIT_Z = Vec4(0.0f, 0.0f, 1.0f, 0.0f);
    const Vec4 Vec4::UNIT_W = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
#endif

    
void Vec4Base::clamp(const Vec4Base& min, const Vec4Base& max)
{
    AX_ASSERT(!(min.x > max.x || min.y > max.y || min.z > max.z || min.w > max.w));

    // Clamp the x value.
    if (x < min.x)
        x = min.x;
    if (x > max.x)
        x = max.x;

    // Clamp the y value.
    if (y < min.y)
        y = min.y;
    if (y > max.y)
        y = max.y;

    // Clamp the z value.
    if (z < min.z)
        z = min.z;
    if (z > max.z)
        z = max.z;

    // Clamp the z value.
    if (w < min.w)
        w = min.w;
    if (w > max.w)
        w = max.w;
}

void Vec4Base::clamp(const Vec4Base& v, const Vec4Base& min, const Vec4Base& max, Vec4Base* dst)
{
    AX_ASSERT(dst);
    AX_ASSERT(!(min.x > max.x || min.y > max.y || min.z > max.z || min.w > max.w));

    // Clamp the x value.
    dst->x = v.x;
    if (dst->x < min.x)
        dst->x = min.x;
    if (dst->x > max.x)
        dst->x = max.x;

    // Clamp the y value.
    dst->y = v.y;
    if (dst->y < min.y)
        dst->y = min.y;
    if (dst->y > max.y)
        dst->y = max.y;

    // Clamp the z value.
    dst->z = v.z;
    if (dst->z < min.z)
        dst->z = min.z;
    if (dst->z > max.z)
        dst->z = max.z;

    // Clamp the w value.
    dst->w = v.w;
    if (dst->w < min.w)
        dst->w = min.w;
    if (dst->w > max.w)
        dst->w = max.w;
}

bool Vec4::isZero() const
{
    return x == 0.0f && y == 0.0f && z == 0.0f && w == 0.0f;
}

bool Vec4::isOne() const
{
    return x == 1.0f && y == 1.0f && z == 1.0f && w == 1.0f;
}

float Vec4::angle(const Vec4& v1, const Vec4& v2)
{
    float dx = v1.w * v2.x - v1.x * v2.w - v1.y * v2.z + v1.z * v2.y;
    float dy = v1.w * v2.y - v1.y * v2.w - v1.z * v2.x + v1.x * v2.z;
    float dz = v1.w * v2.z - v1.z * v2.w - v1.x * v2.y + v1.y * v2.x;

    return std::atan2(std::sqrt(dx * dx + dy * dy + dz * dz) + MATH_FLOAT_SMALL, dot(v1, v2));
}

void Vec4::add(const Vec4& v1, const Vec4& v2, Vec4* dst)
{
    AX_ASSERT(dst);

    dst->x = v1.x + v2.x;
    dst->y = v1.y + v2.y;
    dst->z = v1.z + v2.z;
    dst->w = v1.w + v2.w;
}

float Vec4::distance(const Vec4& v) const
{
    float dx = v.x - x;
    float dy = v.y - y;
    float dz = v.z - z;
    float dw = v.w - w;

    return std::sqrt(dx * dx + dy * dy + dz * dz + dw * dw);
}

float Vec4::distanceSquared(const Vec4& v) const
{
    float dx = v.x - x;
    float dy = v.y - y;
    float dz = v.z - z;
    float dw = v.w - w;

    return (dx * dx + dy * dy + dz * dz + dw * dw);
}

float Vec4::dot(const Vec4& v) const
{
    return (x * v.x + y * v.y + z * v.z + w * v.w);
}

float Vec4::dot(const Vec4& v1, const Vec4& v2)
{
    return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w);
}

float Vec4::length() const
{
    return std::sqrt(x * x + y * y + z * z + w * w);
}

float Vec4::lengthSquared() const
{
    return (x * x + y * y + z * z + w * w);
}

void Vec4::normalize()
{
    float n = x * x + y * y + z * z + w * w;
    // Already normalized.
    if (n == 1.0f)
        return;

    n = std::sqrt(n);
    // Too close to zero.
    if (n < MATH_TOLERANCE)
        return;

    n = 1.0f / n;
    x *= n;
    y *= n;
    z *= n;
    w *= n;
}

Vec4 Vec4::getNormalized() const
{
    Vec4 v(*this);
    v.normalize();
    return v;
}

void Vec4::subtract(const Vec4& v1, const Vec4& v2, Vec4* dst)
{
    AX_ASSERT(dst);

    dst->x = v1.x - v2.x;
    dst->y = v1.y - v2.y;
    dst->z = v1.z - v2.z;
    dst->w = v1.w - v2.w;
}

NS_AX_MATH_END
