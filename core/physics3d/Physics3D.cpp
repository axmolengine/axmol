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

#include "physics3d/Physics3D.h"

#if defined(AX_ENABLE_3D_PHYSICS)

#    if (AX_ENABLE_BULLET_INTEGRATION)

namespace ax
{

AX_DLL const char* physics3dVersion()
{
#        if AX_ENABLE_BULLET_INTEGRATION
    return "bullet2.82";
#        endif
}

}

ax::Vec3 convertbtVector3ToVec3(const btVector3& btVec3)
{
    return ax::Vec3(btVec3.x(), btVec3.y(), btVec3.z());
}

btVector3 convertVec3TobtVector3(const ax::Vec3& vec3)
{
    return btVector3(vec3.x, vec3.y, vec3.z);
}

ax::Mat4 convertbtTransformToMat4(const btTransform& btTrans)
{
    ax::Mat4 mat;
    auto rot  = btTrans.getBasis();
    auto row  = rot.getRow(0);
    mat.m[0]  = row.getX();
    mat.m[4]  = row.getY();
    mat.m[8]  = row.getZ();
    row       = rot.getRow(1);
    mat.m[1]  = row.getX();
    mat.m[5]  = row.getY();
    mat.m[9]  = row.getZ();
    row       = rot.getRow(2);
    mat.m[2]  = row.getX();
    mat.m[6]  = row.getY();
    mat.m[10] = row.getZ();

    row       = btTrans.getOrigin();
    mat.m[12] = row.getX();
    mat.m[13] = row.getY();
    mat.m[14] = row.getZ();
    return mat;
}

btTransform convertMat4TobtTransform(const ax::Mat4& mat4)
{
    btTransform btTrans;
    btTrans.setFromOpenGLMatrix(mat4.m);
    return btTrans;
}

ax::Quaternion convertbtQuatToQuat(const btQuaternion& btQuat)
{
    return ax::Quaternion(btQuat.x(), btQuat.y(), btQuat.z(), btQuat.w());
}

btQuaternion convertQuatTobtQuat(const ax::Quaternion& quat)
{
    return btQuaternion(quat.x, quat.y, quat.z, quat.w);
}

#    endif  // AX_ENABLE_BULLET_INTEGRATION

#endif  // defined(AX_ENABLE_3D_PHYSICS)
