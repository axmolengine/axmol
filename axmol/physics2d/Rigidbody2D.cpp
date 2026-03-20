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

 Check whether applyForce need invoke PhysicsUtility2D::pixelsToMeters

 ****************************************************************************/
#include "axmol/physics2d/Rigidbody2D.h"
#if defined(AX_ENABLE_PHYSICS_2D)
#    include <climits>
#    include <algorithm>
#    include <cmath>

#    include "axmol/2d/Scene.h"
#    include "axmol/physics2d/Collider2D.h"
#    include "axmol/physics2d/Joint2D.h"
#    include "axmol/physics2d/PhysicsWorld2D.h"
#    include "axmol/physics2d/PhysicsUtility2D.h"
#    include "axmol/physics2d/PhysicsConsts.h"

namespace ax
{
const std::string_view Rigidbody2D::COMPONENT_NAME = "Rigidbody2D"sv;

Rigidbody2D::Rigidbody2D()
    : _world(nullptr)
    , _bodyId(b2_nullBodyId)
    , _bodyType(DynamicBody)
    , _rotationEnabled(true)
    , _gravityEnabled(true)
    , _isSleeping(false)
    , _isDamping(false)
    , _autoSyncTransformComplete(false)
    , _collisionDetection(CollisionDetectionMode::Discrete)
    , _area(0.0f)
    , _density(0.0f)
    , _moment(physics2d::DefaultMoment)
    , _velocityLimit(physics2d::Infinity)
    , _angularVelocityLimit(physics2d::Infinity)
    , _linearDamping(0.0f)
    , _angularDamping(0.0f)
    , _angularVelocity(0.0f)
    , _tag(0)
    , _rotationOffset(0)
    , _recordedRotation(0.0f)
    , _recordedAngle(0.0)
    , _recordScaleX(1.f)
    , _recordScaleY(1.f)
    , _recordPosX(0.0f)
    , _recordPosY(0.0f)
{
    _name = COMPONENT_NAME;
}

Rigidbody2D::~Rigidbody2D()
{
    detachFromWorld();
}

Rigidbody2D* Rigidbody2D::create()
{
    Rigidbody2D* body = new Rigidbody2D();
    if (body->init())
    {
        body->autorelease();
        return body;
    }

    AX_SAFE_DELETE(body);
    return nullptr;
}

Rigidbody2D* Rigidbody2D::createCircle(float radius, const PhysicsMaterial2D& material, const Vec2& offset)
{
    Rigidbody2D* body = new Rigidbody2D();
    if (body->init())
    {
        body->addCollider(CircleCollider2D::create(radius, material, offset));
        body->autorelease();
        return body;
    }

    AX_SAFE_DELETE(body);
    return nullptr;
}

Rigidbody2D* Rigidbody2D::createBox(const Vec2& size, const PhysicsMaterial2D& material, const Vec2& offset)
{
    Rigidbody2D* body = new Rigidbody2D();
    if (body->init())
    {
        body->addCollider(BoxCollider2D::create(size, material, offset));
        body->autorelease();
        return body;
    }

    AX_SAFE_DELETE(body);
    return nullptr;
}

Rigidbody2D* Rigidbody2D::createPolygon(std::span<const Vec2> points,
                                        const PhysicsMaterial2D& material,
                                        const Vec2& offset)
{
    Rigidbody2D* body = new Rigidbody2D();
    body->addCollider(PolygonCollider2D::create(points, material, offset));
    body->autorelease();
    return body;
}

Rigidbody2D* Rigidbody2D::createEdgeSegment(const Vec2& a,
                                            const Vec2& b,
                                            const PhysicsMaterial2D& material)
{
    Rigidbody2D* body = new Rigidbody2D();
    body->addCollider(EdgeSegmentCollider2D::create(a, b, material));
    body->setBodyType(StaticBody);

    body->autorelease();
    return body;
}

Rigidbody2D* Rigidbody2D::createEdgeBox(const Vec2& size,
                                        const PhysicsMaterial2D& material,
                                        const Vec2& offset)
{
    Rigidbody2D* body = new Rigidbody2D();
    body->addCollider(EdgeBoxCollider2D::create(size, material, offset));
    body->setBodyType(StaticBody);
    body->autorelease();
    return body;
}

Rigidbody2D* Rigidbody2D::createEdgePolygon(std::span<const Vec2> points,
                                            const PhysicsMaterial2D& material)
{
    Rigidbody2D* body = new Rigidbody2D();
    body->addCollider(EdgePolygonCollider2D::create(points, material));
    body->setBodyType(StaticBody);
    body->autorelease();
    return body;
}

Rigidbody2D* Rigidbody2D::createEdgeChain(std::span<const Vec2> points,
                                          const PhysicsMaterial2D& material)
{
    Rigidbody2D* body = new Rigidbody2D();
    body->addCollider(EdgeChainCollider2D::create(points, material));
    body->setBodyType(StaticBody);
    body->autorelease();
    return body;
}

bool Rigidbody2D::attachToWorld(PhysicsWorld2D* world)
{
    if (isAttached())
        return true;

    do
    {
        if (world)
            _world = world;
        else
        {
            auto scene = _owner->getScene();
            AX_BREAK_IF(!scene);
            _world = scene->getPhysicsWorld2D();
            AX_BREAK_IF(!_world);
        }

        b2BodyDef def      = b2DefaultBodyDef();
        def.type           = static_cast<b2BodyType>(_bodyType);
        def.position       = b2Vec2{0.0f, 0.0f};
        def.linearVelocity = PhysicsUtility2D::tob2Vec2(_linearVelocity);
        def.linearDamping  = _linearDamping;

        def.angularVelocity = _angularVelocity;
        def.angularDamping  = _angularDamping;

        def.gravityScale = _gravityEnabled ? 1.0f : 0.0f;
        def.isAwake      = !_isSleeping;
        def.isBullet     = _collisionDetection == CollisionDetectionMode::Continuous;

        def.userData = this;

        _bodyId = b2CreateBody(_world->internalHandle(), &def);
        AX_BREAK_IF(!b2Body_IsValid(_bodyId));

        for (auto collider : _colliders)
            collider->attachToBody(this);

        auto contentSize     = _owner->getContentSize();
        _ownerCenterOffset.x = 0.5f * contentSize.width;
        _ownerCenterOffset.y = 0.5f * contentSize.height;
        setRotationOffset(_owner->getRotation());

        return true;
    } while (false);

    return false;
}

void Rigidbody2D::detachFromWorld()
{
    if (!isAttached())
        return;

    removeAllColliders();

    auto attachedJointCount = b2Body_GetJointCount(_bodyId);
    if (attachedJointCount > 0)
    {
        tlx::pod_vector<b2JointId> attachedJoints(attachedJointCount);
        attachedJointCount = b2Body_GetJoints(_bodyId, attachedJoints.data(), attachedJointCount);
        for (int i = 0; i < attachedJointCount; ++i)
        {
            b2JointId jointId = attachedJoints[i];
            if (b2Joint_IsValid(jointId))
            {
                auto joint = static_cast<Joint2D*>(b2Joint_GetUserData(jointId));
                if (joint)
                {
                    joint->detachFromBody();
                }
            }
        }
    }

    b2DestroyBody(_bodyId);
    _bodyId = b2_nullBodyId;
}

bool Rigidbody2D::isAttached() const
{
    return b2Body_IsValid(_bodyId);
}

void Rigidbody2D::setBodyType(BodyType bodyType)
{
    if (bodyType != _bodyType)
    {
        _bodyType = bodyType;

        if (isAttached())
        {
            b2Body_SetType(_bodyId, static_cast<b2BodyType>(_bodyType));
            if (isDynamic())
            {
                b2Body_ApplyMassFromShapes(_bodyId);
            }
        }
    }
}

float Rigidbody2D::getMass() const
{
    return isAttached() ? b2Body_GetMass(_bodyId) : 0.0f;
}

float Rigidbody2D::getMoment() const
{
    return isAttached() ? b2Body_GetRotationalInertia(_bodyId) : 0.0f;
}

void Rigidbody2D::setRotationEnable(bool enable)
{
    if (_rotationEnabled != enable)
    {

        _rotationEnabled = enable;

        if (isAttached())
        {
            auto massData              = b2Body_GetMassData(_bodyId);
            massData.rotationalInertia = enable ? _moment : physics2d::Infinity;

            b2Body_SetMassData(_bodyId, massData);
        }
    }
}

void Rigidbody2D::setGravityEnable(bool enable)
{
    _gravityEnabled = enable;

    if (isAttached())
    {
        b2Body_SetGravityScale(_bodyId, enable ? 1.0f : 0.0f);
    }
}

void Rigidbody2D::setRotation(float rotation)
{
    _recordedRotation = rotation;
    _recordedAngle    = -(rotation + _rotationOffset) * (M_PI / 180.0f);

    if (isAttached())
        b2Body_SetTransform(_bodyId, b2Body_GetPosition(_bodyId), b2MakeRot(_recordedAngle));
}

void Rigidbody2D::setScale(float scaleX, float scaleY)
{
    for (auto&& collider : _colliders)
    {
        collider->setScale(scaleX, scaleY);
    }

    if (isAttached())
        b2Body_ApplyMassFromShapes(_bodyId);
}

void Rigidbody2D::setPosition(float positionX, float positionY)
{
    if (!isAttached())
        return;
    Vec2 tt;

    tt.x = (positionX + _positionOffset.x);
    tt.y = (positionY + _positionOffset.y);

    b2Body_SetTransform(_bodyId, PhysicsUtility2D::tob2Vec2(tt), b2Body_GetRotation(_bodyId));
}

Vec2 Rigidbody2D::getPosition() const
{
    if (!isAttached())
        return -_positionOffset;
    auto tt = b2Body_GetPosition(_bodyId);
    return PhysicsUtility2D::toVec2(tt) - _positionOffset;
}

void Rigidbody2D::setPositionOffset(const Vec2& position)
{
    if (!_positionOffset.equals(position))
    {
        Vec2 pos        = getPosition();
        _positionOffset = position;
        setPosition(pos.x, pos.y);
    }
}

float Rigidbody2D::getRotation()
{
    if (!isAttached())
        return 0.0f;
    auto angle = b2Rot_GetAngle(b2Body_GetRotation(_bodyId));
    if (_recordedAngle != angle)
    {
        _recordedAngle    = angle;
        _recordedRotation = -_recordedAngle * 180.0 / M_PI - _rotationOffset;
    }
    return _recordedRotation;
}

Collider2D* Rigidbody2D::addCollider(Collider2D* collider, bool addMassAndMoment /* = true*/)
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

