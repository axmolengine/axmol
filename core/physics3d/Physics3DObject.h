/****************************************************************************
 Copyright (c) 2015-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmolengine.github.io/

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

#ifndef __PHYSICS_3D_OBJECT_H__
#define __PHYSICS_3D_OBJECT_H__

#include "math/Math.h"
#include "base/Object.h"
#include "base/Config.h"

#include <vector>

#if defined(AX_ENABLE_3D_PHYSICS)

#    if (AX_ENABLE_BULLET_INTEGRATION)

class btCollisionShape;
class btRigidBody;
class btPersistentManifold;
class btGhostObject;

NS_AX_BEGIN
/**
 * @addtogroup _3d
 * @{
 */

class Physics3DShape;
class Physics3DWorld;
class Physics3DConstraint;
class Physics3DObject;
/**
 * @brief The collision information of Physics3DObject.
 */
struct AX_DLL Physics3DCollisionInfo
{
    struct CollisionPoint
    {
        Vec3 localPositionOnA;
        Vec3 worldPositionOnA;
        Vec3 localPositionOnB;
        Vec3 worldPositionOnB;
        Vec3 worldNormalOnB;
    };

    Physics3DObject* objA;
    Physics3DObject* objB;
    std::vector<CollisionPoint> collisionPointList;
};
/**
 * @brief Inherit from Object, base class
 */
class AX_DLL Physics3DObject : public Object
{
public:
    typedef std::function<void(const Physics3DCollisionInfo& ci)> CollisionCallbackFunc;

    enum class PhysicsObjType
    {
        UNKNOWN = 0,
        RIGID_BODY,
        COLLIDER,
    };

    /** Get the Physics3DObject Type. */
    virtual PhysicsObjType getObjType() const { return _type; }

    /** Set the user data. */
    void setUserData(void* userData) { _userData = userData; }

    /** Get the user data. */
    void* getUserData() const { return _userData; }

    /** Internal method. Set the pointer of Physics3DWorld. */
    void setPhysicsWorld(Physics3DWorld* world) { _physicsWorld = world; };

    /** Get the pointer of Physics3DWorld. */
    Physics3DWorld* getPhysicsWorld() const { return _physicsWorld; }

    /** Get the world matrix of Physics3DObject. */
    virtual ax::Mat4 getWorldTransform() const = 0;

    /** Set the collision callback function. */
    void setCollisionCallback(const CollisionCallbackFunc& func) { _collisionCallbackFunc = func; };

    /** Get the collision callback function. */
    const CollisionCallbackFunc& getCollisionCallback() const { return _collisionCallbackFunc; }

    /** Check has collision callback function. */
    bool needCollisionCallback() { return _collisionCallbackFunc != nullptr; };

    /** Set the mask of Physics3DObject. */
    void setMask(unsigned int mask) { _mask = mask; };

    /** Get the mask of Physics3DObject. */
    unsigned int getMask() const { return _mask; };

    Physics3DObject()
        : _isEnabled(true), _type(PhysicsObjType::UNKNOWN), _userData(nullptr), _physicsWorld(nullptr), _mask(-1)
    {}
    virtual ~Physics3DObject() {}

protected:
    bool _isEnabled;
    PhysicsObjType _type;
    void* _userData;
    Physics3DWorld* _physicsWorld;
    CollisionCallbackFunc _collisionCallbackFunc;
    unsigned int _mask;
};

/**
 * @brief The description of Physics3DRigidBody.
 */
struct AX_DLL Physics3DRigidBodyDes
{
    float mass;                  // Note: mass equals zero means static, default 0
    ax::Vec3 localInertia;  // default (0, 0, 0)
    Physics3DShape* shape;
    ax::Mat4 originalTransform;
    bool disableSleep;  // it is always active if disabled

    Physics3DRigidBodyDes() : mass(0.f), localInertia(0.f, 0.f, 0.f), shape(nullptr), disableSleep(false) {}
};

/**
 * @brief Inherit from Physics3DObject, the main class for rigid body objects
 */
class AX_DLL Physics3DRigidBody : public Physics3DObject
{
    friend class Physics3DWorld;

public:
    /**
     * Creates a Physics3DRigidBody with Physics3DRigidBody.
     *
     * @return An autoreleased Physics3DRigidBody object.
     */
    static Physics3DRigidBody* create(Physics3DRigidBodyDes* info);

    /** Get the pointer of btRigidBody. */
    btRigidBody* getRigidBody() const { return _btRigidBody; }

