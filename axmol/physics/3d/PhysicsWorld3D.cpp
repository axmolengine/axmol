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

#include "axmol/physics/3d/PhysicsWorld3D.h"

#if defined(AX_ENABLE_PHYSICS_3D)

#    include "axmol/physics/3d/Collider3D.h"
#    include "axmol/physics/3d/Joint3D.h"
#    include "axmol/physics/3d/PhysicsDebugDraw3D.h"
#    include "axmol/physics/3d/PhysicsUtility3D.h"
#    include "axmol/physics/3d/Rigidbody3D.h"
#    include "axmol/renderer/Renderer.h"
#    include "axmol/scene/Scene.h"

#    include <Jolt/Core/Factory.h>
#    include <Jolt/Core/IssueReporting.h>
#    include <Jolt/Core/Memory.h>
#    include <Jolt/Physics/Body/BodyCreationSettings.h>
#    include <Jolt/Physics/Body/BodyLock.h>
#    include <Jolt/Physics/Collision/CastResult.h>
#    include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#    include <Jolt/Physics/Collision/RayCast.h>
#    include <Jolt/Physics/Collision/Shape/EmptyShape.h>
#    include <Jolt/Physics/Collision/ShapeCast.h>
#    include <Jolt/RegisterTypes.h>

#    include <cstdarg>
#    include <iterator>
#    include <mutex>
#    include <utility>

namespace ax
{
namespace
{
void joltTrace(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buffer[1024];
#    if defined(_MSC_VER)
    vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, fmt, args);
#    else
    vsnprintf(buffer, sizeof(buffer), fmt, args);
#    endif
    va_end(args);
    AXLOGD("Jolt: {}", buffer);
}

#    ifdef JPH_ENABLE_ASSERTS
bool joltAssertFailed(const char* expression, const char* message, const char* file, uint32_t line)
{
    AXLOGE("Jolt assert failed: {} message={} at {}:{}", expression, message ? message : "", file, line);
    return false;
}
#    endif

void clearHitResult(PhysicsWorld3D::HitResult& result)
{
    result.hitPosition = Vec3::zero;
    result.hitNormal   = Vec3::zero;
    result.hitActor    = nullptr;
    result.rigidbody   = nullptr;
    result.collider    = nullptr;
}

static PhysicsActor* asActor(const JPH::Body& body)
{
    return std::bit_cast<PhysicsActor*>(static_cast<uintptr_t>(body.GetUserData()));
}

static Collider3D* asSensorCollider(PhysicsActor* actor)
{
    auto* collider =
        actor && actor->getActorType() == PhysicsActor::kCollider ? static_cast<Collider3D*>(actor) : nullptr;
    return collider && collider->isSensor() ? collider : nullptr;
}

static ContactInfo3D reorderSensorContactInfo(const ContactInfo3D& info)
{
    if (asSensorCollider(info.actorA) || !asSensorCollider(info.actorB))
        return info;

    ContactInfo3D reordered = info;
    std::swap(reordered.actorA, reordered.actorB);
    reordered.normal = -reordered.normal;
    return reordered;
}

static void fillContactPoints(ContactInfo3D& info, const JPH::ContactManifold& manifold)
{
    info.normal = jphutil::cast(manifold.mWorldSpaceNormal);
    info.points.clear();
    for (uint32_t i = 0; i < manifold.mRelativeContactPointsOn1.size(); ++i)
    {
        Vec3 worldA = jphutil::cast(manifold.GetWorldSpaceContactPointOn1(i));
        Vec3 worldB = jphutil::cast(manifold.GetWorldSpaceContactPointOn2(i));

        info.points.push_back(ContactInfo3D::ContactPoint{worldA, worldB});
    }
}

static uint64_t makeBodyPairKey(JPH::BodyID body1, JPH::BodyID body2)
{
    const uint32_t a = body1.GetIndexAndSequenceNumber();
    const uint32_t b = body2.GetIndexAndSequenceNumber();
    return a < b ? (uint64_t(a) << 32) | b : (uint64_t(b) << 32) | a;
}

static void fillHitResult(const JPH::Body& body,
                          const Vec3& worldPoint,
                          const Vec3& worldNormal,
                          PhysicsWorld3D::HitResult& result)
{
    clearHitResult(result);
    result.hitPosition = worldPoint;
    result.hitNormal   = worldNormal;
    result.hitActor    = asActor(body);
    if (!result.hitActor)
        return;

    if (result.hitActor->getActorType() == PhysicsActor::kRigidbody)
        result.rigidbody = static_cast<Rigidbody3D*>(result.hitActor);
    else
        result.collider = static_cast<Collider3D*>(result.hitActor);
}

// JoltContext
// File-local singleton that manages Jolt global initialization and termination.
// - Keeps mutex and refcount private to this translation unit.
// - Supports multiple PhysicsWorld3D instances: first Acquire() performs init,
//   last Release() performs term.
// - Does NOT expose a Guard type; callers call Acquire/Release explicitly.
class JoltContext
{
public:
    // Return the file-local singleton instance.
    static JoltContext& instance()
    {
        static JoltContext s_instance;
        return s_instance;
    }

