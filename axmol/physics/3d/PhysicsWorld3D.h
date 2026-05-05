/****************************************************************************
 Copyright (c) 2015-2016 Chukong Technologies Inc.
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

#pragma once

#include "axmol/math/Math.h"
#include "axmol/base/Object.h"
#include "axmol/base/Config.h"
#include "axmol/tlx/flat_set.hpp"
#include "axmol/physics/3d/ContactEvent3D.h"

#include <cstdint>
#include <vector>

#if defined(AX_ENABLE_PHYSICS_3D)

#    include <Jolt/Jolt.h>
#    include <Jolt/Core/JobSystemThreadPool.h>
#    include <Jolt/Core/TempAllocator.h>
#    include <Jolt/Physics/Body/BodyID.h>
#    include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#    include <Jolt/Physics/Collision/ObjectLayer.h>
#    include <Jolt/Physics/Collision/Shape/Shape.h>
#    include <Jolt/Physics/Collision/Shape/SubShapeIDPair.h>
#    include <Jolt/Physics/Collision/ContactListener.h>
#    include <Jolt/Physics/Constraints/TwoBodyConstraint.h>
#    include <Jolt/Physics/PhysicsSystem.h>

namespace ax
{
namespace detail
{
inline constexpr JPH::ObjectLayer kStaticLayer  = 0;
inline constexpr JPH::ObjectLayer kDynamicLayer = 1;
inline constexpr JPH::ObjectLayer kSensorLayer  = 2;
inline constexpr JPH::ObjectLayer kNumLayers    = 3;

inline constexpr JPH::BroadPhaseLayer kBroadPhaseStatic{0};
inline constexpr JPH::BroadPhaseLayer kBroadPhaseDynamic{1};
inline constexpr JPH::BroadPhaseLayer kBroadPhaseSensor{2};
inline constexpr uint32_t kNumBroadPhaseLayers = 3;

struct QueuedContactEvent3D
{
    ContactInfo3D info;
    ContactEvent3D::EventCode code{ContactEvent3D::EventCode::None};
};
}  // namespace detail

/**
 * @addtogroup _3d
 * @{
 */

class PhysicsDebugDraw3D;
class Rigidbody3D;
class Collider3D;
class Joint3D;
class Component;
class Renderer;
class Scene;

/**
 * @brief The physics information container for Axmol 3D physics components.
 */
