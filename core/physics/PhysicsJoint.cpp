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

#include "physics/PhysicsJoint.h"
#if defined(AX_ENABLE_PHYSICS)

#    include "physics/PhysicsBody.h"
#    include "physics/PhysicsWorld.h"
#    include "physics/PhysicsHelper.h"
#    include "2d/Node.h"

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
    bool ret = !_initDirty;
    while (_initDirty)
    {
        ret = createConstraints();
        AX_BREAK_IF(!ret);

        // TODO: can't do follow settings
        for (auto&& joint : _b2Joints)
        {
            // cpConstraintSetMaxForce(subjoint, _maxForce);
            // cpConstraintSetErrorBias(subjoint, cpfpow(1.0f - 0.15f, 60.0f));
            // cpSpaceAddConstraint(_world->_cpSpace, subjoint);
        }
        _initDirty = false;
        ret        = true;
    }

    return ret;
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
            {
                // TODO:
                // cpConstraintSetMaxForce(joint, force);
            }
        });
    }
    else
    {
        _maxForce = force;
        for (auto&& joint : _b2Joints)
        {
            // cpConstraintSetMaxForce(joint, force);
        }
    }
}

PhysicsJointFixed* PhysicsJointFixed::construct(PhysicsBody* a, PhysicsBody* b, const Vec2& anchr)
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

bool PhysicsJointFixed::createConstraints()
{
    do
    {
        _bodyA->getNode()->setPosition(_anchr);
        _bodyB->getNode()->setPosition(_anchr);

        //// add a pivot joint to fixed two body together
        // auto joint = cpPivotJointNew(_bodyA->getCPBody(), _bodyB->getCPBody(), PhysicsHelper::vec22cpv(_anchr));
        // AX_BREAK_IF(!b2Joint_IsValid(joint));
        //_b2Joints.emplace_back(joint);

        //// add a gear joint to make two body have the same rotation.
        // joint = cpGearJointNew(_bodyA->getCPBody(), _bodyB->getCPBody(), 0, 1);
        // AX_BREAK_IF(!b2Joint_IsValid(joint));
        //_b2Joints.emplace_back(joint);

        b2RevoluteJointDef pivotJointDef = b2DefaultRevoluteJointDef();
        pivotJointDef.bodyIdA            = _bodyA->getB2Body();
        pivotJointDef.bodyIdB            = _bodyB->getB2Body();
        pivotJointDef.localAnchorA       = PhysicsHelper::tob2Vec2(_anchr);  // Use your conversion function here
        pivotJointDef.localAnchorB       = b2Body_GetLocalCenterOfMass(_bodyB->getB2Body());
        auto pivotJoint = b2CreateRevoluteJoint(_bodyA->getWorld()->getB2World(), &pivotJointDef);  // FIXME:
        AX_BREAK_IF(!b2Joint_IsValid(pivotJoint));
        _b2Joints.emplace_back(pivotJoint);

        // FIXME: The pulley and gear joints have been removed.I'm not satisfied with how they work in 2.4 and plan to
        // implement improved versions in the future.

        _collisionEnable = false;

        return true;
    } while (false);

    return false;
}

PhysicsJointPin* PhysicsJointPin::construct(PhysicsBody* a, PhysicsBody* b, const Vec2& pivot)
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

PhysicsJointPin* PhysicsJointPin::construct(PhysicsBody* a, PhysicsBody* b, const Vec2& anchr1, const Vec2& anchr2)
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

