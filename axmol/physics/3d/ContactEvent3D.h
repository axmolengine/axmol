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

#if defined(AX_ENABLE_PHYSICS_3D)

#    include "axmol/base/Object.h"
#    include "axmol/math/Math.h"
#    include "axmol/base/EventListenerCustom.h"
#    include "axmol/base/Event.h"
#    include "axmol/base/EventCustom.h"

#    include "yasio/object_pool.hpp"

#    include "axmol/physics/PhysicsActor.h"
#    include "axmol/tlx/inlined_vector.hpp"
#    include <functional>
#    include <vector>

namespace ax
{
class Collider3D;
class Rigidbody3D;
class PhysicsWorld3D;

struct ContactInfo3D
{
    PhysicsActor* actorA{nullptr};
    PhysicsActor* actorB{nullptr};

    // Contact normal in world coordinates from shape A toward shape B.
    Vec3 normal;

    // Contact points in world coordinates.
    struct ContactPoint
    {
        // Contact points on the surface of actor A
        Vec3 pointA;

        // Contact points on the surface of actor B
        Vec3 pointB;
    };

    tlx::inlined_vector<ContactPoint, 4> points;
};

/**
 * @addtogroup physics
 * @{
 * @addtogroup physics_3d
 * @{
 */

/**
 * @brief Contact information.

 * It will created automatically when two shape contact with each other. And it will destroyed automatically when two
 shape separated.
 */
class AX_DLL ContactEvent3D : public EventCustom
{
    friend class ContactListener;
    friend class ContactEventListener3D;
    friend class PhysicsWorld3D;

#    ifndef _AX_GEN_SCRIPT_BINDINGS
    DEFINE_CONCURRENT_OBJECT_POOL_ALLOCATION(ContactEvent3D, 128)
#    endif

public:
    enum class EventCode
    {
        None,

        /** Triggered when two actors first start touching (Asynchronous). */
        ContactBegin,

        /** Triggered when two actors stop touching (Asynchronous). */
        ContactEnd,

        /**
         * Triggered after Jolt reports an active non-sensor contact pair for this step.
         * This is the closest 3D equivalent to the 2D hit callback.
         */
        CollisionHit,

        /** Triggered when a shape enters a sensor's detection area. */
        SensorBegin,

        /** Triggered when a shape leaves a sensor's detection area. */
        SensorEnd,
    };

    /** Get contact actor A. */
    PhysicsActor* getActorA() const { return _contactInfo.actorA; }

    /** Get contact actor B. */
    PhysicsActor* getActorB() const { return _contactInfo.actorB; }
    /** Get the event code */
    EventCode getEventCode() const { return _eventCode; };

    const ContactInfo3D& getContactInfo() const { return _contactInfo; }

private:
    static ContactEvent3D* obtain(ContactInfo3D& info);
    bool init(ContactInfo3D& info);

    void setEventCode(EventCode eventCode) { _eventCode = eventCode; };
    void setResult(bool result) { _result = result; }
    bool resetResult()
    {
        bool ret = _result;
        _result  = true;
        return ret;
    }

private:
    ContactEvent3D();

private:
    EventCode _eventCode{EventCode::None};
    ContactInfo3D _contactInfo;
    bool _result{true};
};

/** Contact listener. It will receive all the contact callbacks. */
class AX_DLL ContactEventListener3D : public EventListenerCustom
{
public:
    /** Create the listener. */
    static ContactEventListener3D* create();
    /** Check the listener is available.

     * @return True if there's one available callback function at least, false if there's no one.
     */
    bool checkAvailable() override;

    /** Clone an object from this listener.*/
    ContactEventListener3D* clone() override;

public:
    /**
     * @brief Two shapes are touching during this step. Return false from the callback to make world ignore the
     * collision this step or true to process it normally.
     */
    std::function<bool(ContactEvent3D* event)> onContactFilter = [](ContactEvent3D*) { return true; };

    std::function<void(ContactEvent3D* event)> onContactBegin;

    std::function<void(ContactEvent3D* event)> onContactEnd;

    /**
     * @brief Two shapes are touching and their collision response has been processed. You can retrieve the collision
     * impulse or kinetic energy at this time if you want to use it to calculate sound volumes or damage amounts.
     */
    std::function<void(ContactEvent3D* event)> onCollisionHit;

    std::function<void(ContactEvent3D* event)> onSensorBegin;

    std::function<void(ContactEvent3D* event)> onSensorEnd;

protected:
    bool init();
    void onEvent(EventCustom* event);

    friend class PhysicsWorld3D;
};

}  // namespace ax

#endif
