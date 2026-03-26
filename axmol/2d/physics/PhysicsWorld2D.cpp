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

#include "axmol/2d/physics/PhysicsWorld2D.h"
#if defined(AX_ENABLE_PHYSICS_2D)
#    include <algorithm>
#    include <climits>

#    include "axmol/2d/physics/Rigidbody2D.h"
#    include "axmol/2d/physics/Collider2D.h"
#    include "axmol/2d/physics/Contact2D.h"
#    include "axmol/2d/physics/Joint2D.h"
#    include "axmol/2d/physics/PhysicsUtility2D.h"

#    include "axmol/2d/DrawNode.h"
#    include "axmol/2d/Scene.h"
#    include "axmol/base/Director.h"
#    include "axmol/base/EventDispatcher.h"
#    include "axmol/base/EventCustom.h"

namespace ax
{
extern const char* PHYSICSCONTACT_EVENT_NAME;

const int PhysicsWorld2D::DEBUGDRAW_NONE    = 0x00;
const int PhysicsWorld2D::DEBUGDRAW_SHAPE   = 0x01;
const int PhysicsWorld2D::DEBUGDRAW_JOINT   = 0x02;
const int PhysicsWorld2D::DEBUGDRAW_CONTACT = 0x04;
const int PhysicsWorld2D::DEBUGDRAW_ALL     = DEBUGDRAW_SHAPE | DEBUGDRAW_JOINT | DEBUGDRAW_CONTACT;

namespace
{
struct RayCastCallbackContext
{
    PhysicsWorld2D* world;
    RayCastHitCallback2D func;
    Ray2D ray;
    void* data;
};

struct BoxQueryCallbackContext
{
    PhysicsWorld2D* world;
    PhysicsQueryCallback2D func;
    void* data;
};

struct BoxQueryResultContext
{
    Vector<Collider2D*>* colliders;
};

struct BoxQueryNearestResultContext
{
    Collider2D** collider;
};

struct PointQueryCallbackContext
{
    PhysicsWorld2D* world;
    PhysicsQueryCallback2D func;
    b2Vec2 p;
    void* data;
};

struct PointQueryResultContext
{
    Vector<Collider2D*>* colliders;
    b2Vec2 p;
};

struct PointQueryNearestResultContext
{
    Collider2D** collider;
    b2Vec2 p;
};
}  // namespace

#    pragma region PhysicsQueryCallbacks2D
struct PhysicsQueryCallbacks2D
{
    /*
     * @return -1 to filter, 0 to terminate, fraction to clip the ray for closest hit, 1 to continue, see box2d
     * b2CastResultFcn
     */
    static float handleRayCast(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context)
    {
        auto info = static_cast<RayCastCallbackContext*>(context);

        Collider2D* collider = static_cast<Collider2D*>(b2Shape_GetUserData(shapeId));
        AX_ASSERT(collider != nullptr);

        RayCastHit2D hitInfo = {collider, PhysicsUtility2D::toVec2(point), PhysicsUtility2D::toVec2(normal), fraction};

        bool continues = info->func(*info->world, hitInfo, info->data);

        return continues ? 1 : 0;
    }

    static bool handleBoxOverlap(b2ShapeId shape, void* context)
    {
        Collider2D* collider = static_cast<Collider2D*>(b2Shape_GetUserData(shape));
        AX_ASSERT(collider != nullptr);

        auto info = static_cast<BoxQueryCallbackContext*>(context);
        return info->func(*info->world, *collider, info->data);
    }

    static bool handleBoxOverlapResult(b2ShapeId shape, void* context)
    {
        Collider2D* collider = static_cast<Collider2D*>(b2Shape_GetUserData(shape));
        AX_ASSERT(collider != nullptr);

        auto info = static_cast<BoxQueryResultContext*>(context);
        info->colliders->pushBack(collider);
        return true;
    }

    static bool handleBoxOverlapNearestResult(b2ShapeId shape, void* context)
    {
        Collider2D* collider = static_cast<Collider2D*>(b2Shape_GetUserData(shape));
        AX_ASSERT(collider != nullptr);

        auto info       = static_cast<BoxQueryNearestResultContext*>(context);
        *info->collider = collider;
        return false;
    }

