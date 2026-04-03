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

#include "axmol/2d/physics/Joint2D.h"
#if defined(AX_ENABLE_PHYSICS_2D)

#    include "axmol/2d/physics/Rigidbody2D.h"
#    include "axmol/2d/physics/PhysicsWorld2D.h"
#    include "axmol/2d/physics/PhysicsUtility2D.h"
#    include "axmol/scene/Node.h"

#    include <numbers>

namespace ax
{
#    pragma region Joint2D
Joint2D::Joint2D(Rigidbody2D* connectedBody) : _connectedBody(connectedBody) {}

Joint2D::~Joint2D()
{
    detachFromBody();
}

void Joint2D::detachFromBody()
{
    if (isAttached())
    {
        b2DestroyJoint(_jointId, true);
        _jointId = b2_nullJointId;
    }
}

bool Joint2D::attachToBody()
{
    _attachedBody = _owner->getRigidbody2D();
    if (!_attachedBody)
    {
        AXLOGE("Joint2D({})::attachToBody: owner node does not have physics body", _name);
        return false;
    }

    if (!_attachedBody->attachToWorld())
    {
        AXLOGE("Joint2D({}):attachToBody: attached body not ready", _name);
        return false;
    }

    _world = _attachedBody->getWorld();

    if (!_connectedBody)
    {
        AXLOGE("Joint2D({})::attachToBody: connected body is null", _name);
        return false;
    }

    // The connected body owner onEnter may not invoked, so we get physics world from attached body
    if (!_connectedBody->attachToWorld(_attachedBody->getWorld()))
    {
        AXLOGE("Joint2D({}):attachToBody: connected body not ready", _name);
        return false;
    }

    return true;
}

void Joint2D::onEnter()
{
    attachToBody();
}

void Joint2D::onExit()
{
    // before detach from body, restore body collision state
    // OPTIMIZE: use a counter to stats disable collision times
    if (!_collisionEnabled && isAttached())
        _attachedBody->setCollisionEnabled(true);

    detachFromBody();
}

void Joint2D::setConnectedBody(Rigidbody2D* body)
{
    // not allow to change connected body after joint is created
    if (_connectedBody || !body)
        return;
    _connectedBody = body;

    // try attach body once _connectedBody is set
    attachToBody();
}

void Joint2D::setCollisionEnabled(bool enable)
{
    if (_collisionEnabled != enable)
    {
        _collisionEnabled = enable;
    }
}

void Joint2D::setAnchorsInParentSpace(const Vec2& anchor)
{
    setAnchorSpace(AnchorSpace::Parent);
    setAnchor(anchor);
    setConnectedAnchor(anchor);
}

void Joint2D::setAnchor(const Vec2& anchor)
{
    _anchor = anchor;
}

void Joint2D::setConnectedAnchor(const Vec2& anchor)
{
    // diable auto configure connected anchor if connected anchor is set.
    _autoConfigureConnectedAnchor = false;
    _connectedAnchor              = anchor;
}

void Joint2D::setAutoConfigureConnectedAnchor(bool val)
{
    _autoConfigureConnectedAnchor = val;
}

void Joint2D::configureBaseSettings(b2JointDef& jd)
{
    jd.bodyIdA          = _attachedBody->internalHandle();
    jd.bodyIdB          = _connectedBody->internalHandle();
    jd.collideConnected = _collisionEnabled;
    jd.userData         = this;
    jd.drawScale        = _drawScale;

    if (!_collisionEnabled)
        _attachedBody->setCollisionEnabled(false);

    auto& localAnchorA = jd.localFrameA.p;
    auto& localAnchorB = jd.localFrameB.p;

    // coinfigure anchorA
    if (_anchorSpace == AnchorSpace::Local)
    {
        localAnchorA = PhysicsUtility2D::tob2Vec2(_anchor);
    }
    else
    {
        _attachedBody->updateTransform();
        localAnchorA = b2Body_GetLocalPoint(_attachedBody->internalHandle(), PhysicsUtility2D::tob2Vec2(_anchor));
    }

    // configure anchorB
    if (_autoConfigureConnectedAnchor)
    {
        b2Vec2 worldAnchorA = b2Body_GetWorldPoint(_attachedBody->internalHandle(), localAnchorA);
        _connectedAnchor    = _anchorSpace == AnchorSpace::Local ? PhysicsUtility2D::toVec2(localAnchorA)
                                                                 : PhysicsUtility2D::toVec2(worldAnchorA);

        if (_connectedBody && _connectedBody->isAttached())
        {
            _connectedBody->updateTransform();
            localAnchorB = b2Body_GetLocalPoint(_connectedBody->internalHandle(), worldAnchorA);
        }
        else
        {
            localAnchorB = worldAnchorA;
        }
    }
    else
    {
        auto connectedAnchor = PhysicsUtility2D::tob2Vec2(_connectedAnchor);

        if (_connectedBody && _connectedBody->isAttached() && _anchorSpace == AnchorSpace::Parent)
        {
            _connectedBody->updateTransform();
            localAnchorB = b2Body_GetLocalPoint(_connectedBody->internalHandle(), connectedAnchor);
        }
        else
        {
            localAnchorB = connectedAnchor;
        }
    }
}

float Joint2D::calcuateDistance(const b2Vec2& anchorA, const b2Vec2& anchorB)
{
    _attachedBody->updateTransform();
    _connectedBody->updateTransform();

    const b2Vec2 worldAnchorA = b2Body_GetWorldPoint(_attachedBody->internalHandle(), anchorA);
    const b2Vec2 worldAnchorB = b2Body_GetWorldPoint(_connectedBody->internalHandle(), anchorB);
    return std::clamp<float>(b2Length(b2Sub(worldAnchorB, worldAnchorA)), physics2d::LinearSlop, physics2d::LargeClamp);
}

#    pragma endregion

#    pragma region FixedJoint2D
FixedJoint2D::FixedJoint2D(Rigidbody2D* connectedBody) : Joint2D(connectedBody)
{
    _name = "FixedJoint2D"sv;
}

FixedJoint2D* FixedJoint2D::create(Rigidbody2D* connectedBody)
{
    auto joint = new FixedJoint2D(connectedBody);
    joint->autorelease();
    return joint;
}

void FixedJoint2D::setDamping(float damping)
{
    _damping = damping;
    if (isAttached())
        b2WeldJoint_SetLinearDampingRatio(_jointId, damping);
}

void FixedJoint2D::setFrequency(float hertz)
{
    _frequency = hertz;
    if (isAttached())
        b2WeldJoint_SetLinearHertz(_jointId, hertz);
}

bool FixedJoint2D::attachToBody()
{
    if (isAttached())
        return true;

    if (!Joint2D::attachToBody())
        return false;

    // Configure the joint definition.
    b2WeldJointDef jointDef = b2DefaultWeldJointDef();
    configureBaseSettings(jointDef.base);

    jointDef.linearDampingRatio = _damping;
    jointDef.linearHertz        = _frequency;

    _jointId = b2CreateWeldJoint(_world->internalHandle(), &jointDef);

    return isAttached();
}

#    pragma endregion

#    pragma region DistanceJoint2D
DistanceJoint2D* DistanceJoint2D::create(Rigidbody2D* connectedBody)
{
    auto joint = new DistanceJoint2D(connectedBody);
    joint->autorelease();
    return joint;
}

void DistanceJoint2D::setDamping(float damping)
{
    _damping = std::clamp(damping, physics2d::LinearSlop, physics2d::MaxForce);
    if (isAttached())
        b2DistanceJoint_SetSpringDampingRatio(_jointId, _damping);
}

DistanceJoint2D::DistanceJoint2D(Rigidbody2D* connectedBody) : Joint2D(connectedBody)
{
    _name = "DistanceJoint2D"sv;
}

bool DistanceJoint2D::attachToBody()
{
    do
    {
        if (isAttached())
            return true;

        if (!Joint2D::attachToBody())
            return false;

        b2DistanceJointDef jointDef = b2DefaultDistanceJointDef();

        configureBaseSettings(jointDef.base);

        _restLength = calcuateDistance(jointDef.base.localFrameA.p, jointDef.base.localFrameB.p);

        // !IMPORTANT
        jointDef.length       = _restLength;
        jointDef.enableLimit  = _useLimits;
        jointDef.enableSpring = _useLimits;
        if (jointDef.enableLimit)
        {
            jointDef.minLength    = _limits.x;
            jointDef.maxLength    = _limits.y;
            jointDef.dampingRatio = _damping;
        }
        else
        {
            jointDef.minLength = jointDef.length;
            jointDef.maxLength = jointDef.length;
        }

        _jointId = b2CreateDistanceJoint(_world->internalHandle(), &jointDef);

        AX_BREAK_IF(!isAttached());

        return true;
    } while (false);

    return false;
}

void DistanceJoint2D::setLength(float value)
{
    _restLength = std::clamp<float>(value, physics2d::LinearSlop, physics2d::LargeClamp);
    if (isAttached())
        b2DistanceJoint_SetLength(_jointId, value);
}

void DistanceJoint2D::setUseLimits(bool bval)
{
    _useLimits = bval;
    if (isAttached())
    {
        b2DistanceJoint_EnableLimit(_jointId, bval);
        b2DistanceJoint_EnableSpring(_jointId, bval);
    }
}

void DistanceJoint2D::setLimits(const JointLengthLimit2D& limits)
{
    _limits.x = std::clamp(limits.x, physics2d::LinearSlop, physics2d::LargeClamp);
    _limits.y = std::clamp(limits.y, physics2d::LinearSlop, physics2d::LargeClamp);
    if (_limits.x > _limits.y)
        std::swap(_limits.x, _limits.y);

    setUseLimits(true);
    if (isAttached())
        b2DistanceJoint_SetLengthRange(_jointId, _limits.x, _limits.x);
}

#    pragma endregion

#    pragma region SpringJoint2D
SpringJoint2D* SpringJoint2D::create(float stiffness, float damping, Rigidbody2D* connectedBody)
{
    auto joint = new SpringJoint2D(stiffness, damping, connectedBody);
    joint->autorelease();
    return joint;
}

SpringJoint2D::SpringJoint2D(float stiffness, float damping, Rigidbody2D* connectedBody) : Joint2D(connectedBody)
{
    _name      = "SpringJoint2D"sv;
    _stiffness = stiffness;
    _damping   = damping;
}

bool SpringJoint2D::attachToBody()
{
    do
    {
        if (isAttached())
            return true;

        if (!Joint2D::attachToBody())
            return false;

        b2DistanceJointDef jointDef = b2DefaultDistanceJointDef();
        configureBaseSettings(jointDef.base);
        if (_restLength < FLT_EPSILON)
            _restLength = calcuateDistance(jointDef.base.localFrameA.p, jointDef.base.localFrameB.p);
        jointDef.length       = _restLength;
        jointDef.enableSpring = true;

        _jointId = b2CreateDistanceJoint(_world->internalHandle(), &jointDef);
        AX_BREAK_IF(!isAttached());

        applySettings();

        return true;
    } while (false);

    return false;
}

void SpringJoint2D::applySettings()
{
    // calculate effective mass
    float m1   = _attachedBody->getMass();
    float m2   = _connectedBody->getMass();
    float mass = (m1 * m2) / (m1 + m2);

    // calculate hertz
    float springHertz = sqrtf(_stiffness / mass) / (2.0f * std::numbers::pi_v<float>);

    // calculate damping ratio
    float springDampingRatio = _damping / (2.0f * sqrtf(_stiffness * mass));

    b2DistanceJoint_SetSpringHertz(_jointId, springHertz);
    b2DistanceJoint_SetSpringDampingRatio(_jointId, springDampingRatio);
}

float SpringJoint2D::getRestLength() const
{
    return _restLength;
}

void SpringJoint2D::setRestLength(float restLength)
{
    _restLength = restLength;
    if (isAttached())
        b2DistanceJoint_SetLength(_jointId, restLength);
}

float SpringJoint2D::getStiffness() const
{
    return _stiffness;
}

void SpringJoint2D::setStiffness(float stiffness)
{
    _stiffness = stiffness;
    if (isAttached())
        applySettings();
}

float SpringJoint2D::getDamping() const
{
    return _damping;
}

void SpringJoint2D::setDamping(float damping)
{
    _damping = damping;
    if (isAttached())
        applySettings();
}

#    pragma endregion

#    pragma region SliderJoint2D
SliderJoint2D* SliderJoint2D::create(Rigidbody2D* connectedBody)
{
    auto joint = new SliderJoint2D(connectedBody);
    joint->autorelease();
    return joint;
}

bool SliderJoint2D::attachToBody()
{
    do
    {
        if (isAttached())
            return true;

        if (!Joint2D::attachToBody())
            return false;

        b2PrismaticJointDef jointDef = b2DefaultPrismaticJointDef();
        configureBaseSettings(jointDef.base);
        jointDef.enableMotor      = _useMotor;
        jointDef.enableLimit      = _useLimits;
        jointDef.motorSpeed       = _motor.x;
        jointDef.maxMotorForce    = _motor.y;
        jointDef.lowerTranslation = _translationLimits.x;
        jointDef.upperTranslation = _translationLimits.y;

        jointDef.base.localFrameA.q = b2MakeRot(MathUtil::radians(_angle));
        jointDef.base.localFrameB.q = jointDef.base.localFrameA.q;

        _jointId = b2CreatePrismaticJoint(_world->internalHandle(), &jointDef);
        AX_BREAK_IF(!isAttached());

        return true;
    } while (false);

    return false;
}

SliderJoint2D::SliderJoint2D(Rigidbody2D* connectedBody) : Joint2D(connectedBody)
{
    _name = "SliderJoint2D"sv;
}

void SliderJoint2D::setAngle(float angle)
{
    _angle = std::clamp(angle, -physics2d::MaxAngleDeg, physics2d::MaxAngleDeg);
    if (isAttached())
    {
        auto frameA = b2Joint_GetLocalFrameA(_jointId);
        auto frameB = b2Joint_GetLocalFrameB(_jointId);
        frameA.q    = b2MakeRot(MathUtil::radians(angle));
        frameB.q    = frameA.q;

        b2Joint_SetLocalFrameA(_jointId, frameA);
        b2Joint_SetLocalFrameB(_jointId, frameB);
    }
}

void SliderJoint2D::setUseMotor(bool enable)
{
    if (_useMotor != enable)
    {
        _useMotor = enable;
        if (isAttached())
            b2PrismaticJoint_EnableMotor(_jointId, enable);
    }
}

void SliderJoint2D::setUseLimits(bool enable)
{
    if (_useMotor != enable)
    {
        _useLimits = enable;

        if (isAttached())
            b2PrismaticJoint_EnableMotor(_jointId, _useLimits);
    }
}

void SliderJoint2D::setMotor(const JointMotor2D& motor)
{
    _motor = motor;
    setUseMotor(true);
    if (isAttached())
    {
        b2PrismaticJoint_SetMotorSpeed(_jointId, motor.x);
        b2PrismaticJoint_SetMaxMotorForce(_jointId, motor.y);
    }
}

void SliderJoint2D::setLimits(const JointTranslationLimits2D& limits)
{
    _translationLimits = limits;
    setUseLimits(true);
    if (isAttached())
        b2PrismaticJoint_SetLimits(_jointId, _translationLimits.x, _translationLimits.y);
}

float SliderJoint2D::getJointTranslation() const
{
    return isAttached() ? b2PrismaticJoint_GetTranslation(_jointId) : 0.0f;
}

float SliderJoint2D::getJointSpeed() const
{
    return isAttached() ? AX_RADIANS_TO_DEGREES(b2PrismaticJoint_GetSpeed(_jointId)) : 0.0f;
}

float SliderJoint2D::getMotorForce() const
{
    return isAttached() ? b2PrismaticJoint_GetMotorForce(_jointId) : 0.0f;
}

#    pragma endregion

#    pragma region WheelJoint2D
WheelJoint2D* WheelJoint2D::create(Rigidbody2D* connectedBody)
{
    auto joint = new WheelJoint2D(connectedBody);
    joint->autorelease();
    return joint;
}

WheelJoint2D::WheelJoint2D(Rigidbody2D* connectedBody) : Joint2D(connectedBody)
{
    _name = "WheelJoint2D"sv;
}

bool WheelJoint2D::attachToBody()
{
    do
    {
        if (isAttached())
            return true;

        if (!Joint2D::attachToBody())
            return false;

        // Configure the joint definition.
        b2WheelJointDef jointDef = b2DefaultWheelJointDef();
        configureBaseSettings(jointDef.base);

        jointDef.enableMotor    = _useMotor;
        jointDef.motorSpeed     = MathUtil::radians(_motor.x);
        jointDef.maxMotorTorque = _motor.y;

        jointDef.enableLimit      = _useLimits;
        jointDef.lowerTranslation = _limits.x;
        jointDef.upperTranslation = _limits.y;

        jointDef.hertz        = _stiffness;
        jointDef.dampingRatio = _damping;

        jointDef.base.localFrameA.q = b2MakeRot(MathUtil::radians(_angle));

        _jointId = b2CreateWheelJoint(_world->internalHandle(), &jointDef);

        AX_BREAK_IF(!isAttached());

        return true;
    } while (false);

    return false;
}

void WheelJoint2D::setUseMotor(bool enable)
{
    if (_useMotor != enable)
    {
        _useMotor = enable;
        if (isAttached())
            b2PrismaticJoint_EnableMotor(_jointId, enable);
    }
}

void WheelJoint2D::setMotor(const JointMotor2D& motor)
{
    _motor = motor;

    // Motor is automatically enabled if motor is set.
    setUseMotor(true);
    if (isAttached())
    {
        b2WheelJoint_SetMotorSpeed(_jointId, MathUtil::radians(motor.x));
        b2WheelJoint_SetMaxMotorTorque(_jointId, motor.y);
    }
}

void WheelJoint2D::setDamping(float damping)
{
    _damping = damping;
    if (isAttached())
        b2WheelJoint_SetSpringDampingRatio(_jointId, damping);
}

void WheelJoint2D::setUseLimits(bool enable)
{
    _useLimits = enable;
    if (isAttached())
        b2WheelJoint_EnableLimit(_jointId, enable);
}

void WheelJoint2D::setLimits(const JointTranslationLimits2D& limits)
{
    _limits = limits;

    if (isAttached())
    {
        b2WheelJoint_EnableLimit(_jointId, true);
        b2WheelJoint_SetLimits(_jointId, _limits.x, _limits.y);
    }
}

void WheelJoint2D::setStiffness(float hertz)
{
    _stiffness = hertz;
    if (isAttached())
        b2WheelJoint_SetSpringHertz(_jointId, hertz);
}

float WheelJoint2D::getMotorTorque() const
{
    return isAttached() ? b2WheelJoint_GetMotorTorque(_jointId) : _motor.y;
}

#    pragma endregion

#    pragma region PivotJoint2D
PivotJoint2D* PivotJoint2D::create(Rigidbody2D* connectedBody)
{
    auto joint = new PivotJoint2D(connectedBody);
    joint->autorelease();
    return joint;
}

PivotJoint2D::PivotJoint2D(Rigidbody2D* connectedBody) : Joint2D(connectedBody)
{
    _name = "PivotJoint2D"sv;
}

bool PivotJoint2D::attachToBody()
{
    do
    {
        if (isAttached())
            return true;

        if (!Joint2D::attachToBody())
            return false;

        // Configure the joint definition.
        b2RevoluteJointDef jointDef = b2DefaultRevoluteJointDef();
        configureBaseSettings(jointDef.base);
        jointDef.enableSpring   = _useSpring;
        jointDef.enableMotor    = _useMotor;
        jointDef.enableLimit    = _useLimits;
        jointDef.motorSpeed     = MathUtil::radians(_motor.x);
        jointDef.maxMotorTorque = _motor.y;
        jointDef.lowerAngle     = MathUtil::radians(_angleLimits.x);
        jointDef.upperAngle     = MathUtil::radians(_angleLimits.y);
        if (jointDef.lowerAngle > jointDef.upperAngle)
            std::swap(jointDef.lowerAngle, jointDef.upperAngle);

        _jointId = b2CreateRevoluteJoint(_world->internalHandle(), &jointDef);

        AX_BREAK_IF(!isAttached());

        return true;
    } while (false);

    return false;
}

void PivotJoint2D::setUseMotor(bool enable)
{
    _useMotor = enable;
    if (isAttached())
        b2RevoluteJoint_EnableMotor(_jointId, enable);
}

void PivotJoint2D::setUseLimits(bool enable)
{
    _useLimits = enable;
    if (isAttached())
        b2RevoluteJoint_EnableLimit(_jointId, enable);
}

void PivotJoint2D::setMotor(const JointMotor2D& motor)
{
    _motor = motor;

    setUseMotor(true);

    if (isAttached())
    {
        b2RevoluteJoint_SetMotorSpeed(_jointId, motor.x);
        b2RevoluteJoint_SetMaxMotorTorque(_jointId, motor.y);
    }
}

float PivotJoint2D::getMotorTorque() const
{
    return isAttached() ? b2RevoluteJoint_GetMotorTorque(_jointId) : 0.0f;
}

void PivotJoint2D::setLimits(const JointAngleLimits2D& limits)
{
    _angleLimits = limits;
    setUseLimits(true);
    if (isAttached())
    {
        // Ensure limits are valid.
        float lower = MathUtil::radians(_angleLimits.x);
        float upper = MathUtil::radians(_angleLimits.y);
        if (lower > upper)
            std::swap(lower, upper);
        b2RevoluteJoint_SetLimits(_jointId, lower, upper);
    }
}

void PivotJoint2D::setUseSpring(bool bval)
{
    _useSpring = bval;
    if (isAttached())
        b2RevoluteJoint_EnableSpring(_jointId, bval);
}

void PivotJoint2D::setSpringDamping(float value)
{
    _damping = value;
    if (isAttached())
        b2RevoluteJoint_SetSpringDampingRatio(_jointId, value);
}

void PivotJoint2D::setSpringFrequency(float value)
{
    _frequency = value;
    if (isAttached())
        b2RevoluteJoint_SetSpringHertz(_jointId, value);
}

float PivotJoint2D::getReferenceAngle() const
{
    return isAttached() ? b2RevoluteJoint_GetAngle(_jointId) : 0.0f;
}

#    pragma endregion

#    pragma region PinJoint2D
PinJoint2D* PinJoint2D::create(const Vec2& pivot, Rigidbody2D* connectedBody)
{
    auto joint = new PinJoint2D(pivot, connectedBody);
    joint->autorelease();
    return joint;
}

PinJoint2D::PinJoint2D(const Vec2& pivot, Rigidbody2D* connectedBody) : PivotJoint2D(connectedBody)
{
    _name = "PinJoint2D"sv;
    setAnchorsInParentSpace(pivot);
    setDrawScale(0.0f);
}
#    pragma endregion

#    pragma region MotorJoint2D
MotorJoint2D* MotorJoint2D::create(Rigidbody2D* connectedBody)
{
    auto joint = new MotorJoint2D(connectedBody);
    joint->autorelease();
    return joint;
}

MotorJoint2D::MotorJoint2D(Rigidbody2D* connectedBody) : Joint2D(connectedBody)
{
    _name = "MotorJoint2D"sv;

    _settings                 = b2DefaultMotorJointDef();
    _settings.maxSpringForce  = physics2d::MaxForce;
    _settings.maxSpringTorque = physics2d::MaxForce;

    _settings.maxVelocityForce  = physics2d::MaxForce;
    _settings.maxVelocityTorque = physics2d::MaxForce;

    _settings.angularVelocity     = M_PI / 2.f;
    _settings.angularHertz        = 2.0f;
    _settings.angularDampingRatio = 0.5f;
}

void MotorJoint2D::setAngularVelocity(float val)
{
    _settings.angularVelocity = MathUtil::radians(val);
    if (isAttached())
        b2MotorJoint_SetAngularVelocity(_jointId, val);
}

void MotorJoint2D::setAngularFrequency(float value)
{
    _settings.angularHertz = value;
    if (isAttached())
        b2MotorJoint_SetAngularHertz(_jointId, value);
}

void MotorJoint2D::setAngularDamping(float val)
{
    _settings.angularDampingRatio = val;
    if (isAttached())
        b2MotorJoint_SetAngularDampingRatio(_jointId, val);
}

void MotorJoint2D::setLinearVelocity(const Vec2& val)
{
    _settings.linearVelocity = PhysicsUtility2D::tob2Vec2(val);
    if (isAttached())
        b2MotorJoint_SetLinearVelocity(_jointId, _settings.linearVelocity);
}

void MotorJoint2D::setLinearFrequency(float val)
{
    _settings.linearHertz = val;
    if (isAttached())
        b2MotorJoint_SetLinearHertz(_jointId, val);
}

void MotorJoint2D::setLinearDamping(float val)
{
    _settings.linearDampingRatio = val;
    if (isAttached())
        b2MotorJoint_SetLinearDampingRatio(_jointId, val);
}

Vec2 MotorJoint2D::getLinearVelocity() const
{
    return PhysicsUtility2D::toVec2(_settings.linearVelocity);
}

bool MotorJoint2D::attachToBody()
{
    do
    {
        if (isAttached())
            return true;

        if (!Joint2D::attachToBody())
            return false;

        // Configure the joint definition.
        b2MotorJointDef jointDef = _settings;
        configureBaseSettings(jointDef.base);

        _jointId = b2CreateMotorJoint(_world->internalHandle(), &jointDef);

        AX_BREAK_IF(!isAttached());

        return true;
    } while (false);

    return false;
}

#    pragma endregion

#    pragma region FilterJoint2D
FilterJoint2D* FilterJoint2D::create(Rigidbody2D* connectedBody)
{
    auto joint = new FilterJoint2D(connectedBody);
    joint->autorelease();
    return joint;
}

FilterJoint2D::FilterJoint2D(Rigidbody2D* connectedBody) : Joint2D(connectedBody)
{
    _name             = "FilterJoint2D"sv;
    _collisionEnabled = false;
}

bool FilterJoint2D::attachToBody()
{
    do
    {
        if (isAttached())
            return true;

        if (!Joint2D::attachToBody())
            return false;

        // Configure the joint definition.
        b2FilterJointDef jointDef = b2DefaultFilterJointDef();
        configureBaseSettings(jointDef.base);

        _jointId = b2CreateFilterJoint(_world->internalHandle(), &jointDef);

        AX_BREAK_IF(!isAttached());

        return true;
    } while (false);

    return false;
}
#    pragma endregion

#    pragma region TargetJoint2D
TargetJoint2D* TargetJoint2D::create(const Vec2& target, Rigidbody2D* connectedBody)
{
    auto joint = new TargetJoint2D(target, connectedBody);
    joint->autorelease();
    return joint;
}

TargetJoint2D::TargetJoint2D(const Vec2& target, Rigidbody2D* connectedBody) : Joint2D(connectedBody)
{
    _name = "TargetJoint2D"sv;
    setAnchorSpace(Joint2D::AnchorSpace::Parent);
    setConnectedAnchor(target);
}

void TargetJoint2D::setMaxForceScale(float maxForceScale)
{
    _maxForceScale = maxForceScale;
    if (isAttached())
    {
        b2MotorJoint_SetMaxSpringForce(_jointId, calculateMaxForce(maxForceScale));
    }
}

float TargetJoint2D::getMaxForce() const
{
    if (isAttached())
    {
        return b2MotorJoint_GetMaxSpringForce(_jointId);
    }
    return 0.0f;
}

void TargetJoint2D::setDamping(float dampingRatio)
{
    _damping = dampingRatio;
    if (isAttached())
    {
        b2MotorJoint_SetLinearDampingRatio(_jointId, _damping);
    }
}

void TargetJoint2D::setFrequency(float hertz)
{
    _frequency = hertz;
    if (isAttached())
    {
        b2MotorJoint_SetLinearHertz(_jointId, hertz);
    }
}

float TargetJoint2D::calculateMaxForce(float maxForceScale) const
{
    if (!_connectedBody || !_connectedBody->isAttached())
        return 0.0f;
    b2MassData massData = b2Body_GetMassData(_connectedBody->internalHandle());
    float g             = b2Length(b2World_GetGravity(_world->internalHandle()));
    return maxForceScale * massData.mass * g;
}

bool TargetJoint2D::attachToBody()
{
    do
    {
        if (isAttached())
            return true;

        if (!Joint2D::attachToBody())
            return false;

        _attachedBody->setBodyType(Rigidbody2D::KinematicBody);
        b2Body_EnableSleep(_attachedBody->internalHandle(), false);

        b2MotorJointDef jointDef = b2DefaultMotorJointDef();
        configureBaseSettings(jointDef.base);

        jointDef.linearHertz        = _frequency;
        jointDef.linearDampingRatio = _damping;

        b2MassData massData = b2Body_GetMassData(_connectedBody->internalHandle());
        float g             = b2Length(b2World_GetGravity(_world->internalHandle()));
        float mg            = massData.mass * std::max(g, 9.8f);

        jointDef.maxSpringForce = _maxForceScale * mg;

        if (massData.mass > 0.0f)
        {
            // This acts like angular friction
            float lever                = sqrtf(massData.rotationalInertia / massData.mass);
            jointDef.maxVelocityTorque = 0.25f * lever * mg;
        }

        _jointId = b2CreateMotorJoint(_world->internalHandle(), &jointDef);

        AX_BREAK_IF(!isAttached());

        return true;
    } while (false);

    return false;
}

#    pragma endregion

}  // namespace ax
#endif  // AX_ENABLE_PHYSICS_2D
