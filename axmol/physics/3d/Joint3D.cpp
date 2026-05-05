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
#include "axmol/physics/3d/Joint3D.h"

#if defined(AX_ENABLE_PHYSICS_3D)

#    include "axmol/physics/3d/PhysicsUtility3D.h"
#    include "axmol/physics/3d/Rigidbody3D.h"
#    include "axmol/physics/3d/PhysicsWorld3D.h"

#    include "Jolt/Physics/Constraints/FixedConstraint.h"
#    include <Jolt/Physics/Constraints/DistanceConstraint.h>
#    include <Jolt/Physics/Constraints/HingeConstraint.h>
#    include <Jolt/Physics/Constraints/PointConstraint.h>
#    include <Jolt/Physics/Constraints/SixDOFConstraint.h>
#    include <Jolt/Physics/Constraints/SliderConstraint.h>
#    include <Jolt/Physics/Constraints/SwingTwistConstraint.h>

#    include <cmath>
#    include <limits>

namespace ax
{

namespace jphutil
{
JPH::EConstraintSpace cast(Joint3D::CoordinateSpace space)
{
    return space == Joint3D::CoordinateSpace::Local ? JPH::EConstraintSpace::LocalToBodyCOM
                                                    : JPH::EConstraintSpace::WorldSpace;
}
static JPH::BodyID cast(Rigidbody3D* rigidbody)
{
    return rigidbody ? rigidbody->internalHandle() : JPH::BodyID{};
}
static JPH::EMotorState toMotorState(bool enabled)
{
    return enabled ? JPH::EMotorState::Velocity : JPH::EMotorState::Off;
}
}  // namespace jphutil

namespace
{
static Vec3 normalizeOrDefault(const Vec3& axis, const Vec3& fallback = Vec3(1.0f, 0.0f, 0.0f))
{
    auto result = axis;
    if (result.lengthSquared() < std::numeric_limits<float>::epsilon())
        return fallback;
    result.normalize();
    return result;
}

// static Vec3 orthogonalAxis(const Vec3& axis)
//{
//     const Vec3 normalized = normalizeOrDefault(axis);
//     const Vec3 helper     = fabsf(normalized.dot(Vec3::UNIT_Y)) < 0.99f ? Vec3::UNIT_Y : Vec3::UNIT_Z;
//     Vec3 out;
//     Vec3::cross(normalized, helper, &out);
//     out.normalize();
//     return out;
// }

// Build an orthonormal basis from a given axis
// Returns (primaryAxis, normalAxis)
static std::pair<Vec3, Vec3> makeOrthoNormalBasis(const Vec3& axis, const Vec3& fallback = Vec3::UNIT_X)
{
    // Normalize input axis or use fallback if invalid
    Vec3 primary = normalizeOrDefault(axis, fallback);

    // Pick a helper axis that is not parallel to primary
    const Vec3 helper = fabsf(primary.dot(Vec3::UNIT_Y)) < 0.99f ? Vec3::UNIT_Y : Vec3::UNIT_Z;

    // Compute a normal axis orthogonal to primary
    Vec3 normal;
    Vec3::cross(primary, helper, &normal);
    normal.normalize();

    return {primary, normal};
}

static Vec3 localPointToWorld(Rigidbody3D* body, const Vec3& localPoint)
{
    return body ? body->local2World(localPoint) : localPoint;
}

static Vec3 localAxisToWorld(Rigidbody3D* body, const Vec3& localAxis)
{
    auto out = body ? body->local2World(localAxis) : localAxis;
    return normalizeOrDefault(out);
}

static Vec3 getWorldAnchor(const Joint3D* joint, bool connected)
{
    const Vec3& anchor = connected ? joint->getConnectedAnchor() : joint->getAnchor();
    if (joint->getCoordinateSpace() == Joint3D::CoordinateSpace::World)
        return anchor;

    auto* body = connected ? joint->getConnectedBody() : joint->getAttachedBody();
    return localPointToWorld(body, anchor);
}

static Vec3 getWorldAxis(const Joint3D* joint, bool connected, const Vec3& fallback = Vec3::UNIT_X)
{
    const Vec3& axis = connected ? joint->getConnectedAxis() : joint->getAxis();
    if (joint->getCoordinateSpace() == Joint3D::CoordinateSpace::World)
        return normalizeOrDefault(axis, fallback);

    auto* body = connected ? joint->getConnectedBody() : joint->getAttachedBody();
    return localAxisToWorld(body, axis);
}

static JointLimits sanitizeDistanceLimits(const JointLimits& limits)
{
    JointLimits sanitized{
        std::clamp(limits.lower, 0.0f, phconsts::UpperBound),
        std::clamp(limits.upper, 0.0f, phconsts::UpperBound),
    };
    if (sanitized.upper < sanitized.lower)
        std::swap(sanitized.lower, sanitized.upper);
    return sanitized;
}

static JointLimits sanitizeSliderLimits(const JointLimits& limits)
{
    JointLimits sanitized{
        std::clamp(limits.lower, -phconsts::UpperBound, phconsts::UpperBound),
        std::clamp(limits.upper, -phconsts::UpperBound, phconsts::UpperBound),
    };
    if (sanitized.upper < sanitized.lower)
        std::swap(sanitized.lower, sanitized.upper);
    return sanitized;
}

static JointLimits sanitizeHingeLimits(const JointLimits& limits)
{
    JointLimits sanitized{
        std::clamp(limits.lower, -180.0f, 0.0f),
        std::clamp(limits.upper, 0.0f, 180.0f),
    };
    if (sanitized.upper < sanitized.lower)
        std::swap(sanitized.lower, sanitized.upper);
    sanitized.lower = std::min(sanitized.lower, 0.0f);
    sanitized.upper = std::max(sanitized.upper, 0.0f);
    return sanitized;
}

static const JointLimits& emptyJointLimits()
{
    static const JointLimits kLimits{};
    return kLimits;
}

static const JointLimits& freeHingeLimits()
{
    static const JointLimits kLimits{-180.0f, 180.0f};
    return kLimits;
}

static const TwistLimits& emptyTwistLimits()
{
    static const TwistLimits kLimits{};
    return kLimits;
}

static void addConstraintToWorld(PhysicsWorld3D* world, JPH::Constraint* constraint)
{
    if (!world || !constraint)
        return;

    world->internalHandle().AddConstraint(constraint);
    if (auto twobodyConstraint = dynamic_cast<JPH::TwoBodyConstraint*>(constraint))
        world->internalHandle().GetBodyInterface().ActivateConstraint(twobodyConstraint);
}

template <typename _TwoBodyConstraintSettings>
static void configureAnchors(Joint3D* joint, _TwoBodyConstraintSettings& settings)
{
    settings.mSpace = jphutil::cast(joint->getCoordinateSpace());

    if constexpr (std::is_same_v<_TwoBodyConstraintSettings, JPH::SixDOFConstraintSettings> ||
                  std::is_same_v<_TwoBodyConstraintSettings, JPH::SwingTwistConstraintSettings>)
    {
        settings.mPosition1 = jphutil::cast(joint->getAnchor());
        settings.mPosition2 = jphutil::cast(joint->getConnectedAnchor());
    }
    else
    {
        settings.mPoint1 = jphutil::cast(joint->getAnchor());
        settings.mPoint2 = jphutil::cast(joint->getConnectedAnchor());
    }
}

}  // namespace

Joint3D::Joint3D(Rigidbody3D* connectedBody)
{
    if (connectedBody)
        bindBody(_connectedBody, connectedBody);

    _axis = _connectedAxis = Vec3(1.0f, 0.0f, 0.0f);
}

Joint3D::~Joint3D()
{
    detachFromBody();
}

void Joint3D::onEnter()
{
    Component::onEnter();

    attachToBody();
}

void Joint3D::onExit()
{
    detachFromBody();

    Component::onExit();
}

bool Joint3D::attachToBody()
{
    if (_world)
        return true;

    // ensure this joint was attached to body which was attached to owner node
    if (!_attachedBody)
    {
        auto attachedBody = getComponent<Rigidbody3D>();
        if (!attachedBody)
        {
            AXLOGE("Joint3D({})::attachToBody: owner node does not have physics body", _name);
            return false;
        }
        if (!attachedBody->attachToWorld())
        {
            AXLOGE("Joint3D({}):attachToBody: attached body not ready", _name);
            return false;
        }

        bindBody(_attachedBody, attachedBody);
    }

    auto world = _attachedBody->getWorld();
    if (_connectedBody)  // connected body is optional
    {
        if (!_connectedBody->attachToWorld(world))
        {
            AXLOGE("Joint3D({}):attachToBody: connected body not ready", _name);
            return false;
        }
    }

    _world = world;
    return true;
}

void Joint3D::detachFromBody()
{
    if (isAttached())
    {
        _world->internalHandle().RemoveConstraint(_constraint);
        _constraint = nullptr;
    }

    if (_attachedBody)
        _attachedBody->removeJoint(this);
    AX_SAFE_RELEASE_NULL(_attachedBody);

    if (_connectedBody)
        _connectedBody->removeJoint(this);
    AX_SAFE_RELEASE_NULL(_connectedBody);

    _world = nullptr;
}

void Joint3D::setConnectedBody(Rigidbody3D* body)
{
    // not allow to change connected body after joint is created
    if (_connectedBody || !body)
        return;

    bindBody(_connectedBody, body);

    detachFromBody();
    attachToBody();
}

void Joint3D::setAnchorsInWorldSpace(const Vec3& anchor)
{
    _coordSpace      = CoordinateSpace::World;
    _anchor          = anchor;
    _connectedAnchor = anchor;
}

void Joint3D::setAnchor(const Vec3& anchor)
{
    _anchor = anchor;
}

void Joint3D::setConnectedAnchor(const Vec3& anchor)
{
    _connectedAnchor = anchor;
}

void Joint3D::setAxesInWorldSpace(const Vec3& axis)
{
    _coordSpace    = CoordinateSpace::World;
    _axis          = axis;
    _connectedAxis = axis;
}

void Joint3D::setAxis(const Vec3& axis)
{
    _axis = axis;
}

void Joint3D::setConnectedAxis(const Vec3& axis)
{
    _connectedAxis = axis;
}

void Joint3D::bindBody(Rigidbody3D*& targetMember, Rigidbody3D* newBody)
{
    if (newBody)
        newBody->addJoint(this);
    Object::assign(targetMember, newBody);
}

// -------------------------------------------------------------------
// Fixed Joint
// -------------------------------------------------------------------
FixedJoint3D* FixedJoint3D::create(Rigidbody3D* connectedBody)
{
    auto joint = new FixedJoint3D(connectedBody);
    joint->autorelease();
    return joint;
}

FixedJoint3D::FixedJoint3D(Rigidbody3D* connectedBody) : Joint3D(connectedBody)
{
    setName("FixedJoint3D"sv);
}

bool FixedJoint3D::attachToBody()
{
    if (isAttached())
        return true;
    if (!Joint3D::attachToBody())
        return false;

    //    JPH::SixDOFConstraintSettings settings;
    //    configureAnchors(this, settings);
    //
    //    for (int i = 0; i < JPH::SixDOFConstraintSettings::Num; ++i)
    //        settings.MakeFixedAxis(static_cast<JPH::SixDOFConstraintSettings::EAxis>(i));

    JPH::FixedConstraintSettings settings;
    configureAnchors(this, settings);

    _constraint = _world->internalHandle().GetBodyInterface().CreateConstraint(
        &settings, _attachedBody->internalHandle(), jphutil::cast(_connectedBody));
    addConstraintToWorld(_world, _constraint);

    return true;
}

// -------------------------------------------------------------------
// DIstance Joint
// -------------------------------------------------------------------
DistanceJoint3D* DistanceJoint3D::create(Rigidbody3D* connectedBody)
{
    auto joint = new DistanceJoint3D(connectedBody);
    joint->autorelease();
    return joint;
}

DistanceJoint3D::DistanceJoint3D(Rigidbody3D* connectedBody) : Joint3D(connectedBody)
{
    setName("DistanceJoint3D"sv);
}

void DistanceJoint3D::setLimits(const JointLimits& limits)
{
    _limits = sanitizeDistanceLimits(limits);

    if (auto d = static_cast<JPH::DistanceConstraint*>(_constraint))
        d->SetDistance(_limits->lower, _limits->upper);
}

void DistanceJoint3D::clearLimits()
{
    _limits.reset();

    if (auto* d = static_cast<JPH::DistanceConstraint*>(_constraint))
    {
        const float currentDistance = getWorldAnchor(this, false).distance(getWorldAnchor(this, true));
        d->SetDistance(currentDistance, currentDistance);
    }
}

const JointLimits& DistanceJoint3D::getLimits() const
{
    return _limits ? *_limits : emptyJointLimits();
}

bool DistanceJoint3D::attachToBody()
{
    if (isAttached())
        return true;
    if (!Joint3D::attachToBody())
        return false;

    JPH::DistanceConstraintSettings settings;
    configureAnchors(this, settings);

    if (_limits)
    {
        settings.mMinDistance = _limits->lower;
        settings.mMaxDistance = _limits->upper;
    }

    settings.mLimitsSpringSettings.mMode      = JPH::ESpringMode::FrequencyAndDamping;
    settings.mLimitsSpringSettings.mFrequency = _spring.frequency;
    settings.mLimitsSpringSettings.mDamping   = _spring.damping;

    _constraint = _world->internalHandle().GetBodyInterface().CreateConstraint(
        &settings, _attachedBody->internalHandle(), jphutil::cast(_connectedBody));
    addConstraintToWorld(_world, _constraint);

    return true;
}

void DistanceJoint3D::setSpring(const JointSpring& spring)
{
    _spring = spring;
}

// -------------------------------------------------------------------
// Pivot Joint
// -------------------------------------------------------------------
PivotJoint3D* PivotJoint3D::create(Rigidbody3D* connectedBody)
{
    auto ret = new PivotJoint3D(connectedBody);
    ret->autorelease();
    return ret;
}

PivotJoint3D::PivotJoint3D(Rigidbody3D* connectedBody) : Joint3D(connectedBody)
{
    setName("PivotJoint3D"sv);
}

bool PivotJoint3D::attachToBody()
{
    if (isAttached())
        return true;
    if (!Joint3D::attachToBody())
        return false;

    JPH::PointConstraintSettings settings;
    configureAnchors(this, settings);

    _constraint = _world->internalHandle().GetBodyInterface().CreateConstraint(
        &settings, _attachedBody->internalHandle(), jphutil::cast(_connectedBody));
    addConstraintToWorld(_world, _constraint);

    return true;
}

void PivotJoint3D::setAnchor(const Vec3& anchor)
{
    Joint3D::setAnchor(anchor);
    if (auto constraint = static_cast<JPH::PointConstraint*>(_constraint))
    {
        constraint->SetPoint1(jphutil::cast(_coordSpace), jphutil::cast(anchor));
    }
}

void PivotJoint3D::setConnectedAnchor(const Vec3& anchor)
{
    Joint3D::setConnectedAnchor(anchor);
    if (auto constraint = static_cast<JPH::PointConstraint*>(_constraint))
        constraint->SetPoint2(jphutil::cast(_coordSpace), jphutil::cast(anchor));
}

// -------------------------------------------------------------------
// Hinge Joint
// -------------------------------------------------------------------
HingeJoint3D* HingeJoint3D::create(Rigidbody3D* connectedBody)
{
    auto* ret = new HingeJoint3D(connectedBody);
    ret->autorelease();
    return ret;
}

HingeJoint3D* HingeJoint3D::createInWorldSpace(const ax::Vec3& anchor,
                                               const ax::Vec3& eulerAxis,
                                               Rigidbody3D* connectedBody)
{
    auto* ret = create(connectedBody);
    ret->setAnchorsInWorldSpace(anchor);
    ret->setAxis(eulerAxis);
    ret->setConnectedAxis(eulerAxis);
    return ret;
}

HingeJoint3D::HingeJoint3D(Rigidbody3D* connectedBody) : Joint3D(connectedBody)
{
    setName("HingeJoint3D"sv);
}

bool HingeJoint3D::attachToBody()
{
    if (isAttached())
        return true;
    if (!Joint3D::attachToBody())
        return false;

    auto [hingeAxis1, normalAxis1] = makeOrthoNormalBasis(_axis, Vec3::UNIT_X);
    auto [hingeAxis2, normalAxis2] = makeOrthoNormalBasis(_connectedAxis, Vec3::UNIT_X);

    JPH::HingeConstraintSettings settings;
    configureAnchors(this, settings);

    settings.mHingeAxis1  = jphutil::cast(hingeAxis1);
    settings.mHingeAxis2  = jphutil::cast(hingeAxis2);
    settings.mNormalAxis1 = jphutil::cast(normalAxis1);
    settings.mNormalAxis2 = jphutil::cast(normalAxis2);

    if (_useMotor)
    {
        settings.mMotorSettings.SetTorqueLimit(_motor.maxForce);
        settings.mMotorSettings.mSpringSettings.mMode      = JPH::ESpringMode::FrequencyAndDamping;
        settings.mMotorSettings.mSpringSettings.mFrequency = _spring.frequency;
        settings.mMotorSettings.mSpringSettings.mDamping   = _spring.damping;
    }

    _constraint = _world->internalHandle().GetBodyInterface().CreateConstraint(
        &settings, _attachedBody->internalHandle(), jphutil::cast(_connectedBody));
    addConstraintToWorld(_world, _constraint);

    auto* hinge = static_cast<JPH::HingeConstraint*>(_constraint);
    if (_limits)
        hinge->SetLimits(MathUtil::radians(_limits->lower), MathUtil::radians(_limits->upper));

    hinge->SetMotorState(jphutil::toMotorState(_useMotor));
    hinge->SetTargetAngularVelocity(MathUtil::radians(_motor.targetVelocity));

    return true;
}

void HingeJoint3D::setUseMotor(bool enabled)
{
    _useMotor = enabled;
    if (auto* hinge = static_cast<JPH::HingeConstraint*>(_constraint))
        hinge->SetMotorState(jphutil::toMotorState(enabled));
}

void HingeJoint3D::setLimits(const JointLimits& limits)
{
    _limits = sanitizeHingeLimits(limits);

    if (auto* hinge = static_cast<JPH::HingeConstraint*>(_constraint))
        hinge->SetLimits(MathUtil::radians(_limits->lower), MathUtil::radians(_limits->upper));
}

void HingeJoint3D::setUseLimits(bool enabled)
{
    if (!enabled)
    {
        clearLimits();
        return;
    }

    if (_limits)
        setLimits(*_limits);
}

void HingeJoint3D::clearLimits()
{
    _limits.reset();

    if (auto hinge = static_cast<JPH::HingeConstraint*>(_constraint))
        hinge->SetLimits(-JPH::JPH_PI, JPH::JPH_PI);
}

const JointLimits& HingeJoint3D::getLimits() const
{
    return _limits ? *_limits : freeHingeLimits();
}

void HingeJoint3D::setMotor(const JointMotor& motor)
{
    _motor = motor;

    // Motor is automatically enabled if motor is set.
    setUseMotor(true);

    if (auto* hinge = static_cast<JPH::HingeConstraint*>(_constraint))
    {
        hinge->GetMotorSettings().SetTorqueLimit(motor.maxForce);
        hinge->SetTargetAngularVelocity(MathUtil::radians(motor.targetVelocity));
    }
}

void HingeJoint3D::setSpring(const JointSpring& spring)
{
    _spring = spring;
}

float HingeJoint3D::getVelocity() const
{
    if (static_cast<JPH::HingeConstraint*>(_constraint))
    {
        const Vec3 axis         = getWorldAxis(this, false, Vec3::UNIT_Y);
        Vec3 relAngularVelocity = _attachedBody->getAngularVelocity();
        if (_connectedBody)
            relAngularVelocity -= _connectedBody->getAngularVelocity();
        return -MathUtil::degrees(relAngularVelocity.dot(axis));
    }
    return 0.0f;
}

float HingeJoint3D::getAngle() const
{
    if (auto* hinge = static_cast<JPH::HingeConstraint*>(_constraint))
        return MathUtil::degrees(hinge->GetCurrentAngle());
    return 0.0f;
}

// -------------------------------------------------------------------
// Slider Joint
// -------------------------------------------------------------------
SliderJoint3D* SliderJoint3D::create(Rigidbody3D* connectedBody)
{
    auto ret = new SliderJoint3D(connectedBody);
    ret->autorelease();
    return ret;
}

SliderJoint3D* SliderJoint3D::createInWorldSpace(const Vec3& anchor,
                                                 const Vec3& connectedAnchor,
                                                 const Vec3& axis,
                                                 Rigidbody3D* connectedBody)
{
    auto ret = create(connectedBody);
    ret->setCoordinateSpace(CoordinateSpace::World);
    ret->setAnchor(anchor);
    ret->setConnectedAnchor(connectedAnchor);
    ret->setAxis(axis);
    ret->setConnectedAxis(axis);
    return ret;
}

SliderJoint3D::SliderJoint3D(Rigidbody3D* connectedBody) : Joint3D(connectedBody)
{
    setName("SliderJoint3D"sv);
}

bool SliderJoint3D::attachToBody()
{
    if (isAttached())
        return true;
    if (!Joint3D::attachToBody())
        return false;

    JPH::SliderConstraintSettings settings;
    configureAnchors(this, settings);

    auto [slideAxis1, normalAxis1] = makeOrthoNormalBasis(_axis, Vec3::UNIT_Y);
    auto [slideAxis2, normalAxis2] = makeOrthoNormalBasis(_connectedAxis, Vec3::UNIT_Y);

    settings.mSliderAxis1 = jphutil::cast(slideAxis1);
    settings.mSliderAxis2 = jphutil::cast(slideAxis2);
    settings.mNormalAxis1 = jphutil::cast(normalAxis1);
    settings.mNormalAxis2 = jphutil::cast(normalAxis2);

    if (_useMotor)
    {
        settings.mMotorSettings.SetForceLimit(_motor.maxForce);

        settings.mMotorSettings.mSpringSettings.mMode      = JPH::ESpringMode::FrequencyAndDamping;
        settings.mMotorSettings.mSpringSettings.mFrequency = _spring.frequency;
        settings.mMotorSettings.mSpringSettings.mDamping   = _spring.damping;
    }

    _constraint = _world->internalHandle().GetBodyInterface().CreateConstraint(
        &settings, _attachedBody->internalHandle(), jphutil::cast(_connectedBody));

    addConstraintToWorld(_world, _constraint);

    auto* slider = static_cast<JPH::SliderConstraint*>(_constraint);
    if (_limits)
        slider->SetLimits(_limits->lower, _limits->upper);
    else
        slider->SetLimits(-FLT_MAX, FLT_MAX);

    slider->SetMotorState(jphutil::toMotorState(_useMotor));
    slider->SetTargetVelocity(_motor.targetVelocity);

    return true;
}

void SliderJoint3D::setLimits(const JointLimits& limits)
{
    _limits = sanitizeSliderLimits(limits);

    if (auto* slider = static_cast<JPH::SliderConstraint*>(_constraint))
        slider->SetLimits(_limits->lower, _limits->upper);
}

void SliderJoint3D::clearLimits()
{
    _limits.reset();

    if (auto* slider = static_cast<JPH::SliderConstraint*>(_constraint))
        slider->SetLimits(-FLT_MAX, FLT_MAX);
}

const JointLimits& SliderJoint3D::getLimits() const
{
    return _limits ? *_limits : emptyJointLimits();
}

void SliderJoint3D::setMotor(const JointMotor& motor)
{
    _motor = motor;

    // Motor is automatically enabled if motor is set.
    setUseMotor(true);

    if (auto* slider = static_cast<JPH::SliderConstraint*>(_constraint))
    {
        auto& motorSettings = slider->GetMotorSettings();
        motorSettings.SetForceLimit(motor.maxForce);
        slider->SetTargetVelocity(motor.targetVelocity);
    }
}

void SliderJoint3D::setSpring(const JointSpring& spring)
{
    _spring = spring;
}

void SliderJoint3D::setUseMotor(bool enabled)
{
    _useMotor = enabled;
    if (auto* slider = static_cast<JPH::SliderConstraint*>(_constraint))
        slider->SetMotorState(jphutil::toMotorState(enabled));
}

float SliderJoint3D::getVelocity() const
{
    if (static_cast<JPH::SliderConstraint*>(_constraint))
    {
        const Vec3 axis        = getWorldAxis(this, false, Vec3::UNIT_Y);
        Vec3 relLinearVelocity = _attachedBody->getLinearVelocity();
        if (_connectedBody)
            relLinearVelocity -= _connectedBody->getLinearVelocity();
        return relLinearVelocity.dot(axis);
    }
    return 0.0f;
}

// -------------------------------------------------------------------
// ConeTwist Joint
// -------------------------------------------------------------------
ConeTwistJoint3D* ConeTwistJoint3D::create(Rigidbody3D* connectedBody)
{
    auto* ret = new ConeTwistJoint3D(connectedBody);
    ret->autorelease();
    return ret;
}

ConeTwistJoint3D* ConeTwistJoint3D::createInWorldSpace(const Vec3& anchor, const Vec3& axis, Rigidbody3D* connectedBody)
{
    auto* ret = create(connectedBody);
    ret->setAnchorsInWorldSpace(anchor);
    ret->setAxis(axis);
    ret->setConnectedAxis(axis);
    return ret;
}

ConeTwistJoint3D::ConeTwistJoint3D(Rigidbody3D* connectedBody) : Joint3D(connectedBody)
{
    setName("ConeTwistJoint3D"sv);
}

bool ConeTwistJoint3D::attachToBody()
{
    if (isAttached())
        return true;
    if (!Joint3D::attachToBody())
        return false;

    JPH::SwingTwistConstraintSettings settings;
    configureAnchors(this, settings);

    auto [slideAxis1, normalAxis1] = makeOrthoNormalBasis(_axis, Vec3::UNIT_Y);
    auto [slideAxis2, normalAxis2] = makeOrthoNormalBasis(_connectedAxis, Vec3::UNIT_Y);

    settings.mTwistAxis1 = jphutil::cast(slideAxis1);
    settings.mTwistAxis2 = jphutil::cast(slideAxis2);
    settings.mPlaneAxis1 = jphutil::cast(normalAxis1);
    settings.mPlaneAxis2 = jphutil::cast(normalAxis2);

    if (_limits)
    {
        settings.mTwistMinAngle       = -MathUtil::radians(_limits->twistSpan) * 0.5f;
        settings.mTwistMaxAngle       = MathUtil::radians(_limits->twistSpan) * 0.5f;
        settings.mNormalHalfConeAngle = MathUtil::radians(_limits->swingSpan1);
        settings.mPlaneHalfConeAngle  = MathUtil::radians(_limits->swingSpan2);
    }
    else
    {
        settings.mTwistMinAngle       = -JPH::JPH_PI;
        settings.mTwistMaxAngle       = JPH::JPH_PI;
        settings.mNormalHalfConeAngle = JPH::JPH_PI;
        settings.mPlaneHalfConeAngle  = JPH::JPH_PI;
    }

    if (_useTwistMotor)
    {
        settings.mTwistMotorSettings.mMaxTorqueLimit            = _twistMotor.maxForce;
        settings.mTwistMotorSettings.mSpringSettings.mMode      = JPH::ESpringMode::FrequencyAndDamping;
        settings.mTwistMotorSettings.mSpringSettings.mFrequency = _twistSpring.frequency;
        settings.mTwistMotorSettings.mSpringSettings.mDamping   = _twistSpring.damping;
    }

    if (_useSwingMotor)
    {
        settings.mSwingMotorSettings.mMaxTorqueLimit            = _swingMotor.maxForce;
        settings.mSwingMotorSettings.mSpringSettings.mMode      = JPH::ESpringMode::FrequencyAndDamping;
        settings.mSwingMotorSettings.mSpringSettings.mFrequency = _swingSpring.frequency;
        settings.mSwingMotorSettings.mSpringSettings.mDamping   = _swingSpring.damping;
    }

    _constraint = _world->internalHandle().GetBodyInterface().CreateConstraint(
        &settings, _attachedBody->internalHandle(), jphutil::cast(_connectedBody));
    addConstraintToWorld(_world, _constraint);

    auto swing = static_cast<JPH::SwingTwistConstraint*>(_constraint);
    swing->SetTwistMotorState(jphutil::toMotorState(_useTwistMotor));
    swing->SetSwingMotorState(jphutil::toMotorState(_useSwingMotor));
    applyMotorVelocity();

    return true;
}

void ConeTwistJoint3D::setUseTwistMotor(bool enabled)
{
    _useTwistMotor = enabled;
    if (auto* constraint = static_cast<JPH::SwingTwistConstraint*>(_constraint))
        constraint->SetTwistMotorState(jphutil::toMotorState(enabled));
    applyMotorVelocity();
}

void ConeTwistJoint3D::setUseSwingMotor(bool enabled)
{
    _useSwingMotor = enabled;
    if (auto* constraint = static_cast<JPH::SwingTwistConstraint*>(_constraint))
        constraint->SetSwingMotorState(jphutil::toMotorState(enabled));
    applyMotorVelocity();
}

void ConeTwistJoint3D::setLimits(const TwistLimits& limits)
{
    _limits = limits;
    applyLimits();
}

void ConeTwistJoint3D::setUseLimits(bool enabled)
{
    if (!enabled)
    {
        clearLimits();
        return;
    }

    if (_limits)
        applyLimits();
}

void ConeTwistJoint3D::clearLimits()
{
    _limits.reset();
    applyLimits();
}

const TwistLimits& ConeTwistJoint3D::getLimits() const
{
    return _limits ? *_limits : emptyTwistLimits();
}

void ConeTwistJoint3D::setTwistMotor(const JointMotor& motor)
{
    _twistMotor = motor;

    setUseTwistMotor(true);

    auto sw = static_cast<JPH::SwingTwistConstraint*>(_constraint);
    if (sw)
        sw->GetTwistMotorSettings().SetTorqueLimit(motor.maxForce);

    applyMotorVelocity();
}

void ConeTwistJoint3D::setSwingMotor(const JointMotor& motor)
{
    _swingMotor = motor;

    setUseSwingMotor(true);

    auto sw = static_cast<JPH::SwingTwistConstraint*>(_constraint);
    if (sw)
        sw->GetSwingMotorSettings().SetTorqueLimit(motor.maxForce);

    applyMotorVelocity();
}

void ConeTwistJoint3D::applyMotorVelocity()
{
    auto* constraint = static_cast<JPH::SwingTwistConstraint*>(_constraint);
    if (!constraint)
        return;

    // Jolt uses a single Vec3 for target angular velocity in Constraint Space.
    // X-component handles Twist, while Y and Z handle Swing.
    JPH::Vec3 targetVelCS = JPH::Vec3::sZero();

    if (_useTwistMotor)
        targetVelCS.SetX(MathUtil::radians(_twistMotor.targetVelocity));

    if (_useSwingMotor)
        targetVelCS.SetY(MathUtil::radians(_swingMotor.targetVelocity));

    constraint->SetTargetAngularVelocityCS(targetVelCS);
}

void ConeTwistJoint3D::applyLimits()
{
    auto* swingTwist = static_cast<JPH::SwingTwistConstraint*>(_constraint);
    if (!swingTwist)
        return;

    if (_limits)
    {
        swingTwist->SetNormalHalfConeAngle(MathUtil::radians(_limits->swingSpan1));
        swingTwist->SetPlaneHalfConeAngle(MathUtil::radians(_limits->swingSpan2));
        swingTwist->SetTwistMinAngle(-MathUtil::radians(_limits->twistSpan) * 0.5f);
        swingTwist->SetTwistMaxAngle(MathUtil::radians(_limits->twistSpan) * 0.5f);
    }
    else
    {
        swingTwist->SetNormalHalfConeAngle(JPH::JPH_PI);
        swingTwist->SetPlaneHalfConeAngle(JPH::JPH_PI);
        swingTwist->SetTwistMinAngle(-JPH::JPH_PI);
        swingTwist->SetTwistMaxAngle(JPH::JPH_PI);
    }
}

void ConeTwistJoint3D::setTwistSpring(const JointSpring& spring)
{
    _twistSpring = spring;
    if (auto* swingTwist = static_cast<JPH::SwingTwistConstraint*>(_constraint))
    {
        auto& springSettings      = swingTwist->GetTwistMotorSettings().mSpringSettings;
        springSettings.mMode      = JPH::ESpringMode::FrequencyAndDamping;
        springSettings.mFrequency = spring.frequency;
        springSettings.mDamping   = spring.damping;
    }
}

void ConeTwistJoint3D::setSwingSpring(const JointSpring& spring)
{
    _swingSpring = spring;
    if (auto* swingTwist = static_cast<JPH::SwingTwistConstraint*>(_constraint))
    {
        auto& springSettings      = swingTwist->GetSwingMotorSettings().mSpringSettings;
        springSettings.mMode      = JPH::ESpringMode::FrequencyAndDamping;
        springSettings.mFrequency = spring.frequency;
        springSettings.mDamping   = spring.damping;
    }
}

Vec3 ConeTwistJoint3D::getPointForAngle(float angle, float length) const
{
    const float angleInRadians = MathUtil::radians(angle);
    return {cosf(angleInRadians) * length, sinf(angleInRadians) * length, 0.0f};
}

// -------------------------------------------------------------------
// SixDof Joint
// -------------------------------------------------------------------
SixDofJoint3D* SixDofJoint3D::create(Rigidbody3D* connectedBody)
{
    auto ret = new SixDofJoint3D(connectedBody);
    ret->autorelease();
    return ret;
}

SixDofJoint3D* SixDofJoint3D::createInWorldSpace(const Vec3& anchor, const Vec3& axis, Rigidbody3D* connectedBody)
{
    auto ret = create(connectedBody);
    ret->setAnchorsInWorldSpace(anchor);
    ret->setAxis(axis);
    ret->setConnectedAxis(axis);
    return ret;
}

SixDofJoint3D::SixDofJoint3D(Rigidbody3D* connectedBody) : Joint3D(connectedBody)
{
    setName("SixDofJoint3D"sv);
}

bool SixDofJoint3D::attachToBody()
{
    if (isAttached())
        return true;
    if (!Joint3D::attachToBody())
        return false;

    JPH::SixDOFConstraintSettings settings;
    configureAnchors(this, settings);
    auto [slideAxis1, normalAxis1] = makeOrthoNormalBasis(_axis, Vec3::UNIT_Y);
    auto [slideAxis2, normalAxis2] = makeOrthoNormalBasis(_connectedAxis, Vec3::UNIT_Y);

    settings.mAxisX1 = jphutil::cast(slideAxis1);
    settings.mAxisY1 = jphutil::cast(normalAxis1);
    settings.mAxisX2 = jphutil::cast(slideAxis2);
    settings.mAxisY2 = jphutil::cast(normalAxis2);

    _constraint = _world->internalHandle().GetBodyInterface().CreateConstraint(
        &settings, _attachedBody->internalHandle(), jphutil::cast(_connectedBody));
    addConstraintToWorld(_world, _constraint);
    setLinearLimits(_linearLimits);
    setAngularLimits(_angularLimits);

    return true;
}

void SixDofJoint3D::setLinearLimits(const SixDofLimits& limits)
{
    _linearLimits = limits;
    if (auto constraint = static_cast<JPH::SixDOFConstraint*>(_constraint))
        constraint->SetTranslationLimits(jphutil::cast(limits.lower), jphutil::cast(limits.upper));
}

void SixDofJoint3D::setAngularLimits(const SixDofLimits& limits)
{
    _angularLimits = limits;
    if (auto* constraint = static_cast<JPH::SixDOFConstraint*>(_constraint))
    {
        constraint->SetRotationLimits(
            jphutil::cast(Vec3(MathUtil::radians(limits.lower.x), MathUtil::radians(limits.lower.y),
                               MathUtil::radians(limits.lower.z))),
            jphutil::cast(Vec3(MathUtil::radians(limits.upper.x), MathUtil::radians(limits.upper.y),
                               MathUtil::radians(limits.upper.z))));
    }
}

bool SixDofJoint3D::isLimited(int limitIndex) const
{
    switch (limitIndex)
    {
    case 0:
        return _linearLimits.lower.x <= _linearLimits.upper.x;
    case 1:
        return _linearLimits.lower.y <= _linearLimits.upper.y;
    case 2:
        return _linearLimits.lower.z <= _linearLimits.upper.z;
    case 3:
        return _angularLimits.lower.x <= _angularLimits.upper.x;
    case 4:
        return _angularLimits.lower.y <= _angularLimits.upper.y;
    case 5:
        return _angularLimits.lower.z <= _angularLimits.upper.z;
    default:
        return false;
    }
}

}  // namespace ax

#endif
