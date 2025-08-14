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

 remark: box2d rigibody

 TODO:

 Check whether applyForce need invoke phlppixelsToMeters

 ****************************************************************************/
#include "axmol/physics/PhysicsBody.h"
#if defined(AX_ENABLE_PHYSICS)

#    include <climits>
#    include <algorithm>
#    include <cmath>

#    include "axmol/2d/Scene.h"
#    include "axmol/physics/PhysicsCollider.h"
#    include "axmol/physics/PhysicsJoint.h"
#    include "axmol/physics/PhysicsWorld.h"
#    include "axmol/physics/PhysicsHelper.h"

#    if 0
static void internalBodyUpdateVelocity(cpBody* body, b2Vec2 gravity, float damping, float dt)
{
    cpBodyUpdateVelocity(body, cpvzero, damping, dt);
    // Skip kinematic bodies.
    if (cpBodyGetType(body) == CP_BODY_TYPE_KINEMATIC)
        return;

    cpAssertSoft(body->m > 0.0f && body->i > 0.0f,
                 "Body's mass and moment must be positive to simulate. (Mass: %f Moment: f)", body->m, body->i);

    ax::PhysicsBody* physicsBody = static_cast<ax::PhysicsBody*>(body->userData);

    if (physicsBody->isGravityEnabled())
        body->v =
            cpvclamp(cpvadd(cpvmult(body->v, damping), cpvmult(cpvadd(gravity, cpvmult(body->f, body->m_inv)), dt)),
                     physicsBody->getVelocityLimit());
    else
        body->v = cpvclamp(cpvadd(cpvmult(body->v, damping), cpvmult(cpvmult(body->f, body->m_inv), dt)),
                           physicsBody->getVelocityLimit());
    cpFloat w_limit = physicsBody->getAngularVelocityLimit();
    body->w         = cpfclamp(body->w * damping + body->t * body->i_inv * dt, -w_limit, w_limit);

    // Reset forces.
    body->f = cpvzero;
    // to check body sanity
    cpBodySetTorque(body, 0.0f);
}
#    endif

// TODO: check whther really require custom update velocity, then should call in world b2World_Step manually
static void internalBodyUpdateVelocity(b2BodyId& bodyId, const b2Vec2& gravity, float damping, float dt)
{
    // Skip kinematic bodies.
    if (b2Body_GetType(bodyId) == b2_kinematicBody)
        return;

    // Ensure the body's mass and inertia are positive
    auto mass = b2Body_GetMass(bodyId);
    assert(mass > 0.0f && b2Body_GetRotationalInertia(bodyId) > 0.0f);

    // Get the user data and cast it to your custom PhysicsBody class
    ax::PhysicsBody* physicsBody = static_cast<ax::PhysicsBody*>(b2Body_GetUserData(bodyId));

    // Update linear velocity
    b2Vec2 velocity = b2Body_GetLinearVelocity(bodyId);
    if (physicsBody->isGravityEnabled())
    {
        velocity += dt * gravity;
    }
    velocity *= damping;
    float velocityLimit = physicsBody->getVelocityLimit();
    velocity.x          = std::clamp(velocity.x, -velocityLimit, velocityLimit);
    velocity.y          = std::clamp(velocity.y, -velocityLimit, velocityLimit);
    b2Body_SetLinearVelocity(bodyId, velocity);

    // Update angular velocity
    float angularVelocity = b2Body_GetAngularVelocity(bodyId);
    angularVelocity *= damping;
    float angularVelocityLimit = physicsBody->getAngularVelocityLimit();
    angularVelocity            = std::clamp(angularVelocity, -angularVelocityLimit, angularVelocityLimit);
    b2Body_SetAngularVelocity(bodyId, angularVelocity);

    // Reset forces and torque (Box2D does not accumulate forces, but we set them to zero for clarity)
    b2Body_SetLinearDamping(bodyId, 0.0f);
    b2Body_SetAngularDamping(bodyId, 0.0f);
}

static void internalApplyDamping(b2BodyId& body, float delta, float linearDamping, float angularDamping)
{
    // Apply linear damping
    b2Vec2 velocity        = b2Body_GetLinearVelocity(body);
    float linearDampFactor = std::clamp(1.0f - delta * linearDamping, 0.0f, 1.0f);
    velocity.x *= linearDampFactor;
    velocity.y *= linearDampFactor;
    b2Body_SetLinearVelocity(body, velocity);

    // Apply angular damping
    float angularVelocity   = b2Body_GetAngularVelocity(body);
    float angularDampFactor = std::clamp(1.0f - delta * angularDamping, 0.0f, 1.0f);
    angularVelocity *= angularDampFactor;
    b2Body_SetAngularVelocity(body, angularVelocity);
}

