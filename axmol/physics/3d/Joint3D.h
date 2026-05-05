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
#include "axmol/scene/Component.h"

#include <optional>

#if defined(AX_ENABLE_PHYSICS_3D)

namespace JPH
{
class Constraint;
}

namespace ax
{

class PhysicsWorld3D;
class Rigidbody3D;

/** Range parameters for linear or angular joints. */
struct JointLimits
{
    float lower = 0.0f;  ///< Lower bound in linear units or degrees, depending on the joint type.
    float upper = 0.0f;  ///< Upper bound in linear units or degrees, depending on the joint type.
};

/** Cone-twist angular limit parameters in degrees. */
struct TwistLimits
{
    float swingSpan1{0};  ///< Swing span around the first swing axis, in degrees.
    float swingSpan2{0};  ///< Swing span around the second swing axis, in degrees.
    float twistSpan{0};   ///< Total twist span, in degrees.
};

/** Six degree-of-freedom linear or angular limits. */
struct SixDofLimits
{
    Vec3 lower{0.0f, 0.0f, 0.0f};  ///< Lower linear bounds or angular bounds in degrees.
    Vec3 upper{0.0f, 0.0f, 0.0f};  ///< Upper linear bounds or angular bounds in degrees.
};

/** Velocity motor parameters for joints. */
struct JointMotor
{
    /// Target speed in degrees per second for angular joints, or linear units per second for translational joints.
    float targetVelocity{0.0f};

    /// Maximum motor force or torque.
    float maxForce{0.0f};
};

/** Spring parameters used by soft limits or spring-based motors. */
struct JointSpring
{
    float frequency{2.0f};  ///< Spring frequency in Hz.
    float damping{1.0f};    ///< Damping ratio.
};

/**
 * Base class for 3D joints.
 * Joint connects the Rigidbody3D on the same node (bodyA) to another Rigidbody3D (bodyB).
 */
class AX_DLL Joint3D : public Component
{
    friend class PhysicsWorld3D;
    friend class Rigidbody3D;

public:
    /** Anchor coordinate space understood by Joint3D. */
    enum class CoordinateSpace
    {
        Local,  ///< Anchor and axis values are relative to each rigid body's center of mass.
        World,  ///< Anchor and axis values are expressed in world space.
    };

    /** Destructor. */
    ~Joint3D() override;

    /** Called when the component enters the active scene graph. */
    void onEnter() override;

    /** Called when the component exits the active scene graph. */
    void onExit() override;

    /** Returns whether the native physics constraint has been created and attached. */
    bool isAttached() const { return _constraint != nullptr; }

    /** Returns the physics world that currently owns the joint, or `nullptr` when detached. */
    PhysicsWorld3D* getWorld() const { return _world; }

    /** Returns the rigid body on the same node as this component. */
    Rigidbody3D* getAttachedBody() const { return _attachedBody; }

    /** Returns the other rigid body connected by this joint. */
    Rigidbody3D* getConnectedBody() const { return _connectedBody; }

    /**
     * Assigns the rigid body connected on the far side of the joint.
     *
     * @param body The connected rigid body. This can only be set once.
     */
    void setConnectedBody(Rigidbody3D* body);

    /** Returns the coordinate space used by anchor and axis values. */
    CoordinateSpace getCoordinateSpace() const { return _coordSpace; }

    /**
     * Sets the coordinate space used by anchors and axes.
     *
     * @param space The new anchor space.
     */
    void setCoordinateSpace(CoordinateSpace space) { _coordSpace = space; }

    /**
     * Sets both anchors to the same world-space position.
     *
     * @param anchor The shared world-space anchor point.
     */
    void setAnchorsInWorldSpace(const Vec3& anchor);

    /**
     * @brief Sets the joint anchors for both rigid bodies.
     *
     * The values are interpreted in the current coordinate space.
     *
     * @param anchor Anchor on the attached rigid body.
     * @param connectedAnchor Anchor on the connected rigid body.
     */
    void setAnchors(const Vec3& anchor, const Vec3& connectedAnchor)
    {
        setAnchor(anchor);
        setConnectedAnchor(connectedAnchor);
    }

    /** Returns the anchor on the attached body. */
    const Vec3& getAnchor() const { return _anchor; }

    /**
     * Sets the anchor on the attached body.
     *
     * @param anchor The anchor value in the current anchor space.
     */
    virtual void setAnchor(const Vec3& anchor);

    /** Returns the anchor on the connected body. */
    const Vec3& getConnectedAnchor() const { return _connectedAnchor; }

