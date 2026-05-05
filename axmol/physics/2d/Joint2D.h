/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
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

#include <functional>

#include "axmol/base/Config.h"
#if defined(AX_ENABLE_PHYSICS_2D)
#    include "axmol/physics/PhysicsConsts.h"

#    include "axmol/base/Object.h"
#    include "axmol/math/Math.h"
#    include "axmol/math/MathUtil.h"
#    include "box2d/box2d.h"

namespace ax
{

class Node;
class Rigidbody2D;
class PhysicsWorld2D;

// default: x=0, y=0
using JointTranslationLimits2D = Vec2;

// Joint Angle limits in degrees,
// default: x=0, y=phconsts::MaxAngleDeg
using JointAngleLimits2D = Vec2;

// Joint Length
// default: phconsts::LinearSlop, phconsts::LinearHuge
using JointLengthLimit2D = Vec2;

// Joint Motor
// default: x(speed)=0.0f, y(maxForce)=phconsts::MaxForce
using JointMotor2D = Vec2;

/**
 * @addtogroup physics
 * @{
 * @addtogroup physics_2d
 * @{
 */

/**
 * @brief An Joint2D object connects two physics bodies together.
 */
class AX_DLL Joint2D : public Component
{
    friend class Rigidbody2D;
    friend class PhysicsWorld2D;

public:
    enum class AnchorSpace
    {
        Local,  // Local to rigidbody
        World,  // World space
    };

    /**
     * @brief Check if this joint is currently attached to the physics world.
     * @return True if the joint is valid and attached, false otherwise.
     */
    bool isAttached() const { return b2Joint_IsValid(_jointId); }

    /**
     * @brief Get the physics world that owns this joint.
     * @return Pointer to the PhysicsWorld2D instance.
     */
    PhysicsWorld2D* getWorld() const { return _world; }

    /**
     * @brief Get the primary body (body A) attached to this joint.
     * @return Pointer to the attached Rigidbody2D.
     */
    Rigidbody2D* getAttachedBody() const { return _attachedBody; }

    /**
     * @brief Set the secondary body (body B) connected to this joint.
     * @param body Pointer to the Rigidbody2D to connect.
     */
    void setConnectedBody(Rigidbody2D* body);

    /**
     * @brief Get the secondary body (body B) connected to this joint.
     * @return Pointer to the connected Rigidbody2D.
     */
    Rigidbody2D* getConnectedBody() const { return _connectedBody; }

    /**
     * @brief Get the current anchor space mode.
     * @return The anchor space (local or world).
     */
    AnchorSpace getAnchorSpace() const { return _anchorSpace; }

    /**
     * @brief Set the anchor space mode.
     * @param space The anchor space (local or world).
     */
    void setAnchorSpace(AnchorSpace space) { _anchorSpace = space; }

    /**
     * @brief Set both attached and connected anchors in world coordinates.
     *
     * This method forces the joint to use world space for anchors. Both
     * the attached body anchor and the connected body anchor will be set
     * to the same world-space position provided.
     *
     * @param anchor The anchor position in world space.
     */
    void setAnchorsInWorldSpace(const Vec2& anchor);

    /**
     * @brief Get the anchor point of the attached body.
     * @return The anchor position in current coordinate space.
     */
    Vec2 getAnchor() const { return _anchor; }

    /**
     * @brief Set both anchors in the current coordinate space.
     * @param anchor Anchor position for the attached body.
     * @param connectedAnchor Anchor position for the connected body.
     */
    void setAnchors(const Vec2& anchor, const Vec2& connectedAnchor)
    {
        setAnchor(anchor);
        setConnectedAnchor(connectedAnchor);
    }

    /**
     * @brief Set the anchor point of the attached body in the current coordinate space.
     * @param anchor The anchor position.
     */
    void setAnchor(const Vec2& anchor);

    /**
     * @brief Get the anchor point of the connected body.
     * @return The connected anchor position in current coordinate space.
     */
    Vec2 getConnectedAnchor() const { return _connectedAnchor; }

