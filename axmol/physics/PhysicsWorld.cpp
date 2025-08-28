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

#include "axmol/physics/PhysicsWorld.h"
#if defined(AX_ENABLE_PHYSICS)
#    include <algorithm>
#    include <climits>

#    include "axmol/physics/PhysicsBody.h"
#    include "axmol/physics/PhysicsCollider.h"
#    include "axmol/physics/PhysicsContact.h"
#    include "axmol/physics/PhysicsJoint.h"
#    include "axmol/physics/PhysicsHelper.h"

#    include "axmol/2d/DrawNode.h"
#    include "axmol/2d/Scene.h"
#    include "axmol/base/Director.h"
#    include "axmol/base/EventDispatcher.h"
#    include "axmol/base/EventCustom.h"

namespace ax
{
// box2d not support FLT_MAX as infinity, oterwise will trigger NAN in world step, so we use a large value instead
const float PHYSICS_INFINITY = 10e6;
extern const char* PHYSICSCONTACT_EVENT_NAME;

const int PhysicsWorld::DEBUGDRAW_NONE    = 0x00;
const int PhysicsWorld::DEBUGDRAW_SHAPE   = 0x01;
const int PhysicsWorld::DEBUGDRAW_JOINT   = 0x02;
const int PhysicsWorld::DEBUGDRAW_CONTACT = 0x04;
const int PhysicsWorld::DEBUGDRAW_ALL     = DEBUGDRAW_SHAPE | DEBUGDRAW_JOINT | DEBUGDRAW_CONTACT;

const float _debugDrawThickness = 0.5f;  // thickness of the DebugDraw lines, circles, dots, polygons

namespace
{
struct RayCastCallbackInfo
{
    PhysicsWorld* world;
    PhysicsRayCastCallbackFunc func;
    Vec2 p1;
    Vec2 p2;
    void* data;
};

struct RectQueryCallbackInfo
{
    PhysicsWorld* world;
    PhysicsQueryRectCallbackFunc func;
    void* data;
};

struct PointQueryCallbackInfo
{
    PhysicsWorld* world;
    PhysicsQueryPointCallbackFunc func;
    b2Vec2 p;
    void* data;
};

struct ShapeQueryCallbackInfo
{
    Vector<PhysicsCollider*>& colliders;
    b2Vec2 p;
};
}  // namespace

struct PhysicsWorldCallback
{
#    if 0
    static cpBool collisionBeginCallbackFunc(cpArbiter* arb,
                                                            struct cpSpace* /*space*/,
                                                            PhysicsWorld* world)
    {
        CP_ARBITER_GET_SHAPES(arb, a, b);

        PhysicsCollider* shapeA = static_cast<PhysicsCollider*>(cpShapeGetUserData(a));
        PhysicsCollider* shapeB = static_cast<PhysicsCollider*>(cpShapeGetUserData(b));
        AX_ASSERT(shapeA != nullptr && shapeB != nullptr);

        auto contact = PhysicsContact::construct(shapeA, shapeB);
        cpArbiterSetUserData(arb, contact);
        contact->_contactInfo = arb;

        return world->collisionBeginCallback(*contact);
    }

    static cpBool collisionPreSolveCallbackFunc(cpArbiter* arb, cpSpace* /*space*/, PhysicsWorld* world)
    {
        return world->collisionPreSolveCallback(*static_cast<PhysicsContact*>(cpArbiterGetUserData(arb)));
    }

    static void collisionPostSolveCallbackFunc(cpArbiter* arb, cpSpace* /*space*/, PhysicsWorld* world)
    {
        world->collisionPostSolveCallback(*static_cast<PhysicsContact*>(cpArbiterGetUserData(arb)));
    }

    static void collisionSeparateCallbackFunc(cpArbiter* arb, cpSpace* /*space*/, PhysicsWorld* world)
    {
        PhysicsContact* contact = static_cast<PhysicsContact*>(cpArbiterGetUserData(arb));

        world->collisionSeparateCallback(*contact);

        delete contact;
    }
#    endif

