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

#include "axmol/physics2d/PhysicsWorld2D.h"
#if defined(AX_ENABLE_PHYSICS_2D)
#    include <algorithm>
#    include <climits>

#    include "axmol/physics2d/Rigidbody2D.h"
#    include "axmol/physics2d/Collider2D.h"
#    include "axmol/physics2d/Contact2D.h"
#    include "axmol/physics2d/Joint2D.h"
#    include "axmol/physics2d/PhysicsUtility2D.h"

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
struct RayCastQueryCallbackContext
{
    PhysicsWorld2D* world;
    PhysicsRayCastCallback func;
    Vec2 p1;
    Vec2 p2;
    void* data;
};

struct BoxQueryCallbackContext
{
    PhysicsWorld2D* world;
    PhysicsQueryRectCallback func;
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
    PhysicsQueryPointCallback func;
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

struct PhysicsWorldCallback
{
    static float handleRayCast(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context)
    {
        if (!continues)
            return fraction;

        auto info = static_cast<RayCastQueryCallbackContext*>(context);

        Collider2D* collider = static_cast<Collider2D*>(b2Shape_GetUserData(shapeId));
        AX_ASSERT(collider != nullptr);

        PhysicsRayCastInfo callbackInfo = {
            collider, info->p1, info->p2, PhysicsUtility2D::toVec2(point), PhysicsUtility2D::toVec2(normal), fraction};

        continues = info->func(*info->world, callbackInfo, info->data);

        return fraction;
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
            {
                continues = info->func(*info->world, *collider, info->data);
            }
        }
        return continues;
    }

