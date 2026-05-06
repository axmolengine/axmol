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
#include "axmol/physics/3d/Collider3D.h"

#if defined(AX_ENABLE_PHYSICS_3D)

#    include "axmol/physics/3d/PhysicsUtility3D.h"
#    include "axmol/physics/3d/PhysicsWorld3D.h"
#    include "axmol/scene/Node.h"
#    include "axmol/scene/Scene.h"

#    include <Jolt/Geometry/IndexedTriangle.h>
#    include <Jolt/Geometry/Triangle.h>
#    include <Jolt/Physics/Body/BodyCreationSettings.h>
#    include <Jolt/Physics/Collision/Shape/BoxShape.h>
#    include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#    include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#    include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#    include <Jolt/Physics/Collision/Shape/HeightFieldShape.h>
#    include <Jolt/Physics/Collision/Shape/MeshShape.h>
#    include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#    include <Jolt/Physics/Collision/Shape/SphereShape.h>
#    include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>

#    include <cstring>

namespace ax
{
namespace
{
bool decomposeTransform(const Mat4& transform, Vec3& translation, Quaternion& rotation)
{
    Vec3 scale;
    transform.decompose(&scale, &rotation, &translation);
    rotation.normalize();
    return true;
}

JPH::Ref<JPH::Shape> createMeshShapeFromTriangles(const std::vector<Vec3>& inputTriangles)
{
    JPH::TriangleList triangles;
    triangles.reserve(inputTriangles.size() / 3);
    for (size_t i = 0; i + 2 < inputTriangles.size(); i += 3)
    {
        triangles.emplace_back(jphutil::cast(inputTriangles[i]), jphutil::cast(inputTriangles[i + 1]),
                               jphutil::cast(inputTriangles[i + 2]));
    }

    JPH::MeshShapeSettings settings(triangles);
    auto result = settings.Create();
    return result.IsValid() ? result.Get().GetPtr() : nullptr;
}
}  // namespace

Collider3D::Collider3D(Type type) : _type(type), _world(nullptr), _userData(nullptr)
{
    _actorType = PhysicsActor::kCollider;
    _transformInPhysics.setIdentity();
    _invTransformInPhysics.setIdentity();
}

Collider3D::~Collider3D()
{
    detachFromWorld();
}

bool Collider3D::init(const PhysicsMaterial& mat)
{
    _shape = createShape();
    if (!_shape)
        return false;
    _shape->SetUserData(std::bit_cast<uintptr_t>(this));

    _material = mat;

    setSensor(false);
    setMaterial(mat);
    setTransformInPhysics(Vec3::ZERO, Quaternion::identity());
    return true;
}

void Collider3D::setEnabled(bool enabled)
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

void Collider3D::onEnter()
{
    Component::onEnter();
    if (_enabled)
        attachToWorld();
}

void Collider3D::onExit()
{
    Component::onExit();
    detachFromWorld();
}

void Collider3D::setAttachedBody(Rigidbody3D* body)
{
    _attachedBody = body;
    if (body)
        _world = body->getWorld();
    else if (_bodyID.IsInvalid())
        _world = nullptr;
}

bool Collider3D::attachToWorld()
{
    if (isAttached())
        return true;

    auto world = _owner->getScene()->getPhysicsWorld3D();
    if (!world)
        return false;

    if (!_shape)
        return false;

    if (_bodyID.IsInvalid())
    {  // create owned static rigidbody
        auto worldTransform = _owner->getNodeToWorldTransform();
        jphutil::stripScaleFromTransform(worldTransform);
        worldTransform *= _invTransformInPhysics;
        Vec3 translation;
        Quaternion rotation;
        decomposeTransform(worldTransform, translation, rotation);

        const auto layer = _sensor ? detail::kSensorLayer : detail::kStaticLayer;
        JPH::BodyCreationSettings settings(_shape.GetPtr(), jphutil::cast(translation), jphutil::cast(rotation),
                                           JPH::EMotionType::Static, layer);
        settings.mUserData    = static_cast<uint64_t>(std::bit_cast<uintptr_t>(this));
        settings.mFriction    = _material.friction;
        settings.mRestitution = _material.restitution;
        settings.mIsSensor    = _sensor;
        // Static Collider don't require Continuous Collision Detection
        settings.mMotionQuality = JPH::EMotionQuality::Discrete;

        auto& bodyInterface = world->internalHandle().GetBodyInterface();
        _bodyID             = bodyInterface.CreateAndAddBody(settings, JPH::EActivation::DontActivate);
        if (_bodyID.IsInvalid())
            return false;
    }

    world->addPhysicsActor(this);
    _world = world;
    return true;
}

void Collider3D::detachFromWorld()
{
    if (isAttachedToWorld())
        _world->removePhysicsActor(this);
}

void Collider3D::invalidate()
{
    if (isAttachedToWorld())
    {
        _world->internalHandle().GetBodyInterface().DestroyBody(_bodyID);
        _world        = nullptr;
        _attachedBody = nullptr;
    }

    _world        = nullptr;
    _attachedBody = nullptr;
}

void Collider3D::setTransformInPhysics(const Vec3& translateInPhysics, const Quaternion& rotInPhysics)
{
    Mat4::createRotation(rotInPhysics, &_transformInPhysics);
    _transformInPhysics.m[12] = translateInPhysics.x;
    _transformInPhysics.m[13] = translateInPhysics.y;
    _transformInPhysics.m[14] = translateInPhysics.z;
    _invTransformInPhysics    = _transformInPhysics.getInversed();
}

void Collider3D::syncPhysicsToNode()
{
    if (!_owner)
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

void Collider3D::syncNodeToPhysics()
{
    if (!_owner || !_world)
        return;

    if (_attachedBody)
    {
        // Collider is shape-only; the owning rigidbody controls its own transform.
        return;
    }

    if (_bodyID.IsInvalid())
        return;

    auto worldTransform = _owner->getNodeToWorldTransform();
    jphutil::stripScaleFromTransform(worldTransform);
    worldTransform *= _invTransformInPhysics;

    Vec3 scale;
    Vec3 translation;
    Quaternion quat;
    worldTransform.decompose(&scale, &quat, &translation);
    quat.normalize();
    _world->internalHandle().GetBodyInterface().SetPositionAndRotation(
        _bodyID, jphutil::cast(translation), jphutil::cast(quat), JPH::EActivation::DontActivate);
}

Mat4 Collider3D::getWorldTransform() const
{
    if (_attachedBody)
        return _attachedBody->getWorldTransform();

    if (!_world || _bodyID.IsInvalid())
        return Mat4::IDENTITY;

    return jphutil::cast(_world->internalHandle().GetBodyInterface().GetWorldTransform(_bodyID));
}

void Collider3D::setSensor(bool sensor)
{
    _sensor = sensor;

    if (!_world)
        return;

    if (_attachedBody)
    {
        if (_attachedBody->isAttached())
            _world->internalHandle().GetBodyInterface().SetIsSensor(_attachedBody->internalHandle(), sensor);
    }
    else if (isAttachedToWorld())
    {
        _world->internalHandle().GetBodyInterface().SetIsSensor(_bodyID, sensor);
    }
}

void Collider3D::setMaterial(const PhysicsMaterial& mat)
{
    _material = mat;

    applyMaterial();
}

void Collider3D::applyMaterial()
{
    JPH::BodyID bodyID = _attachedBody ? _attachedBody->internalHandle() : _bodyID;

    if (bodyID.IsInvalid() || !_world)
        return;
    auto& bodyInterface = _world->internalHandle().GetBodyInterface();
    bodyInterface.SetRestitution(bodyID, _material.restitution);
    bodyInterface.SetFriction(bodyID, _material.friction);
}

BoxCollider3D* BoxCollider3D::create(const Vec3& extent, const PhysicsMaterial& mat)
{
    auto obj = new BoxCollider3D();
    if (obj->init(extent, mat))
    {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

bool BoxCollider3D::init(const Vec3& extent, const PhysicsMaterial& mat)
{
    _extent = extent;
    return Collider3D::init(mat);
}

JPH::Ref<JPH::Shape> BoxCollider3D::createShape() const
{
    return new JPH::BoxShape(jphutil::cast(_extent * 0.5f));
}

SphereCollider3D* SphereCollider3D::create(float radius, const PhysicsMaterial& mat)
{
    auto obj = new SphereCollider3D();
    if (obj->init(radius, mat))
    {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

SphereCollider3D::SphereCollider3D() : Collider3D(Type::SPHERE)
{
    setName("SphereCollider3D"sv);
}

bool SphereCollider3D::init(float radius, const PhysicsMaterial& mat)
{
    _radius = radius;
    return Collider3D::init(mat);
}

JPH::Ref<JPH::Shape> SphereCollider3D::createShape() const
{
    return new JPH::SphereShape(_radius);
}

CylinderCollider3D* CylinderCollider3D::create(float radius, float height, const PhysicsMaterial& mat)
{
    auto obj = new CylinderCollider3D();
    if (obj->init(radius, height, mat))
    {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

CylinderCollider3D::CylinderCollider3D() : Collider3D(Type::CYLINDER)
{
    setName("CylinderCollider3D"sv);
}

bool CylinderCollider3D::init(float radius, float height, const PhysicsMaterial& mat)
{
    _radius = radius;
    _height = height;
    return Collider3D::init(mat);
}

JPH::Ref<JPH::Shape> CylinderCollider3D::createShape() const
{
    return new JPH::CylinderShape(_height * 0.5f, _radius);
}

CapsuleCollider3D* CapsuleCollider3D::create(float radius, float height, const PhysicsMaterial& mat)
{
    auto obj = new CapsuleCollider3D();
    if (obj->init(radius, height, mat))
    {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

CapsuleCollider3D::CapsuleCollider3D() : Collider3D(Type::CAPSULE)
{
    setName("CapsuleCollider3D"sv);
}

bool CapsuleCollider3D::init(float radius, float height, const PhysicsMaterial& mat)
{
    _radius = radius;
    _height = height;
    return Collider3D::init(mat);
}

JPH::Ref<JPH::Shape> CapsuleCollider3D::createShape() const
{
    return new JPH::CapsuleShape(_height * 0.5f, _radius);
}

ConvexCollider3D* ConvexCollider3D::create(std::span<const Vec3> points, const PhysicsMaterial& mat)
{
    auto obj = new ConvexCollider3D();
    if (obj->init(points, mat))
    {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

ConvexCollider3D::ConvexCollider3D() : Collider3D(Type::CONVEX)
{
    setName("ConvexCollider3D"sv);
}

bool ConvexCollider3D::init(std::span<const Vec3> points, const PhysicsMaterial& mat)
{
    _points.assign(points.begin(), points.end());
    return Collider3D::init(mat);
}

JPH::Ref<JPH::Shape> ConvexCollider3D::createShape() const
{
    JPH::Array<JPH::Vec3> points;
    points.reserve(_points.size());
    for (const auto& point : _points)
        points.push_back(jphutil::cast(point));

    JPH::ConvexHullShapeSettings settings(points);
    auto result = settings.Create();
    return result.IsValid() ? result.Get() : nullptr;
}

MeshCollider3D* MeshCollider3D::create(std::span<const Vec3> triangles, const PhysicsMaterial& mat)
{
    auto obj = new MeshCollider3D();
    if (obj->init(triangles, mat))
    {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

MeshCollider3D::MeshCollider3D() : Collider3D(Type::MESH)
{
    setName("MeshCollider3D"sv);
}

bool MeshCollider3D::init(std::span<const Vec3> triangles, const PhysicsMaterial& mat)
{
    _triangles.assign(triangles.begin(), triangles.end());
    return Collider3D::init(mat);
}

JPH::Ref<JPH::Shape> MeshCollider3D::createShape() const
{
    return createMeshShapeFromTriangles(_triangles);
}

HeightFieldCollider3D* HeightFieldCollider3D::create(int heightStickWidth,
                                                     int heightStickLength,
                                                     std::span<const float> heightfieldData,
                                                     float heightScale,
                                                     float minHeight,
                                                     float maxHeight,
                                                     bool flipQuadEdges,
                                                     bool useDiamondSubdivision,
                                                     const PhysicsMaterial& mat)
{
    auto obj = new HeightFieldCollider3D();
    if (obj->init(heightStickWidth, heightStickLength, heightfieldData, heightScale, minHeight, maxHeight,
                  flipQuadEdges, useDiamondSubdivision, mat))
    {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

HeightFieldCollider3D::HeightFieldCollider3D() : Collider3D(Type::HEIGHT_FIELD)
{
    setName("HeightFieldCollider3D"sv);
}

bool HeightFieldCollider3D::init(int heightStickWidth,
                                 int heightStickLength,
                                 std::span<const float> heightfieldData,
                                 float heightScale,
                                 float minHeight,
                                 float maxHeight,
                                 bool flipQuadEdges,
                                 bool useDiamondSubdivision,
                                 const PhysicsMaterial& mat)
{
    _hfParams.width                 = heightStickWidth;
    _hfParams.length                = heightStickLength;
    _hfParams.heightScale           = heightScale;
    _hfParams.minHeight             = minHeight;
    _hfParams.maxHeight             = maxHeight;
    _hfParams.flipQuadEdges         = flipQuadEdges;
    _hfParams.useDiamondSubdivision = useDiamondSubdivision;

    _hfParams.data.assign(heightfieldData.begin(), heightfieldData.end());

    return Collider3D::init(mat);
}

JPH::Ref<JPH::Shape> HeightFieldCollider3D::createShape() const
{
    if (_hfParams.width == _hfParams.length)
    {
        JPH::Array<float> samples;
        samples.resize(static_cast<size_t>(_hfParams.width) * static_cast<size_t>(_hfParams.length));

        const auto& src = _hfParams.data;
        for (size_t i = 0; i < samples.size(); ++i)
            samples[i] = src[i] * _hfParams.heightScale;

        // Apply offset to center the heightfield in X/Z
        JPH::Vec3 offset(-0.5f * (_hfParams.width - 1), 0.0f, -0.5f * (_hfParams.length - 1));

        JPH::HeightFieldShapeSettings settings(samples.data(), offset, JPH::Vec3(1.0f, 1.0f, 1.0f), _hfParams.width);
        settings.mMinHeightValue = _hfParams.minHeight;
        settings.mMaxHeightValue = _hfParams.maxHeight;

        auto result = settings.Create();
        if (result.IsValid())
            return result.Get();
    }

    // fallback: build mesh triangles
    std::vector<Vec3> triangles;
    triangles.reserve(static_cast<size_t>(_hfParams.width - 1) * static_cast<size_t>(_hfParams.length - 1) * 6);

    auto sampleAt = [&](int x, int y) -> float {
        const size_t idx = static_cast<size_t>(y) * static_cast<size_t>(_hfParams.width) + static_cast<size_t>(x);
        return _hfParams.data[idx] * _hfParams.heightScale;
    };

    // normalize coordinates to center terrain
    const float invW = 1.0f / static_cast<float>(_hfParams.width - 1);
    const float invL = 1.0f / static_cast<float>(_hfParams.length - 1);

    for (int y = 0; y < _hfParams.length - 1; ++y)
    {
        for (int x = 0; x < _hfParams.width - 1; ++x)
        {
            // map (x,y) to [-0.5, +0.5] range
            float fx = (static_cast<float>(x) * invW - 0.5f) * (_hfParams.width - 1);
            float fz = (static_cast<float>(y) * invL - 0.5f) * (_hfParams.length - 1);

            float fx1 = ((x + 1) * invW - 0.5f) * (_hfParams.width - 1);
            float fz1 = ((y + 1) * invL - 0.5f) * (_hfParams.length - 1);

            const Vec3 p00(fx, sampleAt(x, y), fz);
            const Vec3 p10(fx1, sampleAt(x + 1, y), fz);
            const Vec3 p01(fx, sampleAt(x, y + 1), fz1);
            const Vec3 p11(fx1, sampleAt(x + 1, y + 1), fz1);

            if (_hfParams.flipQuadEdges)
                triangles.insert(triangles.end(), {p00, p10, p01, p10, p11, p01});
            else
                triangles.insert(triangles.end(), {p00, p10, p11, p00, p11, p01});
        }
    }

    return createMeshShapeFromTriangles(triangles);
}

CompoundCollider3D::CompoundCollider3D() : Collider3D(Type::COMPOUND)
{
    setName("CompoundCollider3D"sv);
}

CompoundCollider3D::~CompoundCollider3D()
{
    for (auto* child : _retainedChildren)
        child->release();
}

CompoundCollider3D* CompoundCollider3D::create(const std::vector<ChildInfo>& children, const PhysicsMaterial& mat)
{
    auto obj = new CompoundCollider3D();
    if (obj->init(children, mat))
    {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

bool CompoundCollider3D::init(const std::vector<ChildInfo>& children, const PhysicsMaterial& mat)
{
    _children = children;
    for (auto& info : _children)
    {
        info.first->retain();
        _retainedChildren.push_back(info.first);
    }
    return Collider3D::init(mat);
}

JPH::Ref<JPH::Shape> CompoundCollider3D::createShape() const
{
    JPH::StaticCompoundShapeSettings settings;
    for (const auto& info : _children)
    {
        auto collider = info.first;
        if (!collider->isValid())
            continue;

        Vec3 scale;
        Vec3 translation;
        Quaternion rotation;
        info.second.decompose(&scale, &rotation, &translation);
        rotation.normalize();
        settings.AddShape(jphutil::cast(translation), jphutil::cast(rotation), collider->internalShape().GetPtr());
    }

    auto result = settings.Create();
    return result.IsValid() ? result.Get() : nullptr;
}

}  // namespace ax

#endif