    /// Called for each shape found in the query. You control how the ray cast
    /// proceeds by returning a float:
    /// return -1: ignore this shape and continue
    /// return 0: terminate the ray cast
    /// return fraction: clip the ray to this point
    /// return 1: don't clip the ray and continue
    /// @param shapeId the shape hit by the ray
    /// @param point the point of initial intersection
    /// @param normal the normal vector at the point of intersection
    /// @param fraction the fraction along the ray at the point of intersection
    ///	@param context the user context
    /// @return -1 to filter, 0 to terminate, fraction to clip the ray for closest hit, 1 to continue
    static float rayCastCallbackFunc(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context)
    {
        if (!continues)
            return fraction;

        auto info = static_cast<RayCastCallbackInfo*>(context);

        PhysicsCollider* collider = static_cast<PhysicsCollider*>(b2Shape_GetUserData(shapeId));
        AX_ASSERT(collider != nullptr);

        PhysicsRayCastInfo callbackInfo = {collider, info->p1, info->p2, phlp::toVec2(point), phlp::toVec2(normal),
                                           fraction};

        continues = info->func(*info->world, callbackInfo, info->data);

        return fraction;
    }

    static bool queryRectCallbackFunc(b2ShapeId shape, void* context)
    {
        PhysicsCollider* collider = static_cast<PhysicsCollider*>(b2Shape_GetUserData(shape));
        AX_ASSERT(collider != nullptr);

        auto info = static_cast<RectQueryCallbackInfo*>(context);
        return info->func(*info->world, *collider, info->data);
    }

    static bool queryPointCallbackFunc(b2ShapeId shape, void* context)
    {
        PhysicsCollider* collider = static_cast<PhysicsCollider*>(b2Shape_GetUserData(shape));
        AX_ASSERT(collider != nullptr);

        bool continues = true;

        auto info = static_cast<PointQueryCallbackInfo*>(context);
        if (b2Shape_TestPoint(shape, info->p))
        {
            b2Vec2 position = b2Body_GetPosition(collider->getBody()->getB2Body());
            if (b2Distance(info->p, position) <= 1e-6)
            {
                continues = info->func(*info->world, *collider, info->data);
            }
        }
        return continues;
    }

    static bool queryShapeCallbackFunc(b2ShapeId shape, void* context)
    {
        PhysicsCollider* collider = static_cast<PhysicsCollider*>(b2Shape_GetUserData(shape));
        AX_ASSERT(collider != nullptr);

        auto info = static_cast<ShapeQueryCallbackInfo*>(context);
        if (b2Shape_TestPoint(shape, info->p))
        {
            info->colliders.pushBack(collider);
        }
        return true;
    }

#    if 0
    static void getShapesAtPointFunc(cpShape* shape,
                                                    cpVect /*point*/,
                                                    cpFloat /*distance*/,
                                                    cpVect /*gradient*/,
                                                    Vector<PhysicsCollider*>* arr)
    {
        PhysicsCollider* physicsShape = static_cast<PhysicsCollider*>(cpShapeGetUserData(shape));
        AX_ASSERT(physicsShape != nullptr);
        arr->pushBack(physicsShape);
    }

#    endif