    // Acquire a reference to the Jolt runtime.
    // If this is the first caller, perform global initialization.
    void acquire()
    {
        std::lock_guard<std::mutex> lk(_mtx);
        if (_refCount++ == 0)
            doInit();
    }

    // Release a reference to the Jolt runtime.
    // If this was the last caller, perform global termination.
    void release()
    {
        std::lock_guard<std::mutex> lk(_mtx);
        if (--_refCount == 0)
            doTerm();
    }

    // Deleted copy/move to keep singleton semantics.
    JoltContext(const JoltContext&)            = delete;
    JoltContext& operator=(const JoltContext&) = delete;
    JoltContext(JoltContext&&)                 = delete;
    JoltContext& operator=(JoltContext&&)      = delete;

private:
    JoltContext()  = default;
    ~JoltContext() = default;

    // Perform Jolt initialization. Marked noexcept to avoid throwing from destructor paths.
    void doInit() noexcept
    {
        // Register allocator and hooks
        JPH::RegisterDefaultAllocator();
        JPH::Trace = joltTrace;
#    ifdef JPH_ENABLE_ASSERTS
        JPH::AssertFailed = joltAssertFailed;
#    endif
        // Create factory and register types
        JPH::Factory::sInstance = new JPH::Factory();
        JPH::RegisterTypes();
    }

    // Perform Jolt termination. Marked noexcept to avoid throwing during cleanup.
    void doTerm() noexcept
    {
        JPH::UnregisterTypes();
        delete JPH::Factory::sInstance;
        JPH::Factory::sInstance = nullptr;
    }

    std::mutex _mtx;   // protects _refCount and init/term
    int _refCount{0};  // guarded by _mtx
};

}  // namespace

PhysicsWorld3D* PhysicsWorld3D::obtain(Scene* scene)
{
    JoltContext::instance().acquire();

    auto world = new PhysicsWorld3D();
    world->init(scene);
    return world;
}

void PhysicsWorld3D::release(PhysicsWorld3D* world)
{
    if (!world)
        return;
    auto referenceCount = world->getReferenceCount();
    world->Object::release();
    if (referenceCount == 1)
        JoltContext::instance().release();
}

PhysicsWorld3D::PhysicsWorld3D()
    : _tempAllocator(16 * 1024 * 1024)
    , _jobSystem(
          JPH::cMaxPhysicsJobs,
          JPH::cMaxPhysicsBarriers,
          std::clamp(std::thread::hardware_concurrency(), 1u, static_cast<unsigned int>(phconsts::PreferredWorkers3D)))
    , _debugDrawer(nullptr)
{}

PhysicsWorld3D::~PhysicsWorld3D()
{
    if (!_physicsActors.empty())
    {
        auto actors = std::exchange(_physicsActors, {});
        for (auto actor : actors)
        {
            removePhysicsActorInternal(actor);
        }
    }

    AX_SAFE_DELETE(_debugDrawer);
}