    static bool handlePointOverlap(b2ShapeId shape, void* context)
    {
        Collider2D* collider = static_cast<Collider2D*>(b2Shape_GetUserData(shape));
        AX_ASSERT(collider != nullptr);

        bool continues = true;

        auto info = static_cast<PointQueryCallbackContext*>(context);
        if (b2Shape_TestPoint(shape, info->p))
        {
            b2Vec2 position = b2Body_GetPosition(collider->getAttachedBody()->internalHandle());
            if (b2Distance(info->p, position) <= 1e-6)
                continues = info->func(*info->world, *collider, info->data);
        }
        return continues;
    }

    static bool handlePointOverlapResult(b2ShapeId shape, void* context)
    {
        Collider2D* collider = static_cast<Collider2D*>(b2Shape_GetUserData(shape));
        AX_ASSERT(collider != nullptr);

        auto info = static_cast<PointQueryResultContext*>(context);
        if (b2Shape_TestPoint(shape, info->p))
            info->colliders->pushBack(collider);
        return true;
    }

    static bool handlePointOverlapNearestResult(b2ShapeId shape, void* context)
    {
        Collider2D* collider = static_cast<Collider2D*>(b2Shape_GetUserData(shape));
        AX_ASSERT(collider != nullptr);

        auto info = static_cast<PointQueryNearestResultContext*>(context);
        if (b2Shape_TestPoint(shape, info->p))
        {
            *info->collider = collider;
            return false;  // stop query if 1 shape hit
        }
        return true;
    }
};

#    pragma endregion

PhysicsWorld2D* PhysicsWorld2D::obtain(Scene* scene)
{
    auto world = new PhysicsWorld2D();
    world->init(scene);
    return world;
}

PhysicsWorld2D::PhysicsWorld2D()
    : _gravity(Vec2(0.0f, -9.8f))
    , _PTMRatio(10.0f)
    , _speed(1.0f)
    , _updateRate(1)
    , _updateRateCount(0)
    , _updateTime(0.0f)
    , _substeps(1)
    , _fixedUpdateRate(0)
    , _eventBits(CollisionEventBits::None)
    , _worldId(b2_nullWorldId)
    , _updateBodyTransform(false)
    , _scene(nullptr)
    , _autoStep(true)
    , _eventDispatcher(nullptr)
{}

PhysicsWorld2D::~PhysicsWorld2D()
{
    if (b2World_IsValid(_worldId))
    {
        b2DestroyWorld(_worldId);
    }
}

bool PhysicsWorld2D::isGlobalEventEnabled(CollisionEventBits events) const
{
    return bitmask::only(_eventBits, events);
}

void PhysicsWorld2D::setGlobalEventEnabled(CollisionEventBits events, bool enabled)
{
    if (enabled)
    {
        _eventBits |= events;
    }
    else
    {
        _eventBits &= events;
    }
}

void PhysicsWorld2D::rayCast(RayCastHitCallback2D func, const Ray2D& ray, void* data)
{
    AXASSERT(func != nullptr, "func shouldn't be nullptr");

    if (func != nullptr)
    {
        RayCastCallbackContext context = {this, func, ray, data};
        b2World_CastRay(_worldId, PhysicsUtility2D::tob2Vec2(ray.origin), PhysicsUtility2D::tob2Vec2(ray.translation),
                        b2DefaultQueryFilter(), PhysicsQueryCallbacks2D::handleRayCast, &context);
    }
}

std::optional<RayCastHit2D> PhysicsWorld2D::rayCastClosest(const Ray2D& ray)
{
    auto castRet = b2World_CastRayClosest(_worldId, PhysicsUtility2D::tob2Vec2(ray.origin),
                                          PhysicsUtility2D::tob2Vec2(ray.translation), b2DefaultQueryFilter());
    std::optional<RayCastHit2D> ret;
    if (castRet.hit)
    {
        ret.emplace();
        ret->collider = static_cast<Collider2D*>(b2Shape_GetUserData(castRet.shapeId));
        ret->normal   = PhysicsUtility2D::toVec2(castRet.normal);
        ret->point    = PhysicsUtility2D::toVec2(castRet.point);
        ret->fraction = castRet.fraction;
    }

    return ret;
}

void PhysicsWorld2D::overlapBox(PhysicsQueryCallback2D func, const Rect& rect, void* data)
{
    AXASSERT(func != nullptr, "func shouldn't be nullptr");

    if (func != nullptr)
    {
        BoxQueryCallbackContext context = {this, func, data};

        auto filter = b2DefaultQueryFilter();
        b2World_OverlapAABB(_worldId, PhysicsUtility2D::tob2AABB(rect), b2DefaultQueryFilter(),
                            PhysicsQueryCallbacks2D::handleBoxOverlap, &context);
    }
}

Collider2D* PhysicsWorld2D::overlapBox(const Rect& rect) const
{
    Collider2D* collider{nullptr};

    BoxQueryNearestResultContext context = {&collider};

    b2World_OverlapAABB(_worldId, PhysicsUtility2D::tob2AABB(rect), b2DefaultQueryFilter(),
                        PhysicsQueryCallbacks2D::handlePointOverlapNearestResult, &context);

    return collider;
}

Vector<Collider2D*> PhysicsWorld2D::overlapBoxAll(const Rect& rect) const
{
    Vector<Collider2D*> arr;

    BoxQueryResultContext context = {&arr};

    b2World_OverlapAABB(_worldId, PhysicsUtility2D::tob2AABB(rect), b2DefaultQueryFilter(),
                        PhysicsQueryCallbacks2D::handlePointOverlapResult, &context);

    return arr;
}

void PhysicsWorld2D::overlapPoint(PhysicsQueryCallback2D func, const Vec2& point, void* data)
{
    AXASSERT(func != nullptr, "func shouldn't be nullptr");

    if (func != nullptr)
    {
        PointQueryCallbackContext context = {this, func, PhysicsUtility2D::tob2Vec2(point), data};

        b2AABB aabb;
        aabb.lowerBound = context.p;
        aabb.upperBound = context.p;

        auto filter = b2DefaultQueryFilter();
        b2World_OverlapAABB(_worldId, aabb, b2DefaultQueryFilter(), PhysicsQueryCallbacks2D::handlePointOverlap,
                            &context);
    }
}

Collider2D* PhysicsWorld2D::overlapPoint(const Vec2& point) const
{
    Collider2D* collider{nullptr};

    PointQueryNearestResultContext context = {&collider, PhysicsUtility2D::tob2Vec2(point)};

    b2AABB aabb;
    b2Vec2 d        = {0.001f, 0.001f};
    aabb.lowerBound = b2Sub(context.p, d);
    aabb.upperBound = b2Sub(context.p, d);

    b2World_OverlapAABB(_worldId, aabb, b2DefaultQueryFilter(),
                        PhysicsQueryCallbacks2D::handlePointOverlapNearestResult, &context);

    return collider;
}

Vector<Collider2D*> PhysicsWorld2D::overlapPointAll(const Vec2& point) const
{
    Vector<Collider2D*> arr;

    PointQueryResultContext context = {&arr, PhysicsUtility2D::tob2Vec2(point)};

    b2AABB aabb;
    b2Vec2 d        = {0.001f, 0.001f};
    aabb.lowerBound = b2Sub(context.p, d);
    aabb.upperBound = b2Sub(context.p, d);

    b2World_OverlapAABB(_worldId, aabb, b2DefaultQueryFilter(), PhysicsQueryCallbacks2D::handlePointOverlapResult,
                        &context);

    return arr;
}

bool PhysicsWorld2D::init(Scene* scene)
{
    do
    {
        /* create the physics world
            @see: https://box2d.org/documentation/md_faq.html#autotoc_md135
            How do I convert pixels to meters?
            Suppose you have a sprite for a character that is 100x100 pixels. You decide to use a scaling factor that is
           0.01. This will make the character physics box 1m x 1m. So go make a physics box that is 1x1. Now suppose the
           character starts out at pixel coordinate (345,679). So position the physics box at (3.45,6.79). Now simulate
           the physics world. Suppose the character physics box moves to (2.31,4.98), so move your character sprite to
           pixel coordinates (231,498).

            Now the only tricky part is choosing a scaling factor. This really depends on your game. You should try to
           get your moving objects in the range 0.1 - 10 meters, with 1 meter being the sweet spot.

            This https://github.com/erincatto/box2d-raylib/blob/main/main.c shows how to convert meters to pixels.
        */
        b2SetLengthUnitsPerMeter(_PTMRatio);

        auto worldDef = b2DefaultWorldDef();

        // Realistic gravity is achieved by multiplying gravity by the length unit.
        worldDef.gravity          = PhysicsUtility2D::tob2Vec2(_gravity * _PTMRatio);
        worldDef.enableSleep      = true;
        worldDef.enableContinuous = true;
        _worldId                  = b2CreateWorld(&worldDef);

        AX_BREAK_IF(!b2World_IsValid(_worldId));

        b2World_SetPreSolveCallback(_worldId, (b2PreSolveFcn*)handlePreSolve, this);

        _scene           = scene;
        _eventDispatcher = scene->getEventDispatcher();

        return true;
    } while (false);

    return false;
}

void PhysicsWorld2D::setGravity(const Vec2& gravity)
{
    _gravity = gravity;
    b2World_SetGravity(_worldId, PhysicsUtility2D::tob2Vec2(_gravity * _PTMRatio));
}

void PhysicsWorld2D::setSlopBias(float slop, float bias)
{
    // NOTE: NOT SUPPORT
}

void PhysicsWorld2D::setSubsteps(int steps)
{
    if (steps > 0)
    {
        _substeps = steps;
        if (steps > 1)
        {
            _updateRate = 1;
        }
    }
}

void PhysicsWorld2D::step(float delta)
{
    if (_autoStep)
    {
        AXLOGD("Physics Warning: You need to close auto step( setAutoStep(false) ) first");
    }
    else
    {
        update(delta, true);
    }
}

void PhysicsWorld2D::update(float delta, bool userCall /* = false*/)
{

    if (_preUpdateCallback)
        _preUpdateCallback();  // fix #11154

    auto sceneToWorldTransform = _scene->getNodeToParentTransform();
    beforeSimulation(_scene, sceneToWorldTransform, 1.f, 1.f, 0.f);

    if (delta < FLT_EPSILON)
    {
        return;
    }

    if (delta < FLT_EPSILON)
        return;

    _updateTime += delta * _speed;

    if (userCall || _fixedUpdateRate <= 0)
    {
        b2World_Step(_worldId, delta, _substeps);
    }
    else
    {
        const float fixedStep = 1.0f / _fixedUpdateRate;
        while (_updateTime >= fixedStep)
        {
            _updateTime -= fixedStep;
            _scene->fixedUpdate(fixedStep);
            b2World_Step(_worldId, fixedStep, _substeps);
        }
    }

    dispatchContactEvents();

    // Update physics position, should loop as the same sequence as node tree.
    // PhysicsWorld2D::afterSimulation() will depend on the sequence.
    afterSimulation(_scene, sceneToWorldTransform, 0.f);

    if (_postUpdateCallback)
        _postUpdateCallback();  // fix #11154
}

void PhysicsWorld2D::beforeSimulation(Node* node,
                                      const Mat4& parentToWorldTransform,
                                      float nodeParentScaleX,
                                      float nodeParentScaleY,
                                      float parentRotation)
{
    auto scaleX   = nodeParentScaleX * node->getScaleX();
    auto scaleY   = nodeParentScaleY * node->getScaleY();
    auto rotation = parentRotation + node->getRotation();

    auto nodeToWorldTransform = parentToWorldTransform * node->getNodeToParentTransform();

    auto rigidbody2D = node->getRigidbody2D();
    if (rigidbody2D)
        rigidbody2D->beforeSimulation(parentToWorldTransform, nodeToWorldTransform, scaleX, scaleY, rotation);

    for (auto&& child : node->getChildren())
        beforeSimulation(child, nodeToWorldTransform, scaleX, scaleY, rotation);
}

void PhysicsWorld2D::afterSimulation(Node* node, const Mat4& parentToWorldTransform, float parentRotation)
{
    auto nodeToWorldTransform = parentToWorldTransform * node->getNodeToParentTransform();
    auto nodeRotation         = parentRotation + node->getRotation();

    auto rigidbody2D = node->getRigidbody2D();
    if (rigidbody2D)
        rigidbody2D->afterSimulation(parentToWorldTransform, parentRotation);

    for (auto&& child : node->getChildren())
        afterSimulation(child, nodeToWorldTransform, nodeRotation);
}

void PhysicsWorld2D::setPostUpdateCallback(const std::function<void()>& callback)
{
    _postUpdateCallback = callback;
}

void PhysicsWorld2D::setPreUpdateCallback(const std::function<void()>& callback)
{
    _preUpdateCallback = callback;
}

void PhysicsWorld2D::dispatchContactEvents()
{
    /**
     * @brief Helper lambda to resolve Colliders from ShapeIds and manage the Contact2D lifecycle.
     */
    auto&& dispatch = [this](b2ShapeId idA, b2ShapeId idB, Contact2D::EventCode code) {
        auto colliderA = static_cast<Collider2D*>(b2Shape_GetUserData(idA));
        auto colliderB = static_cast<Collider2D*>(b2Shape_GetUserData(idB));

        // Obtain and initialize the pooled event object
        auto event = Contact2D::obtain(colliderA, colliderB);
        if (event)
        {
            event->setEventCode(code);
            _eventDispatcher->dispatchEvent(event);
            event->release();
        }
    };

    const b2ContactEvents contactData = b2World_GetContactEvents(_worldId);

    if (bitmask::any(CollisionEventBits::Hit, _eventBits))
    {
        // Dispatch Collision Hit events (contains impulse/speed data)
        for (int i = 0; i < contactData.hitCount; ++i)
        {
            auto& event = contactData.hitEvents[i];
            dispatch(event.shapeIdA, event.shapeIdB, Contact2D::EventCode::CollisionHit);
        }
    }

    if (bitmask::any(CollisionEventBits::Contact, _eventBits))
    {
        // Dispatch Contact Begin events
        for (int i = 0; i < contactData.beginCount; ++i)
        {
            auto& event = contactData.beginEvents[i];
            dispatch(event.shapeIdA, event.shapeIdB, Contact2D::EventCode::ContactBegin);
        }

        // Dispatch Contact End events
        for (int i = 0; i < contactData.endCount; ++i)
        {
            auto& event = contactData.endEvents[i];
            dispatch(event.shapeIdA, event.shapeIdB, Contact2D::EventCode::ContactEnd);
        }
    }

    if (bitmask::any(CollisionEventBits::Sensor, _eventBits))
    {
        auto sensorEvents = b2World_GetSensorEvents(_worldId);

        // Dispatch Sensor Begin events
        for (int i = 0; i < sensorEvents.beginCount; ++i)
        {
            auto& event = sensorEvents.beginEvents[i];
            dispatch(event.sensorShapeId, event.visitorShapeId, Contact2D::EventCode::SensorBegin);
        }

        // Dispatch Sensor End events
        for (int i = 0; i < sensorEvents.endCount; ++i)
        {
            auto& event = sensorEvents.endEvents[i];
            dispatch(event.sensorShapeId, event.visitorShapeId, Contact2D::EventCode::SensorEnd);
        }
    }
}

bool PhysicsWorld2D::handlePreSolve(b2ShapeId shapeIdA,
                                    b2ShapeId shapeIdB,
                                    b2Vec2 point,
                                    b2Vec2 normal,
                                    PhysicsWorld2D* world)
{
    auto event = Contact2D::obtain(static_cast<Collider2D*>(b2Shape_GetUserData(shapeIdA)),
                                   static_cast<Collider2D*>(b2Shape_GetUserData(shapeIdB)));
    event->setPointNormal(PhysicsUtility2D::toVec2(point), PhysicsUtility2D::toVec2(normal));
    bool ret = world->onPreSolve(event);
    event->release();
    return ret;
}

bool PhysicsWorld2D::onPreSolve(Contact2D* contact)
{
    // AXLOGD("onPreSolve: {}, {}", fmt::ptr(contact->getColliderA()), fmt::ptr(contact->getColliderB()));

    contact->setEventCode(Contact2D::EventCode::PreSolve);
    _eventDispatcher->dispatchEvent(contact);

    return contact->resetResult();
}

}  // namespace ax

#endif  // defined(AX_ENABLE_PHYSICS_2D)