    /**
     * Sets the anchor on the connected body.
     *
     * @param anchor The anchor value in the current anchor space.
     */
    virtual void setConnectedAnchor(const Vec3& anchor);

    /**
     * Sets both axes to the same world-space position.
     *
     * @param axis The shared world-space axis.
     */
    void setAxesInWorldSpace(const Vec3& axis);

    /**
     * @brief Sets the joint axes for both rigid bodies.
     *
     * The values are interpreted in the current coordinate space.
     *
     * @param axis Axis on the attached rigid body.
     * @param connectedAxis Axis on the connected rigid body.
     */
    void setAxes(const Vec3& axis, const Vec3& connectedAxis)
    {
        setAxis(axis);
        setConnectedAxis(connectedAxis);
    }

    /** Returns the attached body's joint axis. */
    const Vec3& getAxis() const { return _axis; }

    /**
     * Sets the attached body's joint axis.
     *
     * @param axis The axis expressed in the current anchor space.
     */
    void setAxis(const Vec3& axis);

    /** Returns the secondary body's joint axis. */
    const Vec3& getConnectedAxis() const { return _connectedAxis; }

    /**
     * Sets the connected body's joint axis.
     *
     * @param axis The axis expressed in the current anchor space.
     */
    virtual void setConnectedAxis(const Vec3& axis);

    /** Returns the underlying native constraint pointer. */
    JPH::Constraint* internalHandle() const { return _constraint; }

    /**
     * Stores opaque user data on the joint component.
     *
     * @param userData Caller-owned user pointer.
     */
    void setUserData(void* userData) { _userData = userData; }

    /** Returns the opaque user data pointer assigned to this joint. */
    void* getUserData() const { return _userData; }

protected:
    Joint3D(Rigidbody3D* connectedBody);

    bool isBodyAttached(Rigidbody3D* body) const { return _attachedBody == body || _connectedBody == body; }
    void bindBody(Rigidbody3D*& targetMember, Rigidbody3D* newBody);

    virtual bool attachToBody() = 0;

    void detachFromBody();

    JPH::Constraint* _constraint{nullptr};

    CoordinateSpace _coordSpace{CoordinateSpace::Local};

    // anchor aka pivot
    Vec3 _anchor, _connectedAnchor;

    // euler axis
    Vec3 _axis, _connectedAxis;

    Rigidbody3D* _attachedBody{nullptr};  // automatically obtained from node onEnter
    Rigidbody3D* _connectedBody{nullptr};
    PhysicsWorld3D* _world{nullptr};

    void* _userData{nullptr};
};

// -------------------------------------------------------------------
// Fixed Joint
// -------------------------------------------------------------------
class AX_DLL FixedJoint3D : public Joint3D
{
public:
    /**
     * Creates a fixed joint.
     *
     * @param connectedBody The rigid body connected to the joint.
     * @return An autoreleased fixed joint.
     */
    static FixedJoint3D* create(Rigidbody3D* connectedBody = nullptr);

protected:
    FixedJoint3D(Rigidbody3D* connectedBody);
    bool attachToBody() override;
};

// -------------------------------------------------------------------
// Distance Joint
// -------------------------------------------------------------------
class AX_DLL DistanceJoint3D : public Joint3D
{
public:
    /**
     * Creates a distance joint.
     *
     * @param connectedBody The rigid body connected to the joint.
     * @return An autoreleased distance joint.
     */
    static DistanceJoint3D* create(Rigidbody3D* connectedBody = nullptr);

    /**
     * Sets an explicit minimum and maximum distance range.
     *
     * @param limits Distance range in world units.
     */
    void setLimits(const JointLimits& limits);

    /**
     * Clears the explicit range and reverts to keeping the current rest distance.
     */
    void clearLimits();

    /** Returns whether an explicit distance range is configured. */
    bool hasLimits() const { return _limits.has_value(); }

    /**
     * Returns the configured explicit distance range.
     *
     * @return The configured range, or a zeroed range when using the current rest distance.
     */
    const JointLimits& getLimits() const;

    /**
     * Sets the spring parameters used when the distance range is violated.
     *
     * @param spring Spring settings.
     */
    void setSpring(const JointSpring& spring);

    /** Returns the configured distance spring settings. */
    const JointSpring& getSpring() const { return _spring; }

protected:
    DistanceJoint3D(Rigidbody3D* connectedBody);
    bool attachToBody() override;