void PhysicsWorld3D::dispatchQueuedContactEvent(detail::QueuedContactEvent3D& queued)
{
    if (!_scene)
        return;

    auto* event = ContactEvent3D::obtain(queued.info);
    if (!event)
        return;

    event->setEventCode(queued.code);
    _scene->getEventDispatcher()->dispatchEvent(event);
    event->release();
}

void PhysicsWorld3D::dispatchQueuedEvents()
{
    if (!_scene)
        return;

    std::vector<detail::QueuedContactEvent3D> events;
    {
        std::scoped_lock lock(_eventMutex);
        if (_queuedEvents.empty())
            return;
        events.swap(_queuedEvents);
    }

    for (auto& queued : events)
    {
        dispatchQueuedContactEvent(queued);
    }
}

void PhysicsWorld3D::setPreSolveCallback(PreSolveCallback cb)
{
    _preSolveCallback = std::move(cb);
}

void PhysicsWorld3D::setGravity(const Vec3& gravity)
{
    _physicsSystem.SetGravity(jphutil::cast(gravity));
}

Vec3 PhysicsWorld3D::getGravity() const
{
    return jphutil::cast(_physicsSystem.GetGravity());
}

void PhysicsWorld3D::init(Scene* scene)
{
    _scene = scene;

    _physicsSystem.Init(65536, 0, 65536, 10240, *this, *this, *this);
    _physicsSystem.SetGravity(jphutil::cast(Vec3(0.0f, -9.8f, 0.0f)));
    _physicsSystem.SetContactListener(this);
}

void PhysicsWorld3D::setDebugDrawEnabled(bool enableDebugDraw)
{
    if (enableDebugDraw)
    {
        if (!_debugDrawer)
            _debugDrawer = new PhysicsDebugDraw3D();
    }
    else if (_debugDrawer)
    {
        AX_SAFE_DELETE(_debugDrawer);
    }
}

bool PhysicsWorld3D::isDebugDrawEnabled() const
{
    return !!_debugDrawer;
}

void PhysicsWorld3D::debugDraw(Renderer* renderer)
{
    if (_debugDrawer)
    {
        // draw
        _debugDrawer->clear();
        constexpr JPH::BodyManager::DrawSettings drawSettings{.mDrawShapeWireframe = true};
        _physicsSystem.DrawBodies(drawSettings, JPH::DebugRenderer::sInstance);
        _physicsSystem.DrawConstraints(JPH::DebugRenderer::sInstance);
        _physicsSystem.DrawConstraintLimits(JPH::DebugRenderer::sInstance);
        _debugDrawer->draw(renderer);
    }
}

void PhysicsWorld3D::stepSimulation(float dt)
{

    for (auto* actor : _physicsActors)
        actor->preSimulate();

    _physicsSystem.Update(dt, 1, &_tempAllocator, &_jobSystem);

    dispatchQueuedEvents();

    for (auto* actor : _physicsActors)
        actor->postSimulate();
}

bool PhysicsWorld3D::rayCast(const ax::Vec3& startPos, const ax::Vec3& endPos, HitResult* result)
{
    if (!result)
        return false;

    JPH::RayCastResult hit;
    JPH::RRayCast ray(jphutil::cast(startPos), jphutil::cast(endPos - startPos));
    if (!_physicsSystem.GetNarrowPhaseQuery().CastRay(ray, hit))
    {
        clearHitResult(*result);
        return false;
    }

    const Vec3 hitPos = jphutil::cast(ray.GetPointOnRay(hit.mFraction));
    Vec3 hitNormal    = Vec3::yAxis;
    JPH::BodyLockRead lock(_physicsSystem.GetBodyLockInterface(), hit.mBodyID);
    if (lock.Succeeded())
    {
        auto& body = lock.GetBody();
        hitNormal  = jphutil::cast(body.GetWorldSpaceSurfaceNormal(hit.mSubShapeID2, jphutil::cast(hitPos)));

        fillHitResult(body, hitPos, hitNormal, *result);
    }
    return result->hitActor;
}