    /**
     * Apply a force.
     *
     * @param   force the value of the force
     * @param   rel_pos the position of the force
     */
    void applyForce(const ax::Vec3& force, const ax::Vec3& rel_pos);

    /**
     * Apply a central force.
     *
     * @param   force the value of the force
     */
    void applyCentralForce(const ax::Vec3& force);

    /**
     * Apply a central impulse.
     *
     * @param   impulse the value of the impulse
     */
    void applyCentralImpulse(const ax::Vec3& impulse);

    /**
     * Apply a torque.
     *
     * @param   torque the value of the torque
     */
    void applyTorque(const ax::Vec3& torque);

    /**
     * Apply a torque impulse.
     *
     * @param   torque the value of the torque
     */
    void applyTorqueImpulse(const ax::Vec3& torque);

    /**
     * Apply a impulse.
     *
     * @param   impulse the value of the impulse
     * @param   rel_pos the position of the impulse
     */
    void applyImpulse(const ax::Vec3& impulse, const ax::Vec3& rel_pos);

    /** Damps the velocity, using the given linearDamping and angularDamping. */
    void applyDamping(float timeStep);

    /** Set the linear velocity. */
    void setLinearVelocity(const ax::Vec3& lin_vel);

    /** Get the linear velocity. */
    ax::Vec3 getLinearVelocity() const;

    /** Set the linear factor. */
    void setLinearFactor(const ax::Vec3& linearFactor);

    /** Get the linear factor. */
    ax::Vec3 getLinearFactor() const;

    /** Set the angular factor. */
    void setAngularFactor(const ax::Vec3& angFac);

    /** Set the angular factor, use unified factor. */
    void setAngularFactor(float angFac);

    /** Get the angular factor. */
    ax::Vec3 getAngularFactor() const;

    /** Set the angular velocity. */
    void setAngularVelocity(const ax::Vec3& ang_vel);

    /** Get the angular velocity. */
    ax::Vec3 getAngularVelocity() const;

    /** Set the center of mass. */
    void setCenterOfMassTransform(const ax::Mat4& xform);

    /** Get the center of mass. */
    ax::Mat4 getCenterOfMassTransform() const;

    /** Set linear damping and angular damping. */
    void setDamping(float lin_damping, float ang_damping);

    /** Get linear damping. */
    float getLinearDamping() const;

    /** Get angular damping. */
    float getAngularDamping() const;

    /** Set the acceleration. */
    void setGravity(const ax::Vec3& acceleration);

    /** Get the acceleration. */
    ax::Vec3 getGravity() const;

    /** Set the inverse of local inertia. */
    void setInvInertiaDiagLocal(const ax::Vec3& diagInvInertia);

    /** Get the inverse of local inertia. */
    ax::Vec3 getInvInertiaDiagLocal() const;

    /** Set mass and inertia. */
    void setMassProps(float mass, const ax::Vec3& inertia);

    /** Get inverse of mass. */
    float getInvMass() const;

    /** Get total force. */
    ax::Vec3 getTotalForce() const;

    /** Get total torque. */
    ax::Vec3 getTotalTorque() const;

    /** Set restitution. */
    void setRestitution(float rest);

    /** Get restitution. */
    float getRestitution() const;

    /** Set friction. */
    void setFriction(float frict);

    /** Get friction. */
    float getFriction() const;

    /** Set rolling friction. */
    void setRollingFriction(float frict);

    /** Get rolling friction. */
    float getRollingFriction() const;

    /** Set hit friction. */
    void setHitFraction(float hitFraction);

    /** Get hit friction. */
    float getHitFraction() const;

    /** Set motion threshold, don't do continuous collision detection if the motion (in one step) is less then
     * ccdMotionThreshold */
    void setCcdMotionThreshold(float ccdMotionThreshold);

    /** Get motion threshold. */
    float getCcdMotionThreshold() const;

    /** Set swept sphere radius. */
    void setCcdSweptSphereRadius(float radius);

    /** Get swept sphere radius. */
    float getCcdSweptSphereRadius() const;

    /** Set kinematic object. */
    void setKinematic(bool kinematic);

    /** Check rigid body is kinematic object. */
    bool isKinematic() const;

    /** override. */
    virtual ax::Mat4 getWorldTransform() const override;

    /** Get constraint by index. */
    Physics3DConstraint* getConstraint(unsigned int idx) const;

    /** Get the total number of constraints. */
    unsigned int getConstraintCount() const;