    /**
     * @brief Set the anchor point of the connected body in the current coordinate space.
     * @param anchor The connected anchor position.
     */
    void setConnectedAnchor(const Vec2& anchor);

    /**
     * @brief Check if collision between the two bodies is enabled.
     * @return True if collision is enabled, false otherwise.
     */
    bool isCollisionEnabled() const { return _collisionEnabled; }

    /**
     * @brief Enable or disable collision between the two bodies connected by this joint.
     * @param enable True to enable collision, false to disable.
     */
    void setCollisionEnabled(bool enable);

    /**
     * @brief Enable or disable automatic configuration of the connected anchor.
     * @param val True to auto-configure, false to use manual settings.
     */
    void setAutoConfigureConnectedAnchor(bool val);

    /**
     * @brief Check if automatic configuration of the connected anchor is enabled.
     * @return True if auto-configuration is enabled, false otherwise.
     */
    bool isAutoConfigureConnectedAnchor() const { return _autoConfigureConnectedAnchor; }

    /**
     * @brief Set the debug draw scale for this joint.
     *
     * A scale of 0 disables joint debug drawing. This depends on Axmol's
     * modifications to Box2D joint rendering logic.
     *
     * @param scale The scale factor for debug drawing.
     */
    void setDrawScale(float scale) { _drawScale = scale; }

    /**
     * @brief Get this joint's tag.
     * @return An integer identifier for the joint.
     */
    int getTag() const { return _tag; }

    /**
     * @brief Set this joint's tag.
     * @param tag An integer identifier for the joint.
     */
    void setTag(int tag) { _tag = tag; }

protected:
    void onEnter() override;
    void onExit() override;

    Joint2D(Rigidbody2D* connectedBody);
    virtual ~Joint2D() = 0;

    void configureBaseSettings(b2JointDef& jd);
    float calcuateDistance(const b2Vec2& anchorA, const b2Vec2& anchorB);

    /** attach to body */
    virtual bool attachToBody();

    /* detach from body */
    void detachFromBody();

    Rigidbody2D* _connectedBody;

    b2JointId _jointId{b2_nullJointId};
    Rigidbody2D* _attachedBody{nullptr};
    PhysicsWorld2D* _world{nullptr};

    bool _collisionEnabled{true};
    bool _destroyMark{false};
    bool _autoConfigureConnectedAnchor{false};
    int _tag{0};

    float _drawScale{phconsts::PixelsPerMeter};

    Vec2 _anchor;           // attached anchor, coordinate depends on _anchorSpace
    Vec2 _connectedAnchor;  // connected anchor,  coordinate depends on _anchorSpace

    AnchorSpace _anchorSpace = AnchorSpace::Local;
};

/**
 * @brief A fixed joint fuses the two bodies together at a reference point. Fixed joints are useful for creating complex
 * shapes that can be broken apart later.
 */
class AX_DLL FixedJoint2D : public Joint2D
{
public:
    /** Create a fixed joint.
     @param anchr It's the pivot position.
     @return A object pointer.
     */
    static FixedJoint2D* create(Rigidbody2D* connectedBody);

    float getDamping() const { return _damping; };
    void setDamping(float damping);

    float getFrequency() const { return _frequency; }
    void setFrequency(float hertz);

protected:
    FixedJoint2D(Rigidbody2D* connectedBody);

    bool attachToBody() override;

    float _damping{0.f};
    float _frequency{0.f};
};

/** Set the fixed distance with two bodies */
class AX_DLL DistanceJoint2D : public Joint2D
{
public:
    /** Create a fixed distance joint.
     @return A object pointer.
     */
    static DistanceJoint2D* create(Rigidbody2D* connectedBody);

    /** Get the rest length of a distance joint */
    float getLength() const { return _restLength; }

    /** Set the rest length of a distance joint */
    void setLength(float distance);

    bool isUseLimits() const { return _useLimits; }
    void setUseLimits(bool bval);

    const JointLengthLimit2D& getLimits() const { return _limits; }
    void setLimits(const JointLengthLimit2D& limits);

    /** Get spring damping */
    float getDamping() const { return _damping; };