        if (isAttached())
            collider->attachToBody(this);

        _colliders.pushBack(collider);
    }

    return collider;
}

void Rigidbody2D::applyForce(const Vec2& force, const Vec2& offset)
{
    if (!isAttached())
        return;
    if (isDynamic() && getMass() != physics2d::MaxMass)
    {
        b2Vec2 worldPoint = b2Body_GetWorldPoint(_bodyId, PhysicsUtility2D::tob2Vec2(offset));

        // Apply the force at the world point
        b2Body_ApplyForce(_bodyId, PhysicsUtility2D::tob2Vec2(force), worldPoint, true);
    }
}

void Rigidbody2D::resetForces()
{
    if (!isAttached())
        return;
    // Reset the body's forces (Box2D accumulates forces over time)
    b2Body_SetLinearVelocity(_bodyId, b2Vec2_zero);

    // Apply the force to the center of the body
    b2Body_ApplyForceToCenter(_bodyId, b2Vec2_zero, true);
}

void Rigidbody2D::applyImpulse(const Vec2& impulse, const Vec2& offset)
{
    if (!isAttached())
        return;
    b2Vec2 worldPoint = b2Body_GetWorldPoint(_bodyId, PhysicsUtility2D::tob2Vec2(offset));

    // Apply the impulse at the world point
    b2Body_ApplyLinearImpulse(_bodyId, PhysicsUtility2D::tob2Vec2(impulse), worldPoint, true);
}

