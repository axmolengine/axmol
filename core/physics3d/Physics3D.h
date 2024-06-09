/****************************************************************************
 Copyright (c) 2015-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#ifndef __PHYSICS_3D_H__
#define __PHYSICS_3D_H__

#include "base/Config.h"
#include "math/Math.h"

#if defined(AX_ENABLE_3D_PHYSICS)

#    include "physics3d/Physics3DShape.h"
#    include "physics3d/PhysicsMeshRenderer.h"
#    include "physics3d/Physics3DWorld.h"
#    include "physics3d/Physics3DDebugDrawer.h"
#    include "physics3d/Physics3DObject.h"
#    include "physics3d/Physics3DComponent.h"
#    include "physics3d/Physics3DConstraint.h"

NS_AX_BEGIN

AX_DLL const char* physics3dVersion();

NS_AX_END

#    if (AX_ENABLE_BULLET_INTEGRATION)

// include bullet header files
#        include "bullet/LinearMath/btTransform.h"
#        include "bullet/LinearMath/btVector3.h"
#        include "bullet/LinearMath/btQuaternion.h"

#        include "bullet/btBulletCollisionCommon.h"
#        include "bullet/btBulletDynamicsCommon.h"
#        include "bullet/BulletCollision/CollisionDispatch/btGhostObject.h"

// convert between cocos and bullet
ax::Vec3 convertbtVector3ToVec3(const btVector3& btVec3);
btVector3 convertVec3TobtVector3(const ax::Vec3& vec3);
ax::Mat4 convertbtTransformToMat4(const btTransform& btTrans);
btTransform convertMat4TobtTransform(const ax::Mat4& mat4);
ax::Quaternion convertbtQuatToQuat(const btQuaternion& btQuat);
btQuaternion convertQuatTobtQuat(const ax::Quaternion& quat);

#    endif  // AX_ENABLE_BULLET_INTEGRATION

#endif  // defined(AX_ENABLE_3D_PHYSICS)

#endif  // __PHYSICS_3D_H__