    /** Active or inactive. */
    void setActive(bool active);

    Physics3DRigidBody();
    virtual ~Physics3DRigidBody();

    bool init(Physics3DRigidBodyDes* info);

    void addConstraint(Physics3DConstraint* constraint);
    void removeConstraint(Physics3DConstraint* constraint);
    void removeConstraint(unsigned int idx);

protected:
    btRigidBody* _btRigidBody;
    Physics3DShape* _physics3DShape;
    std::vector<Physics3DConstraint*> _constraintList;
};

/**
 * @brief The description of Physics3DCollider.
 */
struct AX_DLL Physics3DColliderDes
{
    /**shape pointer*/
    Physics3DShape* shape;
    /**original world Transform*/
    ax::Mat4 originalTransform;
    /**Is collider a trigger?*/
    bool isTrigger;
    /**the friction*/
    float friction;
    /**the rolling friction*/
    float rollingFriction;
    /**the restitution*/
    float restitution;
    /**the hit fraction*/
    float hitFraction;
    /**the swept sphere radius*/
    float ccdSweptSphereRadius;
    /**the motion threshold*/
    float ccdMotionThreshold;

    Physics3DColliderDes()
        : shape(nullptr)
        , isTrigger(false)
        , friction(0.5f)
        , rollingFriction(0.0f)
        , restitution(0.0f)
        , hitFraction(1.0f)
        , ccdSweptSphereRadius(0.0f)
        , ccdMotionThreshold(0.0f)
    {}
};

/**
 * @brief Inherit from Physics3DObject, the main class for Colliders.
 */
class AX_DLL Physics3DCollider : public Physics3DObject
{
public:
    /**
     * Creates a Physics3DCollider with Physics3DColliderDes.
     *
     * @return An autoreleased Physics3DCollider object.
     */
    static Physics3DCollider* create(Physics3DColliderDes* info);

    /** Get the pointer of btGhostObject.
     *  @return The pointer of btGhostObject.
     */
    btGhostObject* getGhostObject() const { return _btGhostObject; }

    /** Set trigger.
     *  @param isTrigger Is a trigger.
     */
    void setTrigger(bool isTrigger);

    /** Check is a trigger.
     *  @return Is a trigger.
     */
    bool isTrigger() const;

    /** Set restitution.
     *  @param rest The restitution.
     */
    void setRestitution(float rest);

    /** Get restitution.
     *  @return The restitution.
     */
    float getRestitution() const;

    /** Set friction.
     *  @param frict The friction.
     */
    void setFriction(float frict);

    /** Get friction.
     *  @return The friction.
     */
    float getFriction() const;

    /** Set rolling friction.
     *  @param frict The rolling friction.
     */
    void setRollingFriction(float frict);

    /** Get rolling friction.
     *  @return The rolling friction.
     */
    float getRollingFriction() const;

    /** Set hit friction.
     *  @param hitFraction The hit friction.
     */
    void setHitFraction(float hitFraction);

    /** Get hit friction.
     *  @return The hit friction.
     */
    float getHitFraction() const;

    /** Set motion threshold, don't do continuous collision detection if the motion (in one step) is less then
     * ccdMotionThreshold.
     *  @param ccdMotionThreshold The motion threshold.
     */
    void setCcdMotionThreshold(float ccdMotionThreshold);

    /** Get motion threshold.
     *  @return The motion threshold.
     */
    float getCcdMotionThreshold() const;

    /** Set swept sphere radius.
     *  @param radius The swept sphere radius.
     */
    void setCcdSweptSphereRadius(float radius);

    /** Get swept sphere radius.
     *  @return The swept sphere radius.
     */
    float getCcdSweptSphereRadius() const;

    /** override. */
    virtual ax::Mat4 getWorldTransform() const;

    /** Set a callback when trigger enter. */
    std::function<void(Physics3DObject* otherObject)> onTriggerEnter;

    /** Set a callback when trigger exit. */
    std::function<void(Physics3DObject* otherObject)> onTriggerExit;

    Physics3DCollider();
    virtual ~Physics3DCollider();

    bool init(Physics3DColliderDes* info);

protected:
    btGhostObject* _btGhostObject;
    Physics3DShape* _physics3DShape;
};

// end of 3d group
/// @}

NS_AX_END

#    endif  // AX_ENABLE_BULLET_INTEGRATION

#endif  // defined(AX_ENABLE_3D_PHYSICS)

#endif  // __PHYSICS_3D_OBJECT_H__