void Rigidbody2D::applyTorque(float torque)
{
    if (!isAttached())
        return;
    b2Body_ApplyTorque(_bodyId, torque, true);
}

void Rigidbody2D::setVelocity(const Vec2& velocity)
{
    _linearVelocity = velocity;
    if (_bodyType == StaticBody)
    {
        AXLOGD("physics warning: you can't set angular velocity for a static body.");
        return;
    }

    if (isAttached())
        b2Body_SetLinearVelocity(_bodyId, PhysicsUtility2D::tob2Vec2(velocity));
}

Vec2 Rigidbody2D::getVelocity()
{
    return isAttached() ? PhysicsUtility2D::toVec2(b2Body_GetLinearVelocity(_bodyId)) : _linearVelocity;
}

Vec2 Rigidbody2D::getVelocityAtLocalPoint(const Vec2& point)
{
    if (!isAttached())
        return Vec2::ZERO;
    b2Vec2 worldPoint = b2Body_GetWorldPoint(_bodyId, PhysicsUtility2D::tob2Vec2(point));

    auto r          = b2Sub(worldPoint, b2Body_GetPosition(_bodyId));
    auto av         = b2Body_GetAngularVelocity(_bodyId);
    b2Vec2 velocity = b2Add(b2Body_GetLinearVelocity(_bodyId), b2Vec2{-r.y * av, r.x * av});

    return PhysicsUtility2D::toVec2(velocity);
}