    /** Set spring damping, distance limits requires damping not zero */
    void setDamping(float damping);

protected:
    DistanceJoint2D(Rigidbody2D* connectedBody);
    bool attachToBody() override;

    JointLengthLimit2D _limits;
    float _restLength{0.f};
    float _damping{0.5f};

    bool _useLimits{false};
};

/** Connecting two physics bodies together with a spring. */
class AX_DLL SpringJoint2D : public Joint2D
{
public:
    /** Create a Spring joint.

     @param stiffness It's the spring constant.
     @param damping It's how soft to make the damping of the spring.
     @return A object pointer.
     */
    static SpringJoint2D* create(float stiffness, float damping, Rigidbody2D* connectedBody);

    /** Get the distance of the anchor points.*/
    float getRestLength() const;

    /** Set the distance of the anchor points.*/
    void setRestLength(float restLength);

    /** Get the spring constant.*/
    float getStiffness() const;

    /** Set the spring constant.*/
    void setStiffness(float stiffness);

    /** Get the spring soft constant.*/
    float getDamping() const;

    /** Set the spring soft constant.*/
    void setDamping(float damping);

protected:
    SpringJoint2D(float stiffness, float damping, Rigidbody2D* connectedBody);
    bool attachToBody() override;

    void applySettings();

    float _restLength{0.f};
    float _stiffness;
    float _damping;
};

class AX_DLL SliderJoint2D : public Joint2D
{
public:
    /** Create a slider joint.
     @return A object pointer.
     */
    static SliderJoint2D* create(Rigidbody2D* connectedBody);

    /** Get angle in degrees */
    float getAngle() const { return _angle; }

    /** Sets angle in degrees */
    void setAngle(float angle);

    /** Sets angle in radians */
    void setAngleInRadians(float angle) { setAngle(MathUtil::degrees(angle)); }

    bool isUseMotor() const { return _useMotor; }
    void setUseMotor(bool enable);

    bool isUseLimits() const { return _useLimits; }
    void setUseLimits(bool enable);

    JointMotor2D getMotor() const { return _motor; }
    void setMotor(const JointMotor2D& motor);

    JointTranslationLimits2D GetLimits() const { return _translationLimits; }
    void setLimits(const JointTranslationLimits2D& limits);

    float getJointTranslation() const;

    float getJointSpeed() const;

    float getMotorForce() const;

protected:
    SliderJoint2D(Rigidbody2D* connectedBody);
    bool attachToBody() override;

    float _angle{0.0f};  // angle in degrees
    JointTranslationLimits2D _translationLimits;
    JointMotor2D _motor;
    bool _useMotor{false};
    bool _useLimits{false};
};

class AX_DLL WheelJoint2D : public Joint2D
{
public:
    /** Create a damped rotary spring joint.
     @return A object pointer.
     */
    static WheelJoint2D* create(Rigidbody2D* connectedBody);

    bool isUseMotor() const { return _useMotor; }
    void setUseMotor(bool enable);

    JointMotor2D getMotor() const { return _motor; }
    void setMotor(const JointMotor2D& motor);

    bool isUseLimits() const { return _useMotor; }
    void setUseLimits(bool enable);

    JointTranslationLimits2D getLimits() const { return _limits; }
    void setLimits(const JointTranslationLimits2D& motor);

    /** Get the spring soft constant.*/
    float getDamping() const { return _damping; }

    /** Set the spring soft constant.*/
    void setDamping(float damping);

    float getStiffness() const { return _stiffness; }
    void setStiffness(float hertz);

    float getMotorTorque() const;

protected:
    WheelJoint2D(Rigidbody2D* connectedBody);

    bool attachToBody() override;

    JointMotor2D _motor;
    JointTranslationLimits2D _limits;

    float _damping{0.7f};
    float _stiffness{2.0f};
    float _angle{90.0f};

    bool _useMotor{false};
    bool _useLimits{false};
};

// A Hinge/Pivot/Revolute Joint
class AX_DLL PivotJoint2D : public Joint2D
{
public:
    /** Create a damped rotary spring joint.
     @return A object pointer.
     */
    static PivotJoint2D* create(Rigidbody2D* connectedBody);