    std::optional<JointLimits> _limits;
    JointSpring _spring;
};

// -------------------------------------------------------------------
// Pivot Joint
// -------------------------------------------------------------------
class AX_DLL PivotJoint3D : public Joint3D
{
public:
    /**
     * Creates a pivot joint.
     *
     * @param connectedBody The rigid body connected to the joint.
     * @return An autoreleased pivot joint.
     */
    static PivotJoint3D* create(Rigidbody3D* connectedBody = nullptr);

    /**
     * Sets the pivot anchor on the attached body.
     *
     * @param anchor The anchor value in the current anchor space.
     */
    void setAnchor(const Vec3& anchor) override;

    /**
     * Sets the pivot anchor on the connected body.
     *
     * @param anchor The anchor value in the current anchor space.
     */
    void setConnectedAnchor(const Vec3& anchor) override;

protected:
    PivotJoint3D(Rigidbody3D* connectedBody);
    bool attachToBody() override;
};

// -------------------------------------------------------------------
// Hinge Joint
// -------------------------------------------------------------------
class AX_DLL HingeJoint3D : public Joint3D
{
public:
    /**
     * Creates a hinge joint.
     *
     * The joint uses local anchor and axis values by default. Configure them with
     * setAnchor(), setConnectedAnchor(), setAxis(), and setConnectedAxis().
     *
     * @param connectedBody The rigid body connected to the joint.
     * @return An autoreleased hinge joint.
     */
    static HingeJoint3D* create(Rigidbody3D* connectedBody = nullptr);

    /**
     * Creates a hinge joint with shared world-space anchor and axis values.
     *
     * @param anchor The shared anchor position.
     * @param axis The primary hinge axis.
     * @param connectedBody The rigid body connected to the joint.
     * @return An autoreleased hinge joint.
     */
    static HingeJoint3D* createInWorldSpace(const Vec3& anchor, const Vec3& axis, Rigidbody3D* connectedBody = nullptr);

    /**
     * Convenience wrapper for enabling or disabling the velocity motor.
     *
     * @param enabled `true` enables velocity mode, `false` disables the motor.
     */
    void setUseMotor(bool enabled);

    /** Returns whether the hinge velocity motor is enabled. */
    bool isUseMotor() const { return _useMotor; }

    /**
     * Sets the angular limit range in degrees.
     *
     * The hinge rest orientation is treated as zero. Valid ranges must satisfy
     * `lower <= 0 <= upper`.
     *
     * @param limits Angular limit range in degrees.
     */
    void setLimits(const JointLimits& limits);

    /**
     * Compatibility wrapper that enables or clears hinge limits.
     *
     * @param enabled `true` reapplies the cached limit range, `false` clears the limits.
     */
    void setUseLimits(bool enabled);

    /** Clears the explicit angular limits and restores free hinge rotation. */
    void clearLimits();

    /** Returns whether explicit hinge limits are configured. */
    bool hasLimits() const { return _limits.has_value(); }

    /**
     * Returns the configured hinge limits in degrees.
     *
     * @return The configured limit range, or a free-rotation range when no explicit limit is set.
     */
    const JointLimits& getLimits() const;

    /**
     * Sets hinge motor parameters.
     *
     * @param motor Motor settings in degrees per second and torque units.
     */
    void setMotor(const JointMotor& motor);

    /**
     * Returns the configured motor parameters.
     */
    JointMotor getMotor() const { return _motor; }

    /**
     * Sets the spring parameters used when the slider motor is violated.
     *
     * @param spring Spring settings.
     */
    void setSpring(const JointSpring& spring);

    /** Returns the configured slider motor spring settings. */
    const JointSpring& getSpring() const { return _spring; }

    /**
     * Returns the current relative angular velocity projected onto the hinge axis.
     *
     * @return Relative angular velocity in degrees per second.
     */
    float getVelocity() const;

    /**
     * Returns the current hinge angle relative to its rest orientation.
     *
     * @return Current angle in degrees.
     */
    float getAngle() const;

protected:
    HingeJoint3D(Rigidbody3D* connectedBody);

    bool attachToBody() override;

private:
    std::optional<JointLimits> _limits;
    JointMotor _motor;
    JointSpring _spring;
    bool _useMotor{false};
};

// -------------------------------------------------------------------
// Slider Joint
// -------------------------------------------------------------------
class AX_DLL SliderJoint3D : public Joint3D
{
public:
    /**
     * Creates a slider joint.
     *
     * The joint uses local anchor and axis values by default. Configure them with
     * setAnchor(), setConnectedAnchor(), setAxis(), and setConnectedAxis().
     *
     * @param connectedBody The rigid body connected to the joint.
     * @return An autoreleased slider joint.
     */
    static SliderJoint3D* create(Rigidbody3D* connectedBody = nullptr);

