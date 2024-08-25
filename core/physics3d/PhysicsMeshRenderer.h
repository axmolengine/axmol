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

#ifndef __PHYSICS_MESH_RENDERER_H__
#define __PHYSICS_MESH_RENDERER_H__

#include "base/Config.h"
#include "3d/MeshRenderer.h"
#include "physics3d/Physics3DObject.h"
#include "physics3d/Physics3DComponent.h"

#if defined(AX_ENABLE_3D_PHYSICS)

#    if (AX_ENABLE_BULLET_INTEGRATION)

namespace ax
{
/**
 * @addtogroup _3d
 * @{
 */

/**
 * @brief Convenient class to create a rigid body with a MeshRenderer
 */
class AX_DLL PhysicsMeshRenderer : public ax::MeshRenderer
{
public:
    /** creates a PhysicsMeshRenderer */
    static PhysicsMeshRenderer* create(std::string_view modelPath,
                                   Physics3DRigidBodyDes* rigidDes,
                                   const ax::Vec3& translateInPhysics = ax::Vec3::ZERO,
                                   const ax::Quaternion& rotInPhsyics = ax::Quaternion::ZERO);

    /** creates a PhysicsMeshRenderer with a collider */
    static PhysicsMeshRenderer* createWithCollider(std::string_view modelPath,
                                               Physics3DColliderDes* colliderDes,
                                               const ax::Vec3& translateInPhysics = ax::Vec3::ZERO,
                                               const ax::Quaternion& rotInPhsyics = ax::Quaternion::ZERO);

    /** Get the Physics3DObject. */
    Physics3DObject* getPhysicsObj() const;

    /** Set synchronization flag, see Physics3DComponent. */
    void setSyncFlag(Physics3DComponent::PhysicsSyncFlag syncFlag);

    /** synchronize node transformation to physics. */
    void syncNodeToPhysics();

    /** synchronize physics transformation to node. */
    void syncPhysicsToNode();

    PhysicsMeshRenderer();
    virtual ~PhysicsMeshRenderer();

protected:
    Physics3DComponent* _physicsComponent;
};

// end of 3d group
/// @}
}

#    endif  // AX_ENABLE_BULLET_INTEGRATION

#endif  // defined(AX_ENABLE_3D_PHYSICS)

#endif  // __PHYSICS_MESH_RENDERER_H__