    bool isUseMotor() const { return _useMotor; }
    void setUseMotor(bool enable);

    bool isUseLimits() const { return _useLimits; }
    void setUseLimits(bool enable);

    JointMotor2D getMotor() const { return _motor; }
    void setMotor(const JointMotor2D& motor);
    float getMotorTorque() const;

    JointAngleLimits2D getLimits() const { return _angleLimits; }
    void setLimits(const JointAngleLimits2D& limits);

    bool isUseSpring() const { return _useSpring; }
    void setUseSpring(bool bval);

    float getSpringDamping() const { return _damping; }
    void setSpringDamping(float value);

    float getSpringFrequency() const { return _frequency; }
    void setSpringFrequency(float hertz);

    float getReferenceAngle() const;

protected:
    PivotJoint2D(Rigidbody2D* connectedBody);

    bool attachToBody() override;

    bool _useMotor{false};
    bool _useLimits{false};
    bool _useSpring{false};

    JointMotor2D _motor;
    JointAngleLimits2D _angleLimits;

    float _damping{0.7f};
    float _frequency{2.0f};
};

class AX_DLL PinJoint2D : public PivotJoint2D
{
public:
    /** Create A Pin Joint
     * @param pivot, The ping pivot in parent coordinate space
     */
    static PinJoint2D* create(const Vec2& pivot, Rigidbody2D* connectedBody);

protected:
    PinJoint2D(const Vec2& pivot, Rigidbody2D* connectedBody);
};

class AX_DLL MotorJoint2D : public Joint2D
{
public:
    /** Create a damped rotary spring joint.
     @return A object pointer.
     */
    static MotorJoint2D* create(Rigidbody2D* connectedBody);

    /** Set angular velocity in degrees */
    void setAngularVelocity(float val);
    void setAngularFrequency(float val);
    void setAngularDamping(float val);

    void setLinearVelocity(const Vec2& val);
    void setLinearFrequency(float val);
    void setLinearDamping(float val);

    /** Get angular velocity in degrees */
    float getAngularVelocity() const { return MathUtil::degrees(_settings.angularVelocity); }
    float getAngularFrequency() const { return _settings.angularHertz; }
    float getAngularDamping() const { return _settings.angularDampingRatio; }

    Vec2 getLinearVelocity() const;
    float getLinearFrequency() const { return _settings.linearHertz; }
    float getLinearDamping() const { return _settings.linearDampingRatio; }

protected:
    MotorJoint2D(Rigidbody2D* connectedBody);

    bool attachToBody() override;

    b2MotorJointDef _settings;
};

/// <summary>
/// A filter joint is used to disable collision between two specific bodies.
/// </summary>
class AX_DLL FilterJoint2D : public Joint2D
{
public:
    /** Create a damped rotary spring joint.
     @return A object pointer.
     */
    static FilterJoint2D* create(Rigidbody2D* connectedBody);

protected:
    FilterJoint2D(Rigidbody2D* connectedBody);

    bool attachToBody() override;
};

class AX_DLL TargetJoint2D : public Joint2D
{
public:
    /** Create a TargetJoint2D joint.
     * @param target The input location in connected rigidbody parent coordinate space
     * @return A object pointer.
     */
    static TargetJoint2D* create(const Vec2& target, Rigidbody2D* connectedBody);

    void setMaxForceScale(float maxForceScale);
    float getMaxForceScale() const { return _maxForceScale; }
    float getMaxForce() const;

    void setDamping(float _damping);
    float getDamping() const { return _damping; }

    void setFrequency(float hertz);
    float getFrequency() const { return _frequency; }

protected:
    TargetJoint2D(const Vec2& target, Rigidbody2D* connectedBody);
    bool attachToBody() override;
    float calculateMaxForce(float maxForceScale) const;

    float _damping{1.0f};
    float _frequency{7.5f};
    float _maxForceScale{100.f};
};

/** @} */
/** @} */

}  // namespace ax

#endif  // defined(AX_ENABLE_PHYSICS_2D)