    /**
     * Creates a slider joint with world-space anchors and a shared world-space sliding axis.
     *
     * @param anchor Anchor on the attached body.
     * @param connectedAnchor Anchor on the connected body.
     * @param axis Sliding axis.
     * @param connectedBody The rigid body connected to the joint.
     * @return An autoreleased slider joint.
     */
    static SliderJoint3D* createInWorldSpace(const Vec3& anchor,
                                             const Vec3& connectedAnchor,
                                             const Vec3& axis,
                                             Rigidbody3D* connectedBody = nullptr);

    /**
     * Sets the allowed linear travel range.
     *
     * @param limits Linear travel range in world units.
     */
    void setLimits(const JointLimits& limits);

    /** Clears the explicit travel range and restores free slider motion. */
    void clearLimits();

    /** Returns whether explicit slider limits are configured. */
    bool hasLimits() const { return _limits.has_value(); }

    /**
     * Returns the configured linear travel range.
     *
     * @return The configured range, or a zeroed range when no explicit limit is set.
     */
    const JointLimits& getLimits() const;

    /**
     * Convenience wrapper for enabling or disabling the velocity motor.
     *
     * @param enabled `true` enables velocity mode, `false` disables the motor.
     */
    void setUseMotor(bool enabled);

    /** Returns whether the slider velocity motor is enabled. */
    bool isUseMotor() const { return _useMotor; }

    /**
     * Sets slider motor parameters.
     *
     * @param motor Motor settings in world units per second and force units.
     */
    void setMotor(const JointMotor& motor);

    /** Returns the configured slider motor parameters. */
    JointMotor getMotor() const { return _motor; }

    /**
     * Sets the spring parameters used when the slider motor is violated.
     *
     * @param spring Spring settings.
     */
    void setSpring(const JointSpring& spring);

    /** Returns the configured slider motor spring settings. */
    const JointSpring& getSpring() const { return _spring; }

    /**
     * Returns the current relative linear velocity projected onto the slider axis.
     *
     * @return Relative linear velocity in world units per second.
     */
    float getVelocity() const;

protected:
    SliderJoint3D(Rigidbody3D* connectedBody);

    bool attachToBody() override;

private:
    std::optional<JointLimits> _limits;
    JointMotor _motor;
    JointSpring _spring;
    bool _useMotor{false};
};

// -------------------------------------------------------------------
// ConeTwist Joint
// -------------------------------------------------------------------
class AX_DLL ConeTwistJoint3D : public Joint3D
{
public:
    /**
     * Creates a cone-twist joint.
     *
     * The joint uses local anchor and axis values by default. Configure them with
     * setAnchor(), setConnectedAnchor(), setAxis(), and setConnectedAxis().
     *
     * @param connectedBody The rigid body connected to the joint.
     * @return An autoreleased cone-twist joint.
     */
    static ConeTwistJoint3D* create(Rigidbody3D* connectedBody = nullptr);

    /**
     * Creates a cone-twist joint with shared world-space anchor and twist axis values.
     *
     * @param anchor The shared anchor position.
     * @param axis The primary twist axis.
     * @param connectedBody The rigid body connected to the joint.
     * @return An autoreleased cone-twist joint.
     */
    static ConeTwistJoint3D* createInWorldSpace(const Vec3& anchor,
                                                const Vec3& axis,
                                                Rigidbody3D* connectedBody = nullptr);

    /**
     * Convenience wrapper for enabling or disabling the twist velocity motor.
     *
     * @param enabled `true` enables velocity mode, `false` disables the motor.
     */
    void setUseTwistMotor(bool enabled);

    /** Returns whether the twist velocity motor is enabled. */
    bool isUseTwistMotor() const { return _useTwistMotor; }

    /**
     * Convenience wrapper for enabling or disabling the swing velocity motor.
     *
     * @param enabled `true` enables velocity mode, `false` disables the motor.
     */
    void setUseSwingMotor(bool enabled);

    /** Returns whether the swing velocity motor is enabled. */
    bool isUseSwingMotor() const { return _useSwingMotor; }

    /**
     * Sets the cone and twist angular limits in degrees.
     *
     * @param limits Cone and twist limits.
     */
    void setLimits(const TwistLimits& limits);

    /**
     * Compatibility wrapper that enables or clears cone and twist limits.
     *
     * @param enabled `true` reapplies the cached limits, `false` clears the limits.
     */
    void setUseLimits(bool enabled);

