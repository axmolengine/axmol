/****************************************************************************
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
#include "axmol/physics/3d/Collider3D.h"
#include "axmol/tlx/bitmask.hpp"

#include <cstdint>
#include <functional>
#include <vector>

#if defined(AX_ENABLE_PHYSICS_3D)

#    include <Jolt/Jolt.h>
#    include <Jolt/Physics/Body/BodyID.h>
#    include <Jolt/Physics/Body/MotionType.h>

namespace ax
{
class Collider3D;
class Joint3D;
class PhysicsWorld3D;

class AX_DLL Rigidbody3D : public PhysicsActor
{
    friend class Joint3D;
    friend class PhysicsWorld3D;

public:
    /**
     * @brief Collision detection quality used by the rigid body.
     */
    enum class CollisionDetectionMode
    {
        Discrete,    ///< Uses discrete collision detection.
        Continuous,  ///< Uses continuous collision detection for fast-moving bodies.
    };

    /**
     * @brief Motion behavior used by the physics solver.
     */
    enum class MotionType
    {
        Unspec    = -1,                               ///< Motion type has not been explicitly configured.
        Static    = (int)JPH::EMotionType::Static,    ///< Immovable body that can collide with moving bodies.
        Dynamic   = (int)JPH::EMotionType::Dynamic,   ///< Body simulated by forces, impulses, and gravity.
        Kinematic = (int)JPH::EMotionType::Kinematic  ///< Body moved by user code while affecting dynamics.
    };

    /**
     * @brief Creates an autoreleased rigid body component.
     * @param collider Collider used to create the native body shape, or `nullptr` to attach one later.
     * @param mass Body mass in kilograms. Values greater than zero default to dynamic motion.
     * @return An autoreleased Rigidbody3D instance.
     */
    static Rigidbody3D* create(Collider3D* collider = nullptr, float mass = 0);

    /** @brief Destroys the rigid body and detaches it from the physics world. */
    ~Rigidbody3D() override;

    /**
     * @brief Enables or disables the rigid body component.
     * @param enabled True to attach or keep the body active, false to detach it.
     */
    void setEnabled(bool enabled) override;

    /** @brief Returns whether the native body is created and attached to world. */
    bool isAttached() const { return !_bodyID.IsInvalid() && _world; }

    /** @brief Returns the physics world that owns this body, or `nullptr` when detached. */
    PhysicsWorld3D* getWorld() const { return _world; }

    /**
     * @brief Returns the native Jolt body identifier.
     * @return Native Jolt body ID, or an invalid ID when detached.
     */
    [[internal]] JPH::BodyID internalHandle() const { return _bodyID; }

    /**
     * @brief Sets the mass of the body.
     * @param mass Mass value in kilograms.
     */
    void setMass(float mass);

    /**
     * @brief Gets the mass of the body.
     * @return Current mass in kilograms.
     */
    float getMass() const;

    /**
     * @brief Sets the inertia tensor of the body.
     * @param inertia Inertia vector.
     */
    void setInertia(const Vec3& inertia);

    /**
     * @brief Gets the inertia tensor of the body.
     * @return Inertia vector.
     */
    Vec3 getInertia() const;

    /**
     * @brief Sets both mass and inertia tensor.
     * @param mass Mass value.
     * @param inertia Inertia vector.
     */
    void setMassData(float mass, const Vec3& inertia);

    /**
     * @brief Sets the linear velocity of the body.
     * @param velocity Linear velocity vector.
     */
    void setLinearVelocity(const Vec3& velocity);

    /**
     * @brief Gets the linear velocity of the body.
     * @return Linear velocity vector.
     */
    Vec3 getLinearVelocity() const;

    /**
     * @brief Sets the angular velocity of the body.
     * @param velocity Angular velocity vector.
     */
    void setAngularVelocity(const Vec3& velocity);

    /**
     * @brief Gets the angular velocity of the body.
     * @return Angular velocity vector.
     */
    Vec3 getAngularVelocity() const;

    /**
     * @brief Sets the linear damping factor.
     * @param damping Linear damping coefficient.
     */
    void setLinearDamping(float damping);

    /**
     * @brief Gets the linear damping factor.
     * @return Linear damping coefficient.
     */
    float getLinearDamping() const;

    /**
     * @brief Sets the angular damping factor.
     * @param damping Angular damping coefficient.
     */
    void setAngularDamping(float damping);

    /**
     * @brief Gets the angular damping factor.
     * @return Angular damping coefficient.
     */
    float getAngularDamping() const;

    /**
     * @brief Gets the total force applied to the body.
     * @return Force vector.
     */
    Vec3 getTotalForce() const;

    /**
     * @brief Gets the total torque applied to the body.
     * @return Torque vector.
     */
    Vec3 getTotalTorque() const;

    /**
     * @brief Gets the physics material of the collider.
     * @return Physics material.
     */
    PhysicsMaterial getMaterial() const { return _collider ? _collider->getMaterial() : PhysicsMaterial{}; }

    /**
     * @brief Sets the physics material of the collider.
     * @param mat Physics material.
     */
    void setMaterial(const PhysicsMaterial& mat);

    /**
     * @brief Gets the collision detection mode.
     * @return Current collision detection mode.
     */
    CollisionDetectionMode getCollisionDetectionMode() const { return _collisionDetection; }

    /**
     * @brief Sets the collision detection mode.
     * @param mode Collision detection mode.
     */
    void setCollisionDetectionMode(CollisionDetectionMode mode);

    /**
     * @brief Sets the motion type to dynamic (affected by forces, gravity, collisions).
     */
    void setDynamic() { setMotionType(MotionType::Dynamic); }

    /**
     * @brief Checks if the body is dynamic.
     * @return True if motion type is dynamic.
     */
    bool isDynamic() const { return _motionType == MotionType::Dynamic; }

    /**
     * @brief Sets the motion type to kinematic (not affected by forces, moved by script/animation).
     */
    void setKinematic() { setMotionType(MotionType::Kinematic); }

    /**
     * @brief Checks if the body is kinematic.
     * @return True if motion type is kinematic.
     */
    bool isKinematic() const { return _motionType == MotionType::Kinematic; }

    /**
     * @brief Sets the motion type of the body.
     * @param motionType Motion type (e.g., Dynamic, Kinematic).
     */
    void setMotionType(MotionType motionType);

    /**
     * @brief Gets the current motion type of the body.
     * @return The motion type.
     */
    MotionType getMotionType() const { return _motionType; }

    /**
     * @brief Enables or disables sleeping for the body.
     * @param allow True to allow the body to sleep when inactive.
     */
    void setAllowSleeping(bool allow);

    /**
     * @brief Checks if the body is allowed to sleep.
     * @return True if sleeping is allowed, false otherwise.
     */
    bool isAllowSleeping() const { return _allowSleeping; }

    /**
     * @brief Sets the gravity scale factor.
     * @param scale Multiplier for global gravity (1.0 = normal gravity, 0 disables gravity).
     */
    void setGravityScale(float scale);

    /**
     * @brief Gets the current gravity scale factor.
     * @return The multiplier applied to global gravity.
     */
    float getGravityScale() const { return _gravityScale; }

    /**
     * @brief Enables or disables rotation along specific axes.
     *
     * This function sets which rotational degrees of freedom (DOFs) are permitted
     * for the rigidbody. Each axis can be individually enabled or disabled.
     *
     * @param axes Boolean vector indicating rotation state:
     *                - enabled.x : rotation around X axis
     *                - enabled.y : rotation around Y axis
     *                - enabled.z : rotation around Z axis
     *
     * Passing true allows rotation along that axis, false locks it.
     */
    void setRotationEnabled(bvec3 axes);

    /**
     * @brief Checks if rotation along the given axis is enabled.
     *
     * This function inspects the allowed degrees of freedom (DOFs) for the rigidbody
     * and returns whether rotation around the specified axis is currently permitted.
     *
     * @param axis Axis index to check:
     *             - 0: X axis
     *             - 1: Y axis
     *             - 2: Z axis
     *
     * @return True if rotation around the given axis is enabled, false otherwise.
     */
    bool isRotationEnabled(int axis) const;

    /**
     * @brief Apply a continuous force to the rigidbody.
     *
     * The force is applied at the given offset relative to the body's center of mass.
     * This will affect the linear velocity of the rigidbody.
     *
     * @param force The force vector to apply (in Newtons).
     * @param offset The position offset from the center of mass where the force is applied.
     *
     * @note This method only has an effect if the rigidbody is attached to the physics world
     *       (i.e., isAttached() returns true).
     */
    void applyForce(const Vec3& force, const Vec3& offset);

    /**
     * @brief Apply a continuous torque to the rigidbody.
     *
     * The torque will affect the angular velocity of the rigidbody.
     *
     * @param torque The torque vector to apply (in Newton-meters).
     *
     * @note This method only has an effect if the rigidbody is attached to the physics world
     *       (i.e., isAttached() returns true).
     */
    void applyTorque(const Vec3& torque);

    /**
     * @brief Apply an instantaneous linear impulse to the rigidbody.
     *
     * The impulse is applied at the given offset relative to the body's center of mass.
     * This will immediately change the linear velocity of the rigidbody.
     *
     * @param impulse The impulse vector to apply (in Newton-seconds).
     * @param offset The position offset from the center of mass where the impulse is applied.
     *
     * @note This method only has an effect if the rigidbody is attached to the physics world
     *       (i.e., isAttached() returns true).
     */
    void applyImpulse(const Vec3& impulse, const Vec3& offset);

    /**
     * @brief Apply an instantaneous angular impulse to the rigidbody.
     *
     * The angular impulse will immediately change the angular velocity of the rigidbody.
     *
     * @param impulse The angular impulse vector to apply (in kg·m²/s).
     *
     * @note This method only has an effect if the rigidbody is attached to the physics world
     *       (i.e., isAttached() returns true).
     */
    void applyAngularImpulse(const Vec3& impulse);

    /**
     * @brief Activate or deactivate the rigidbody in the physics simulation.
     *
     * This is a transient (instantaneous) operation that changes whether the body
     * is considered active by the physics world. An active body participates in
     * simulation and collision detection, while an inactive body is ignored.
     *
     * @param active True to activate the body, false to deactivate it.
     *
     * @note This method only has an effect if the rigidbody is attached to the physics world
     *       (i.e., isAttached() returns true).
     */
    void setActive(bool active);

    /** @brief Returns the collider that supplies this body's collision shape. */
    Collider3D* getCollider() const { return _collider; }

    /**
     * @brief Sets the transform offset between the owner node and the native body.
     * @param translateInPhysics Translation offset in the body's local physics space.
     * @param rotInPhysics Rotation offset in the body's local physics space.
     */
    void setTransformInPhysics(const Vec3& translateInPhysics, const Quat& rotInPhysics);

    /** @brief Pushes the owner node's transform into the native physics body. */
    void syncNodeToPhysics() override;

    /** @brief Applies the native physics body's transform to the owner node. */
    void syncPhysicsToNode() override;

    /**
     * @brief Returns the rigid body's current world transform.
     * @return World transform including the physics transform offset.
     */
    Mat4 getWorldTransform() const override;

    void setPosition(const Vec3& point, bool teleport = false);
    Vec3 getPosition() const;

    void setRotation(const Quat& rot, bool teleport = false);
    Quat getRotation() const;

    /** Convert the world point to local. */
    Vec3 world2Local(const Vec3& point);

    /** Convert the local point to world. */
    Vec3 local2World(const Vec3& point);

    /**
     * @brief Stores opaque caller-owned user data on this body.
     * @param userData Pointer value to associate with the body.
     */
    void setUserData(void* userData) { _userData = userData; }

    /** @brief Returns the opaque user data pointer assigned to this body. */
    void* getUserData() const { return _userData; }

