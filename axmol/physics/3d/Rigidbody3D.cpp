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
#include "axmol/physics/3d/Rigidbody3D.h"

#if defined(AX_ENABLE_PHYSICS_3D)

#    include "axmol/physics/3d/Collider3D.h"
#    include "axmol/physics/3d/Joint3D.h"
#    include "axmol/physics/3d/PhysicsUtility3D.h"
#    include "axmol/physics/3d/PhysicsWorld3D.h"
#    include "axmol/scene/Node.h"
#    include "axmol/scene/Scene.h"

#    include <Jolt/Physics/Body/Body.h>
#    include <Jolt/Physics/Body/BodyCreationSettings.h>
#    include <Jolt/Physics/Body/BodyLock.h>
#    include <Jolt/Physics/Collision/Shape/EmptyShape.h>

#    include <algorithm>

namespace ax
{

namespace
{

static bool withBodyReadLock(const Rigidbody3D* body, const std::function<void(const JPH::Body&)>& fn)
{
    if (!body->isAttached())
        return false;

    auto world = body->getWorld();
    JPH::BodyLockRead lock(world->internalHandle().GetBodyLockInterface(), body->internalHandle());
    if (!lock.Succeeded())
        return false;

    fn(lock.GetBody());
    return true;
}

static bool withBodyWriteLock(const Rigidbody3D* body, const std::function<void(JPH::Body&)>& fn)
{
    if (!body->isAttached())
        return false;

    auto world = body->getWorld();
    JPH::BodyLockWrite lock(world->internalHandle().GetBodyLockInterface(), body->internalHandle());
    if (!lock.Succeeded())
        return false;

    fn(lock.GetBody());
    return true;
}

static bool validateInertia(const Vec3& inertia)
{
    return inertia.x != 0.0f || inertia.y != 0.0f || inertia.z != 0.0f;
}

static void clampMassData(float& mass, Vec3& inertia)
{
    // Mass must be positive, otherwise treat as static body
    if (mass <= phconsts::LowerBound)
        mass = 0.0f;

    inertia.x = (inertia.x > phconsts::LowerBound) ? inertia.x : 0.0f;
    inertia.y = (inertia.y > phconsts::LowerBound) ? inertia.y : 0.0f;
    inertia.z = (inertia.z > phconsts::LowerBound) ? inertia.z : 0.0f;
}

static Vec3 extractTranslation(const Mat4& transform)
{
    Vec3 out;
    transform.getTranslation(&out);
    return out;
}

static JPH::EMotionQuality toJoltMotionQuality(Rigidbody3D::CollisionDetectionMode mode)
{
    return mode == Rigidbody3D::CollisionDetectionMode::Discrete ? JPH::EMotionQuality::Discrete
                                                                 : JPH::EMotionQuality::LinearCast;
}

static bool canShapeAutoCalculateInertia(JPH::RefConst<JPH::Shape>& shape)
{
    auto type = shape->GetSubType();
    return type != JPH::EShapeSubType::Mesh && type != JPH::EShapeSubType::Triangle;
}

static bool tryBuildMassProperties(Collider3D* collider,
                                   float mass,
                                   const Vec3& inertia,
                                   JPH::MassProperties& outMassProperties)
{
    if (!collider)
        return false;

    JPH::RefConst<JPH::Shape> shape = collider->internalShape();
    if (!shape)
        return false;

    if (validateInertia(inertia))
    {
        outMassProperties.mMass    = mass;
        outMassProperties.mInertia = JPH::Mat44::sScale(jphutil::cast(inertia));
        return true;
    }

    if (!canShapeAutoCalculateInertia(shape))
        return false;

    outMassProperties = shape->GetMassProperties();
    outMassProperties.ScaleToMass(mass);
    return true;
}

static void eraseUnordered(std::vector<Joint3D*>& comps, std::vector<Joint3D*>::iterator& it)
{
    // swap with last and pop_back
    if (it != comps.end() - 1)
        *it = comps.back();
    comps.pop_back();
}

}  // namespace

Rigidbody3D::Rigidbody3D() : _allowedDOFs((uint32_t)JPH::EAllowedDOFs::All)
{
    _actorType = PhysicsActor::kRigidbody;
    setName("Rigidbody3D"sv);
    _transformInPhysics.setIdentity();
    _invTransformInPhysics.setIdentity();
}

Rigidbody3D::~Rigidbody3D()
{
    detachFromWorld();
    if (_collider)
    {
        _collider->setAttachedBody(nullptr);
        _collider->release();
        _collider = nullptr;
    }
}

Rigidbody3D* Rigidbody3D::create(Collider3D* collider, float mass)
{
    auto ret = new Rigidbody3D();
    if (ret->init(collider, mass))
    {
        ret->autorelease();
        return ret;
    }

    AX_SAFE_DELETE(ret);
    return nullptr;
}

bool Rigidbody3D::init(Collider3D* collider, float mass)
{
    Component::init();

    if (collider)
    {
        _collider = collider;
        _collider->retain();
    }

    _mass = mass;

    setTransformInPhysics(Vec3::ZERO, Quaternion::identity());
    return true;
}

void Rigidbody3D::setEnabled(bool enabled)
{
    bool oldEnabled = _enabled;
    Component::setEnabled(enabled);
    if (oldEnabled == _enabled || !_owner)
        return;

    if (_enabled)
        attachToWorld();
    else
        detachFromWorld();
}

void Rigidbody3D::onEnter()
{
    Component::onEnter();
    if (_enabled)
        attachToWorld();
}

void Rigidbody3D::onExit()
{
    Component::onExit();
    detachFromWorld();
}

bool Rigidbody3D::attachToWorld(PhysicsWorld3D* world)
{
    if (isAttached())
        return true;

    if (!world)
    {
        if (!_owner || !_owner->getScene())
            return false;
        world = _owner->getScene()->getPhysicsWorld3D();
        if (!world)
            return false;
    }

    // Obtain shape: use collider's shape or a fallback EmptyShape
    JPH::RefConst<JPH::Shape> shape =
        _collider ? _collider->internalShape() : JPH::RefConst<JPH::Shape>(new JPH::EmptyShape());
    if (!shape)
        return false;

    // Compute world transform for the body and remove node scale
    Mat4 worldTransform = _owner->getNodeToWorldTransform();
    jphutil::stripScaleFromTransform(worldTransform);
    worldTransform *= _invTransformInPhysics;

    // Decompose transform into scale/rotation/translation
    Vec3 scale;
    Vec3 translation;
    Quaternion rotation;
    worldTransform.decompose(&scale, &rotation, &translation);
    rotation.normalize();

    clampMassData(_mass, _inertia);

    // Determine motion type: Kinematic > Dynamic (mass>0) > Static
    if (_mass > phconsts::LowerBound && shape->GetType() == JPH::EShapeType::Empty)
    {
        _mass = 0.0f;
        AXLOGW("Invalid configuration: EmptyShape cannot be Dynamic. Forcing mass=0.");
    }

    if (_motionType == MotionType::Unspec)
        _motionType = _mass > phconsts::LowerBound ? MotionType::Dynamic : MotionType::Static;
    else if (_motionType != MotionType::Static && _mass < phconsts::LowerBound)
    {
        AXLOGW("Invalid configuration: Dynamic/Kinematic body requires positive mass. Forcing motionType=Static.");
        _motionType = MotionType::Static;
    }

    // Choose object layer based on sensor flag and motion type
    const auto objectLayer = _collider && _collider->isSensor()
                                 ? detail::kSensorLayer
                                 : (_motionType == MotionType::Static ? detail::kStaticLayer : detail::kDynamicLayer);

    JPH::BodyCreationSettings settings(shape.GetPtr(), jphutil::cast(translation), jphutil::cast(rotation),
                                       (JPH::EMotionType)_motionType, objectLayer);

    // Default to: do not calculate mass at all
    settings.mOverrideMassProperties = JPH::EOverrideMassProperties::MassAndInertiaProvided;
    // Fill other settings
    settings.mUserData                     = std::bit_cast<uintptr_t>(this);
    settings.mAllowSleeping                = _allowSleeping;
    settings.mMotionQuality                = toJoltMotionQuality(_collisionDetection);
    settings.mCollideKinematicVsNonDynamic = isKinematic();
    settings.mAllowDynamicOrKinematic      = _mass > phconsts::LowerBound;

    if (_motionType != MotionType::Static)
    {
        if (validateInertia(_inertia))
        {
            settings.mOverrideMassProperties          = JPH::EOverrideMassProperties::MassAndInertiaProvided;
            settings.mMassPropertiesOverride.mMass    = _mass;
            settings.mMassPropertiesOverride.mInertia = JPH::Mat44::sScale(jphutil::cast(_inertia));
        }
        else if (canShapeAutoCalculateInertia(shape))
        {
            settings.mOverrideMassProperties       = JPH::EOverrideMassProperties::CalculateInertia;
            settings.mMassPropertiesOverride.mMass = _mass;
        }
        else
        {
            //  Fallback: force to static with a warning
            AXLOGW(
                "Rigidbody3D: Dynamic/Kinematic body with '{}' shape requires explicit "
                "localInertia. Mass={} is ignored, forcing static.",
                static_cast<int>(shape->GetType()), _mass);
        }
    }

    if (_collider)
    {
        settings.mFriction    = _collider->getMaterial().friction;
        settings.mRestitution = _collider->getMaterial().restitution;
        settings.mIsSensor    = _collider->isSensor();
    }

    settings.mLinearVelocity  = jphutil::cast(_linearVelocity);
    settings.mAngularVelocity = jphutil::cast(_angularVelocity);
    settings.mLinearDamping   = _linearDamping;
    settings.mAngularDamping  = _angularDamping;
    settings.mGravityFactor   = _gravityScale;
    settings.mAllowedDOFs     = static_cast<JPH::EAllowedDOFs>(_allowedDOFs);

    // Create and add the body to the physics system
    auto& bodyInterface = world->internalHandle().GetBodyInterface();
    _bodyID             = bodyInterface.CreateAndAddBody(
        settings, _motionType == MotionType::Static ? JPH::EActivation::DontActivate : JPH::EActivation::Activate);
    if (_bodyID.IsInvalid())
        return false;

    // Register this actor with the world
    world->addPhysicsActor(this);
    _world = world;

    // If we have a collider, attach it to the created body
    if (_collider)
        _collider->setAttachedBody(this);

    return true;
}

void Rigidbody3D::detachFromWorld()
{
    if (!isAttached())
        return;

    _world->removePhysicsActor(this);
}

void Rigidbody3D::invalidate()
{
    if (!_joints.empty())
    {
        auto joints = std::exchange(_joints, {});
        for (auto joint : joints)
            joint->detachFromBody();
    }

    auto& bodyInterface = _world->internalHandle().GetBodyInterface();
    bodyInterface.DestroyBody(_bodyID);
    _bodyID = JPH::BodyID{};
    if (_collider)
        _collider->setAttachedBody(nullptr);
    _world = nullptr;
}

void Rigidbody3D::setTransformInPhysics(const Vec3& translateInPhysics, const Quaternion& rotInPhysics)
{
    Mat4::createRotation(rotInPhysics, &_transformInPhysics);
    _transformInPhysics.m[12] = translateInPhysics.x;
    _transformInPhysics.m[13] = translateInPhysics.y;
    _transformInPhysics.m[14] = translateInPhysics.z;
    _invTransformInPhysics    = _transformInPhysics.getInversed();
}

void Rigidbody3D::syncPhysicsToNode()
{
    if (!isAttached())
        return;

    Mat4 parentMat;
    if (_owner->getParent())
        parentMat = _owner->getParent()->getNodeToWorldTransform();

    auto mat = parentMat.getInversed() * getWorldTransform();
    jphutil::stripScaleFromTransform(mat);
    mat *= _transformInPhysics;

    Vec3 scale;
    Vec3 translation;
    Quaternion quat;
    mat.decompose(&scale, &quat, &translation);
    quat.normalize();
    _owner->setPosition3D(translation);
    _owner->setRotationQuat(quat);
}

void Rigidbody3D::syncNodeToPhysics()
{
    if (!_owner || !isAttached())
        return;

    auto mat = _owner->getNodeToWorldTransform();
    jphutil::stripScaleFromTransform(mat);
    mat *= _invTransformInPhysics;
    Vec3 scale;
    Vec3 translation;
    Quaternion quat;
    mat.decompose(&scale, &quat, &translation);
    quat.normalize();

    _world->internalHandle().GetBodyInterface().SetPositionAndRotation(_bodyID, jphutil::cast(translation),
                                                                       jphutil::cast(quat), JPH::EActivation::Activate);
}

void Rigidbody3D::setPosition(const Vec3& point, bool teleport)
{
    if (!isAttached())
        return;

    // Preserve current rotation
    JPH::BodyLockRead lock(_world->internalHandle().GetBodyLockInterface(), _bodyID);
    if (!lock.Succeeded())
        return;

    JPH::Quat currentRotation = lock.GetBody().GetRotation();

    auto activation = teleport ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
    _world->internalHandle().GetBodyInterface().SetPositionAndRotation(_bodyID, jphutil::cast(point), currentRotation,
                                                                       activation);
}

Vec3 Rigidbody3D::getPosition() const
{
    if (!isAttached())
        return Vec3::ZERO;

    return jphutil::cast(_world->internalHandle().GetBodyInterface().GetPosition(_bodyID));
}

void Rigidbody3D::setRotation(const Quaternion& rot, bool teleport)
{
    if (!isAttached())
        return;

    // Preserve current position
    JPH::BodyLockRead lock(_world->internalHandle().GetBodyLockInterface(), _bodyID);
    if (!lock.Succeeded())
        return;

    JPH::RVec3 currentPosition = lock.GetBody().GetPosition();

    auto activation = teleport ? JPH::EActivation::Activate : JPH::EActivation::DontActivate;
    _world->internalHandle().GetBodyInterface().SetPositionAndRotation(_bodyID, currentPosition, jphutil::cast(rot),
                                                                       activation);
}

Quaternion Rigidbody3D::getRotation() const
{
    if (!isAttached())
        return Quaternion::identity();

    return jphutil::cast(_world->internalHandle().GetBodyInterface().GetRotation(_bodyID));
}

Mat4 Rigidbody3D::getCenterOfMassTransform() const
{
    Mat4 transform = Mat4::IDENTITY;
    if (!isAttached())
        return transform;
    transform = jphutil::cast(_world->internalHandle().GetBodyInterface().GetCenterOfMassTransform(_bodyID));
    return transform;
}

Mat4 Rigidbody3D::getWorldTransform() const
{
    Mat4 transform = Mat4::IDENTITY;
    withBodyReadLock(this, [&](const JPH::Body& body) { transform = jphutil::cast(body.GetWorldTransform()); });
    return transform;
}

void Rigidbody3D::applyForce(const Vec3& force, const Vec3& offset)
{
    if (!isAttached())
        return;

    const Vec3 point = extractTranslation(getCenterOfMassTransform()) + offset;
    _world->internalHandle().GetBodyInterface().AddForce(_bodyID, jphutil::cast(force), jphutil::cast(point));
}

void Rigidbody3D::applyTorque(const Vec3& torque)
{
    if (!isAttached())
        return;
    _world->internalHandle().GetBodyInterface().AddTorque(_bodyID, jphutil::cast(torque));
}

void Rigidbody3D::applyAngularImpulse(const Vec3& impulse)
{
    if (!isAttached())
        return;
    _world->internalHandle().GetBodyInterface().AddAngularImpulse(_bodyID, jphutil::cast(impulse));
}

void Rigidbody3D::applyImpulse(const Vec3& impulse, const Vec3& offset)
{
    if (!isAttached())
        return;

    const Vec3 point = extractTranslation(getCenterOfMassTransform()) + offset;
    _world->internalHandle().GetBodyInterface().AddImpulse(_bodyID, jphutil::cast(impulse), jphutil::cast(point));
}

void Rigidbody3D::setLinearVelocity(const Vec3& linearVelocity)
{
    _linearVelocity = linearVelocity;
    if (!isAttached())
        return;

    _world->internalHandle().GetBodyInterface().SetLinearVelocity(_bodyID, jphutil::cast(linearVelocity));
}

Vec3 Rigidbody3D::getLinearVelocity() const
{
    if (!isAttached())
        return Vec3::ZERO;
    return jphutil::cast(_world->internalHandle().GetBodyInterface().GetLinearVelocity(_bodyID));
}

void Rigidbody3D::setAngularVelocity(const Vec3& velocity)
{
    _angularVelocity = velocity;
    if (!isAttached())
        return;

    _world->internalHandle().GetBodyInterface().SetAngularVelocity(_bodyID, jphutil::cast(velocity));
}

Vec3 Rigidbody3D::getAngularVelocity() const
{
    if (!isAttached())
        return _angularVelocity;
    return jphutil::cast(_world->internalHandle().GetBodyInterface().GetAngularVelocity(_bodyID));
}

void Rigidbody3D::setLinearDamping(float damping)
{
    _linearDamping = damping;
    if (!isAttached())
        return;

    withBodyWriteLock(this, [damping](JPH::Body& body) {
        if (auto motion = body.GetMotionPropertiesUnchecked())
            motion->SetLinearDamping(damping);
    });
}

float Rigidbody3D::getLinearDamping() const
{
    float damping = _linearDamping;
    withBodyReadLock(this, [&](const JPH::Body& body) {
        if (const auto motion = body.GetMotionPropertiesUnchecked())
            damping = motion->GetLinearDamping();
    });
    return damping;
}

void Rigidbody3D::setAngularDamping(float damping)
{
    _angularDamping = damping;
    if (!isAttached())
        return;

    withBodyWriteLock(this, [damping](JPH::Body& body) {
        if (auto motion = body.GetMotionPropertiesUnchecked())
            motion->SetAngularDamping(damping);
    });
}

float Rigidbody3D::getAngularDamping() const
{
    float damping = _angularDamping;
    withBodyReadLock(this, [&](const JPH::Body& body) {
        if (const auto motion = body.GetMotionPropertiesUnchecked())
            damping = motion->GetAngularDamping();
    });
    return damping;
}

Vec3 Rigidbody3D::world2Local(const Vec3& point)
{
    if (isAttached())
    {
        Mat4 transform = getCenterOfMassTransform();
        transform.inverse();
        Vec4 result = transform * Vec4(point.x, point.y, point.z, 1.0f);
        return Vec3(result.x, result.y, result.z);
    }
    else if (_owner)
    {
        // Compute body world transform from node transform and physics offset
        Mat4 nodeWorld = _owner->getNodeToWorldTransform();
        jphutil::stripScaleFromTransform(nodeWorld);  // Remove scaling
        Mat4 bodyWorld = nodeWorld * _invTransformInPhysics;
        bodyWorld.inverse();
        Vec4 result = bodyWorld * Vec4(point.x, point.y, point.z, 1.0f);
        return Vec3(result.x, result.y, result.z);
    }
    return Vec3::ZERO;
}

Vec3 Rigidbody3D::local2World(const Vec3& point)
{
    if (isAttached())
    {
        Mat4 transform = getCenterOfMassTransform();
        Vec4 result    = transform * Vec4(point.x, point.y, point.z, 1.0f);
        return Vec3(result.x, result.y, result.z);
    }
    else if (_owner)
    {
        Mat4 nodeWorld = _owner->getNodeToWorldTransform();
        jphutil::stripScaleFromTransform(nodeWorld);
        Mat4 bodyWorld = nodeWorld * _invTransformInPhysics;
        Vec4 result    = bodyWorld * Vec4(point.x, point.y, point.z, 1.0f);
        return Vec3(result.x, result.y, result.z);
    }
    return Vec3::ZERO;
}

void Rigidbody3D::setInertia(const Vec3& inertia)
{
    _inertia = inertia;
    setMassData(_mass, _inertia);
}

Vec3 Rigidbody3D::getInertia() const
{
    if (!isAttached())
        return _inertia;

    Vec3 inverseInertia;
    withBodyReadLock(this, [&inverseInertia](const JPH::Body& body) {
        if (auto motion = body.GetMotionPropertiesUnchecked())
            inverseInertia = jphutil::cast(motion->GetInverseInertiaDiagonal());
    });

    return Vec3{(inverseInertia.x != 0.0f) ? 1.0f / inverseInertia.x : 0.0f,
                (inverseInertia.y != 0.0f) ? 1.0f / inverseInertia.y : 0.0f,
                (inverseInertia.z != 0.0f) ? 1.0f / inverseInertia.z : 0.0f};
}

void Rigidbody3D::setMass(float mass)
{
    setMassData(mass, _inertia);
}

float Rigidbody3D::getMass() const
{
    if (!isAttached())
        return _mass;

    float invMass = 0.0f;
    withBodyReadLock(this, [&](const JPH::Body& body) {
        if (const auto motion = body.GetMotionPropertiesUnchecked())
            invMass = motion->GetInverseMassUnchecked();
    });
    return invMass > 0 ? 1 / invMass : 0.0f;
}

void Rigidbody3D::setMassData(float mass, const Vec3& inertia)
{

    _mass    = mass;
    _inertia = inertia;

    clampMassData(_mass, _inertia);

    if (!isAttached())
        return;
    if (_mass < phconsts::LowerBound || !validateInertia(_inertia))
    {
        AXLOGW("setMassData failed, invalid ");
        return;
    }
    withBodyWriteLock(this, [this](JPH::Body& body) {
        if (auto motion = body.GetMotionPropertiesUnchecked())
        {
            JPH::MassProperties mp;
            mp.mMass    = _mass;
            mp.mInertia = JPH::Mat44::sScale(jphutil::cast(_inertia));
            motion->SetMassProperties(motion->GetAllowedDOFs(), mp);
        }
    });
}

Vec3 Rigidbody3D::getTotalForce() const
{
    Vec3 total = Vec3::ZERO;
    withBodyReadLock(this, [&](const JPH::Body& body) {
        if (!body.IsStatic())
            total = jphutil::cast(body.GetAccumulatedForce());
    });
    return total;
}

Vec3 Rigidbody3D::getTotalTorque() const
{
    Vec3 total = Vec3::ZERO;
    withBodyReadLock(this, [&](const JPH::Body& body) {
        if (!body.IsStatic())
            total = jphutil::cast(body.GetAccumulatedTorque());
    });
    return total;
}

void Rigidbody3D::setMaterial(const PhysicsMaterial& mat)
{
    if (_collider)
        _collider->setMaterial(mat);
}

void Rigidbody3D::setCollisionDetectionMode(CollisionDetectionMode mode)
{
    _collisionDetection = mode;
    if (!isAttached())
        return;

    _world->internalHandle().GetBodyInterface().SetMotionQuality(_bodyID, toJoltMotionQuality(mode));
}

void Rigidbody3D::setMotionType(MotionType motionType)
{
    // rigidbody with empty shape not allow modify motionType
    if (!_collider)
        return;

    // rigidbody with zero mass not allow modify motion Type when motionType is Static
    if (_motionType == MotionType::Static && _mass <= 0.0f)
        return;

    if (_motionType == motionType)
        return;

    _motionType = motionType;

    if (!isAttached())
        return;

    _world->internalHandle().GetBodyInterface().SetMotionType(_bodyID, (JPH::EMotionType)_motionType,
                                                              JPH::EActivation::Activate);
    bool kinematic = _motionType == MotionType::Kinematic;
    withBodyWriteLock(this, [kinematic](JPH::Body& body) { body.SetCollideKinematicVsNonDynamic(kinematic); });
}

void Rigidbody3D::setAllowSleeping(bool allow)
{
    _allowSleeping = allow;

    if (!isAttached())
        return;

    withBodyWriteLock(this, [allow](JPH::Body& body) { body.SetAllowSleeping(allow); });
    if (!allow)
        _world->internalHandle().GetBodyInterface().ActivateBody(_bodyID);
}

void Rigidbody3D::setGravityScale(float scale)
{
    _gravityScale = scale;

    if (!isAttached())
        return;
    _world->internalHandle().GetBodyInterface().SetGravityFactor(_bodyID, scale);
}

void Rigidbody3D::setActive(bool active)
{
    if (!_collider || _motionType == MotionType::Static)
        return;

    if (!isAttached())
        return;

    if (active)
        _world->internalHandle().GetBodyInterface().ActivateBody(_bodyID);
    else
        _world->internalHandle().GetBodyInterface().DeactivateBody(_bodyID);
}

void Rigidbody3D::setRotationEnabled(bvec3 axes)
{
    _allowedDOFs = static_cast<uint32_t>(JPH::EAllowedDOFs::All);

    if (!axes.x)
        _allowedDOFs &= ~static_cast<uint32_t>(JPH::EAllowedDOFs::RotationX);
    if (!axes.y)
        _allowedDOFs &= ~static_cast<uint32_t>(JPH::EAllowedDOFs::RotationY);
    if (!axes.z)
        _allowedDOFs &= ~static_cast<uint32_t>(JPH::EAllowedDOFs::RotationZ);

    if (!isAttached())
        return;

    withBodyWriteLock(this, [this](JPH::Body& body) {
        auto motion = body.GetMotionPropertiesUnchecked();
        if (!motion)
            return;

        JPH::MassProperties massProperties;
        if (!tryBuildMassProperties(_collider, _mass, _inertia, massProperties))
            return;

        motion->SetMassProperties(static_cast<JPH::EAllowedDOFs>(_allowedDOFs), massProperties);
    });
}

bool Rigidbody3D::isRotationEnabled(int axis) const
{
    switch (axis)
    {
    case 0:  // X
        return (_allowedDOFs & static_cast<uint32_t>(JPH::EAllowedDOFs::RotationX)) != 0;
    case 1:  // Y
        return (_allowedDOFs & static_cast<uint32_t>(JPH::EAllowedDOFs::RotationY)) != 0;
    case 2:  // Z
        return (_allowedDOFs & static_cast<uint32_t>(JPH::EAllowedDOFs::RotationZ)) != 0;
    default:
        return false;
    }
}

void Rigidbody3D::addJoint(Joint3D* joint)
{
    if (!joint || joint->isBodyAttached(this))
        return;

    _joints.push_back(joint);
}

void Rigidbody3D::removeJoint(Joint3D* joint)
{
    if (!joint || !joint->isBodyAttached(this))
        return;

    auto it = std::find(_joints.begin(), _joints.end(), joint);
    if (it != _joints.end())
    {
        eraseUnordered(_joints, it);
    }
}

}  // namespace ax

#endif
