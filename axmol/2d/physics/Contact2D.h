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

#pragma once

#include "axmol/base/Config.h"
#if defined(AX_ENABLE_PHYSICS_2D)

#    include "axmol/base/Object.h"
#    include "axmol/math/Math.h"
#    include "axmol/base/EventListenerCustom.h"
#    include "axmol/base/Event.h"
#    include "axmol/base/EventCustom.h"

#    include "yasio/object_pool.hpp"

namespace ax
{
class Collider2D;
class Rigidbody2D;
class PhysicsWorld2D;

struct ManifoldPoint2D
{
    Vec2 point;

    /// The impulse along the manifold normal vector.
    float normalImpulse{0};

    /// The friction impulse
    float tangentImpulse{0};

    /// Relative normal velocity pre-solve. Used for hit events. If the normal impulse is
    /// zero then there was no hit. Negative means shapes are approaching.
    float normalVelocity{0};
};

struct Contact2DInfo
{
    static constexpr int POINT_MAX = 2;  // Box2D 2D manifold supports up to 2 points

    // Contact points in world coordinates.
    // Each entry represents a collision point between two shapes.
    // Simple collisions usually generate one point, while polygon vs polygon
    // contacts may produce two points.
    ManifoldPoint2D points[POINT_MAX];
    int pointCount{0};

    // Contact normal in world coordinates.
    // Box2D convention: the normal always points from shape A toward shape B.
    // Chipmunk convention: the normal points toward the first shape passed to the Arbiter.
    // This means the same collision may yield opposite normal directions depending on engine.
    // In Axmol we unify this by returning the Box2D normal (A->B), but contributors should be
    // aware of the difference when porting logic from Chipmunk.
    Vec2 normal;
};

/**
 * @addtogroup physics
 * @{
 * @addtogroup physics_2d
 * @{
 */

/**
 * @brief Contact information.

 * It will created automatically when two shape contact with each other. And it will destroyed automatically when two
 shape separated.
 */
class AX_DLL Contact2D : public EventCustom
{
    friend class Contact2DListener;
    friend class PhysicsWorld2D;

#    ifndef _AX_GEN_SCRIPT_BINDINGS
    DEFINE_CONCURRENT_OBJECT_POOL_ALLOCATION(Contact2D, 128)
#    endif

public:
    enum class EventCode
    {
        None,

        /**
         * Synchronous callback triggered before the solver runs.
         * Allows for manual contact modification (e.g., disabling collisions).
         */
        PreSolve,

        /** Triggered when two shapes first start touching (Asynchronous). */
        ContactBegin,

        /** Triggered when two shapes stop touching (Asynchronous). */
        ContactEnd,

        /**
         * Triggered on high-intensity impacts with impulse data (Asynchronous).
         * Requires b2Shape_EnableHitEvents and a world hit threshold.
         */
        CollisionHit,

        /** Triggered when a shape enters a sensor's detection area. */
        SensorBegin,

        /** Triggered when a shape leaves a sensor's detection area. */
        SensorEnd,
    };

    /** Get contact shape A. */
    Collider2D* getColliderA() const { return _colliderA; }

    /** Get contact shape B. */
    Collider2D* getColliderB() const { return _colliderB; }

    /** Get the event code */
    EventCode getEventCode() const { return _eventCode; };

    const Contact2DInfo& getContactInfo() const { return _contactInfo; }

private:
    static Contact2D* obtain(Collider2D* a, Collider2D* b);
    bool init(Collider2D* a, Collider2D* b);

    void setPointNormal(const Vec2& point, const Vec2& normal)
    {
        _contactInfo.points[0].point = point;
        _contactInfo.pointCount      = 1;
        _contactInfo.normal          = normal;
    }
    void setEventCode(EventCode eventCode) { _eventCode = eventCode; };
    void setResult(bool result) { _result = result; }
    bool resetResult()
    {
        bool ret = _result;
        _result  = true;
        return ret;
    }

