/****************************************************************************
 Copyright (c) 2015-2016 Chukong Technologies Inc.
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

#pragma once

#include "axmol/base/Config.h"
#include "axmol/math/Math.h"

#if defined(AX_ENABLE_PHYSICS_3D)

#    include <Jolt/Jolt.h>
#    include <Jolt/Math/Vec3.h>
#    include <Jolt/Math/Quat.h>
#    include <Jolt/Math/Mat44.h>

namespace ax
{
namespace jphutil
{
// Color
inline ax::Color cast(JPH::ColorArg v)
{
    return ax::Color{ax::Color32{v}};
}

// Vec3
inline JPH::Vec3 cast(const ax::Vec3& v)
{
    return {v.x, v.y, v.z};
}

inline ax::Vec3 cast(const JPH::Vec3& v)
{
    return {v.GetX(), v.GetY(), v.GetZ()};
}

inline ax::Vec3 cast(const JPH::Float3& v)
{
    return {v.x, v.y, v.z};
}

// Quat
inline JPH::Quat cast(const ax::Quat& q)
{
    return {q.x, q.y, q.z, q.w};
}

inline ax::Quat cast(const JPH::Quat& q)
{
    return {q.GetX(), q.GetY(), q.GetZ(), q.GetW()};
}

// Mat4
JPH::Mat44 cast(const ax::Mat4& m);
ax::Mat4 cast(const JPH::Mat44& m);

void stripScaleFromTransform(Mat4& mat);

}  // namespace jphutil

}  // namespace ax

#endif  // defined(AX_ENABLE_PHYSICS_3D)
