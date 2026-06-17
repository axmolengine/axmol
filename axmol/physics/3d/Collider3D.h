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
#pragma once

#include "axmol/base/Config.h"
#include "axmol/math/Math.h"
#include "axmol/physics/PhysicsActor.h"

#include <cstdint>
#include <functional>
#include <vector>
#include <span>

#if defined(AX_ENABLE_PHYSICS_3D)
#    include <Jolt/Jolt.h>
#    include <Jolt/Core/Reference.h>
#    include <Jolt/Physics/Body/BodyID.h>
#    include <Jolt/Physics/Collision/Shape/Shape.h>

namespace JPH
{
class Shape;
}

namespace ax
{
class PhysicsWorld3D;
class Rigidbody3D;

struct PhysicsMaterial
{
    float friction{0.5f};     ///< Friction coefficient
    float restitution{0.0f};  ///< Restitution (bounciness)
};

/** @brief Default friction and restitution used by 3D colliders. */
inline constexpr PhysicsMaterial DEFAULT_PHYSICS_MATERIAL = PhysicsMaterial{};

/**
 * @brief Base class for 3D collider components.
 *
 * Provides common functionality and virtual shape creation.
 */
class AX_DLL Collider3D : public PhysicsActor
{
    friend class PhysicsWorld3D;
    friend class Rigidbody3D;

public:
    /**
     * @brief Collider shape category.
     */
    enum class Type
    {
        UNKNOWN = 0,   ///< Unknown or uninitialized shape.
        BOX,           ///< Box shape.
        SPHERE,        ///< Sphere shape.
        CYLINDER,      ///< Cylinder shape.
        CAPSULE,       ///< Capsule shape.
        CONVEX,        ///< Convex hull shape.
        MESH,          ///< Triangle mesh shape.
        HEIGHT_FIELD,  ///< Height-field terrain shape.
        COMPOUND       ///< Compound shape made from child colliders.
    };

    /** @brief Callback type used by trigger-style collider notifications. */
    using TriggerCallback = std::function<void(Component* other)>;

    /** @brief Destroys the collider and releases the native shape/body resources. */
    ~Collider3D() override;

    /**
     * @brief Enables or disables the collider component.
     * @param enabled True to attach or keep the collider active, false to detach it.
     */
    void setEnabled(bool enabled) override;

    /** @brief Returns whether the native collision shape has been created. */
    bool isValid() const { return _shape; }

    /** @brief Returns whether this collider owns a standalone native body. */
    bool hasOwnBody() const { return !_bodyID.IsInvalid(); }

    /** @brief Returns whether the collider is attached to either a world or rigid body. */
    bool isAttached() const { return _world || _attachedBody; }

    /** @brief Returns whether the collider is attached to a world. */
    bool isAttachedToWorld() const { return _world != nullptr && !_bodyID.IsInvalid(); }

    /** @brief Returns the physics world that owns this collider, or `nullptr` when detached. */
    PhysicsWorld3D* getWorld() const { return _world; }

    /** @brief Returns this collider's shape category. */
    Type getType() const { return _type; }

    /**
     * @brief Returns the native body ID for standalone colliders.
     * @return Native Jolt body ID, or an invalid ID when this collider is body-owned or detached.
     */
    [[internal]] JPH::BodyID internalBody() const { return _bodyID; }

    /**
     * @brief Returns the native Jolt shape reference.
     * @return Mutable native shape reference.
     */
    [[internal]] JPH::Ref<JPH::Shape>& internalShape() { return _shape; }

    /**
     * @brief Sets the relative transform from the node to the physics representation.
     * @param translateInPhysics Translation offset in local physics space.
     * @param rotInPhysics Rotation offset in local physics space.
     */
    void setTransformInPhysics(const Vec3& translateInPhysics, const Quat& rotInPhysics);

    /** @brief Synchronizes the owner node transform to the native physics body. */
    void syncNodeToPhysics() override;

    /** @brief Synchronizes the native physics transform back to the owner node. */
    void syncPhysicsToNode() override;

    /**
     * @brief Returns the collider's current world transform.
     * @return World transform including the physics transform offset.
     */
    Mat4 getWorldTransform() const override;

    /**
     * @brief Enables or disables sensor behavior.
     * @param bval True to make the collider report contacts without physical collision response.
     */
    void setSensor(bool bval);

    /** @brief Returns whether this collider is configured as a sensor. */
    bool isSensor() const { return _sensor; }

    /** @brief Returns the collider material settings. */
    const PhysicsMaterial& getMaterial() const { return _material; }

    /**
     * @brief Sets the collider material and applies it to the native shape/body when possible.
     * @param mat Material settings to use for this collider.
     */
    void setMaterial(const PhysicsMaterial& mat);