    static bool continues;
};

bool PhysicsWorldCallback::continues = true;

bool PhysicsWorld::collisionBeginCallback(PhysicsContact& contact)
{
    bool ret = true;

    PhysicsCollider* shapeA = contact.getShapeA();
    PhysicsCollider* shapeB = contact.getShapeB();
    PhysicsBody* bodyA      = shapeA->getBody();
    PhysicsBody* bodyB      = shapeB->getBody();
    auto&& jointsA          = bodyA->getJoints();

    // check the joint is collision enable or not
    for (PhysicsJoint* joint : jointsA)
    {
        if (std::find(_joints.begin(), _joints.end(), joint) == _joints.end())
        {
            continue;
        }

        if (!joint->isCollisionEnabled())
        {
            PhysicsBody* body = joint->getBodyA() == bodyA ? joint->getBodyB() : joint->getBodyA();

            if (body == bodyB)
            {
                contact.setNotificationEnable(false);
                return false;
            }
        }
    }

    // bitmask check
    if ((shapeA->getCategoryBitmask() & shapeB->getContactTestBitmask()) == 0 ||
        (shapeA->getContactTestBitmask() & shapeB->getCategoryBitmask()) == 0)
    {
        contact.setNotificationEnable(false);
    }

    if (shapeA->getGroup() != 0 && shapeA->getGroup() == shapeB->getGroup())
    {
        ret = shapeA->getGroup() > 0;
    }
    else
    {
        if ((shapeA->getCategoryBitmask() & shapeB->getCollisionBitmask()) == 0 ||
            (shapeB->getCategoryBitmask() & shapeA->getCollisionBitmask()) == 0)
        {
            ret = false;
        }
    }

    if (contact.isNotificationEnabled())
    {
        contact.setEventCode(PhysicsContact::EventCode::BEGIN);
        contact.setWorld(this);
        _eventDispatcher->dispatchEvent(&contact);
    }

    return ret ? contact.resetResult() : false;
}

bool PhysicsWorld::collisionPreSolveCallback(PhysicsContact& contact)
{
    if (!contact.isNotificationEnabled())
    {
        return true;
    }

    contact.setEventCode(PhysicsContact::EventCode::PRESOLVE);
    contact.setWorld(this);
    _eventDispatcher->dispatchEvent(&contact);

    return contact.resetResult();
}

void PhysicsWorld::collisionPostSolveCallback(PhysicsContact& contact)
{
    if (!contact.isNotificationEnabled())
    {
        return;
    }

    contact.setEventCode(PhysicsContact::EventCode::POSTSOLVE);
    contact.setWorld(this);
    _eventDispatcher->dispatchEvent(&contact);
}

void PhysicsWorld::collisionSeparateCallback(PhysicsContact& contact)
{
    if (!contact.isNotificationEnabled())
    {
        return;
    }

    contact.setEventCode(PhysicsContact::EventCode::SEPARATE);
    contact.setWorld(this);
    _eventDispatcher->dispatchEvent(&contact);
}

void PhysicsWorld::rayCast(PhysicsRayCastCallbackFunc func, const Vec2& point1, const Vec2& point2, void* data)
{
    AXASSERT(func != nullptr, "func shouldn't be nullptr");

    if (func != nullptr)
    {
        if (!_delayAddBodies.empty() || !_delayRemoveBodies.empty())
        {
            updateBodies();
        }
        RayCastCallbackInfo info = {this, func, point1, point2, data};

        auto translation                = point2 - point1;
        PhysicsWorldCallback::continues = true;
        b2World_CastRay(_b2World, phlp::tob2Vec2(point1), phlp::tob2Vec2(translation), b2DefaultQueryFilter(),
                        PhysicsWorldCallback::rayCastCallbackFunc, &info);

#    if 0
        auto result = b2World_CastRayClosest(_b2World, phlp::tob2Vec2(point1), phlp::tob2Vec2(translation), b2DefaultQueryFilter());
        if (b2Shape_IsValid(result.shapeId))
        {
            auto collider = static_cast<PhysicsCollider*>(b2Shape_GetUserData(result.shapeId));
            PhysicsRayCastInfo rayCastInfo{collider, info.p1, info.p2, phlp::toVec2(result.point), phlp::toVec2(result.normal),
                                           result.fraction};
            func(*this, rayCastInfo, data);
        }
#    endif
    }
}

void PhysicsWorld::queryRect(PhysicsQueryRectCallbackFunc func, const Rect& rect, void* data)
{
    AXASSERT(func != nullptr, "func shouldn't be nullptr");

    if (func != nullptr)
    {
        if (!_delayAddBodies.empty() || !_delayRemoveBodies.empty())
        {
            updateBodies();
        }
        RectQueryCallbackInfo info = {this, func, data};

        auto filter = b2DefaultQueryFilter();
        b2World_OverlapAABB(_b2World, phlp::tob2AABB(rect), b2DefaultQueryFilter(),
                            PhysicsWorldCallback::queryRectCallbackFunc, &info);
    }
}

void PhysicsWorld::queryPoint(PhysicsQueryPointCallbackFunc func, const Vec2& point, void* data)
{
    AXASSERT(func != nullptr, "func shouldn't be nullptr");

    if (func != nullptr)
    {
        if (!_delayAddBodies.empty() || !_delayRemoveBodies.empty())
        {
            updateBodies();
        }

        PointQueryCallbackInfo info = {this, func, phlp::tob2Vec2(point), data};

        b2AABB aabb;
        aabb.lowerBound = info.p;
        aabb.upperBound = info.p;

        auto filter = b2DefaultQueryFilter();
        b2World_OverlapAABB(_b2World, aabb, b2DefaultQueryFilter(), PhysicsWorldCallback::queryRectCallbackFunc, &info);
    }
}

Vector<PhysicsCollider*> PhysicsWorld::getShapes(const Vec2& point) const
{
    Vector<PhysicsCollider*> arr;

    ShapeQueryCallbackInfo info = {arr, phlp::tob2Vec2(point)};

    b2AABB aabb;
    aabb.lowerBound = info.p;
    aabb.upperBound = info.p;

    b2World_OverlapAABB(_b2World, aabb, b2DefaultQueryFilter(), PhysicsWorldCallback::queryShapeCallbackFunc, &info);

    return arr;
}

PhysicsCollider* PhysicsWorld::getShape(const Vec2& point) const
{
    // TODO: use b2World_OverlapCircle
    // cpShape* shape =
    //     cpSpacePointQueryNearest(_cpSpace, phlpvec22cpv(point), 0, CP_SHAPE_FILTER_ALL, nullptr);
    // return shape == nullptr ? nullptr : static_cast<PhysicsCollider*>(cpShapeGetUserData(shape));
    return nullptr;
}

bool PhysicsWorld::init()
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
        def.gravity = phlp::tob2Vec2(_gravity * _PTMRatio);

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