bool PhysicsWorld3D::sweepTest(Collider3D* collider,
                               const ax::Mat4& startTransform,
                               const ax::Mat4& endTransform,
                               HitResult* result)
{
    if (!collider || !result)
        return false;

    if (!collider->internalShape())
    {
        clearHitResult(*result);
        return false;
    }

    JPH::ClosestHitCollisionCollector<JPH::CastShapeCollector> collector;
    JPH::ShapeCastSettings settings;
    settings.mBackFaceModeTriangles = JPH::EBackFaceMode::CollideWithBackFaces;

    const Vec3 startPos(startTransform.m[12], startTransform.m[13], startTransform.m[14]);
    const Vec3 endPos(endTransform.m[12], endTransform.m[13], endTransform.m[14]);
    const JPH::RShapeCast shapeCast =
        JPH::RShapeCast::sFromWorldTransform(collider->internalShape().GetPtr(), JPH::Vec3::sReplicate(1.0f),
                                             jphutil::cast(startTransform), jphutil::cast(endPos - startPos));

    _physicsSystem.GetNarrowPhaseQuery().CastShape(shapeCast, settings, JPH::RVec3::sZero(), collector);
    if (!collector.HadHit())
    {
        clearHitResult(*result);
        return false;
    }

    JPH::BodyLockRead lock(_physicsSystem.GetBodyLockInterface(), collector.mHit.mBodyID2);
    if (lock.Succeeded())
    {
        const Vec3 hitPos    = jphutil::cast(collector.mHit.mContactPointOn2);
        const Vec3 hitNormal = jphutil::cast(-collector.mHit.mPenetrationAxis.Normalized());

        fillHitResult(lock.GetBody(), hitPos, hitNormal, *result);
    }
    return result->hitActor != nullptr;
}

void PhysicsWorld3D::setGlobalEventEnabled(ContactEventBits events, bool enabled)
{
    if (enabled)
        _eventBits |= events;
    else
        _eventBits &= ~events;
}

bool PhysicsWorld3D::isGlobalEventEnabled(ContactEventBits events) const
{
    return bitmask::only(_eventBits, events);
}

void PhysicsWorld3D::addPhysicsActor(PhysicsActor* actor)
{
    if (_physicsActors.contains(actor))
        return;

    if (actor->getActorType() == PhysicsActor::kRigidbody)
    {
        auto rigidbody      = static_cast<Rigidbody3D*>(actor);
        auto& bodyInterface = _physicsSystem.GetBodyInterface();
        if (!bodyInterface.IsAdded(rigidbody->internalHandle()))
            bodyInterface.AddBody(rigidbody->internalHandle(),
                                  rigidbody->getMotionType() != Rigidbody3D::MotionType::Static
                                      ? JPH::EActivation::Activate
                                      : JPH::EActivation::DontActivate);
    }
    else if (actor->getActorType() == PhysicsActor::kCollider)
    {
        auto collider       = static_cast<Collider3D*>(actor);
        auto& bodyInterface = _physicsSystem.GetBodyInterface();
        if (!bodyInterface.IsAdded(collider->internalBody()))
            bodyInterface.AddBody(collider->internalBody(), JPH::EActivation::DontActivate);
    }

    _physicsActors.insert(actor);
}

void PhysicsWorld3D::removePhysicsActor(PhysicsActor* actor)
{
    if (!actor)
        return;
    auto it = _physicsActors.find(actor);
    if (it == _physicsActors.end())
        return;

    removePhysicsActorInternal(actor);

    _physicsActors.erase(it);
}