    /** Clears explicit cone and twist limits and restores free rotation. */
    void clearLimits();

    /** Returns whether explicit cone and twist limits are configured. */
    bool hasLimits() const { return _limits.has_value(); }

    /**
     * Returns the configured cone and twist limits.
     *
     * @return The configured limits, or a zeroed limit set when the joint is unrestricted.
     */
    const TwistLimits& getLimits() const;

    /**
     * Sets twist motor parameters.
     *
     * @param motor Twist motor settings in degrees per second and torque units.
     */
    void setTwistMotor(const JointMotor& motor);

    /** Returns the configured twist motor parameters. */
    const JointMotor& getTwistMotor() const { return _twistMotor; }

    /**
     * Sets swing motor parameters.
     *
     * @param motor Swing motor settings in degrees per second and torque units.
     */
    void setSwingMotor(const JointMotor& motor);

    /** Returns the configured swing motor parameters. */
    const JointMotor& getSwingMotor() const { return _swingMotor; }

    /**
     * Sets the twist motor spring settings.
     *
     * @param spring Twist motor spring settings.
     */
    void setTwistSpring(const JointSpring& spring);

    /** Returns the configured twist motor spring settings. */
    const JointSpring& getTwistSpring() const { return _twistSpring; }

    /**
     * Sets the swing motor spring settings.
     *
     * @param spring Swing motor spring settings.
     */
    void setSwingSpring(const JointSpring& spring);

    /** Returns the configured swing motor spring settings. */
    const JointSpring& getSwingSpring() const { return _swingSpring; }

    /**
     * Returns a 2D helper point on the cone cross-section for debug drawing.
     *
     * @param angle Angle in degrees.
     * @param length Radius from the origin.
     * @return Point on the XY plane.
     */
    Vec3 getPointForAngle(float angle, float length) const;

protected:
    ConeTwistJoint3D(Rigidbody3D* connectedBody);
    bool attachToBody() override;

    void applyMotorVelocity();
    void applyLimits();

    std::optional<TwistLimits> _limits;
    JointMotor _twistMotor;
    JointMotor _swingMotor;

    JointSpring _twistSpring;
    JointSpring _swingSpring;

    bool _useTwistMotor{false};
    bool _useSwingMotor{false};
};

// -------------------------------------------------------------------
// Six-DOF Joint
// -------------------------------------------------------------------
class AX_DLL SixDofJoint3D : public Joint3D
{
public:
    /**
     * Creates a six degree-of-freedom joint.
     *
     * The joint uses local anchor and axis values by default. Configure them with
     * setAnchor(), setConnectedAnchor(), setAxis(), and setConnectedAxis().
     *
     * @param connectedBody The rigid body connected to the joint.
     * @return An autoreleased six-DOF joint.
     */
    static SixDofJoint3D* create(Rigidbody3D* connectedBody = nullptr);

    /**
     * Creates a six degree-of-freedom joint with shared world-space anchor and reference axis values.
     *
     * @param anchor The shared anchor position.
     * @param axis The primary reference axis.
     * @param connectedBody The rigid body connected to the joint.
     * @return An autoreleased six-DOF joint.
     */
    static SixDofJoint3D* createInWorldSpace(const Vec3& anchor,
                                             const Vec3& axis,
                                             Rigidbody3D* connectedBody = nullptr);

    /**
     * Sets translation limits in world units.
     *
     * @param limits Translation limit range for each axis.
     */
    void setLinearLimits(const SixDofLimits& limits);

    /** Returns the configured translation limits. */
    const SixDofLimits& getLinearLimits() const { return _linearLimits; }

    /**
     * Sets angular limits in degrees.
     *
     * @param limits Angular limit range for each axis in degrees.
     */
    void setAngularLimits(const SixDofLimits& limits);

    /** Returns the configured angular limits in degrees. */
    const SixDofLimits& getAngularLimits() const { return _angularLimits; }

    /**
     * Returns whether the specified degree of freedom is limited.
     *
     * @param limitIndex Axis index in the order TX, TY, TZ, RX, RY, RZ.
     * @return `true` if the axis is currently constrained.
     */
    bool isLimited(int limitIndex) const;

protected:
    SixDofJoint3D(Rigidbody3D* connectedBody);
    bool attachToBody() override;

    SixDofLimits _linearLimits;
    SixDofLimits _angularLimits;
};

}  // namespace ax

#endif  // defined(AX_ENABLE_PHYSICS_3D)