bool PhysicsJointPin::createConstraints()
{
    do
    {
        b2RevoluteJointDef pivotJointDef = b2DefaultRevoluteJointDef();
        pivotJointDef.bodyIdA            = _bodyA->getB2Body();
        pivotJointDef.bodyIdB            = _bodyB->getB2Body();
        pivotJointDef.localAnchorA       = PhysicsHelper::tob2Vec2(_anchr1);  // Use your conversion function here
        if (_useSpecificAnchr)
        {
            pivotJointDef.localAnchorB = b2Body_GetLocalCenterOfMass(_bodyB->getB2Body());
        }
        else
        {
            pivotJointDef.localAnchorB = PhysicsHelper::tob2Vec2(_anchr2);  // Use your conversion function here
        }

        auto joint = b2CreateRevoluteJoint(_bodyA->getWorld()->getB2World(), &pivotJointDef);  // FIXME:

        AX_BREAK_IF(!b2Joint_IsValid(joint));
        _b2Joints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

PhysicsJointLimit* PhysicsJointLimit::construct(PhysicsBody* a,
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

PhysicsJointLimit* PhysicsJointLimit::construct(PhysicsBody* a, PhysicsBody* b, const Vec2& anchr1, const Vec2& anchr2)
{
    return construct(a, b, anchr1, anchr2, 0, b->local2World(anchr1).getDistance(a->local2World(anchr2)));
}

bool PhysicsJointLimit::createConstraints()
{
    do
    {
        b2PrismaticJointDef def = b2DefaultPrismaticJointDef();
        def.bodyIdA             = _bodyA->getB2Body();
        def.bodyIdB             = _bodyB->getB2Body();
        def.localAnchorA        = PhysicsHelper::tob2Vec2(_anchr1);
        def.localAnchorB        = PhysicsHelper::tob2Vec2(_anchr2);
        def.localAxisA          = b2Vec2{1.0f, 0.0f};  // Axis along which the bodies can slide
        def.enableLimit         = true;
        def.lowerTranslation    = _min;
        def.upperTranslation    = _max;

        auto joint = b2CreatePrismaticJoint(_bodyA->getWorld()->getB2World(), &def);  // FIXME:

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

PhysicsJointDistance* PhysicsJointDistance::construct(PhysicsBody* a,
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

bool PhysicsJointDistance::createConstraints()
{
    do
    {
        /*auto joint = cpPinJointNew(_bodyA->getCPBody(), _bodyB->getCPBody(), PhysicsHelper::vec22cpv(_anchr1),
                                   PhysicsHelper::vec22cpv(_anchr2));*/
        auto def         = b2DefaultDistanceJointDef();
        def.bodyIdA      = _bodyA->getB2Body();
        def.bodyIdB      = _bodyB->getB2Body();
        def.localAnchorA = PhysicsHelper::tob2Vec2(_anchr1);
        def.localAnchorB = PhysicsHelper::tob2Vec2(_anchr2);

        auto joint = b2CreateDistanceJoint(_bodyA->getWorld()->getB2World(), &def);  // FIXME:

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

PhysicsJointSpring* PhysicsJointSpring::construct(PhysicsBody* a,
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

bool PhysicsJointSpring::createConstraints()
{
    do
    {
        /*auto joint = cpDampedSpringNew(_bodyA->getCPBody(), _bodyB->getCPBody(), PhysicsHelper::vec22cpv(_anchr1),
                                       PhysicsHelper::vec22cpv(_anchr2),
                                       _bodyB->local2World(_anchr1).getDistance(_bodyA->local2World(_anchr2)),
                                       _stiffness, _damping);*/
        b2DistanceJointDef def = b2DefaultDistanceJointDef();
        def.bodyIdA            = _bodyA->getB2Body();
        def.bodyIdB            = _bodyB->getB2Body();
        def.localAnchorA       = PhysicsHelper::tob2Vec2(_anchr1);
        def.localAnchorB       = PhysicsHelper::tob2Vec2(_anchr2);
        def.length             = _bodyB->local2World(_anchr1).getDistance(_bodyA->local2World(_anchr2));
        def.hertz              = _stiffness;  // The spring frequency in Hertz
        def.dampingRatio       = _damping;    // The damping ratio

        auto joint = b2CreateDistanceJoint(_bodyA->getWorld()->getB2World(), &def);  // FIXME:
        AX_BREAK_IF(!b2Joint_IsValid(joint));
        _b2Joints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

Vec2 PhysicsJointSpring::getAnchr1() const
{
    AX_PJOINT_CACHE_READ(_anchr1);
    return PhysicsHelper::toVec2(b2Joint_GetLocalAnchorA(_b2Joints[0]));
}

void PhysicsJointSpring::setAnchr1(const Vec2& anchr)
{
    // NOTE: NOT SUPPORT
    // AX_PJOINT_CACHE_WRITE2(_anchr1, cpDampedSpringSetAnchorA, anchr, PhysicsHelper::vec22cpv(anchr));
}

Vec2 PhysicsJointSpring::getAnchr2() const
{
    AX_PJOINT_CACHE_READ(_anchr2);
    return PhysicsHelper::toVec2(b2Joint_GetLocalAnchorB(_b2Joints[0]));
}

void PhysicsJointSpring::setAnchr2(const Vec2& anchr)
{
    // NOTE: NOT SUPPORT
    // AX_PJOINT_CACHE_WRITE2(_anchr2, cpDampedSpringSetAnchorB, anchr, PhysicsHelper::vec22cpv(anchr));
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

PhysicsJointGroove* PhysicsJointGroove::construct(PhysicsBody* a,
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

bool PhysicsJointGroove::createConstraints()
{
    do
    {
        /*auto joint = cpGrooveJointNew(_bodyA->getCPBody(), _bodyB->getCPBody(), PhysicsHelper::vec22cpv(_grooveA),
                                      PhysicsHelper::vec22cpv(_grooveB), PhysicsHelper::vec22cpv(_anchr2));*/
        b2PrismaticJointDef def = b2DefaultPrismaticJointDef();
        def.bodyIdA             = _bodyA->getB2Body();
        def.bodyIdB             = _bodyB->getB2Body();
        def.localAnchorA        = PhysicsHelper::tob2Vec2(_grooveA);
        def.localAnchorB        = PhysicsHelper::tob2Vec2(_anchr2);
        def.localAxisA          = PhysicsHelper::tob2Vec2(_grooveB) - PhysicsHelper::tob2Vec2(_grooveA);
        def.lowerTranslation    = 0.0f;
        def.upperTranslation    = b2Distance(PhysicsHelper::tob2Vec2(_grooveA), PhysicsHelper::tob2Vec2(_grooveB));
        def.enableLimit         = true;

        auto joint = b2CreatePrismaticJoint(_bodyA->getWorld()->getB2World(), &def);  // FIXME:
        AX_BREAK_IF(!b2Joint_IsValid(joint));
        _b2Joints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

Vec2 PhysicsJointGroove::getGrooveA() const
{
    AX_PJOINT_CACHE_READ(_grooveA);
    return PhysicsHelper::toVec2(b2Joint_GetLocalAnchorA(_b2Joints.front()));
}

void PhysicsJointGroove::setGrooveA(const Vec2& grooveA)
{
    // NOTE: NOT SUPPORT
    // AX_PJOINT_CACHE_WRITE2(_grooveA, cpGrooveJointSetGrooveA, grooveA, PhysicsHelper::vec22cpv(grooveA));
}

Vec2 PhysicsJointGroove::getGrooveB() const
{
    AX_PJOINT_CACHE_READ(_grooveB);

    return _grooveB;
    // return PhysicsHelper::cpv2vec2(cpGrooveJointGetGrooveB(_b2Joints.front()));
}

void PhysicsJointGroove::setGrooveB(const Vec2& grooveB)
{
    // NOTE: NOT SUPPORT
    // AX_PJOINT_CACHE_WRITE2(_grooveB, cpGrooveJointSetGrooveB, grooveB, PhysicsHelper::vec22cpv(grooveB));
}

Vec2 PhysicsJointGroove::getAnchr2() const
{
    AX_PJOINT_CACHE_READ(_anchr2);
    return PhysicsHelper::toVec2(b2Joint_GetLocalAnchorB(_b2Joints.front()));
}

void PhysicsJointGroove::setAnchr2(const Vec2& anchr2)
{
    // AX_PJOINT_CACHE_WRITE2(_anchr2, cpGrooveJointSetAnchorB, anchr2, PhysicsHelper::vec22cpv(anchr2));
}

PhysicsJointRotarySpring* PhysicsJointRotarySpring::construct(PhysicsBody* a,
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

bool PhysicsJointRotarySpring::createConstraints()
{
    do
    {
        // auto joint = cpDampedRotarySpringNew(_bodyA->getCPBody(), _bodyB->getCPBody(),
        //                                      _bodyB->getRotation() - _bodyA->getRotation(), _stiffness, _damping);

        // Define the revolute joint (damped rotary spring equivalent in Box2D)
        b2RevoluteJointDef def = b2DefaultRevoluteJointDef();
        def.bodyIdA            = _bodyA->getB2Body();
        def.bodyIdB            = _bodyB->getB2Body();
        def.referenceAngle =
            AX_DEGREES_TO_RADIANS(_bodyB->getRotation() - _bodyA->getRotation());  // Set the reference angle
        def.enableMotor    = true;
        def.maxMotorTorque = _stiffness;  // Set stiffness as max motor torque
        def.motorSpeed     = 0.0f;        // Initial motor speed
        def.enableLimit    = false;       // No limits

        auto joint = b2CreateRevoluteJoint(_bodyA->getWorld()->getB2World(), &def);  // FIXME:
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

PhysicsJointRotaryLimit* PhysicsJointRotaryLimit::construct(PhysicsBody* a, PhysicsBody* b, float min, float max)
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

PhysicsJointRotaryLimit* PhysicsJointRotaryLimit::construct(PhysicsBody* a, PhysicsBody* b)
{
    return construct(a, b, 0.0f, 0.0f);
}

bool PhysicsJointRotaryLimit::createConstraints()
{
    do
    {
        /*auto joint = cpRotaryLimitJointNew(_bodyA->getCPBody(), _bodyB->getCPBody(), _min, _max);*/

        b2RevoluteJointDef def = b2DefaultRevoluteJointDef();
        def.bodyIdA            = _bodyA->getB2Body();
        def.bodyIdB            = _bodyB->getB2Body();
        def.referenceAngle     = 0.0f;  // Set the reference angle
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

PhysicsJointRatchet* PhysicsJointRatchet::construct(PhysicsBody* a, PhysicsBody* b, float phase, float ratchet)
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

bool PhysicsJointRatchet::createConstraints()
{
    do
    {
        /*auto joint =
            cpRatchetJointNew(_bodyA->getCPBody(), _bodyB->getCPBody(), _phase,
           PhysicsHelper::cpfloat2float(_ratchet));*/

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
    // return PhysicsHelper::cpfloat2float(cpRatchetJointGetPhase(_b2Joints.front()));
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

PhysicsJointGear* PhysicsJointGear::construct(PhysicsBody* a, PhysicsBody* b, float phase, float ratio)
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

bool PhysicsJointGear::createConstraints()
{
    do
    {
        // auto joint = cpGearJointNew(_bodyA->getCPBody(), _bodyB->getCPBody(), _phase, _ratio);
        //  FIXME: The pulley and gear joints have been removed.I'm not satisfied with how they work in 2.4 and plan to
        auto joint = b2_nullJointId;
        AX_BREAK_IF(!b2Joint_IsValid(joint));
        _b2Joints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

float PhysicsJointGear::getPhase() const
{
    AX_PJOINT_CACHE_READ(_phase);
    // return PhysicsHelper::cpfloat2float(cpGearJointGetPhase(_b2Joints.front()));
    return _phase;
}

void PhysicsJointGear::setPhase(float phase)
{
    // AX_PJOINT_CACHE_WRITE(_phase, cpGearJointSetPhase, phase);
}

float PhysicsJointGear::getRatio() const
{
    AX_PJOINT_CACHE_READ(_ratio);
    // return PhysicsHelper::cpfloat2float(cpGearJointGetRatio(_b2Joints.front()));
    return _ratio;
}

void PhysicsJointGear::setRatio(float ratio)
{
    // AX_PJOINT_CACHE_WRITE(_ratio, cpGearJointSetRatio, ratio);
}

PhysicsJointMotor* PhysicsJointMotor::construct(PhysicsBody* a, PhysicsBody* b, float rate)
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

bool PhysicsJointMotor::createConstraints()
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
        def.maxMotorTorque     = 1000.0f;  // Set the maximum torque the motor can apply

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
    // return PhysicsHelper::cpfloat2float(cpSimpleMotorGetRate(_b2Joints.front()));
    return _rate;
}

void PhysicsJointMotor::setRate(float rate)
{
    AX_PJOINT_CACHE_WRITE(_rate, b2RevoluteJoint_SetMotorSpeed, rate);
    // b2RevoluteJoint_SetMotorSpeed(_b2Joints[0], _rate);
}

}  // namespace ax
#endif  // AX_ENABLE_PHYSICS
