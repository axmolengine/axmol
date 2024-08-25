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
#    include "chipmunk/chipmunk.h"

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
#        define LIKELY(x) (__builtin_expect((x), 1))
#        define UNLIKELY(x) (__builtin_expect((x), 0))
#    else
#        define LIKELY(x) (x)
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

#    define AX_PJOINT_CACHE_WRITE2(field, method, arg, convertedArg)                    \
        do                                                                              \
        {                                                                               \
            if (UNLIKELY(_initDirty))                                                   \
            {                                                                           \
                _writeCache->field.set(arg);                                            \
                delay([this, arg]() { method(_cpConstraints.front(), convertedArg); }); \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                method(_cpConstraints.front(), convertedArg);                           \
            }                                                                           \
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
    // reset the shapes collision group
    setCollisionEnable(true);

    for (cpConstraint* joint : _cpConstraints)
    {
        cpConstraintFree(joint);
    }
    _cpConstraints.clear();

    delete _writeCache;
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

        for (auto&& subjoint : _cpConstraints)
        {
            cpConstraintSetMaxForce(subjoint, _maxForce);
            cpConstraintSetErrorBias(subjoint, cpfpow(1.0f - 0.15f, 60.0f));
            cpSpaceAddConstraint(_world->_cpSpace, subjoint);
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
            for (auto&& joint : _cpConstraints)
            {
                cpConstraintSetMaxForce(joint, force);
            }
        });
    }
    else
    {
        _maxForce = force;
        for (auto&& joint : _cpConstraints)
        {
            cpConstraintSetMaxForce(joint, force);
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

        // add a pivot joint to fixed two body together
        auto joint = cpPivotJointNew(_bodyA->getCPBody(), _bodyB->getCPBody(), PhysicsHelper::vec22cpv(_anchr));
        AX_BREAK_IF(joint == nullptr);
        _cpConstraints.emplace_back(joint);

        // add a gear joint to make two body have the same rotation.
        joint = cpGearJointNew(_bodyA->getCPBody(), _bodyB->getCPBody(), 0, 1);
        AX_BREAK_IF(joint == nullptr);
        _cpConstraints.emplace_back(joint);

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
        cpConstraint* joint = nullptr;
        if (_useSpecificAnchr)
        {
            joint = cpPivotJointNew2(_bodyA->getCPBody(), _bodyB->getCPBody(), PhysicsHelper::vec22cpv(_anchr1),
                                     PhysicsHelper::vec22cpv(_anchr2));
        }
        else
        {
            joint = cpPivotJointNew(_bodyA->getCPBody(), _bodyB->getCPBody(), PhysicsHelper::vec22cpv(_anchr1));
        }

        AX_BREAK_IF(joint == nullptr);
        _cpConstraints.emplace_back(joint);

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
        auto joint = cpSlideJointNew(_bodyA->getCPBody(), _bodyB->getCPBody(), PhysicsHelper::vec22cpv(_anchr1),
                                     PhysicsHelper::vec22cpv(_anchr2), _min, _max);

        AX_BREAK_IF(joint == nullptr);
        _cpConstraints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

float PhysicsJointLimit::getMin() const
{
    AX_PJOINT_CACHE_READ(_min);
    return PhysicsHelper::cpfloat2float(cpSlideJointGetMin(_cpConstraints.front()));
}

void PhysicsJointLimit::setMin(float min)
{
    AX_PJOINT_CACHE_WRITE(_min, cpSlideJointSetMin, min);
}

float PhysicsJointLimit::getMax() const
{
    AX_PJOINT_CACHE_READ(_max);
    return PhysicsHelper::cpfloat2float(cpSlideJointGetMax(_cpConstraints.front()));
}

void PhysicsJointLimit::setMax(float max)
{
    AX_PJOINT_CACHE_WRITE(_max, cpSlideJointSetMax, max);
}

Vec2 PhysicsJointLimit::getAnchr1() const
{
    AX_PJOINT_CACHE_READ(_anchr1);
    return PhysicsHelper::cpv2vec2(cpSlideJointGetAnchorA(_cpConstraints.front()));
}

void PhysicsJointLimit::setAnchr1(const Vec2& anchr)
{
    AX_PJOINT_CACHE_WRITE2(_anchr1, cpSlideJointSetAnchorA, anchr, PhysicsHelper::vec22cpv(anchr));
}

Vec2 PhysicsJointLimit::getAnchr2() const
{
    AX_PJOINT_CACHE_READ(_anchr2);
    return PhysicsHelper::cpv2vec2(cpSlideJointGetAnchorB(_cpConstraints.front()));
}

void PhysicsJointLimit::setAnchr2(const Vec2& anchr)
{
    AX_PJOINT_CACHE_WRITE2(_anchr2, cpSlideJointSetAnchorB, anchr, PhysicsHelper::vec22cpv(anchr));
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
        auto joint = cpPinJointNew(_bodyA->getCPBody(), _bodyB->getCPBody(), PhysicsHelper::vec22cpv(_anchr1),
                                   PhysicsHelper::vec22cpv(_anchr2));
        AX_BREAK_IF(joint == nullptr);
        _cpConstraints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

float PhysicsJointDistance::getDistance() const
{
    AX_PJOINT_CACHE_READ(_distance);
    return PhysicsHelper::cpfloat2float(cpPinJointGetDist(_cpConstraints.front()));
}

void PhysicsJointDistance::setDistance(float distance)
{
    AX_PJOINT_CACHE_WRITE(_distance, cpPinJointSetDist, distance);
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
        auto joint = cpDampedSpringNew(_bodyA->getCPBody(), _bodyB->getCPBody(), PhysicsHelper::vec22cpv(_anchr1),
                                       PhysicsHelper::vec22cpv(_anchr2),
                                       _bodyB->local2World(_anchr1).getDistance(_bodyA->local2World(_anchr2)),
                                       _stiffness, _damping);

        AX_BREAK_IF(joint == nullptr);
        _cpConstraints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

Vec2 PhysicsJointSpring::getAnchr1() const
{
    AX_PJOINT_CACHE_READ(_anchr1);
    return PhysicsHelper::cpv2vec2(cpDampedSpringGetAnchorA(_cpConstraints.front()));
}

void PhysicsJointSpring::setAnchr1(const Vec2& anchr)
{
    AX_PJOINT_CACHE_WRITE2(_anchr1, cpDampedSpringSetAnchorA, anchr, PhysicsHelper::vec22cpv(anchr));
}

Vec2 PhysicsJointSpring::getAnchr2() const
{
    AX_PJOINT_CACHE_READ(_anchr2);
    return PhysicsHelper::cpv2vec2(cpDampedSpringGetAnchorB(_cpConstraints.front()));
}

void PhysicsJointSpring::setAnchr2(const Vec2& anchr)
{
    AX_PJOINT_CACHE_WRITE2(_anchr2, cpDampedSpringSetAnchorB, anchr, PhysicsHelper::vec22cpv(anchr));
}

float PhysicsJointSpring::getRestLength() const
{
    AX_PJOINT_CACHE_READ(_restLength);
    return PhysicsHelper::cpfloat2float(cpDampedSpringGetRestLength(_cpConstraints.front()));
}

void PhysicsJointSpring::setRestLength(float restLength)
{
    AX_PJOINT_CACHE_WRITE(_restLength, cpDampedSpringSetRestLength, restLength);
}

float PhysicsJointSpring::getStiffness() const
{
    AX_PJOINT_CACHE_READ(_stiffness);
    return PhysicsHelper::cpfloat2float(cpDampedSpringGetStiffness(_cpConstraints.front()));
}

void PhysicsJointSpring::setStiffness(float stiffness)
{
    AX_PJOINT_CACHE_WRITE(_stiffness, cpDampedSpringSetStiffness, stiffness);
}

float PhysicsJointSpring::getDamping() const
{
    AX_PJOINT_CACHE_READ(_damping);
    return PhysicsHelper::cpfloat2float(cpDampedSpringGetDamping(_cpConstraints.front()));
}

void PhysicsJointSpring::setDamping(float damping)
{
    AX_PJOINT_CACHE_WRITE(_damping, cpDampedSpringSetDamping, damping);
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
        auto joint = cpGrooveJointNew(_bodyA->getCPBody(), _bodyB->getCPBody(), PhysicsHelper::vec22cpv(_grooveA),
                                      PhysicsHelper::vec22cpv(_grooveB), PhysicsHelper::vec22cpv(_anchr2));

        AX_BREAK_IF(joint == nullptr);
        _cpConstraints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

Vec2 PhysicsJointGroove::getGrooveA() const
{
    AX_PJOINT_CACHE_READ(_grooveA);
    return PhysicsHelper::cpv2vec2(cpGrooveJointGetGrooveA(_cpConstraints.front()));
}

void PhysicsJointGroove::setGrooveA(const Vec2& grooveA)
{
    AX_PJOINT_CACHE_WRITE2(_grooveA, cpGrooveJointSetGrooveA, grooveA, PhysicsHelper::vec22cpv(grooveA));
}

Vec2 PhysicsJointGroove::getGrooveB() const
{
    AX_PJOINT_CACHE_READ(_grooveB);
    return PhysicsHelper::cpv2vec2(cpGrooveJointGetGrooveB(_cpConstraints.front()));
}

void PhysicsJointGroove::setGrooveB(const Vec2& grooveB)
{
    AX_PJOINT_CACHE_WRITE2(_grooveB, cpGrooveJointSetGrooveB, grooveB, PhysicsHelper::vec22cpv(grooveB));
}

Vec2 PhysicsJointGroove::getAnchr2() const
{
    AX_PJOINT_CACHE_READ(_anchr2);
    return PhysicsHelper::cpv2vec2(cpGrooveJointGetAnchorB(_cpConstraints.front()));
}

void PhysicsJointGroove::setAnchr2(const Vec2& anchr2)
{
    AX_PJOINT_CACHE_WRITE2(_anchr2, cpGrooveJointSetAnchorB, anchr2, PhysicsHelper::vec22cpv(anchr2));
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
        auto joint = cpDampedRotarySpringNew(_bodyA->getCPBody(), _bodyB->getCPBody(),
                                             _bodyB->getRotation() - _bodyA->getRotation(), _stiffness, _damping);

        AX_BREAK_IF(joint == nullptr);
        _cpConstraints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

float PhysicsJointRotarySpring::getRestAngle() const
{
    AX_PJOINT_CACHE_READ(_restAngle);
    return PhysicsHelper::cpfloat2float(cpDampedRotarySpringGetRestAngle(_cpConstraints.front()));
}

void PhysicsJointRotarySpring::setRestAngle(float restAngle)
{
    AX_PJOINT_CACHE_WRITE(_restAngle, cpDampedRotarySpringSetRestAngle, restAngle);
}

float PhysicsJointRotarySpring::getStiffness() const
{
    AX_PJOINT_CACHE_READ(_stiffness);
    return PhysicsHelper::cpfloat2float(cpDampedRotarySpringGetStiffness(_cpConstraints.front()));
}

void PhysicsJointRotarySpring::setStiffness(float stiffness)
{
    AX_PJOINT_CACHE_WRITE(_stiffness, cpDampedRotarySpringSetStiffness, stiffness);
}

float PhysicsJointRotarySpring::getDamping() const
{
    AX_PJOINT_CACHE_READ(_damping);
    return PhysicsHelper::cpfloat2float(cpDampedRotarySpringGetDamping(_cpConstraints.front()));
}

void PhysicsJointRotarySpring::setDamping(float damping)
{
    AX_PJOINT_CACHE_WRITE(_damping, cpDampedRotarySpringSetDamping, damping);
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
        auto joint = cpRotaryLimitJointNew(_bodyA->getCPBody(), _bodyB->getCPBody(), _min, _max);

        AX_BREAK_IF(joint == nullptr);
        _cpConstraints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

float PhysicsJointRotaryLimit::getMin() const
{
    AX_PJOINT_CACHE_READ(_min);
    return PhysicsHelper::cpfloat2float(cpRotaryLimitJointGetMin(_cpConstraints.front()));
}

void PhysicsJointRotaryLimit::setMin(float min)
{
    AX_PJOINT_CACHE_WRITE(_min, cpRotaryLimitJointSetMin, min);
}

float PhysicsJointRotaryLimit::getMax() const
{
    AX_PJOINT_CACHE_READ(_max);
    return PhysicsHelper::cpfloat2float(cpRotaryLimitJointGetMax(_cpConstraints.front()));
}

void PhysicsJointRotaryLimit::setMax(float max)
{
    AX_PJOINT_CACHE_WRITE(_max, cpRotaryLimitJointSetMax, max);
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
        auto joint =
            cpRatchetJointNew(_bodyA->getCPBody(), _bodyB->getCPBody(), _phase, PhysicsHelper::cpfloat2float(_ratchet));

        AX_BREAK_IF(joint == nullptr);
        _cpConstraints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

float PhysicsJointRatchet::getAngle() const
{
    AX_PJOINT_CACHE_READ(_angle);
    return PhysicsHelper::cpfloat2float(cpRatchetJointGetAngle(_cpConstraints.front()));
}

void PhysicsJointRatchet::setAngle(float angle)
{
    AX_PJOINT_CACHE_WRITE(_angle, cpRatchetJointSetAngle, angle);
}

float PhysicsJointRatchet::getPhase() const
{
    AX_PJOINT_CACHE_READ(_phase);
    return PhysicsHelper::cpfloat2float(cpRatchetJointGetPhase(_cpConstraints.front()));
}

void PhysicsJointRatchet::setPhase(float phase)
{
    AX_PJOINT_CACHE_WRITE(_phase, cpRatchetJointSetPhase, phase);
}

float PhysicsJointRatchet::getRatchet() const
{
    AX_PJOINT_CACHE_READ(_ratchet);
    return PhysicsHelper::cpfloat2float(cpRatchetJointGetRatchet(_cpConstraints.front()));
}

void PhysicsJointRatchet::setRatchet(float ratchet)
{
    AX_PJOINT_CACHE_WRITE(_ratchet, cpRatchetJointSetRatchet, ratchet);
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
        auto joint = cpGearJointNew(_bodyA->getCPBody(), _bodyB->getCPBody(), _phase, _ratio);

        AX_BREAK_IF(joint == nullptr);
        _cpConstraints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

float PhysicsJointGear::getPhase() const
{
    AX_PJOINT_CACHE_READ(_phase);
    return PhysicsHelper::cpfloat2float(cpGearJointGetPhase(_cpConstraints.front()));
}

void PhysicsJointGear::setPhase(float phase)
{
    AX_PJOINT_CACHE_WRITE(_phase, cpGearJointSetPhase, phase);
}

float PhysicsJointGear::getRatio() const
{
    AX_PJOINT_CACHE_READ(_ratio);
    return PhysicsHelper::cpfloat2float(cpGearJointGetRatio(_cpConstraints.front()));
}

void PhysicsJointGear::setRatio(float ratio)
{
    AX_PJOINT_CACHE_WRITE(_ratio, cpGearJointSetRatio, ratio);
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
        auto joint = cpSimpleMotorNew(_bodyA->getCPBody(), _bodyB->getCPBody(), _rate);

        AX_BREAK_IF(joint == nullptr);
        _cpConstraints.emplace_back(joint);

        return true;
    } while (false);

    return false;
}

float PhysicsJointMotor::getRate() const
{
    AX_PJOINT_CACHE_READ(_rate);
    return PhysicsHelper::cpfloat2float(cpSimpleMotorGetRate(_cpConstraints.front()));
}

void PhysicsJointMotor::setRate(float rate)
{
    AX_PJOINT_CACHE_WRITE(_rate, cpSimpleMotorSetRate, rate);
}

}
#endif  // AX_ENABLE_PHYSICS
