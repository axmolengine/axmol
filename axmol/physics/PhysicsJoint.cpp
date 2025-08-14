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

#include "axmol/physics/PhysicsJoint.h"
#if defined(AX_ENABLE_PHYSICS)

#    include "axmol/physics/PhysicsBody.h"
#    include "axmol/physics/PhysicsWorld.h"
#    include "axmol/physics/PhysicsHelper.h"
#    include "axmol/2d/Node.h"

#    include <numbers>

namespace ax
{

template <typename T>
class Optional
{

public:
    Optional() {}
    Optional(T d) : _isSet(true), _data(d) {}
    Optional(const Optional& t) : _isSet(t._isSet), _data(t._data) {}

    // bool isNull()       const { return !_isSet; }
    // bool isDefineded()  const { return _isSet; }
    // bool isEmpty()      const { return !_isSet; }

    T get() const
    {
        AXASSERT(_isSet, "data should be set!");
        return _data;
    }
    void set(T d)
    {
        _isSet = true;
        _data  = d;
    }

private:
    bool _isSet = false;
    T _data;
};

class WriteCache
{
public:
    Optional<Vec2> _grooveA;
    Optional<Vec2> _grooveB;
    Optional<Vec2> _anchr1;
    Optional<Vec2> _anchr2;
    Optional<float> _min;
    Optional<float> _max;
    Optional<float> _distance;
    Optional<float> _restLength;
    Optional<float> _restAngle;
    Optional<float> _stiffness;
    Optional<float> _damping;
    Optional<float> _angle;
    Optional<float> _phase;
    Optional<float> _ratchet;
    Optional<float> _ratio;
    Optional<float> _rate;
};

#    if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
#        define LIKELY(x)   (__builtin_expect((x), 1))
#        define UNLIKELY(x) (__builtin_expect((x), 0))
#    else
#        define LIKELY(x)   (x)
#        define UNLIKELY(x) (x)
#    endif

#    define AX_PJOINT_CACHE_READ(field)          \
        do                                       \
        {                                        \
            if (UNLIKELY(_initDirty))            \
            {                                    \
                return _writeCache->field.get(); \
            }                                    \
        } while (0)

#    define AX_PJOINT_CACHE_WRITE2(field, method, arg, convertedArg)               \
        do                                                                         \
        {                                                                          \
            if (UNLIKELY(_initDirty))                                              \
            {                                                                      \
                _writeCache->field.set(arg);                                       \
                delay([this, arg]() { method(_b2Joints.front(), convertedArg); }); \
            }                                                                      \
            else                                                                   \
            {                                                                      \
                method(_b2Joints.front(), convertedArg);                           \
            }                                                                      \
        } while (0)

#    define AX_PJOINT_CACHE_WRITE(field, method, arg) AX_PJOINT_CACHE_WRITE2(field, method, arg, arg)

static void b2JointSetMaxForce(b2JointId joint, float force)
{
    if (!b2Joint_IsValid(joint))
    {
        AXLOGE("b2JointSetMaxForce: joint is invalid");
        return;
    }
    auto jointType = b2Joint_GetType(joint);
    switch (jointType)
    {
    case b2_distanceJoint:
        b2DistanceJoint_SetMaxMotorForce(joint, force);
        break;
    case b2_filterJoint:
        break;
    case b2_motorJoint:
        b2MotorJoint_SetMaxForce(joint, force);
        break;
    case b2_mouseJoint:
        b2MouseJoint_SetMaxForce(joint, force);
        break;
    case b2_prismaticJoint:
        b2PrismaticJoint_SetMaxMotorForce(joint, force);
        break;
    case b2_revoluteJoint:
        break;
    case b2_weldJoint:
        break;
    case b2_wheelJoint:
        break;
    }
}

static float b2Body_GetAngle(const b2BodyId& bodyId)
{
    b2Rot rot   = b2Body_GetRotation(bodyId);
    float angle = b2Rot_GetAngle(rot);
    return angle;
}

PhysicsJoint::PhysicsJoint()
    : _bodyA(nullptr)
    , _bodyB(nullptr)
    , _world(nullptr)
    , _enable(false)
    , _collisionEnable(true)
    , _destroyMark(false)
    , _tag(0)
    , _maxForce(PHYSICS_INFINITY)
    , _initDirty(true)
{
    _writeCache = new WriteCache();
}

PhysicsJoint::~PhysicsJoint()
{
    deatchFromBody();
}

void PhysicsJoint::deatchFromBody()
{
    for (auto& joint : _b2Joints)
    {
        if(b2Joint_IsValid(joint))
            b2DestroyJoint(joint);
    }
    _b2Joints.clear();

    AX_SAFE_DELETE(_writeCache);
}

bool PhysicsJoint::init(ax::PhysicsBody* a, ax::PhysicsBody* b)
{
    do
    {
        AXASSERT(a != nullptr && b != nullptr, "the body passed in is nil");
        AXASSERT(a != b, "the two bodies are equal");

        _bodyA = a;
        _bodyB = b;
        _bodyA->_joints.emplace_back(this);
        _bodyB->_joints.emplace_back(this);

        return true;
    } while (false);

    return false;
}

bool PhysicsJoint::initJoint()
{
    if (_initDirty)
    {
        auto ret = buildConstraints();
        if (ret) {
            for (auto&& joint : _b2Joints)
                b2JointSetMaxForce(joint, _maxForce);
        }
        _initDirty = false;
        return ret;
    }
    return false;
}

void PhysicsJoint::flushDelayTasks()
{
    for (const auto& tsk : _delayTasks)
    {
        tsk();
    }
    _delayTasks.clear();
}

void PhysicsJoint::setEnable(bool enable)
{
    if (_enable != enable)
    {
        _enable = enable;

        if (_world)
        {
            if (enable)
            {
                _world->addJoint(this);
            }
            else
            {
                _world->removeJoint(this, false);
            }
        }
    }
}

void PhysicsJoint::setCollisionEnable(bool enable)
{
    if (_collisionEnable != enable)
    {
        _collisionEnable = enable;
    }
}

void PhysicsJoint::removeFormWorld()
{
    if (_world)
    {
        _world->removeJoint(this, false);
    }
}

void PhysicsJoint::setMaxForce(float force)
{
    if (_initDirty)
    {
        delay([this, force]() {
            _maxForce = force;
            for (auto&& joint : _b2Joints)
                b2JointSetMaxForce(joint, force);
        });
    }
    else
    {
        _maxForce = force;
        for (auto&& joint : _b2Joints)
            b2JointSetMaxForce(joint, force);
    }
}

PhysicsJointFixed* PhysicsJointFixed::instantiate(PhysicsBody* a, PhysicsBody* b, const Vec2& anchr)
{
    auto joint = new PhysicsJointFixed();

    if (joint->init(a, b))
    {
        joint->_anchr = anchr;
        return joint;
    }

    AX_SAFE_DELETE(joint);
    return nullptr;
}

bool PhysicsJointFixed::buildConstraints()
{
    do
    {
        _bodyA->getNode()->setPosition(_anchr);
        _bodyB->getNode()->setPosition(_anchr);

        //// add a pivot joint to fixed two body together
        // auto joint = cpPivotJointNew(_bodyA->getCPBody(), _bodyB->getCPBody(), phlp::vec22cpv(_anchr));
        // AX_BREAK_IF(!b2Joint_IsValid(joint));
        //_b2Joints.emplace_back(joint);

        //// add a gear joint to make two body have the same rotation.
        // joint = cpGearJointNew(_bodyA->getCPBody(), _bodyB->getCPBody(), 0, 1);
        // AX_BREAK_IF(!b2Joint_IsValid(joint));
        //_b2Joints.emplace_back(joint);

        // FIXME: not 100% identical to cpPivotJointNew and cpGearJointNew
        b2Vec2 a            = phlp::tob2Vec2(_anchr);
        b2WeldJointDef wd   = b2DefaultWeldJointDef();
        wd.bodyIdA          = _bodyA->getB2Body();
        wd.bodyIdB          = _bodyB->getB2Body();
        wd.localAnchorA     = b2Body_GetLocalPoint(wd.bodyIdA, a);
        wd.localAnchorB     = b2Body_GetLocalPoint(wd.bodyIdB, a);
        wd.referenceAngle   = b2Body_GetAngle(wd.bodyIdB) - b2Body_GetAngle(wd.bodyIdA);
        wd.collideConnected = false;
        auto weld           = b2CreateWeldJoint(_bodyA->getWorld()->getB2World(), &wd);
        AX_BREAK_IF(!b2Joint_IsValid(weld));
        _b2Joints.emplace_back(weld);

        _collisionEnable = false;

        return true;
    } while (false);

    return false;
}

PhysicsJointPin* PhysicsJointPin::instantiate(PhysicsBody* a, PhysicsBody* b, const Vec2& pivot)
{
    auto joint = new PhysicsJointPin();

    if (joint->init(a, b))
    {
        joint->_anchr1           = pivot;
        joint->_useSpecificAnchr = false;
        return joint;
    }

    AX_SAFE_DELETE(joint);
    return nullptr;
}

PhysicsJointPin* PhysicsJointPin::instantiate(PhysicsBody* a, PhysicsBody* b, const Vec2& anchr1, const Vec2& anchr2)
{
    auto joint = new PhysicsJointPin();

    if (joint->init(a, b))
    {
        joint->_anchr1           = anchr1;
        joint->_anchr2           = anchr2;
        joint->_useSpecificAnchr = true;

        return joint;
    }

    AX_SAFE_DELETE(joint);
    return nullptr;
}

bool PhysicsJointPin::buildConstraints()
{
    do
    {
        b2RevoluteJointDef pivotJointDef = b2DefaultRevoluteJointDef();
        pivotJointDef.bodyIdA            = _bodyA->getB2Body();
        pivotJointDef.bodyIdB            = _bodyB->getB2Body();
        pivotJointDef.collideConnected   = false;
        pivotJointDef.localAnchorA       = b2Body_GetLocalPoint(_bodyA->getB2Body(), phlp::tob2Vec2(_anchr1));
        if (_useSpecificAnchr)
        {
            pivotJointDef.localAnchorB = b2Body_GetLocalPoint(_bodyB->getB2Body(), phlp::tob2Vec2(_anchr2));
        }
        else
        {
            pivotJointDef.localAnchorB = b2Body_GetLocalPoint(_bodyB->getB2Body(), phlp::tob2Vec2(_anchr1));
        }

        auto joint = b2CreateRevoluteJoint(_bodyA->getWorld()->getB2World(), &pivotJointDef);

        AX_BREAK_IF(!b2Joint_IsValid(joint));
        _b2Joints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

PhysicsJointLimit* PhysicsJointLimit::instantiate(PhysicsBody* a,
                                                PhysicsBody* b,
                                                const Vec2& anchr1,
                                                const Vec2& anchr2,
                                                float min,
                                                float max)
{
    auto joint = new PhysicsJointLimit();

    if (joint->init(a, b))
    {
        joint->_anchr1 = anchr1;
        joint->_anchr2 = anchr2;
        joint->_min    = min;
        joint->_max    = max;

        return joint;
    }

    AX_SAFE_DELETE(joint);
    return nullptr;
}

PhysicsJointLimit* PhysicsJointLimit::instantiate(PhysicsBody* a, PhysicsBody* b, const Vec2& anchr1, const Vec2& anchr2)
{
    return instantiate(a, b, anchr1, anchr2, 0, b->local2World(anchr1).getDistance(a->local2World(anchr2)));
}

bool PhysicsJointLimit::buildConstraints()
{
    do
    {
        // alternative implementation using b2PrismaticJointDef
        // b2PrismaticJointDef def = b2DefaultPrismaticJointDef();
        // def.bodyIdA             = _bodyA->getB2Body();
        // def.bodyIdB             = _bodyB->getB2Body();
        // def.localAnchorA        = phlp::tob2Vec2(_anchr1);
        // def.localAnchorB        = phlp::tob2Vec2(_anchr2);
        // def.localAxisA          = b2Vec2{1.0f, 0.0f};  // Axis along which the bodies can slide
        // def.enableLimit         = true;
        // def.lowerTranslation    = _min;
        // def.upperTranslation    = _max;
        //
        // auto joint = b2CreatePrismaticJoint(_bodyA->getWorld()->getB2World(), &def);

        b2DistanceJointDef jd = b2DefaultDistanceJointDef();

        jd.bodyIdA = _bodyA->getB2Body();
        jd.bodyIdB = _bodyB->getB2Body();

        jd.localAnchorA = phlp::tob2Vec2(_anchr1);
        jd.localAnchorB = phlp::tob2Vec2(_anchr2);

        // !IMPORTANT: calculate the length based on the world position of the anchors
        jd.length = _bodyB->local2World(_anchr1).getDistance(_bodyA->local2World(_anchr2));

        jd.minLength = _min;
        jd.maxLength = _max;

        b2JointId joint = b2CreateDistanceJoint(_bodyA->getWorld()->getB2World(), &jd);

        AX_BREAK_IF(!b2Joint_IsValid(joint));
        _b2Joints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

float PhysicsJointLimit::getMin() const
{
    return _min;
}

void PhysicsJointLimit::setMin(float min)
{
    _min = min;
    b2PrismaticJoint_SetLimits(_b2Joints[0], min, _max);
}

float PhysicsJointLimit::getMax() const
{
    return _max;
}

void PhysicsJointLimit::setMax(float max)
{
    _max = max;

    b2PrismaticJoint_SetLimits(_b2Joints[0], _min, max);
}

Vec2 PhysicsJointLimit::getAnchr1() const
{
    return _anchr1;
}

void PhysicsJointLimit::setAnchr1(const Vec2& anchr)
{
    // NOTE: NOT SUPPORT
}

Vec2 PhysicsJointLimit::getAnchr2() const
{
    return _anchr2;
}

void PhysicsJointLimit::setAnchr2(const Vec2& anchr)
{
    // NOTE: NOT SUPPORT
}

PhysicsJointDistance* PhysicsJointDistance::instantiate(PhysicsBody* a,
                                                      PhysicsBody* b,
                                                      const Vec2& anchr1,
                                                      const Vec2& anchr2)
{
    auto joint = new PhysicsJointDistance();

    if (joint->init(a, b))
    {
        joint->_anchr1 = anchr1;
        joint->_anchr2 = anchr2;

        return joint;
    }

    AX_SAFE_DELETE(joint);
    return nullptr;
}

bool PhysicsJointDistance::buildConstraints()
{
    do
    {
        b2DistanceJointDef def = b2DefaultDistanceJointDef();

        def.bodyIdA = _bodyA->getB2Body();
        def.bodyIdB = _bodyB->getB2Body();

        def.localAnchorA = phlp::tob2Vec2(_anchr1);
        def.localAnchorB = phlp::tob2Vec2(_anchr2);

        // !IMPORTANT
        def.length = _bodyB->local2World(_anchr1).getDistance(_bodyA->local2World(_anchr2));

        // Set both min and max to the same value for a fixed distance joint
        def.minLength = def.length;
        def.maxLength = def.length;

        b2JointId joint = b2CreateDistanceJoint(_bodyA->getWorld()->getB2World(), &def);

        AX_BREAK_IF(!b2Joint_IsValid(joint));
        _b2Joints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

float PhysicsJointDistance::getDistance() const
{
    AX_PJOINT_CACHE_READ(_distance);
    return b2DistanceJoint_GetLength(_b2Joints[0]);
}

void PhysicsJointDistance::setDistance(float distance)
{
    AX_PJOINT_CACHE_WRITE(_distance, b2DistanceJoint_SetLength, distance);
}

PhysicsJointSpring* PhysicsJointSpring::instantiate(PhysicsBody* a,
                                                  PhysicsBody* b,
                                                  const Vec2& anchr1,
                                                  const Vec2& anchr2,
                                                  float stiffness,
                                                  float damping)
{
    auto joint = new PhysicsJointSpring();

    if (joint->init(a, b))
    {
        joint->_anchr1    = anchr1;
        joint->_anchr2    = anchr2;
        joint->_stiffness = stiffness;
        joint->_damping   = damping;

        return joint;
    }

    AX_SAFE_DELETE(joint);
    return nullptr;
}

bool PhysicsJointSpring::buildConstraints()
{
    do
    {
        b2DistanceJointDef def = b2DefaultDistanceJointDef();
        def.bodyIdA            = _bodyA->getB2Body();
        def.bodyIdB            = _bodyB->getB2Body();
        def.localAnchorA       = phlp::tob2Vec2(_anchr1);
        def.localAnchorB       = phlp::tob2Vec2(_anchr2);
        def.length             = _bodyB->local2World(_anchr1).getDistance(_bodyA->local2World(_anchr2));
        def.enableSpring       = true;

        // calculate effective mass
        float m1   = _bodyA->getMass();
        float m2   = _bodyB->getMass();
        float mass = (m1 * m2) / (m1 + m2);

        // calculate hertz
        def.hertz = sqrtf(_stiffness / mass) / (2.0f * std::numbers::pi_v<float>);

        // calculate damping ratio
        def.dampingRatio = _damping / (2.0f * sqrtf(_stiffness * mass));

        auto joint = b2CreateDistanceJoint(_bodyA->getWorld()->getB2World(), &def);
        AX_BREAK_IF(!b2Joint_IsValid(joint));
        _b2Joints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

Vec2 PhysicsJointSpring::getAnchr1() const
{
    AX_PJOINT_CACHE_READ(_anchr1);
    return phlp::toVec2(b2Joint_GetLocalAnchorA(_b2Joints[0]));
}

void PhysicsJointSpring::setAnchr1(const Vec2& anchr)
{
    // NOTE: NOT SUPPORT
    // AX_PJOINT_CACHE_WRITE2(_anchr1, cpDampedSpringSetAnchorA, anchr, phlp::vec22cpv(anchr));
}

Vec2 PhysicsJointSpring::getAnchr2() const
{
    AX_PJOINT_CACHE_READ(_anchr2);
    return phlp::toVec2(b2Joint_GetLocalAnchorB(_b2Joints[0]));
}

void PhysicsJointSpring::setAnchr2(const Vec2& anchr)
{
    // NOTE: NOT SUPPORT
    // AX_PJOINT_CACHE_WRITE2(_anchr2, cpDampedSpringSetAnchorB, anchr, phlp::vec22cpv(anchr));
}

float PhysicsJointSpring::getRestLength() const
{
    AX_PJOINT_CACHE_READ(_restLength);
    return b2DistanceJoint_GetLength(_b2Joints[0]);
}

void PhysicsJointSpring::setRestLength(float restLength)
{
    // FIXME:
    AX_PJOINT_CACHE_WRITE(_restLength, b2DistanceJoint_SetLength, restLength);
}

float PhysicsJointSpring::getStiffness() const
{
    AX_PJOINT_CACHE_READ(_stiffness);
    return b2DistanceJoint_GetSpringHertz(_b2Joints[0]);
}

void PhysicsJointSpring::setStiffness(float stiffness)
{
    AX_PJOINT_CACHE_WRITE(_stiffness, b2DistanceJoint_SetSpringHertz, stiffness);
}

float PhysicsJointSpring::getDamping() const
{
    AX_PJOINT_CACHE_READ(_damping);
    return b2DistanceJoint_GetSpringDampingRatio(_b2Joints[0]);
}

void PhysicsJointSpring::setDamping(float damping)
{
    AX_PJOINT_CACHE_WRITE(_damping, b2DistanceJoint_SetSpringDampingRatio, damping);
}

PhysicsJointGroove* PhysicsJointGroove::instantiate(PhysicsBody* a,
                                                  PhysicsBody* b,
                                                  const Vec2& grooveA,
                                                  const Vec2& grooveB,
                                                  const Vec2& anchr2)
{
    auto joint = new PhysicsJointGroove();

    if (joint->init(a, b))
    {
        joint->_grooveA = grooveA;
        joint->_grooveB = grooveB;
        joint->_anchr2  = anchr2;

        return joint;
    }

    AX_SAFE_DELETE(joint);
    return nullptr;
}

bool PhysicsJointGroove::buildConstraints()
{
    do
    {
        auto world = _bodyA->getWorld()->getB2World();

        b2PrismaticJointDef def = b2DefaultPrismaticJointDef();
        def.bodyIdA             = _bodyA->getB2Body();
        def.bodyIdB             = _bodyB->getB2Body();
        def.localAnchorA        = phlp::tob2Vec2(_grooveA);
        def.localAnchorB        = phlp::tob2Vec2(_anchr2);
        def.localAxisA          = phlp::tob2Vec2(_grooveB) - phlp::tob2Vec2(_grooveA);
        def.lowerTranslation    = 0.0f;
        def.upperTranslation    = b2Distance(phlp::tob2Vec2(_grooveA), phlp::tob2Vec2(_grooveB));
        def.enableLimit         = true;

        b2JointId joint = b2CreatePrismaticJoint(world, &def);
        AX_BREAK_IF(!b2Joint_IsValid(joint));
        _b2Joints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

Vec2 PhysicsJointGroove::getGrooveA() const
{
    AX_PJOINT_CACHE_READ(_grooveA);
    return phlp::toVec2(b2Joint_GetLocalAnchorA(_b2Joints.front()));
}

void PhysicsJointGroove::setGrooveA(const Vec2& grooveA)
{
    // NOTE: NOT SUPPORT
    // AX_PJOINT_CACHE_WRITE2(_grooveA, cpGrooveJointSetGrooveA, grooveA, phlp::vec22cpv(grooveA));
}

Vec2 PhysicsJointGroove::getGrooveB() const
{
    AX_PJOINT_CACHE_READ(_grooveB);

    return _grooveB;
    // return phlp::cpv2vec2(cpGrooveJointGetGrooveB(_b2Joints.front()));
}

void PhysicsJointGroove::setGrooveB(const Vec2& grooveB)
{
    // NOTE: NOT SUPPORT
    // AX_PJOINT_CACHE_WRITE2(_grooveB, cpGrooveJointSetGrooveB, grooveB, phlp::vec22cpv(grooveB));
}

Vec2 PhysicsJointGroove::getAnchr2() const
{
    AX_PJOINT_CACHE_READ(_anchr2);
    return phlp::toVec2(b2Joint_GetLocalAnchorB(_b2Joints.front()));
}

void PhysicsJointGroove::setAnchr2(const Vec2& anchr2)
{
    // AX_PJOINT_CACHE_WRITE2(_anchr2, cpGrooveJointSetAnchorB, anchr2, phlp::vec22cpv(anchr2));
}

PhysicsJointRotarySpring* PhysicsJointRotarySpring::instantiate(PhysicsBody* a,
                                                              PhysicsBody* b,
                                                              float stiffness,
                                                              float damping)
{
    auto joint = new PhysicsJointRotarySpring();

    if (joint->init(a, b))
    {
        joint->_stiffness = stiffness;
        joint->_damping   = damping;

        return joint;
    }

    AX_SAFE_DELETE(joint);
    return nullptr;
}

bool PhysicsJointRotarySpring::buildConstraints()
{
    do
    {
        auto world = _bodyA->getWorld()->getB2World();
        // Note: not 100% indentical to cpDampedRotarySpringNew
        // auto joint = cpDampedRotarySpringNew(_bodyA->getCPBody(), _bodyB->getCPBody(),
        //                                      _bodyB->getRotation() - _bodyA->getRotation(), _stiffness, _damping);
        b2MotorJointDef motorDef = b2DefaultMotorJointDef();
        motorDef.bodyIdA         = _bodyA->getB2Body();
        motorDef.bodyIdB         = _bodyB->getB2Body();
        motorDef.angularOffset   = AX_DEGREES_TO_RADIANS(_bodyB->getRotation()) -
                                 AX_DEGREES_TO_RADIANS(_bodyA->getRotation()); 
        motorDef.maxTorque        = 10000.0f;
        motorDef.correctionFactor = 1.0f;
        auto offset               = _bodyA->getPosition() - _bodyB->getPosition();
        motorDef.linearOffset     = phlp::tob2Vec2(offset);
        auto joint                = b2CreateMotorJoint(world, &motorDef);

        AX_BREAK_IF(!b2Joint_IsValid(joint));
        _b2Joints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

float PhysicsJointRotarySpring::getRestAngle() const
{
    AX_PJOINT_CACHE_READ(_restAngle);
    return b2RevoluteJoint_GetAngle(_b2Joints.front());
}

void PhysicsJointRotarySpring::setRestAngle(float restAngle)
{
    // NOTE: NOT SUPPORT
    // AX_PJOINT_CACHE_WRITE(_restAngle, b2RevoluteJoint_SetAngle, restAngle);
}

float PhysicsJointRotarySpring::getStiffness() const
{
    AX_PJOINT_CACHE_READ(_stiffness);
    return b2RevoluteJoint_GetSpringHertz(_b2Joints.front());
}

void PhysicsJointRotarySpring::setStiffness(float stiffness)
{
    AX_PJOINT_CACHE_WRITE(_stiffness, b2RevoluteJoint_SetSpringHertz, stiffness);
}

float PhysicsJointRotarySpring::getDamping() const
{
    AX_PJOINT_CACHE_READ(_damping);
    return b2RevoluteJoint_GetSpringDampingRatio(_b2Joints.front());
}

void PhysicsJointRotarySpring::setDamping(float damping)
{
    AX_PJOINT_CACHE_WRITE(_damping, b2RevoluteJoint_SetSpringDampingRatio, damping);
}

PhysicsJointRotaryLimit* PhysicsJointRotaryLimit::instantiate(PhysicsBody* a, PhysicsBody* b, float min, float max)
{
    auto joint = new PhysicsJointRotaryLimit();

    if (joint->init(a, b))
    {
        joint->_min = min;
        joint->_max = max;

        return joint;
    }

    AX_SAFE_DELETE(joint);
    return nullptr;
}

PhysicsJointRotaryLimit* PhysicsJointRotaryLimit::instantiate(PhysicsBody* a, PhysicsBody* b)
{
    return instantiate(a, b, 0.0f, 0.0f);
}

bool PhysicsJointRotaryLimit::buildConstraints()
{
    do
    {
        /*auto joint = cpRotaryLimitJointNew(_bodyA->getCPBody(), _bodyB->getCPBody(), _min, _max);*/

        // Note: Not 100% indentical to cpRotaryLimitJointNew
        b2RevoluteJointDef def = b2DefaultRevoluteJointDef();
        def.bodyIdA            = _bodyA->getB2Body();
        def.bodyIdB            = _bodyB->getB2Body();

        def.enableLimit        = true;
        def.lowerAngle         = _min;  // Set the lower angle limit
        def.upperAngle         = _max;  // Set the upper angle limit

        auto joint = b2CreateRevoluteJoint(_bodyA->getWorld()->getB2World(), &def);  // FIXME:
        AX_BREAK_IF(!b2Joint_IsValid(joint));
        _b2Joints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

float PhysicsJointRotaryLimit::getMin() const
{
    AX_PJOINT_CACHE_READ(_min);
    return b2RevoluteJoint_GetLowerLimit(_b2Joints.front());
}

void PhysicsJointRotaryLimit::setMin(float min)
{
    // AX_PJOINT_CACHE_WRITE(_min, cpRotaryLimitJointSetMin, min);

    _min = min;
    b2RevoluteJoint_SetLimits(_b2Joints[0], min, _max);
}

float PhysicsJointRotaryLimit::getMax() const
{
    AX_PJOINT_CACHE_READ(_max);
    return b2RevoluteJoint_GetUpperLimit(_b2Joints.front());
}

void PhysicsJointRotaryLimit::setMax(float max)
{
    // AX_PJOINT_CACHE_WRITE(_max, cpRotaryLimitJointSetMax, max);
    _max = max;
    b2RevoluteJoint_SetLimits(_b2Joints[0], _min, max);
}

PhysicsJointRatchet* PhysicsJointRatchet::instantiate(PhysicsBody* a, PhysicsBody* b, float phase, float ratchet)
{
    auto joint = new PhysicsJointRatchet();

    if (joint->init(a, b))
    {
        joint->_phase   = phase;
        joint->_ratchet = ratchet;

        return joint;
    }

    AX_SAFE_DELETE(joint);
    return nullptr;
}

bool PhysicsJointRatchet::buildConstraints()
{
    do
    {
        /*auto joint =
            cpRatchetJointNew(_bodyA->getCPBody(), _bodyB->getCPBody(), _phase,
           phlp::cpfloat2float(_ratchet));*/

        b2RevoluteJointDef def = b2DefaultRevoluteJointDef();
        def.bodyIdA            = _bodyA->getB2Body();
        def.bodyIdB            = _bodyB->getB2Body();
        def.referenceAngle     = _phase;  // Set the initial phase
        def.enableLimit        = true;
        def.lowerAngle         = 0.0f;      // Set the lower angle limit (the ratchet step)
        def.upperAngle         = _ratchet;  // Set the upper angle limit (the ratchet step)
        def.enableMotor        = true;
        def.motorSpeed         = 0.0f;   // Initial motor speed
        def.maxMotorTorque     = 10.0f;  // Set a high torque to simulate the ratchet

        auto joint = b2CreateRevoluteJoint(_bodyA->getWorld()->getB2World(), &def);  // FIXME:
        AX_BREAK_IF(!b2Joint_IsValid(joint));
        _b2Joints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

float PhysicsJointRatchet::getAngle() const
{
    AX_PJOINT_CACHE_READ(_angle);
    return b2RevoluteJoint_GetAngle(_b2Joints.front());
}

void PhysicsJointRatchet::setAngle(float angle)
{
    // NOTE: NOT SUPPORT
    // AX_PJOINT_CACHE_WRITE(_angle, b2RevoluteJoint_SetAngle, angle);
}

float PhysicsJointRatchet::getPhase() const
{
    AX_PJOINT_CACHE_READ(_phase);
    // return phlp::cpfloat2float(cpRatchetJointGetPhase(_b2Joints.front()));
    return _phase;
}

void PhysicsJointRatchet::setPhase(float phase)
{
    // NOTE: NOT SUPPORT
    // AX_PJOINT_CACHE_WRITE(_phase, cpRatchetJointSetPhase, phase);
}

float PhysicsJointRatchet::getRatchet() const
{
    AX_PJOINT_CACHE_READ(_ratchet);
    return b2RevoluteJoint_GetUpperLimit(_b2Joints.front());
}

void PhysicsJointRatchet::setRatchet(float ratchet)
{
    // AX_PJOINT_CACHE_WRITE(_ratchet, cpRatchetJointSetRatchet, ratchet);
    b2RevoluteJoint_SetLimits(_b2Joints[0], 0, ratchet);
}

PhysicsJointGear* PhysicsJointGear::instantiate(PhysicsBody* a, PhysicsBody* b, float phase, float ratio)
{
    auto joint = new PhysicsJointGear();

    if (joint->init(a, b))
    {
        joint->_phase = phase;
        joint->_ratio = ratio;

        return joint;
    }

    AX_SAFE_DELETE(joint);
    return nullptr;
}

bool PhysicsJointGear::buildConstraints()
{
    do
    {
        // auto joint = cpGearJointNew(_bodyA->getCPBody(), _bodyB->getCPBody(), _phase, _ratio);
        // gear and pulley joint removed (temporarily) in box2d-3.x, see https://box2d.org/documentation/md_migration.html
        auto world = _bodyA->getWorld()->getB2World();
        auto bodyA = _bodyA->getB2Body();
        auto bodyB = _bodyB->getB2Body();

        // bodyA, bodyB, phase, ratio
        b2MotorJointDef motorDef = b2DefaultMotorJointDef();
        motorDef.bodyIdA = bodyA;
        motorDef.bodyIdB = bodyB;

        b2Vec2 posA           = b2Body_GetWorldCenterOfMass(bodyA);
        b2Vec2 posB           = b2Body_GetWorldCenterOfMass(bodyB);
        motorDef.linearOffset = posB - posA;

        float angleA           = b2Body_GetAngle(bodyA);
        float angleB           = b2Body_GetAngle(bodyB);
        motorDef.angularOffset = _phase - (angleA + _ratio * angleB);

        motorDef.maxForce         = 0.0f;
        motorDef.maxTorque        = 10000.0f;
        motorDef.correctionFactor = 1.0f;

        b2JointId joint = b2CreateMotorJoint(world, &motorDef);

        AX_BREAK_IF(!b2Joint_IsValid(joint));
        _b2Joints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

float PhysicsJointGear::getPhase() const
{
    AX_PJOINT_CACHE_READ(_phase);
    // return phlp::cpfloat2float(cpGearJointGetPhase(_b2Joints.front()));
    return _phase;
}

void PhysicsJointGear::setPhase(float phase)
{
    // AX_PJOINT_CACHE_WRITE(_phase, cpGearJointSetPhase, phase);
}

float PhysicsJointGear::getRatio() const
{
    AX_PJOINT_CACHE_READ(_ratio);
    // return phlp::cpfloat2float(cpGearJointGetRatio(_b2Joints.front()));
    return _ratio;
}

void PhysicsJointGear::setRatio(float ratio)
{
    // AX_PJOINT_CACHE_WRITE(_ratio, cpGearJointSetRatio, ratio);
}

PhysicsJointMotor* PhysicsJointMotor::instantiate(PhysicsBody* a, PhysicsBody* b, float rate)
{
    auto joint = new PhysicsJointMotor();

    if (joint->init(a, b))
    {
        joint->_rate = rate;

        return joint;
    }

    AX_SAFE_DELETE(joint);
    return nullptr;
}

bool PhysicsJointMotor::buildConstraints()
{
    do
    {
        /*auto joint = cpSimpleMotorNew(_bodyA->getCPBody(), _bodyB->getCPBody(), _rate);*/

        // Define the motor joint (simple motor equivalent in Box2D)
        b2RevoluteJointDef def = b2DefaultRevoluteJointDef();
        def.bodyIdA            = _bodyA->getB2Body();
        def.bodyIdB            = _bodyB->getB2Body();

        def.localAnchorA       = b2Vec2_zero;  // Set the local anchor points as required
        def.localAnchorB       = b2Vec2_zero;  // Set the local anchor points as required

        def.referenceAngle     = 0.0f;         // Set the reference angle
        def.enableMotor        = true;
        def.motorSpeed         = _rate;    // Set the motor speed (rate)
        def.maxMotorTorque     = 10000.0f;  // Set the maximum torque the motor can apply

        auto joint = b2CreateRevoluteJoint(_bodyA->getWorld()->getB2World(), &def);  // FIXME:
        setRate(_rate);
        AX_BREAK_IF(!b2Joint_IsValid(joint));
        _b2Joints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

float PhysicsJointMotor::getRate() const
{
    AX_PJOINT_CACHE_READ(_rate);
    // return phlp::cpfloat2float(cpSimpleMotorGetRate(_b2Joints.front()));
    return _rate;
}

void PhysicsJointMotor::setRate(float rate)
{
    AX_PJOINT_CACHE_WRITE(_rate, b2RevoluteJoint_SetMotorSpeed, rate);
    // b2RevoluteJoint_SetMotorSpeed(_b2Joints[0], _rate);
}

}  // namespace ax
#endif  // AX_ENABLE_PHYSICS