        return true;
    } while (false);

    return false;
}

void PhysicsWorld::addBody(PhysicsBody* body)
{
    AXASSERT(body != nullptr, "the body can not be nullptr");

    addBodyOrDelay(body);
    _bodies.pushBack(body);
    body->_world = this;
    body->setFixedUpdate(_fixedRate > 0);
}

void PhysicsWorld::addBodyOrDelay(PhysicsBody* body)
{
    auto removeBodyIter = _delayRemoveBodies.find(body);
    if (removeBodyIter != _delayRemoveBodies.end())
    {
        _delayRemoveBodies.erase(removeBodyIter);
        return;
    }

    if (_delayAddBodies.find(body) == _delayAddBodies.end())
    {
        _delayAddBodies.pushBack(body);
    }
}

void PhysicsWorld::updateBodies()
{
    if (_isWorldLocked)
    {
        return;
    }

    // issue #4944, contact callback will be invoked when add/remove body, _delayAddBodies maybe changed, so we need
    // make a copy.
    auto addCopy = _delayAddBodies;
    _delayAddBodies.clear();
    for (auto&& body : addCopy)
    {
        // doAddBody(body);
    }

    auto removeCopy = _delayRemoveBodies;
    _delayRemoveBodies.clear();
    for (auto&& body : removeCopy)
    {
        doRemoveBody(body);
    }
}

void PhysicsWorld::removeBody(int tag)
{
    for (auto&& body : _bodies)
    {
        if (body->getTag() == tag)
        {
            removeBody(body);
            return;
        }
    }
}

void PhysicsWorld::removeBody(PhysicsBody* body)
{
    if (body->getWorld() != this)
    {
        AXLOGD("Physics Warning: this body doesn't belong to this world");
        return;
    }

    // destroy the body's joints
    auto removeCopy = body->_joints;
    for (auto&& joint : removeCopy)
    {
        removeJoint(joint, true);
    }
    body->_joints.clear();

    removeBodyOrDelay(body);
    _bodies.eraseObject(body);
    body->_world = nullptr;
}

void PhysicsWorld::removeBodyOrDelay(PhysicsBody* body)
{
    if (_delayAddBodies.getIndex(body) != AX_INVALID_INDEX)
    {
        _delayAddBodies.eraseObject(body);
        return;
    }

    if (_isWorldLocked)
    {
        if (_delayRemoveBodies.getIndex(body) == AX_INVALID_INDEX)
        {
            _delayRemoveBodies.pushBack(body);
        }
    }
    else
    {
        doRemoveBody(body);
    }
}

void PhysicsWorld::removeJoint(PhysicsJoint* joint, bool destroy)
{
    if (joint)
    {
        if (joint->getWorld() != this && destroy)
        {
            AXLOGD(
                "physics warning: the joint is not in this world, it won't be destroyed until the body it connects is "
                "destroyed");
            return;
        }

        joint->_destroyMark = destroy;

        bool removedFromDelayAdd = false;
        auto it                  = std::find(_delayAddJoints.begin(), _delayAddJoints.end(), joint);
        if (it != _delayAddJoints.end())
        {
            _delayAddJoints.erase(it);
            removedFromDelayAdd = true;
        }

        if (_isWorldLocked)
        {
            if (removedFromDelayAdd)
                return;
            if (std::find(_delayRemoveJoints.rbegin(), _delayRemoveJoints.rend(), joint) == _delayRemoveJoints.rend())
            {
                _delayRemoveJoints.emplace_back(joint);
            }
        }
        else
        {
            doRemoveJoint(joint);
        }
    }
}