namespace ax
{
extern const float PHYSICS_INFINITY;

const std::string PhysicsBody::COMPONENT_NAME = "PhysicsBody";

namespace
{
static const float MASS_DEFAULT   = 1.0;
static const float MOMENT_DEFAULT = 200;
}  // namespace

PhysicsBody::PhysicsBody()
    : _world(nullptr)
    , _b2Body(b2_nullBodyId)
    , _dynamic(true)
    , _rotationEnabled(true)
    , _gravityEnabled(true)
    , _area(0.0f)
    , _density(0.0f)
    , _moment(MOMENT_DEFAULT)
    , _velocityLimit(PHYSICS_INFINITY)
    , _angularVelocityLimit(PHYSICS_INFINITY)
    , _isDamping(false)
    , _linearDamping(0.0f)
    , _angularDamping(0.0f)
    , _tag(0)
    , _rotationOffset(0)
    , _recordedRotation(0.0f)
    , _recordedAngle(0.0)
    , _recordScaleX(1.f)
    , _recordScaleY(1.f)
    , _recordPosX(0.0f)
    , _recordPosY(0.0f)
    , _fixedUpdate(false)
{
    _name = COMPONENT_NAME;
}

PhysicsBody::~PhysicsBody()
{
    deatchFromWOrld();
}

void PhysicsBody::deatchFromWOrld()
{
    removeAllColliders();

    for (auto&& joint : _joints)
    {
        PhysicsBody* other = joint->getBodyA() == this ? joint->getBodyB() : joint->getBodyA();
        other->removeJoint(joint);
        delete joint;
    }
    _joints.clear();

    if (b2Body_IsValid(_b2Body))
    {
        b2DestroyBody(_b2Body);
        _b2Body = b2_nullBodyId;
    }
}

PhysicsBody* PhysicsBody::create()
{
    PhysicsBody* body = new PhysicsBody();
    if (body->init())
    {
        body->autorelease();
        return body;
    }

    AX_SAFE_DELETE(body);
    return nullptr;
}

PhysicsBody* PhysicsBody::createCircle(float radius, const PhysicsMaterial& material, const Vec2& offset)
{
    PhysicsBody* body = new PhysicsBody();
    if (body->init())
    {
        body->addCollider(PhysicsColliderCircle::create(body, radius, material, offset));
        body->autorelease();
        return body;
    }

    AX_SAFE_DELETE(body);
    return nullptr;
}

PhysicsBody* PhysicsBody::createBox(const Vec2& size, const PhysicsMaterial& material, const Vec2& offset)
{
    PhysicsBody* body = new PhysicsBody();
    if (body->init())
    {
        body->addCollider(PhysicsColliderBox::create(body, size, material, offset));
        body->autorelease();
        return body;
    }

    AX_SAFE_DELETE(body);
    return nullptr;
}

PhysicsBody* PhysicsBody::createPolygon(const Vec2* points,
                                        int count,
                                        const PhysicsMaterial& material,
                                        const Vec2& offset)
{
    PhysicsBody* body = new PhysicsBody();
    if (body->init())
    {
        body->addCollider(PhysicsColliderPolygon::create(body, points, count, material, offset));
        body->autorelease();
        return body;
    }

    AX_SAFE_DELETE(body);
    return nullptr;
}

PhysicsBody* PhysicsBody::createEdgeSegment(const Vec2& a,
                                            const Vec2& b,
                                            const PhysicsMaterial& material,
                                            float border /* = 1*/)
{
    PhysicsBody* body = new PhysicsBody();
    if (body->init())
    {
        body->addCollider(PhysicsColliderEdgeSegment::create(body, a, b, material, border));
        body->setDynamic(false);

        body->autorelease();
        return body;
    }

    AX_SAFE_DELETE(body);
    return nullptr;
}

PhysicsBody* PhysicsBody::createEdgeBox(const Vec2& size,
                                        const PhysicsMaterial& material,
                                        float border /* = 1*/,
                                        const Vec2& offset)
{
    PhysicsBody* body = new PhysicsBody();
    if (body->init())
    {
        body->addCollider(PhysicsColliderEdgeBox::create(body, size, material, border, offset));
        body->setDynamic(false);
        body->autorelease();
        return body;
    }

    AX_SAFE_DELETE(body);

    return nullptr;
}

PhysicsBody* PhysicsBody::createEdgePolygon(const Vec2* points,
                                            int count,
                                            const PhysicsMaterial& material,
                                            float border /* = 1*/)
{
    PhysicsBody* body = new PhysicsBody();
    if (body->init())
    {
        body->addCollider(PhysicsColliderEdgePolygon::create(body, points, count, material, border));
        body->setDynamic(false);
        body->autorelease();
        return body;
    }

    AX_SAFE_DELETE(body);

    return nullptr;
}

PhysicsBody* PhysicsBody::createEdgeChain(const Vec2* points,
                                          int count,
                                          const PhysicsMaterial& material,
                                          float border /* = 1*/)
{
    PhysicsBody* body = new PhysicsBody();
    if (body->init())
    {
        body->addCollider(PhysicsColliderEdgeChain::create(body, points, count, material, border));
        body->setDynamic(false);
        body->autorelease();
        return body;
    }

    AX_SAFE_DELETE(body);

    return nullptr;
}

bool PhysicsBody::init()
{
    do
    {
        _world            = Director::getInstance()->getRunningScene()->getPhysicsWorld();
        b2WorldId worldId = _world->_b2World;
        AX_BREAK_IF(!b2World_IsValid(worldId));

        b2BodyDef def = b2DefaultBodyDef();
        def.type      = b2_dynamicBody;
        def.position  = b2Vec2{0.0f, 0.0f};

        _b2Body = b2CreateBody(worldId, &def);
        AX_BREAK_IF(!b2Body_IsValid(_b2Body));

        b2Body_SetUserData(_b2Body, this);

        // TODO:
        // cpBodySetVelocityUpdateFunc(_cpBody, internalBodyUpdateVelocity);

        return true;
    } while (false);

    return false;
}

void PhysicsBody::removeJoint(PhysicsJoint* joint)
{
    auto it = std::find(_joints.begin(), _joints.end(), joint);

    if (it != _joints.end())
    {
        (*it)->deatchFromBody();
        _joints.erase(it);
    }
}

void PhysicsBody::setDynamic(bool dynamic)
{
#    if 1
    if (dynamic != _dynamic)
    {
        _dynamic = dynamic;
        if (dynamic)
        {
            b2Body_SetType(_b2Body, b2_dynamicBody);
            b2Body_ApplyMassFromShapes(_b2Body);
        }
        else
        {
            b2Body_SetType(_b2Body, b2_kinematicBody);
        }
    }
#    endif
}

float PhysicsBody::getMass() const
{
    return b2Body_GetMass(_b2Body);
}

float PhysicsBody::getMoment() const
{
    return b2Body_GetRotationalInertia(_b2Body);
}

void PhysicsBody::setRotationEnable(bool enable)
{
    if (_rotationEnabled != enable)
    {
        // cpBodySetMoment(_cpBody, enable ? _moment : PHYSICS_INFINITY);
        auto massData              = b2Body_GetMassData(_b2Body);
        massData.rotationalInertia = enable ? _moment : PHYSICS_INFINITY;
        b2Body_SetMassData(_b2Body, massData);
        _rotationEnabled = enable;
    }
}

void PhysicsBody::setGravityEnable(bool enable)
{
    _gravityEnabled = enable;
}

void PhysicsBody::setRotation(float rotation)
{
    _recordedRotation = rotation;
    _recordedAngle    = -(rotation + _rotationOffset) * (M_PI / 180.0f);
    b2Body_SetTransform(_b2Body, b2Body_GetPosition(_b2Body), b2MakeRot(_recordedAngle));
}

void PhysicsBody::setScale(float scaleX, float scaleY)
{
    for (auto&& collider : _colliders)
    {
        collider->setScale(scaleX, scaleY);
    }

    b2Body_ApplyMassFromShapes(_b2Body);
}

void PhysicsBody::setPosition(float positionX, float positionY)
{
    Vec2 tt;

    tt.x = (positionX + _positionOffset.x);
    tt.y = (positionY + _positionOffset.y);

    b2Body_SetTransform(_b2Body, phlp::tob2Vec2(tt), b2Body_GetRotation(_b2Body));
}

Vec2 PhysicsBody::getPosition() const
{
    auto tt = b2Body_GetPosition(_b2Body);
    return phlp::toVec2(tt) - _positionOffset;
}

void PhysicsBody::setPositionOffset(const Vec2& position)
{
    if (!_positionOffset.equals(position))
    {
        Vec2 pos        = getPosition();
        _positionOffset = position;
        setPosition(pos.x, pos.y);
    }
}

float PhysicsBody::getRotation()
{
    auto angle = b2Rot_GetAngle(b2Body_GetRotation(_b2Body));
    if (_recordedAngle != angle)
    {
        _recordedAngle    = angle;
        _recordedRotation = -_recordedAngle * 180.0 / M_PI - _rotationOffset;
    }
    return _recordedRotation;
}

PhysicsCollider* PhysicsBody::addCollider(PhysicsCollider* collider, bool addMassAndMoment /* = true*/)
{
    if (collider == nullptr)
        return nullptr;

    // add collider to body
    if (_colliders.getIndex(collider) == -1)
    {
        // calculate the area, mass, and density
        // area must update before mass, because the density changes depend on it.
        if (addMassAndMoment)
        {
            _area += collider->getArea();
        }

        _colliders.pushBack(collider);
    }

    return collider;
}

void PhysicsBody::applyForce(const Vec2& force, const Vec2& offset)
{
    if (_dynamic && getMass() != PHYSICS_INFINITY)
    {
        b2Vec2 worldPoint = b2Body_GetWorldPoint(_b2Body, phlp::tob2Vec2(offset));

        // Apply the force at the world point
        b2Body_ApplyForce(_b2Body, phlp::tob2Vec2(force), worldPoint, true);
    }
}

void PhysicsBody::resetForces()
{
    // Reset the body's forces (Box2D accumulates forces over time)
    b2Body_SetLinearVelocity(_b2Body, b2Vec2_zero);

    // Apply the force to the center of the body
    b2Body_ApplyForceToCenter(_b2Body, b2Vec2_zero, true);
}

void PhysicsBody::applyImpulse(const Vec2& impulse, const Vec2& offset)
{
    b2Vec2 worldPoint = b2Body_GetWorldPoint(_b2Body, phlp::tob2Vec2(offset));

    // Apply the impulse at the world point
    b2Body_ApplyLinearImpulse(_b2Body, phlp::tob2Vec2(impulse), worldPoint, true);
}

void PhysicsBody::applyTorque(float torque)
{
    b2Body_ApplyTorque(_b2Body, torque, true);
}

void PhysicsBody::setVelocity(const Vec2& velocity)
{
    if (b2Body_GetType(_b2Body) == b2_staticBody)
    {
        AXLOGD("physics warning: you can't set velocity for a static body.");
        return;
    }

    b2Body_SetLinearVelocity(_b2Body, PhysicsHelper::tob2Vec2(velocity));
}

Vec2 PhysicsBody::getVelocity()
{
    return PhysicsHelper::toVec2(b2Body_GetLinearVelocity(_b2Body));
}

Vec2 PhysicsBody::getVelocityAtLocalPoint(const Vec2& point)
{
    // return PhysicsHelper::cpv2vec2(cpBodyGetVelocityAtLocalPoint(_cpBody, PhysicsHelper::vec22cpv(point)));

    b2Vec2 worldPoint = b2Body_GetWorldPoint(_b2Body, PhysicsHelper::tob2Vec2(point));

    auto r          = b2Sub(worldPoint, b2Body_GetPosition(_b2Body));
    auto av         = b2Body_GetAngularVelocity(_b2Body);
    b2Vec2 velocity = b2Add(b2Body_GetLinearVelocity(_b2Body), b2Vec2{-r.y * av, r.x * av});

    return PhysicsHelper::toVec2(velocity);
}

Vec2 PhysicsBody::getVelocityAtWorldPoint(const Vec2& point)
{
    // return PhysicsHelper::cpv2vec2(cpBodyGetVelocityAtWorldPoint(_cpBody, PhysicsHelper::vec22cpv(point)));
    b2Vec2 r        = b2Sub(PhysicsHelper::tob2Vec2(point), b2Body_GetPosition(_b2Body));
    auto av         = b2Body_GetAngularVelocity(_b2Body);
    b2Vec2 velocity = b2Add(b2Body_GetLinearVelocity(_b2Body), b2Vec2{-r.y * av, r.x * av});
    return PhysicsHelper::toVec2(velocity);
}

void PhysicsBody::setAngularVelocity(float velocity)
{
    if (b2Body_GetType(_b2Body) == b2_staticBody)
    {
        AXLOGD("physics warning: you can't set angular velocity for a static body.");
        return;
    }

    // cpBodySetAngularVelocity(_cpBody, velocity);
    b2Body_SetAngularVelocity(_b2Body, velocity);
}

float PhysicsBody::getAngularVelocity()
{
    // return PhysicsHelper::cpfloat2float(cpBodyGetAngularVelocity(_cpBody));
    return b2Body_GetAngularVelocity(_b2Body);
}

void PhysicsBody::setVelocityLimit(float limit)
{
    _velocityLimit = limit;
}

float PhysicsBody::getVelocityLimit()
{
    return _velocityLimit;
}

void PhysicsBody::setAngularVelocityLimit(float limit)
{
    _angularVelocityLimit = limit;
}

float PhysicsBody::getAngularVelocityLimit()
{
    return _angularVelocityLimit;
}

PhysicsCollider* PhysicsBody::getCollider(int tag) const
{
    for (auto&& collider : _colliders)
    {
        if (collider->getTag() == tag)
        {
            return collider;
        }
    }

    return nullptr;
}

void PhysicsBody::removeCollider(int tag, bool reduceMassAndMoment /* = true*/)
{
    for (auto&& collider : _colliders)
    {
        if (collider->getTag() == tag)
        {
            removeCollider(collider, reduceMassAndMoment);
            return;
        }
    }
}

void PhysicsBody::removeCollider(PhysicsCollider* collider, bool reduceMassAndMoment /* = true*/)
{
    if (_colliders.getIndex(collider) != -1)
    {
        // deduce the area, mass and moment
        // area must update before mass, because the density changes depend on it.
        if (reduceMassAndMoment)
        {
            _area -= collider->getArea();
        }

        collider->deatchFromBody();
        _colliders.eraseObject(collider);
    }
}

void PhysicsBody::removeAllColliders(bool reduceMassAndMoment /* = true*/)
{
    for (auto&& collider : _colliders)
    {
        // deduce the area, mass and moment
        // area must update before mass, because the density changes depend on it.
        if (reduceMassAndMoment)
        {
            _area -= collider->getArea();
        }

        collider->deatchFromBody();
    }

    _colliders.clear();
}

void PhysicsBody::removeFromWorld()
{
    removeFromPhysicsWorld();
}

void PhysicsBody::setEnabled(bool enable)
{
    if (_enabled != enable)
    {
        _enabled = enable;

        if (_world)
        {
            if (enable)
            {
                _world->addBodyOrDelay(this);
            }
            else
            {
                _world->removeBodyOrDelay(this);
            }
        }
    }
}

bool PhysicsBody::isResting() const
{
    return !b2Body_IsAwake(_b2Body);
}

void PhysicsBody::setResting(bool rest) const
{
    if (rest && !isResting())
    {
        b2Body_SetAwake(_b2Body, false);
    }
    else if (!rest && isResting())
    {
        b2Body_SetAwake(_b2Body, true);
    }
}

void PhysicsBody::update(float delta)
{
    // damping compute
    if (!_fixedUpdate && _isDamping && _dynamic && !isResting())
    {
        internalApplyDamping(_b2Body, delta, _linearDamping, _angularDamping);
    }
}

void PhysicsBody::fixedUpdate(float delta)
{
    if (_fixedUpdate && _isDamping && _dynamic && !isResting())
    {
        internalApplyDamping(_b2Body, delta, _linearDamping, _angularDamping);
    }
}

void PhysicsBody::setCategoryBitmask(int bitmask)
{
    for (auto&& collider : _colliders)
    {
        collider->setCategoryBitmask(bitmask);
    }
}

int PhysicsBody::getCategoryBitmask() const
{
    if (!_colliders.empty())
    {
        return _colliders.front()->getCategoryBitmask();
    }
    else
    {
        return UINT_MAX;
    }
}

void PhysicsBody::setContactTestBitmask(int bitmask)
{
    for (auto&& collider : _colliders)
    {
        collider->setContactTestBitmask(bitmask);
    }
}

int PhysicsBody::getContactTestBitmask() const
{
    if (!_colliders.empty())
    {
        return _colliders.front()->getContactTestBitmask();
    }
    else
    {
        return 0x00000000;
    }
}

void PhysicsBody::setCollisionBitmask(int bitmask)
{
    for (auto&& collider : _colliders)
    {
        collider->setCollisionBitmask(bitmask);
    }
}

int PhysicsBody::getCollisionBitmask() const
{
    if (!_colliders.empty())
    {
        return _colliders.front()->getCollisionBitmask();
    }
    else
    {
        return UINT_MAX;
    }
}

void PhysicsBody::setGroup(int group)
{
    for (auto&& collider : _colliders)
    {
        collider->setGroup(group);
    }
}

int PhysicsBody::getGroup() const
{
    if (!_colliders.empty())
    {
        return _colliders.front()->getGroup();
    }
    else
    {
        return 0;
    }
}

void PhysicsBody::setRotationOffset(float rotation)
{
    if (std::abs(_rotationOffset - rotation) > 0.5f)
    {
        float rot       = getRotation();
        _rotationOffset = rotation;
        setRotation(rot);
    }
}

Vec2 PhysicsBody::world2Local(const Vec2& point)
{
    return phlp::toVec2(b2Body_GetLocalPoint(_b2Body, phlp::tob2Vec2(point)));
}

Vec2 PhysicsBody::local2World(const Vec2& point)
{
    return phlp::toVec2(b2Body_GetWorldPoint(_b2Body, phlp::tob2Vec2(point)));
}

void PhysicsBody::beforeSimulation(const Mat4& parentToWorldTransform,
                                   const Mat4& nodeToWorldTransform,
                                   float scaleX,
                                   float scaleY,
                                   float rotation)
{
    if (_recordScaleX != scaleX || _recordScaleY != scaleY)
    {
        _recordScaleX = scaleX;
        _recordScaleY = scaleY;
        setScale(scaleX, scaleY);
    }

    // set rotation
    if (_recordedRotation != rotation)
    {
        setRotation(rotation);
    }

    // set position
    auto worldPosition = _ownerCenterOffset;
    nodeToWorldTransform.transformVector(worldPosition.x, worldPosition.y, worldPosition.z, 1.f, &worldPosition);
    setPosition(worldPosition.x, worldPosition.y);

    _recordPosX = worldPosition.x;
    _recordPosY = worldPosition.y;

    if (_owner->getAnchorPoint() != Vec2::ANCHOR_MIDDLE)
    {
        parentToWorldTransform.getInversed().transformVector(worldPosition.x, worldPosition.y, worldPosition.z, 1.f,
                                                             &worldPosition);
        _offset.x = worldPosition.x - _owner->getPositionX();
        _offset.y = worldPosition.y - _owner->getPositionY();
    }
}

void PhysicsBody::afterSimulation(const Mat4& parentToWorldTransform, float parentRotation)
{
    // set Node position
    auto tmp = getPosition();
    Vec3 positionInParent(tmp.x, tmp.y, 0.f);
    if (_recordPosX != positionInParent.x || _recordPosY != positionInParent.y)
    {
        parentToWorldTransform.getInversed().transformVector(positionInParent.x, positionInParent.y, positionInParent.z,
                                                             1.f, &positionInParent);
        _owner->setPosition(positionInParent.x - _offset.x, positionInParent.y - _offset.y);
    }

    // set Node rotation
    _owner->setRotation(getRotation() - parentRotation);
}

void PhysicsBody::onEnter()
{
    addToPhysicsWorld();
}

void PhysicsBody::onExit()
{
    removeFromPhysicsWorld();
}

void PhysicsBody::onAdd()
{
    _owner->_physicsBody = this;
    auto contentSize     = _owner->getContentSize();
    _ownerCenterOffset.x = 0.5f * contentSize.width;
    _ownerCenterOffset.y = 0.5f * contentSize.height;

    setRotationOffset(_owner->getRotation());

    // component may be added after onEnter() has been invoked, so we should add
    // this line to make sure physics body is added to physics world
    addToPhysicsWorld();
}

void PhysicsBody::onRemove()
{
    AXASSERT(_owner != nullptr, "_owner can't be nullptr");

    removeFromPhysicsWorld();

    _owner->_physicsBody = nullptr;
}

void PhysicsBody::addToPhysicsWorld()
{
    if (_owner)
    {
        auto scene = _owner->getScene();
        if (scene)
            scene->getPhysicsWorld()->addBody(this);
    }
}

void PhysicsBody::removeFromPhysicsWorld()
{
    if (_owner)
    {
        auto scene = _owner->getScene();
        if (scene)
            scene->getPhysicsWorld()->removeBody(this);
    }
}

}  // namespace ax

#endif  // defined(AX_ENABLE_PHYSICS)
