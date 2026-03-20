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

namespace ax
{

class Collider2D;
class Rigidbody2D;
class PhysicsWorld2D;

typedef struct AX_DLL PhysicsContactData
{
    static const int POINT_MAX = 4;
    Vec2 points[POINT_MAX];
    int count;
    Vec2 normal;

    PhysicsContactData() : count(0) {}
} PhysicsContactData;

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
public:
    enum class EventCode
    {
        NONE,
        BEGIN,
        PRESOLVE,
        POSTSOLVE,
        SEPARATE
    };

    /** Get contact shape A. */
    Collider2D* getColliderA() const { return _colliderA; }

    /** Get contact shape B. */
    Collider2D* getColliderB() const { return _colliderB; }

    /** Get contact data. */
    const PhysicsContactData* getContactData() const { return _contactData; }

    /** Get previous contact data */
    const PhysicsContactData* getPreContactData() const { return _preContactData; }

    /**
     * Get data.
     * @lua NA
     */
    void* getData() const { return _data; }

    /**
     * @brief Set data to contact.

     * You must manage the memory yourself, Generally you can set data at contact begin, and destroy it at contact
     separate.
     *
     * @lua NA
     */
    void setData(void* data) { _data = data; }

    /** Get the event code */
    EventCode getEventCode() const { return _eventCode; };

private:
    static Contact2D* obtain(Collider2D* a, Collider2D* b);
    bool init(Collider2D* a, Collider2D* b);

    void setEventCode(EventCode eventCode) { _eventCode = eventCode; };
    bool isNotificationEnabled() const { return _notificationEnable; }
    void setNotificationEnable(bool enable) { _notificationEnable = enable; }
    PhysicsWorld2D* getWorld() const { return _world; }
    void setWorld(PhysicsWorld2D* world) { _world = world; }
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
    PhysicsWorld2D* _world;
    Collider2D* _colliderA;
    Collider2D* _colliderB;
    EventCode _eventCode;
    bool _notificationEnable;
    bool _result;

    void* _data;
    void* _contactInfo;
    PhysicsContactData* _contactData;
    PhysicsContactData* _preContactData;

    friend class EventListenerPhysicsContact;
    friend struct PhysicsWorldCallback;
    friend class PhysicsWorld2D;
};

/**
 * @brief Presolve value generated when onContactPreSolve called.
 */
class AX_DLL PhysicsContactPreSolve
{
public:
    /** Get restitution between two bodies.*/
    float getRestitution() const;
    /** Get friction between two bodies.*/
    float getFriction() const;
    /** Get surface velocity between two bodies.*/
    Vec2 getSurfaceVelocity() const;
    /** Set the restitution.*/
    void setRestitution(float restitution);
    /** Set the friction.*/
    void setFriction(float friction);
    /** Set the surface velocity.*/
    void setSurfaceVelocity(const Vec2& velocity);
    /** Ignore the rest of the contact presolve and postsolve callbacks. */
    void ignore();

private:
    PhysicsContactPreSolve(void* contactInfo);
    ~PhysicsContactPreSolve();

private:
    void* _contactInfo;

    friend class EventListenerPhysicsContact;
};

/**
 * @brief Postsolve value generated when onContactPostSolve called.
 */
class AX_DLL PhysicsContactPostSolve
{
public:
    /** Get restitution between two bodies.*/
    float getRestitution() const;
    /** Get friction between two bodies.*/
    float getFriction() const;
    /** Get surface velocity between two bodies.*/
    Vec2 getSurfaceVelocity() const;

private:
    PhysicsContactPostSolve(void* contactInfo);
    ~PhysicsContactPostSolve();

private:
    void* _contactInfo;

    friend class EventListenerPhysicsContact;
};

/** Contact listener. It will receive all the contact callbacks. */
class AX_DLL EventListenerPhysicsContact : public EventListenerCustom
{
public:
    /** Create the listener. */
    static EventListenerPhysicsContact* create();

    /** Check the listener is available.

     * @return True if there's one available callback function at least, false if there's no one.
     */
    bool checkAvailable() override;

    /** Clone an object from this listener.*/
    EventListenerPhysicsContact* clone() override;

protected:
    /**
     * It will be call when two body have contact.
     * if return false, it will not invoke callbacks.
     */
    virtual bool hitTest(Collider2D* shapeA, Collider2D* shapeB);

public:
    /**
     * @brief It will called at two shapes start to contact, and only call it once.
     */
    std::function<bool(Contact2D& contact)> onContactBegin;
    /**
     * @brief Two shapes are touching during this step. Return false from the callback to make world ignore the
     * collision this step or true to process it normally. Additionally, you may override collision values, restitution,
     * or surface velocity values.
     */
    std::function<bool(Contact2D& contact, PhysicsContactPreSolve& solve)> onContactPreSolve;
    /**
     * @brief Two shapes are touching and their collision response has been processed. You can retrieve the collision
     * impulse or kinetic energy at this time if you want to use it to calculate sound volumes or damage amounts. See
     * cpArbiter for more info
     */
    std::function<void(Contact2D& contact, const PhysicsContactPostSolve& solve)> onContactPostSolve;
    /**
     * @brief It will called at two shapes separated, and only call it once.
     * onContactBegin and onContactSeparate will called in pairs.
     */
    std::function<void(Contact2D& contact)> onContactSeparate;

protected:
    bool init();
    void onEvent(EventCustom* event);

protected:
    EventListenerPhysicsContact();
    virtual ~EventListenerPhysicsContact();

    friend class PhysicsWorld2D;
};

/** This event listener only be called when bodyA and bodyB have contacts. */
class AX_DLL EventListenerPhysicsContactWithBodies : public EventListenerPhysicsContact
{
public:
    /** Create the listener. */
    static EventListenerPhysicsContactWithBodies* create(Rigidbody2D* bodyA, Rigidbody2D* bodyB);

    bool hitTest(Collider2D* shapeA, Collider2D* shapeB) override;

    EventListenerPhysicsContactWithBodies* clone() override;

protected:
    Rigidbody2D* _a;
    Rigidbody2D* _b;

protected:
    EventListenerPhysicsContactWithBodies();
    virtual ~EventListenerPhysicsContactWithBodies();
};

/** This event listener only be called when shapeA and shapeB have contacts. */
class AX_DLL EventListenerPhysicsContactWithShapes : public EventListenerPhysicsContact
{
public:
    /** Create the listener. */
    static EventListenerPhysicsContactWithShapes* create(Collider2D* shapeA, Collider2D* shapeB);

    bool hitTest(Collider2D* shapeA, Collider2D* shapeB) override;
    EventListenerPhysicsContactWithShapes* clone() override;

protected:
    Collider2D* _a;
    Collider2D* _b;

protected:
    EventListenerPhysicsContactWithShapes();
    virtual ~EventListenerPhysicsContactWithShapes();
};

/** This event listener only be called when shapeA or shapeB is in the group your specified */
class AX_DLL EventListenerPhysicsContactWithGroup : public EventListenerPhysicsContact
{
public:
    /** Create the listener. */
    static EventListenerPhysicsContactWithGroup* create(int group);

    bool hitTest(Collider2D* shapeA, Collider2D* shapeB) override;
    EventListenerPhysicsContactWithGroup* clone() override;

protected:
    int _group;

protected:
    EventListenerPhysicsContactWithGroup();
    virtual ~EventListenerPhysicsContactWithGroup();
};

/** @} */
/** @} */

}  // namespace ax

#endif  // defined(AX_ENABLE_PHYSICS_2D)