class AX_DLL PhysicsWorld3D : public Object,
                              public JPH::ContactListener,
                              public JPH::ObjectVsBroadPhaseLayerFilter,
                              public JPH::ObjectLayerPairFilter,
                              public JPH::BroadPhaseLayerInterface
{
    friend class Rigidbody3D;
    friend class Collider3D;
    friend class Joint3D;

public:
    using PreSolveCallback = std::function<bool(const ContactInfo3D&)>;

    /**
     * @brief Result data returned by ray casts and sweep tests.
     */
    struct HitResult
    {
        ax::Vec3 hitPosition;             ///< World-space hit position.
        ax::Vec3 hitNormal;               ///< World-space hit normal.
        PhysicsActor* hitActor{nullptr};  ///< Physics actor hit by the query.

        Rigidbody3D* rigidbody{nullptr};  ///< Hit rigid body, or `nullptr` when the actor is not a rigid body.
        Collider3D* collider{nullptr};    ///< Hit collider, or `nullptr` when the actor is not a collider.
    };

    /**
     * @brief Obtains the 3D physics world associated with a scene.
     * @param scene Scene that owns the physics world.
     * @return An autoreleased PhysicsWorld3D object.
     */
    static PhysicsWorld3D* obtain(Scene* scene);

    /**
     * @brief Releases a world obtained through obtain().
     * @param world Physics world to release. Passing `nullptr` is allowed.
     */
    static void release(PhysicsWorld3D* world);

    /**
     * @brief Returns the native Jolt physics system.
     * @return Mutable native physics system reference.
     */
    JPH::PhysicsSystem& internalHandle() { return _physicsSystem; }

    /**
     * @brief Returns the native Jolt physics system.
     * @return Const native physics system reference.
     */
    const JPH::PhysicsSystem& internalHandle() const { return _physicsSystem; }

    /**
     * @brief Returns the scene that owns this physics world.
     * @return Owning scene, or `nullptr` before initialization.
     */
    Scene* getScene() const { return _scene; }

    /**
     * @brief Set the PreSolve callback for the 3D physics world.
     *
     * The PreSolve callback is invoked during JoltPhysics contact
     * validation (OnContactValidate), allowing the user to decide
     * whether a contact should be accepted or rejected. The callback
     * runs inside the JobSystem worker threads and must return
     * immediately.
     *
     * @param cb User-provided callback function with signature:
     *        `bool(const ContactInfo3D&)`
     *        - Return true to accept the contact.
     *        - Return false to reject the contact.
     *
     * @warning PreSolve callbacks execute in a multithreaded context:
     *          - Do not access non-thread-safe shared data.
     *          - Do not block or wait on the main thread.
     *          - Execution order of multiple callbacks is not guaranteed,
     *            which may lead to non-deterministic behavior.
     *
     * @note If no callback is set, all contacts are accepted by default.
     */
    void setPreSolveCallback(PreSolveCallback cb);

    /**
     * @brief Sets gravity for the physics world.
     * @param gravity Gravity vector in world units per second squared.
     */
    void setGravity(const Vec3& gravity);

    /**
     * @brief Gets the current world gravity.
     * @return Gravity vector in world units per second squared.
     */
    Vec3 getGravity() const;

    /**
     * @brief Advances the physics simulation by one step.
     * @param dt Step duration in seconds.
     */
    void stepSimulate(float dt);

    /**
     * @brief Enables or disables debug drawing for this world.
     * @param enableDebugDraw True to enable debug rendering.
     */
    void setDebugDrawEnabled(bool enableDebugDraw);

    /**
     * @brief Returns whether debug drawing is enabled.
     * @return True when debug drawing is enabled.
     */
    bool isDebugDrawEnabled() const;

    /**
     * @brief Renders debug physics geometry.
     * @param renderer Renderer used by the owning scene.
     * @note This is called automatically by Scene.
     */
    [[internal]] void debugDraw(ax::Renderer* renderer);

    /**
     * @brief Casts a ray through the physics world.
     * @param startPos The start position of ray.
     * @param endPos The end position of ray.
     * @param result Output hit result, or `nullptr` to only test for any hit.
     * @return True if the ray intersects a physics actor.
     */
    bool rayCast(const ax::Vec3& startPos, const ax::Vec3& endPos, HitResult* result);

    /**
     * @brief Performs a swept shape cast on all objects in the PhysicsWorld3D.
     * @param collider Collider shape to sweep.
     * @param startTransform World transform at the start of the sweep.
     * @param endTransform World transform at the end of the sweep.
     * @param result Output hit result, or `nullptr` to only test for any hit.
     * @return True if the swept collider intersects a physics actor.
     */
    bool sweepTest(Collider3D* collider,
                   const ax::Mat4& startTransform,
                   const ax::Mat4& endTransform,
                   HitResult* result);

    /** @brief Constructs an uninitialized 3D physics world. */
    PhysicsWorld3D();

    /** @brief Destroys the world and releases native physics resources. */
    ~PhysicsWorld3D() override;

    /**
     * @brief Enable or disable specific collision events globally in this physics world.
     *
     * This modifies the world's global event mask by turning on or off the specified event bits.
     * Colliders can only generate these events if they are enabled both locally and globally.
     *
     * @note This setting should be applied before any rigid-body components are added to nodes,
     *       because global event state cannot be changed at runtime once rigidbodies are active.
     *       By default, all global collision events are disabled (mask = 0).
     *
     * @param events Collision event bits to modify.
     * @param enabled True to enable the specified events globally, false to disable them.
     */
    void setGlobalEventEnabled(ContactEventBits events, bool enabled);

    /**
     * @brief Returns whether all requested collision event bits are globally enabled.
     * @param events Collision event bits to test.
     * @return True when the specified event bits are enabled globally.
     */
    bool isGlobalEventEnabled(ContactEventBits events) const;

    void addPhysicsActor(PhysicsActor* actor);
    void removePhysicsActor(PhysicsActor* actor);

protected:
    void init(Scene* scene);

    ContactEventBits getGlobalEventBits() const { return _eventBits; }

    void dispatchQueuedContactEvent(detail::QueuedContactEvent3D& queued);
    void dispatchQueuedEvents();

    void removePhysicsActorInternal(PhysicsActor* actor);

    // Helper methods
    bool checkSensorEvent(const ContactInfo3D& info) const;
    void queueContactEvent(ContactInfo3D&& info, ContactEvent3D::EventCode code);

#    pragma region JPH::ContactListener
    JPH::ValidateResult OnContactValidate(const JPH::Body& body1,
                                          const JPH::Body& body2,
                                          JPH::RVec3Arg baseOffset,
                                          const JPH::CollideShapeResult& collisionResult) override;

    void OnContactAdded(const JPH::Body& body1,
                        const JPH::Body& body2,
                        const JPH::ContactManifold& manifold,
                        JPH::ContactSettings& settings) override;

    void OnContactPersisted(const JPH::Body& body1,
                            const JPH::Body& body2,
                            const JPH::ContactManifold& manifold,
                            JPH::ContactSettings& settings) override;

    void OnContactRemoved(const JPH::SubShapeIDPair& subShapePair) override;
#    pragma endregion

    // JPH::ObjectLayerPairFilter
    bool ShouldCollide(JPH::ObjectLayer layer1, JPH::ObjectLayer layer2) const override;

    // JPH::ObjectVsBroadPhaseLayerFilter
    bool ShouldCollide(JPH::ObjectLayer layer1, JPH::BroadPhaseLayer layer2) const override;

#    pragma region BroadPhaseLayerInterface
    uint32_t GetNumBroadPhaseLayers() const override { return detail::kNumBroadPhaseLayers; }

    JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer layer) const override;

#    if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer layer) const override;
#    endif
#    pragma endregion

    Scene* _scene{nullptr};

    JPH::PhysicsSystem _physicsSystem;
    JPH::JobSystemThreadPool _jobSystem;
    JPH::TempAllocatorImplWithMallocFallback _tempAllocator;
    std::unordered_map<uint64_t, ContactInfo3D> _pairContacts;
    std::vector<detail::QueuedContactEvent3D> _queuedEvents;
    std::mutex _eventMutex;

    PreSolveCallback _preSolveCallback;

    tlx::flat_set<PhysicsActor*> _physicsActors;
    ContactEventBits _eventBits{ContactEventBits::None};
    PhysicsDebugDraw3D* _debugDrawer;
};

// end of 3d group
/// @}
}  // namespace ax

#endif  // defined(AX_ENABLE_PHYSICS_3D)