void PhysicsWorld::updateJoints()
{
    if (_isWorldLocked)
    {
        return;
    }

    for (auto&& joint : _delayAddJoints)
    {
        joint->_world = this;
        if (joint->initJoint())
        {
            _joints.emplace_back(joint);
        }
        else
        {
            delete joint;
        }
    }
    _delayAddJoints.clear();

    for (auto&& joint : _delayRemoveJoints)
    {
        doRemoveJoint(joint);
    }
    _delayRemoveJoints.clear();

    for (auto&& joint : _joints)
    {
        joint->flushDelayTasks();
    }
}

void PhysicsWorld::addJoint(PhysicsJoint* joint)
{
    if (joint)
    {
        AXASSERT(joint->getWorld() == nullptr, "Can not add joint already add to other world!");

        joint->_world = this;
        auto it       = std::find(_delayRemoveJoints.begin(), _delayRemoveJoints.end(), joint);
        if (it != _delayRemoveJoints.end())
        {
            _delayRemoveJoints.erase(it);
            return;
        }

        if (std::find(_delayAddJoints.begin(), _delayAddJoints.end(), joint) == _delayAddJoints.end())
        {
            _delayAddJoints.emplace_back(joint);
        }
    }
}

void PhysicsWorld::removeAllJoints(bool destroy)
{
    auto removeCopy = _joints;
    for (auto&& joint : removeCopy)
    {
        removeJoint(joint, destroy);
    }
}

void PhysicsWorld::doRemoveBody(PhysicsBody* body)
{
    AXASSERT(body != nullptr, "the body can not be nullptr");

    // remove shapes
    // for (auto&& shape : body->getShapes())
    //{
    //    removeShape(shape);
    //}

    // TODO: body->removeSelf

    // remove body
    // if (cpSpaceContainsBody(_cpSpace, body->_cpBody))
    // {
    //     cpSpaceRemoveBody(_cpSpace, body->_cpBody);
    // }

    body->deatchFromWOrld();
}

void PhysicsWorld::doRemoveJoint(PhysicsJoint* joint)
{
    // TODO: joint->removeSelf
    for (auto&& constraint : joint->_b2Joints)
    {
        // cpSpaceRemoveConstraint(_cpSpace, constraint);
    }
    _joints.remove(joint);
    joint->_world = nullptr;

    if (joint->getBodyA())
    {
        joint->getBodyA()->removeJoint(joint);
    }

    if (joint->getBodyB())
    {
        joint->getBodyB()->removeJoint(joint);
    }

    if (joint->_destroyMark)
    {
        delete joint;
    }
}

void PhysicsWorld::removeAllBodies()
{
    for (auto&& child : _bodies)
    {
        removeBodyOrDelay(child);
        child->_world = nullptr;
    }

    _bodies.clear();
}

const Vector<PhysicsBody*>& PhysicsWorld::getAllBodies() const
{
    return _bodies;
}

PhysicsBody* PhysicsWorld::getBody(int tag) const
{
    for (auto&& body : _bodies)
    {
        if (body->getTag() == tag)
        {
            return body;
        }
    }

    return nullptr;
}

void PhysicsWorld::setGravity(const Vec2& gravity)
{
    _gravity = gravity;
    b2World_SetGravity(_b2World, phlp::tob2Vec2(_gravity * _PTMRatio));
}

void PhysicsWorld::setSlopBias(float slop, float bias)
{
    // NOTE: NOT SUPPORT
    // cpSpaceSetCollisionSlop(_cpSpace, slop);
    // cpSpaceSetCollisionBias(_cpSpace, bias);
}

void PhysicsWorld::setSubsteps(int steps)
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

void PhysicsWorld::step(float delta)
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