    static bool handlePointOverlapResult(b2ShapeId shape, void* context)
    {
        Collider2D* collider = static_cast<Collider2D*>(b2Shape_GetUserData(shape));
        AX_ASSERT(collider != nullptr);

        auto info = static_cast<PointQueryResultContext*>(context);
        if (b2Shape_TestPoint(shape, info->p))
        {
            info->colliders->pushBack(collider);
        }
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

    static bool continues;
};

bool PhysicsWorldCallback::continues = true;

bool PhysicsWorld2D::collisionBeginCallback(Contact2D& contact)
{
    bool ret = true;

    Collider2D* colliderA = contact.getColliderA();
    Collider2D* colliderB = contact.getColliderB();
    Rigidbody2D* bodyA    = colliderA->getAttachedBody();
    Rigidbody2D* bodyB    = colliderB->getAttachedBody();

#    if 0
    auto&& jointsA          = bodyA->getJoints();

    // check the joint is collision enable or not

    for (Joint2D* joint : jointsA)
    {
        if (!joint->isCollisionEnabled())
        {
            Rigidbody2D* body = joint->getBodyA() == bodyA ? joint->getBodyB() : joint->getBodyA();

            if (body == bodyB)
            {
                contact.setNotificationEnable(false);
                return false;
            }
        }
    }
#    endif

    // bitmask check
    if ((colliderA->getCategoryBitmask() & colliderB->getContactTestBitmask()) == 0 ||
        (colliderA->getContactTestBitmask() & colliderB->getCategoryBitmask()) == 0)
    {
        contact.setNotificationEnable(false);
    }

    if (colliderA->getGroup() != 0 && colliderA->getGroup() == colliderB->getGroup())
    {
        ret = colliderA->getGroup() > 0;
    }
    else
    {
        if ((colliderA->getCategoryBitmask() & colliderB->getCollisionBitmask()) == 0 ||
            (colliderB->getCategoryBitmask() & colliderA->getCollisionBitmask()) == 0)
        {
            ret = false;
        }
    }

    if (contact.isNotificationEnabled())
    {
        contact.setEventCode(Contact2D::EventCode::BEGIN);
        contact.setWorld(this);
        _eventDispatcher->dispatchEvent(&contact);
    }

    return ret ? contact.resetResult() : false;
}

bool PhysicsWorld2D::collisionPreSolveCallback(Contact2D& contact)
{
    if (!contact.isNotificationEnabled())
    {
        return true;
    }

    contact.setEventCode(Contact2D::EventCode::PRESOLVE);
    contact.setWorld(this);
    _eventDispatcher->dispatchEvent(&contact);

    return contact.resetResult();
}

void PhysicsWorld2D::collisionPostSolveCallback(Contact2D& contact)
{
    if (!contact.isNotificationEnabled())
    {
        return;
    }

    contact.setEventCode(Contact2D::EventCode::POSTSOLVE);
    contact.setWorld(this);
    _eventDispatcher->dispatchEvent(&contact);
}

void PhysicsWorld2D::collisionSeparateCallback(Contact2D& contact)
{
    if (!contact.isNotificationEnabled())
    {
        return;
    }

    contact.setEventCode(Contact2D::EventCode::SEPARATE);
    contact.setWorld(this);
    _eventDispatcher->dispatchEvent(&contact);
}

void PhysicsWorld2D::rayCast(PhysicsRayCastCallback func, const Vec2& point1, const Vec2& point2, void* data)
{
    AXASSERT(func != nullptr, "func shouldn't be nullptr");

    if (func != nullptr)
    {
        RayCastQueryCallbackContext context = {this, func, point1, point2, data};

        auto translation                = point2 - point1;
        PhysicsWorldCallback::continues = true;
        b2World_CastRay(_b2World, PhysicsUtility2D::tob2Vec2(point1), PhysicsUtility2D::tob2Vec2(translation),
                        b2DefaultQueryFilter(), PhysicsWorldCallback::handleRayCast, &context);
    }
}

void PhysicsWorld2D::overlapBox(PhysicsQueryRectCallback func, const Rect& rect, void* data)
{
    AXASSERT(func != nullptr, "func shouldn't be nullptr");

    if (func != nullptr)
    {
        BoxQueryCallbackContext context = {this, func, data};

        auto filter = b2DefaultQueryFilter();
        b2World_OverlapAABB(_b2World, PhysicsUtility2D::tob2AABB(rect), b2DefaultQueryFilter(),
                            PhysicsWorldCallback::handleBoxOverlap, &context);
    }
}

Collider2D* PhysicsWorld2D::overlapBox(const Rect& rect) const
{
    Collider2D* collider{nullptr};

    BoxQueryNearestResultContext context = {&collider};

    b2World_OverlapAABB(_b2World, PhysicsUtility2D::tob2AABB(rect), b2DefaultQueryFilter(),
                        PhysicsWorldCallback::handlePointOverlapNearestResult, &context);

    return collider;
}

Vector<Collider2D*> PhysicsWorld2D::overlapBoxAll(const Rect& rect) const
{
    Vector<Collider2D*> arr;

    BoxQueryResultContext context = {&arr};

    b2World_OverlapAABB(_b2World, PhysicsUtility2D::tob2AABB(rect), b2DefaultQueryFilter(),
                        PhysicsWorldCallback::handlePointOverlapResult, &context);

    return arr;
}

void PhysicsWorld2D::overlapPoint(PhysicsQueryPointCallback func, const Vec2& point, void* data)
{
    AXASSERT(func != nullptr, "func shouldn't be nullptr");

    if (func != nullptr)
    {
        PointQueryCallbackContext context = {this, func, PhysicsUtility2D::tob2Vec2(point), data};

        b2AABB aabb;
        aabb.lowerBound = context.p;
        aabb.upperBound = context.p;

        auto filter = b2DefaultQueryFilter();
        b2World_OverlapAABB(_b2World, aabb, b2DefaultQueryFilter(), PhysicsWorldCallback::handlePointOverlap, &context);
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

    b2World_OverlapAABB(_b2World, aabb, b2DefaultQueryFilter(), PhysicsWorldCallback::handlePointOverlapNearestResult,
                        &context);

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

    b2World_OverlapAABB(_b2World, aabb, b2DefaultQueryFilter(), PhysicsWorldCallback::handlePointOverlapResult,
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

        auto def = b2DefaultWorldDef();
        // Realistic gravity is achieved by multiplying gravity by the length unit.
        def.gravity = PhysicsUtility2D::tob2Vec2(_gravity * _PTMRatio);

        _b2World = b2CreateWorld(&def);

        AX_BREAK_IF(!b2World_IsValid(_b2World));

        // Do we want to let bodies sleep?
        b2World_EnableSleeping(_b2World, true);
        b2World_EnableContinuous(_b2World, true);

        // TODO:
        // cpCollisionHandler* handler = cpSpaceAddDefaultCollisionHandler(_cpSpace);
        // handler->userData           = this;
        // handler->beginFunc          = (cpCollisionBeginFunc)PhysicsWorldCallback::collisionBeginCallbackFunc;
        // handler->preSolveFunc       = (cpCollisionPreSolveFunc)PhysicsWorldCallback::collisionPreSolveCallbackFunc;
        // handler->postSolveFunc      = (cpCollisionPostSolveFunc)PhysicsWorldCallback::collisionPostSolveCallbackFunc;
        // handler->separateFunc       = (cpCollisionSeparateFunc)PhysicsWorldCallback::collisionSeparateCallbackFunc;

        _scene           = scene;
        _eventDispatcher = scene->getEventDispatcher();

        return true;
    } while (false);

    return false;
}

void PhysicsWorld2D::removeBody(int tag) {}

void PhysicsWorld2D::removeBody(Rigidbody2D* body)
{
    if (body->getWorld() != this)
    {
        AXLOGD("Physics Warning: this body doesn't belong to this world");
        return;
    }

    body->_world = nullptr;
}

void PhysicsWorld2D::removeAllBodies() {}

Rigidbody2D* PhysicsWorld2D::getBody(int tag) const
{
    return nullptr;
}

void PhysicsWorld2D::setGravity(const Vec2& gravity)
{
    _gravity = gravity;
    b2World_SetGravity(_b2World, PhysicsUtility2D::tob2Vec2(_gravity * _PTMRatio));
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
        b2World_Step(_b2World, delta, _substeps);
    }
    else
    {
        const float fixedStep = 1.0f / _fixedUpdateRate;
        while (_updateTime >= fixedStep)
        {
            _updateTime -= fixedStep;
            _scene->fixedUpdate(fixedStep);
            b2World_Step(_b2World, fixedStep, _substeps);
        }
    }

    // Update physics position, should loop as the same sequence as node tree.
    // PhysicsWorld2D::afterSimulation() will depend on the sequence.
    afterSimulation(_scene, sceneToWorldTransform, 0.f);

    if (_postUpdateCallback)
        _postUpdateCallback();  // fix #11154
}

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
    , _b2World(b2_nullWorldId)
    , _updateBodyTransform(false)
    , _scene(nullptr)
    , _autoStep(true)
    , _eventDispatcher(nullptr)
{}

PhysicsWorld2D::~PhysicsWorld2D()
{
    removeAllBodies();
    if (b2World_IsValid(_b2World))
    {
        b2DestroyWorld(_b2World);
    }
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

}  // namespace ax

#endif  // defined(AX_ENABLE_PHYSICS_2D)