    /**
     * @brief Stores opaque caller-owned user data on this collider.
     * @param userData Pointer value to associate with the collider.
     */
    void setUserData(void* userData) { _userData = userData; }

    /** @brief Returns the opaque user data pointer assigned to this collider. */
    void* getUserData() const { return _userData; }

protected:
    Collider3D(Type type);

    void invalidate();

    /** @brief Attaches the collider when its owner node enters the active scene. */
    void onEnter() override;

    /** @brief Detaches the collider when its owner node exits the active scene. */
    void onExit() override;

    /**
     * @brief Sets this collider as the shape for a rigid body.
     * @param body Rigid body that should own this collider. Passing `nullptr` is ignored.
     */
    void setAttachedBody(Rigidbody3D* body);

    /**
     * @brief Attaches this collider directly to the physics world.
     * @return True if successfully attached.
     */
    bool attachToWorld();

    /** @brief Detaches this collider from the current physics world. */
    void detachFromWorld();

    void applyMaterial();

    /**
     * Initialize the collider with a descriptor and relative transform.
     * Subclasses must override createShape() to provide the native shape object.
     */
    virtual bool init(const PhysicsMaterial& mat);

    /** Create the native collision shape. Called once during init. */
    virtual JPH::Ref<JPH::Shape> createShape() const = 0;

    // Common data
    Type _type{Type::UNKNOWN};
    JPH::BodyID _bodyID;  // aka btGhostObject
    float _ccdMotionThreshold{0.0f};
    bool _sensor{false};
    Mat4 _transformInPhysics;
    Mat4 _invTransformInPhysics;

    JPH::Ref<JPH::Shape> _shape;

    Rigidbody3D* _attachedBody{nullptr};

    PhysicsMaterial _material;
    PhysicsWorld3D* _world;

    void* _userData;
};

// -------------------------------------------------------------------
// Concrete shape subclasses
// -------------------------------------------------------------------

class AX_DLL BoxCollider3D : public Collider3D
{
public:
    /**
     * @brief Creates an autoreleased box collider.
     * @param extent Half extents of the box along each local axis.
     * @param mat Material settings for the collider.
     * @return An autoreleased BoxCollider3D instance.
     */
    static BoxCollider3D* create(const Vec3& extent, const PhysicsMaterial& mat = DEFAULT_PHYSICS_MATERIAL);

protected:
    BoxCollider3D() : Collider3D(Type::BOX) {}
    bool init(const Vec3& extent, const PhysicsMaterial& mat = DEFAULT_PHYSICS_MATERIAL);

    JPH::Ref<JPH::Shape> createShape() const override;

private:
    Vec3 _extent;
};

class AX_DLL SphereCollider3D : public Collider3D
{
public:
    /**
     * @brief Creates an autoreleased sphere collider.
     * @param radius Sphere radius.
     * @param mat Material settings for the collider.
     * @return An autoreleased SphereCollider3D instance.
     */
    static SphereCollider3D* create(float radius, const PhysicsMaterial& mat = DEFAULT_PHYSICS_MATERIAL);

protected:
    SphereCollider3D();
    bool init(float radius, const PhysicsMaterial& mat = DEFAULT_PHYSICS_MATERIAL);

    JPH::Ref<JPH::Shape> createShape() const override;

private:
    float _radius{0.0f};
};

class AX_DLL CylinderCollider3D : public Collider3D
{
public:
    /**
     * @brief Creates an autoreleased cylinder collider.
     * @param radius Cylinder radius.
     * @param height Cylinder height.
     * @param mat Material settings for the collider.
     * @return An autoreleased CylinderCollider3D instance.
     */
    static CylinderCollider3D* create(float radius,
                                      float height,
                                      const PhysicsMaterial& mat = DEFAULT_PHYSICS_MATERIAL);

protected:
    CylinderCollider3D();
    bool init(float radius, float height, const PhysicsMaterial& mat = DEFAULT_PHYSICS_MATERIAL);

    JPH::Ref<JPH::Shape> createShape() const override;

private:
    float _radius;
    float _height;
};

class AX_DLL CapsuleCollider3D : public Collider3D
{
public:
    /**
     * @brief Creates an autoreleased capsule collider.
     * @param radius Capsule radius.
     * @param height Capsule cylindrical section height.
     * @param mat Material settings for the collider.
     * @return An autoreleased CapsuleCollider3D instance.
     */
    static CapsuleCollider3D* create(float radius, float height, const PhysicsMaterial& mat = DEFAULT_PHYSICS_MATERIAL);

protected:
    CapsuleCollider3D();
    bool init(float radius, float height, const PhysicsMaterial& mat = DEFAULT_PHYSICS_MATERIAL);