void PhysicsWorld3D::removePhysicsActorInternal(PhysicsActor* actor)
{
    auto& bodyInterface = _physicsSystem.GetBodyInterface();
    if (actor->getActorType() == PhysicsActor::kRigidbody)
    {
        auto rigidbody = static_cast<Rigidbody3D*>(actor);
        if (rigidbody->isAttached())
        {
            auto bodyID = rigidbody->internalHandle();
            if (bodyInterface.IsAdded(bodyID))
                bodyInterface.RemoveBody(bodyID);
            rigidbody->invalidate();
        }
    }
    else if (actor->getActorType() == PhysicsActor::kCollider)
    {
        auto collider = static_cast<Collider3D*>(actor);
        if (collider->hasOwnBody())
        {
            auto bodyId = collider->internalBody();
            if (bodyInterface.IsAdded(bodyId))
                bodyInterface.RemoveBody(bodyId);
            collider->invalidate();
        }
    }
}

JPH::ValidateResult PhysicsWorld3D::OnContactValidate(const JPH::Body& body1,
                                                      const JPH::Body& body2,
                                                      JPH::RVec3Arg,
                                                      const JPH::CollideShapeResult& collisionResult)
{
    if (!_scene)
        return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;

    if (!isGlobalEventEnabled(ContactEventBits::PreSolve))
        return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;

    auto actorA = asActor(body1);
    auto actorB = asActor(body2);
    if (!actorA || !actorB)
        return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;

    if (!actorA->isEventEnabled(ContactEventBits::PreSolve) && !actorB->isEventEnabled(ContactEventBits::PreSolve))
        return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;

    const Vec3 worldA = jphutil::cast(collisionResult.mContactPointOn1);
    const Vec3 worldB = jphutil::cast(collisionResult.mContactPointOn2);

    const Vec3 normal = jphutil::cast(-collisionResult.mPenetrationAxis.Normalized());

    ContactInfo3D info{.actorA = actorA, .actorB = actorB, .normal = normal};
    info.points.push_back({worldA, worldB});

    if (!_preSolveCallback || _preSolveCallback(info))
        return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;

    return JPH::ValidateResult::RejectAllContactsForThisBodyPair;
}

void PhysicsWorld3D::OnContactAdded(const JPH::Body& body1,
                                    const JPH::Body& body2,
                                    const JPH::ContactManifold& manifold,
                                    JPH::ContactSettings&)
{

    ContactInfo3D info{.actorA = asActor(body1), .actorB = asActor(body2)};
    if (!info.actorA || !info.actorB)
        return;

    fillContactPoints(info, manifold);

    const uint64_t pairKey = makeBodyPairKey(body1.GetID(), body2.GetID());
    {
        std::scoped_lock lock(_eventMutex);
        _pairContacts[pairKey] = info;
    }

    const bool isSensor = body1.IsSensor() || body2.IsSensor();
    if (isSensor)
    {
        ContactInfo3D sensorInfo = reorderSensorContactInfo(info);
        if (checkSensorEvent(sensorInfo))
            queueContactEvent(std::move(sensorInfo), ContactEvent3D::EventCode::SensorBegin);
        return;
    }

    if (isGlobalEventEnabled(ContactEventBits::Hit) &&
        (info.actorA->isEventEnabled(ContactEventBits::Hit) || info.actorB->isEventEnabled(ContactEventBits::Hit)))
    {
        Vec3 normal         = jphutil::cast(manifold.mWorldSpaceNormal);
        Vec3 velA           = jphutil::cast(body1.GetLinearVelocity());
        Vec3 velB           = jphutil::cast(body2.GetLinearVelocity());
        float approachSpeed = (velA - velB).dot(normal);

        if (approachSpeed > 0.0f && manifold.mPenetrationDepth < 1)
        {
            queueContactEvent(ContactInfo3D(info), ContactEvent3D::EventCode::CollisionHit);
        }
    }

    if (isGlobalEventEnabled(ContactEventBits::Contact) && (info.actorA->isEventEnabled(ContactEventBits::Contact) ||
                                                            info.actorB->isEventEnabled(ContactEventBits::Contact)))
        queueContactEvent(std::move(info), ContactEvent3D::EventCode::ContactBegin);
}