Vec2 Rigidbody2D::getVelocityAtWorldPoint(const Vec2& point)
{
    if (!isAttached())
        return Vec2::ZERO;
    b2Vec2 r        = b2Sub(PhysicsUtility2D::tob2Vec2(point), b2Body_GetPosition(_bodyId));
    auto av         = b2Body_GetAngularVelocity(_bodyId);
    b2Vec2 velocity = b2Add(b2Body_GetLinearVelocity(_bodyId), b2Vec2{-r.y * av, r.x * av});
    return PhysicsUtility2D::toVec2(velocity);
}

void Rigidbody2D::setAngularVelocity(float velocity)
{
    if (_bodyType == StaticBody)
    {
        AXLOGD("physics warning: you can't set angular velocity for a static body.");
        return;
    }

    _angularVelocity = velocity;
    if (isAttached())
        b2Body_SetAngularVelocity(_bodyId, velocity);
}

float Rigidbody2D::getAngularVelocity()
{
    return isAttached() ? b2Body_GetAngularVelocity(_bodyId) : _angularVelocity;
}

void Rigidbody2D::setVelocityLimit(float limit)
{
    _velocityLimit = limit;
}

float Rigidbody2D::getVelocityLimit()
{
    return _velocityLimit;
}

void Rigidbody2D::setAngularVelocityLimit(float limit)
{
    _angularVelocityLimit = limit;
}

float Rigidbody2D::getAngularVelocityLimit()
{
    return _angularVelocityLimit;
}

Collider2D* Rigidbody2D::getCollider(int tag) const
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

void Rigidbody2D::removeCollider(int tag, bool reduceMassAndMoment /* = true*/)
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

void Rigidbody2D::removeCollider(Collider2D* collider, bool reduceMassAndMoment /* = true*/)
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

void Rigidbody2D::removeAllColliders(bool reduceMassAndMoment /* = true*/)
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

void Rigidbody2D::removeFromWorld()
{
    detachFromWorld();
}

void Rigidbody2D::setEnabled(bool enable)
{
    if (_enabled != enable)
    {
        _enabled = enable;
    }
}

void Rigidbody2D::setSleeping(bool sleeping)
{
    _isSleeping = sleeping;
    if (isAttached())
        b2Body_SetAwake(_bodyId, !sleeping);
}

bool Rigidbody2D::isSleeping() const
{
    return isAttached() ? b2Body_IsAwake(_bodyId) : _isSleeping;
}

