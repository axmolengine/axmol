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

struct ContactInfo2D
{
    static constexpr int POINT_MAX = 2;  // Box2D 2D manifold supports up to 2 points

    Collider2D* colliderA{nullptr};
    Collider2D* colliderB{nullptr};

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

    void setPointAndNormal(const Vec2& p, const Vec2& norm)
    {
        this->pointCount      = 1;
        this->points[0].point = p;

        this->normal = norm;
    }
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
class AX_DLL ContactEvent2D : public EventCustom
{
    friend class ContactEventListener2D;
    friend class PhysicsWorld2D;

#    ifndef _AX_GEN_SCRIPT_BINDINGS
    DEFINE_CONCURRENT_OBJECT_POOL_ALLOCATION(ContactEvent2D, 128)
#    endif

public:
    enum class EventCode
    {
        None,

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
    Collider2D* getColliderA() const { return _contactInfo.colliderA; }

    /** Get contact shape B. */
    Collider2D* getColliderB() const { return _contactInfo.colliderB; }

    /** Get the event code */
    EventCode getEventCode() const { return _eventCode; };

    const ContactInfo2D& getContactInfo() const { return _contactInfo; }

private:
    static ContactEvent2D* obtain(Collider2D* a, Collider2D* b);
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
    ContactEvent2D();
    ~ContactEvent2D();

private:
    EventCode _eventCode;
    ContactInfo2D _contactInfo;
    bool _result;
};

/** Contact listener. It will receive all the contact callbacks. */
class AX_DLL ContactEventListener2D : public EventListenerCustom
{
public:
    /** Create the listener. */
    static ContactEventListener2D* create();

    /*
     * Create the listener with two rigidbodies. It will only receive contact events between these two bodies.
     */
    static ContactEventListener2D* create(Rigidbody2D* bodyA, Rigidbody2D* bodyB);

    /** Check the listener is available.

     * @return True if there's one available callback function at least, false if there's no one.
     */
    bool checkAvailable() override;

    /** Clone an object from this listener.*/
    ContactEventListener2D* clone() override;

public:
    /**
     * @brief Callback to filter contact events.
     * Return true to allow the event to be processed,
     * or false to ignore it and skip subsequent callbacks.
     */
    std::function<bool(ContactEvent2D* event)> onContactFilter;

    /**
     * @brief Called when two shapes begin to touch.
     */
    std::function<void(ContactEvent2D* event)> onContactBegin;

    /**
     * @brief Called when two shapes separate. This is only called once.
     * onContactBegin and onContactEnd are always invoked in pairs.
     */
    std::function<void(ContactEvent2D* event)> onContactEnd;

    /**
     * @brief Called when two shapes are touching and their collision
     * response has been processed. At this point you can retrieve
     * collision impulses or kinetic energy, useful for calculating
     * sound volumes or damage amounts.
     */
    std::function<void(ContactEvent2D* event)> onCollisionHit;

    /**
     * @brief Called when a sensor begins detecting another shape.
     */
    std::function<void(ContactEvent2D* event)> onSensorBegin;

    /**
     * @brief Called when a sensor stops detecting another shape.
     */
    std::function<void(ContactEvent2D* event)> onSensorEnd;

protected:
    bool init();
    void onEvent(EventCustom* event);

protected:
    ContactEventListener2D();
    ContactEventListener2D(Rigidbody2D* bodyA, Rigidbody2D* bodyB);
    virtual ~ContactEventListener2D();

    friend class PhysicsWorld2D;
};

/** @} */
/** @} */

}  // namespace ax

#endif  // defined(AX_ENABLE_PHYSICS_2D)
