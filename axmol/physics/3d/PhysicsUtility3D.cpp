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

#include "axmol/physics/3d/PhysicsUtility3D.h"

#if defined(AX_ENABLE_PHYSICS_3D)

namespace ax
{
namespace jphutil
{
JPH::Mat44 cast(const ax::Mat4& mat4)
{
    return JPH::Mat44(JPH::Vec4(mat4.m[0], mat4.m[1], mat4.m[2], mat4.m[3]),
                      JPH::Vec4(mat4.m[4], mat4.m[5], mat4.m[6], mat4.m[7]),
                      JPH::Vec4(mat4.m[8], mat4.m[9], mat4.m[10], mat4.m[11]),
                      JPH::Vec4(mat4.m[12], mat4.m[13], mat4.m[14], mat4.m[15]));
}

ax::Mat4 cast(const JPH::Mat44& mat4)
{
    const JPH::Vec4 c0 = mat4.GetColumn4(0);
    const JPH::Vec4 c1 = mat4.GetColumn4(1);
    const JPH::Vec4 c2 = mat4.GetColumn4(2);
    const JPH::Vec4 c3 = mat4.GetColumn4(3);

    ax::Mat4 mat = ax::Mat4::IDENTITY;
    mat.m[0]     = c0.GetX();
    mat.m[1]     = c0.GetY();
    mat.m[2]     = c0.GetZ();
    mat.m[3]     = c0.GetW();
    mat.m[4]     = c1.GetX();
    mat.m[5]     = c1.GetY();
    mat.m[6]     = c1.GetZ();
    mat.m[7]     = c1.GetW();
    mat.m[8]     = c2.GetX();
    mat.m[9]     = c2.GetY();
    mat.m[10]    = c2.GetZ();
    mat.m[11]    = c2.GetW();
    mat.m[12]    = c3.GetX();
    mat.m[13]    = c3.GetY();
    mat.m[14]    = c3.GetZ();
    mat.m[15]    = c3.GetW();
    return mat;
}

void stripScaleFromTransform(Mat4& mat)
{
    float oneOverLen = 1.f / sqrtf(mat.m[0] * mat.m[0] + mat.m[1] * mat.m[1] + mat.m[2] * mat.m[2]);
    mat.m[0] *= oneOverLen;
    mat.m[1] *= oneOverLen;
    mat.m[2] *= oneOverLen;

    oneOverLen = 1.f / sqrtf(mat.m[4] * mat.m[4] + mat.m[5] * mat.m[5] + mat.m[6] * mat.m[6]);
    mat.m[4] *= oneOverLen;
    mat.m[5] *= oneOverLen;
    mat.m[6] *= oneOverLen;

    oneOverLen = 1.f / sqrtf(mat.m[8] * mat.m[8] + mat.m[9] * mat.m[9] + mat.m[10] * mat.m[10]);
    mat.m[8] *= oneOverLen;
    mat.m[9] *= oneOverLen;
    mat.m[10] *= oneOverLen;
}

}  // namespace jphutil

}  // namespace ax

#endif