    void generateContactData();

private:
    Contact2D();
    ~Contact2D();

private:
    Collider2D* _colliderA;
    Collider2D* _colliderB;
    EventCode _eventCode;
    Contact2DInfo _contactInfo;
    bool _result;
};

/** Contact listener. It will receive all the contact callbacks. */
class AX_DLL Contact2DListener : public EventListenerCustom
{
public:
    /** Create the listener. */
    static Contact2DListener* create();

    /** Check the listener is available.

     * @return True if there's one available callback function at least, false if there's no one.
     */
    bool checkAvailable() override;

    /** Clone an object from this listener.*/
    Contact2DListener* clone() override;

protected:
    /**
     * It will be call when two body have contact.
     * if return false, it will not invoke callbacks.
     */
    virtual bool hitTest(Collider2D* shapeA, Collider2D* shapeB);

public:
    /**
     * @brief Two shapes are touching during this step. Return false from the callback to make world ignore the
     * collision this step or true to process it normally. Additionally, you may override collision values, restitution,
     * or surface velocity values.
     */
    std::function<bool(Contact2D* contact)> onPreSolve;

    std::function<void(Contact2D* contact)> onContactBegin;

    /**
     * @brief It will called at two shapes separated, and only call it once.
     * onContactBegin and onContactSeparate will called in pairs.
     */
    std::function<void(Contact2D* contact)> onContactEnd;

    /**
     * @brief Two shapes are touching and their collision response has been processed. You can retrieve the collision
     * impulse or kinetic energy at this time if you want to use it to calculate sound volumes or damage amounts.
     */
    std::function<void(Contact2D* contact)> onCollisionHit;

    std::function<void(Contact2D* contact)> onSensorBegin;

    std::function<void(Contact2D* contact)> onSensorEnd;

protected:
    bool init();
    void onEvent(EventCustom* event);

protected:
    Contact2DListener();
    virtual ~Contact2DListener();

    friend class PhysicsWorld2D;
};

/** This event listener only be called when bodyA and bodyB have contacts. */
class AX_DLL Contact2DListenerWithBodies : public Contact2DListener
{
public:
    /** Create the listener. */
    static Contact2DListenerWithBodies* create(Rigidbody2D* bodyA, Rigidbody2D* bodyB);

    bool hitTest(Collider2D* shapeA, Collider2D* shapeB) override;

    Contact2DListenerWithBodies* clone() override;

protected:
    Rigidbody2D* _a;
    Rigidbody2D* _b;

protected:
    Contact2DListenerWithBodies();
    virtual ~Contact2DListenerWithBodies();
};

/** This event listener only be called when shapeA and shapeB have contacts. */
class AX_DLL Contact2DListenerWithShapes : public Contact2DListener
{
public:
    /** Create the listener. */
    static Contact2DListenerWithShapes* create(Collider2D* shapeA, Collider2D* shapeB);

    bool hitTest(Collider2D* shapeA, Collider2D* shapeB) override;
    Contact2DListenerWithShapes* clone() override;

protected:
    Collider2D* _a;
    Collider2D* _b;

protected:
    Contact2DListenerWithShapes();
    virtual ~Contact2DListenerWithShapes();
};

/** This event listener only be called when shapeA or shapeB is in the group your specified */
class AX_DLL Contact2DListenerWithGroup : public Contact2DListener
{
public:
    /** Create the listener. */
    static Contact2DListenerWithGroup* create(int group);

    bool hitTest(Collider2D* shapeA, Collider2D* shapeB) override;
    Contact2DListenerWithGroup* clone() override;

protected:
    int _group;

protected:
    Contact2DListenerWithGroup();
    virtual ~Contact2DListenerWithGroup();
};

/** @} */
/** @} */

}  // namespace ax

#endif  // defined(AX_ENABLE_PHYSICS_2D)