    JPH::Ref<JPH::Shape> createShape() const override;

private:
    float _radius;
    float _height;
};

class AX_DLL ConvexCollider3D : public Collider3D
{
public:
    /**
     * @brief Creates an autoreleased convex hull collider.
     * @param points Points used to build the convex hull.
     * @param mat Material settings for the collider.
     * @return An autoreleased ConvexCollider3D instance.
     */
    static ConvexCollider3D* create(std::span<const Vec3> points,
                                    const PhysicsMaterial& mat = DEFAULT_PHYSICS_MATERIAL);

protected:
    ConvexCollider3D();
    bool init(std::span<const Vec3> points, const PhysicsMaterial& mat = DEFAULT_PHYSICS_MATERIAL);

    JPH::Ref<JPH::Shape> createShape() const override;

private:
    std::vector<Vec3> _points;
};

class AX_DLL MeshCollider3D : public Collider3D
{
public:
    /**
     * @brief Creates an autoreleased triangle mesh collider.
     * @param triangles Triangle vertices, grouped as consecutive triples.
     * @param mat Material settings for the collider.
     * @return An autoreleased MeshCollider3D instance.
     */
    static MeshCollider3D* create(std::span<const Vec3> triangles,
                                  const PhysicsMaterial& mat = DEFAULT_PHYSICS_MATERIAL);

protected:
    MeshCollider3D();
    bool init(std::span<const Vec3> triangles, const PhysicsMaterial& mat = DEFAULT_PHYSICS_MATERIAL);

    JPH::Ref<JPH::Shape> createShape() const override;

private:
    std::vector<Vec3> _triangles;
};

class AX_DLL HeightFieldCollider3D : public Collider3D
{
public:
    /**
     * @brief Creates an autoreleased height-field collider.
     * @param heightStickWidth Number of samples along the height-field width.
     * @param heightStickLength Number of samples along the height-field length.
     * @param heightfieldData Height samples stored in row-major order.
     * @param heightScale Scale applied to each height sample.
     * @param minHeight Minimum expected height value.
     * @param maxHeight Maximum expected height value.
     * @param flipQuadEdges True to flip terrain quad diagonal edges.
     * @param useDiamondSubdivision True to use diamond subdivision for the height field.
     * @param mat Material settings for the collider.
     * @return An autoreleased HeightFieldCollider3D instance.
     */
    static HeightFieldCollider3D* create(int heightStickWidth,
                                         int heightStickLength,
                                         std::span<const float> heightfieldData,
                                         float heightScale,
                                         float minHeight,
                                         float maxHeight,
                                         bool flipQuadEdges,
                                         bool useDiamondSubdivision,
                                         const PhysicsMaterial& mat = DEFAULT_PHYSICS_MATERIAL);

protected:
    HeightFieldCollider3D();
    bool init(int heightStickWidth,
              int heightStickLength,
              std::span<const float> heightfieldData,
              float heightScale,
              float minHeight,
              float maxHeight,
              bool flipQuadEdges,
              bool useDiamondSubdivision,
              const PhysicsMaterial& mat = DEFAULT_PHYSICS_MATERIAL);

    JPH::Ref<JPH::Shape> createShape() const override;

private:
    struct HeightFieldParams
    {
        int width;
        int length;
        float heightScale;
        float minHeight;
        float maxHeight;
        bool flipQuadEdges;
        bool useDiamondSubdivision;
        std::vector<float> data;
    };
    HeightFieldParams _hfParams;
};

class AX_DLL CompoundCollider3D : public Collider3D
{
public:
    /** @brief Child collider and local transform pair used by CompoundCollider3D. */
    using ChildInfo = std::pair<Collider3D*, Mat4>;

    /**
     * @brief Creates an autoreleased compound collider from child colliders.
     * @param children Child colliders and transforms relative to the compound collider.
     * @param mat Material settings for the compound collider.
     * @return An autoreleased CompoundCollider3D instance.
     */
    static CompoundCollider3D* create(const std::vector<ChildInfo>& children,
                                      const PhysicsMaterial& mat = DEFAULT_PHYSICS_MATERIAL);

protected:
    CompoundCollider3D();
    ~CompoundCollider3D() override;

    bool init(const std::vector<ChildInfo>& children, const PhysicsMaterial& mat = DEFAULT_PHYSICS_MATERIAL);

    JPH::Ref<JPH::Shape> createShape() const override;

private:
    std::vector<ChildInfo> _children;
    std::vector<Collider3D*> _retainedChildren;  // For reference counting
};

}  // namespace ax

#endif  // defined(AX_ENABLE_PHYSICS_3D)