void Rigidbody2D::setCollisionDetectionMode(CollisionDetectionMode mode)
{
    _collisionDetection = mode;
    if (isAttached())
        b2Body_SetBullet(_bodyId, mode == CollisionDetectionMode::Continuous);
}

void Rigidbody2D::setCategoryBitmask(int bitmask)
{
    for (auto&& collider : _colliders)
    {
        collider->setCategoryBitmask(bitmask);
    }
}

int Rigidbody2D::getCategoryBitmask() const
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

void Rigidbody2D::setContactTestBitmask(int bitmask)
{
    for (auto&& collider : _colliders)
    {
        collider->setContactTestBitmask(bitmask);
    }
}

int Rigidbody2D::getContactTestBitmask() const
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

void Rigidbody2D::setCollisionBitmask(int bitmask)
{
    for (auto&& collider : _colliders)
    {
        collider->setCollisionBitmask(bitmask);
    }
}

int Rigidbody2D::getCollisionBitmask() const
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

void Rigidbody2D::setGroup(int group)
{
    for (auto&& collider : _colliders)
    {
        collider->setGroup(group);
    }
}

int Rigidbody2D::getGroup() const
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

void Rigidbody2D::setRotationOffset(float rotation)
{
    if (std::abs(_rotationOffset - rotation) > 0.5f)
    {
        float rot       = getRotation();
        _rotationOffset = rotation;
        setRotation(rot);
    }
}

Vec2 Rigidbody2D::world2Local(const Vec2& point)
{
    return isAttached() ? PhysicsUtility2D::toVec2(b2Body_GetLocalPoint(_bodyId, PhysicsUtility2D::tob2Vec2(point)))
                        : Vec2::ZERO;
}

Vec2 Rigidbody2D::local2World(const Vec2& point)
{
    return isAttached() ? PhysicsUtility2D::toVec2(b2Body_GetWorldPoint(_bodyId, PhysicsUtility2D::tob2Vec2(point)))
                        : Vec2::ZERO;
}

float Rigidbody2D::getAngleRadians()
{
    if (isAttached())
    {
        b2Rot rot   = b2Body_GetRotation(_bodyId);
        float angle = b2Rot_GetAngle(rot);
        return angle;
    }

    return 0.0f;
}

void Rigidbody2D::syncTransform()
{
    if (_autoSyncTransformComplete)
        return;

    auto parent = _owner->getParent();
    if (parent)
        syncTransform(parent->getNodeToWorldTransform(), _owner->getNodeToWorldTransform(), _owner->getScaleX(),
                      _owner->getScaleY(), _owner->getRotation());
    else
        syncTransform(Mat4::IDENTITY, _owner->getNodeToWorldTransform(), _owner->getScaleX(), _owner->getScaleY(),
                      _owner->getRotation());

    _autoSyncTransformComplete = true;
}

void Rigidbody2D::beforeSimulation(const Mat4& parentToWorldTransform,
                                   const Mat4& nodeToWorldTransform,
                                   float scaleX,
                                   float scaleY,
                                   float rotation)
{
    syncTransform(parentToWorldTransform, nodeToWorldTransform, scaleX, scaleY, rotation);
}

void Rigidbody2D::syncTransform(const Mat4& parentToWorldTransform,
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

void Rigidbody2D::afterSimulation(const Mat4& parentToWorldTransform, float parentRotation)
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

void Rigidbody2D::onEnter()
{
    attachToWorld();
}

void Rigidbody2D::onExit()
{
    detachFromWorld();
}

void Rigidbody2D::onAdd()
{
    _owner->_rigidbody2D = this;

    // component may be added after onEnter() has been invoked, so we should add
    // this line to make sure physics body is added to physics world
    attachToWorld();
}

void Rigidbody2D::onRemove()
{
    AXASSERT(_owner != nullptr, "_owner can't be nullptr");

    detachFromWorld();
}

}  // namespace ax

#endif  // defined(AX_ENABLE_PHYSICS_2D)