void PhysicsWorld::update(float delta, bool userCall /* = false*/)
{

    if (_preUpdateCallback)
        _preUpdateCallback();  // fix #11154

    if (!_delayAddBodies.empty())
    {
        updateBodies();
    }
    else if (!_delayRemoveBodies.empty())
    {
        updateBodies();
    }

    auto sceneToWorldTransform = _scene->getNodeToParentTransform();
    beforeSimulation(_scene, sceneToWorldTransform, 1.f, 1.f, 0.f);

    if (!_delayAddJoints.empty() || !_delayRemoveJoints.empty())
    {
        updateJoints();
    }

    if (delta < FLT_EPSILON)
    {
        return;
    }

    if (userCall)
    {
        b2World_Step(_b2World, delta, 4);
    }
    else
    {
        _updateTime += delta;
        if (_fixedRate)
        {
            const float step = 1.0f / _fixedRate;
            const float dt   = step * _speed;
            while (_updateTime > step)
            {
                _updateTime -= step;
                for (auto&& body : _bodies)
                {
                    body->fixedUpdate(dt);
                }
                _scene->fixedUpdate(dt);

                b2World_Step(_b2World, dt, 1);
            }
        }
        else
        {
            if (++_updateRateCount >= _updateRate)
            {
                const float dt = _updateTime * _speed / _substeps;
                b2World_Step(_b2World, 1 / 60.f, _substeps);
                _updateRateCount = 0;
                _updateTime      = 0.0f;
            }

            _updateRateCount = 0;
            _updateTime      = 0.0f;
        }
    }

    // Update physics position, should loop as the same sequence as node tree.
    // PhysicsWorld::afterSimulation() will depend on the sequence.
    afterSimulation(_scene, sceneToWorldTransform, 0.f);

    if (_postUpdateCallback)
        _postUpdateCallback();  // fix #11154
}

PhysicsWorld* PhysicsWorld::construct(Scene* scene)
{
    PhysicsWorld* world = new PhysicsWorld();
    if (world->init())
    {
        world->_scene           = scene;
        world->_eventDispatcher = scene->getEventDispatcher();
        return world;
    }

    AX_SAFE_DELETE(world);
    return nullptr;
}

PhysicsWorld::PhysicsWorld()
    : _gravity(Vec2(0.0f, -9.8f))
    , _PTMRatio(10.0f)
    , _speed(1.0f)
    , _updateRate(1)
    , _updateRateCount(0)
    , _updateTime(0.0f)
    , _substeps(1)
    , _fixedRate(0)
    , _b2World(b2_nullWorldId)
    , _updateBodyTransform(false)
    , _scene(nullptr)
    , _autoStep(true)
    , _eventDispatcher(nullptr)
{}

PhysicsWorld::~PhysicsWorld()
{
    removeAllJoints(true);
    removeAllBodies();
    if (b2World_IsValid(_b2World))
    {
        b2DestroyWorld(_b2World);
    }
}

void PhysicsWorld::beforeSimulation(Node* node,
                                    const Mat4& parentToWorldTransform,
                                    float nodeParentScaleX,
                                    float nodeParentScaleY,
                                    float parentRotation)
{
    auto scaleX   = nodeParentScaleX * node->getScaleX();
    auto scaleY   = nodeParentScaleY * node->getScaleY();
    auto rotation = parentRotation + node->getRotation();

    auto nodeToWorldTransform = parentToWorldTransform * node->getNodeToParentTransform();

    auto physicsBody = node->getPhysicsBody();
    if (physicsBody)
    {
        physicsBody->beforeSimulation(parentToWorldTransform, nodeToWorldTransform, scaleX, scaleY, rotation);
    }

    for (auto&& child : node->getChildren())
        beforeSimulation(child, nodeToWorldTransform, scaleX, scaleY, rotation);
}

void PhysicsWorld::afterSimulation(Node* node, const Mat4& parentToWorldTransform, float parentRotation)
{
    auto nodeToWorldTransform = parentToWorldTransform * node->getNodeToParentTransform();
    auto nodeRotation         = parentRotation + node->getRotation();

    auto physicsBody = node->getPhysicsBody();
    if (physicsBody)
    {
        physicsBody->afterSimulation(parentToWorldTransform, parentRotation);
    }

    for (auto&& child : node->getChildren())
        afterSimulation(child, nodeToWorldTransform, nodeRotation);
}

void PhysicsWorld::setPostUpdateCallback(const std::function<void()>& callback)
{
    _postUpdateCallback = callback;
}

void PhysicsWorld::setPreUpdateCallback(const std::function<void()>& callback)
{
    _preUpdateCallback = callback;
}

}  // namespace ax

#endif  // defined(AX_ENABLE_PHYSICS)