void PhysicsWorld3D::OnContactPersisted(const JPH::Body& /*body1*/,
                                        const JPH::Body& /*body2*/,
                                        const JPH::ContactManifold& /*manifold*/,
                                        JPH::ContactSettings&)
{}

void PhysicsWorld3D::OnContactRemoved(const JPH::SubShapeIDPair& subShapePair)
{
    const uint64_t pairKey = makeBodyPairKey(subShapePair.GetBody1ID(), subShapePair.GetBody2ID());
    ContactInfo3D info;
    bool hadInfo = false;
    {
        std::scoped_lock lock(_eventMutex);
        auto it = _pairContacts.find(pairKey);
        if (it != _pairContacts.end())
        {
            info    = it->second;
            hadInfo = true;
            _pairContacts.erase(it);
        }
    }

    if (!hadInfo)
        return;

    if (_physicsSystem.WereBodiesInContact(subShapePair.GetBody1ID(), subShapePair.GetBody2ID()))
        return;

    const bool isSensor = asSensorCollider(info.actorA) || asSensorCollider(info.actorB);

    if (isSensor)
    {
        ContactInfo3D sensorInfo = reorderSensorContactInfo(info);
        if (checkSensorEvent(sensorInfo))
            queueContactEvent(std::move(sensorInfo), ContactEvent3D::EventCode::SensorEnd);
        return;
    }

    if (isGlobalEventEnabled(ContactEventBits::Contact) && (info.actorA->isEventEnabled(ContactEventBits::Contact) ||
                                                            info.actorB->isEventEnabled(ContactEventBits::Contact)))
        queueContactEvent(std::move(info), ContactEvent3D::EventCode::ContactEnd);
}

// JPH::ObjectLayerPairFilter
bool PhysicsWorld3D::ShouldCollide(JPH::ObjectLayer layer1, JPH::ObjectLayer layer2) const
{
    if (layer1 == detail::kSensorLayer || layer2 == detail::kSensorLayer)
        return true;

    if (layer1 == detail::kStaticLayer && layer2 == detail::kStaticLayer)
        return false;

    return true;
}

// JPH::ObjectVsBroadPhaseLayerFilter
bool PhysicsWorld3D::ShouldCollide(JPH::ObjectLayer layer1, JPH::BroadPhaseLayer layer2) const
{
    if (layer1 == detail::kSensorLayer)
        return true;

    if (layer1 == detail::kStaticLayer)
        return layer2 == detail::kBroadPhaseDynamic || layer2 == detail::kBroadPhaseSensor;

    return true;
}

JPH::BroadPhaseLayer PhysicsWorld3D::GetBroadPhaseLayer(JPH::ObjectLayer layer) const
{
    switch (layer)
    {
    case detail::kStaticLayer:
        return detail::kBroadPhaseStatic;
    case detail::kSensorLayer:
        return detail::kBroadPhaseSensor;
    case detail::kDynamicLayer:
    default:
        return detail::kBroadPhaseDynamic;
    }
}

#    if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
const char* PhysicsWorld3D::GetBroadPhaseLayerName(JPH::BroadPhaseLayer layer) const
{
    switch (layer.GetValue())
    {
    case 0:
        return "STATIC";
    case 1:
        return "DYNAMIC";
    case 2:
        return "SENSOR";
    default:
        return "UNKNOWN";
    }
}
#    endif

bool PhysicsWorld3D::checkSensorEvent(const ContactInfo3D& info) const
{
    return isGlobalEventEnabled(ContactEventBits::Sensor) && (info.actorA->isEventEnabled(ContactEventBits::Sensor) ||
                                                              info.actorB->isEventEnabled(ContactEventBits::Sensor));
}

void PhysicsWorld3D::queueContactEvent(ContactInfo3D&& info, ContactEvent3D::EventCode code)
{
    if (!info.actorA || !info.actorB)
        return;

    std::scoped_lock lock(_eventMutex);
    _queuedEvents.push_back({std::move(info), code});
}

}  // namespace ax

#endif