protected:
    Rigidbody3D();

    bool init(Collider3D* collider, float mass);

    void invalidate();

    /** @brief Attaches the body when its owner node enters the active scene. */
    void onEnter() override;

    /** @brief Detaches the body when its owner node exits the active scene. */
    void onExit() override;

    /**
     * @brief Attaches this body to a physics world.
     * @param world Target world, or `nullptr` to use the owner scene's physics world.
     * @return True if the body is attached or was already attached.
     */
    bool attachToWorld(PhysicsWorld3D* world = nullptr);

    /** @brief Removes this body from its current physics world. */
    void detachFromWorld();

    /**
     * @brief Returns the current center-of-mass transform.
     * @return Center-of-mass transform in world space when attached, otherwise the cached transform.
     */
    Mat4 getCenterOfMassTransform() const;

    void addJoint(Joint3D* joint);
    void removeJoint(Joint3D* joint);

    uint32_t _allowedDOFs;

    JPH::BodyID _bodyID{};

    float _mass{0.0f};
    Vec3 _inertia{Vec3::zero};

    float _gravityScale{1.0f};

    Vec3 _linearVelocity{Vec3::zero};
    Vec3 _angularVelocity{Vec3::zero};
    float _linearDamping{0.0f};
    float _angularDamping{0.0f};

    bool _allowSleeping{true};
    MotionType _motionType{MotionType::Unspec};
    CollisionDetectionMode _collisionDetection{CollisionDetectionMode::Discrete};
    Mat4 _transformInPhysics;
    Mat4 _invTransformInPhysics;

    Collider3D* _collider{nullptr};
    PhysicsWorld3D* _world{nullptr};

    std::vector<Joint3D*> _joints;

    void* _userData{nullptr};
};

}  // namespace ax

#endif  // defined(AX_ENABLE_PHYSICS_3D)
