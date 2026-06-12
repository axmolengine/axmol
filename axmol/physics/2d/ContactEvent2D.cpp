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
#include "axmol/physics/2d/ContactEvent2D.h"
#if defined(AX_ENABLE_PHYSICS_2D)

#    include "axmol/physics/2d/Rigidbody2D.h"
#    include "axmol/physics/2d/PhysicsUtility2D.h"
#    include "axmol/base/CustomEvent.h"

namespace ax
{

const char* CONTACT_2D_EVENT_NAME = "contact-2d";

ContactEvent2D::ContactEvent2D() : CustomEvent(CONTACT_2D_EVENT_NAME), _eventCode(EventCode::None), _result(true) {}

ContactEvent2D::~ContactEvent2D() {}

ContactEvent2D* ContactEvent2D::obtain(Collider2D* a, Collider2D* b)
{
    ContactEvent2D* contact = new ContactEvent2D();
    if (contact->init(a, b))
    {
        return contact;
    }

    AX_SAFE_DELETE(contact);
    return nullptr;
}

bool ContactEvent2D::init(Collider2D* a, Collider2D* b)
{
    do
    {
        AX_BREAK_IF(a == nullptr || b == nullptr);

        _contactInfo.colliderA = a;
        _contactInfo.colliderB = b;

        return true;
    } while (false);

    return false;
}

void ContactEvent2D::generateContactData()
{
    if (!_contactInfo.colliderA)
        return;

    // simple get 1 contact from bodyA
    auto body = _contactInfo.colliderA->getAttachedBody();
    if (!body->isAttached())
        return;

    auto bodyId       = body->internalHandle();
    auto contactCount = b2Body_GetContactCapacity(bodyId);
    if (contactCount <= 0)
        return;

    b2ContactData contactData;
    if (b2Body_GetContactData(bodyId, &contactData, 1) != 1)
        return;

    _contactInfo.normal     = b2util::cast(contactData.manifold.normal);
    _contactInfo.pointCount = contactData.manifold.pointCount;
    for (auto i = 0; i < contactData.manifold.pointCount && i < ContactInfo2D::POINT_MAX; ++i)
    {
        _contactInfo.points[i].point          = b2util::cast(contactData.manifold.points[i].clipPoint);
        _contactInfo.points[i].normalImpulse  = contactData.manifold.points[i].normalImpulse;
        _contactInfo.points[i].tangentImpulse = contactData.manifold.points[i].tangentImpulse;
        _contactInfo.points[i].normalVelocity = contactData.manifold.points[i].normalVelocity;
    }
}

#    pragma region contact 2d listeners

ContactEventListener2D* ContactEventListener2D::create()
{
    ContactEventListener2D* obj = new ContactEventListener2D();

    if (obj->init())
    {
        obj->autorelease();
        return obj;
    }

    AX_SAFE_DELETE(obj);
    return nullptr;
}

ContactEventListener2D* ContactEventListener2D::create(Rigidbody2D* bodyA, Rigidbody2D* bodyB)
{
    ContactEventListener2D* obj = new ContactEventListener2D(bodyA, bodyB);

    if (obj->init())
    {
        obj->autorelease();
        return obj;
    }

    AX_SAFE_DELETE(obj);
    return nullptr;
}

ContactEventListener2D::ContactEventListener2D()
{
    onContactFilter = [](ContactEvent2D*) { return true; };
}
ContactEventListener2D::ContactEventListener2D(Rigidbody2D* bodyA, Rigidbody2D* bodyB)
{
    onContactFilter = [bodyA, bodyB](ContactEvent2D* event) {
        auto colliderA = event->getColliderA();
        auto colliderB = event->getColliderB();
        return (colliderA->getAttachedBody() == bodyA && colliderB->getAttachedBody() == bodyB) ||
               (colliderA->getAttachedBody() == bodyB && colliderB->getAttachedBody() == bodyA);
    };
}

ContactEventListener2D::~ContactEventListener2D()
{
    onContactFilter = nullptr;
}

bool ContactEventListener2D::init()
{
    auto func = [this](CustomEvent* event) -> void { onEvent(event); };

    return CustomEventListener::init(CONTACT_2D_EVENT_NAME, func);
}

void ContactEventListener2D::onEvent(CustomEvent* event)
{
    ContactEvent2D* contactEvent = dynamic_cast<ContactEvent2D*>(event);

    if (contactEvent == nullptr)
    {
        return;
    }

    switch (contactEvent->getEventCode())
    {
    case ContactEvent2D::EventCode::ContactBegin:
    {
        if (onContactBegin && onContactFilter(contactEvent))
        {
            contactEvent->generateContactData();
            onContactBegin(contactEvent);
        }
        break;
    }
    case ContactEvent2D::EventCode::ContactEnd:
    {
        if (onContactEnd && onContactFilter(contactEvent))
        {
            contactEvent->generateContactData();
            onContactEnd(contactEvent);
        }
        break;
    }
    case ContactEvent2D::EventCode::CollisionHit:
    {
        if (onCollisionHit && onContactFilter(contactEvent))
        {
            contactEvent->generateContactData();
            onCollisionHit(contactEvent);
        }
        break;
    }
    case ContactEvent2D::EventCode::SensorBegin:
    {
        if (onSensorBegin && onContactFilter(contactEvent))
        {
            onSensorBegin(contactEvent);
        }
        break;
    }
    case ContactEvent2D::EventCode::SensorEnd:
    {
        if (onSensorEnd && onContactFilter(contactEvent))
        {
            onSensorEnd(contactEvent);
        }
        break;
    }
    default:
        break;
    }
}

bool ContactEventListener2D::checkAvailable()
{
    if (!onContactFilter || (!onContactBegin && !onContactEnd && !onCollisionHit && !onSensorBegin && !onSensorEnd))
    {
        AXASSERT(false, "Invalid ContactEventListener2D.");
        return false;
    }

    return true;
}

ContactEventListener2D* ContactEventListener2D::clone()
{
    ContactEventListener2D* obj = ContactEventListener2D::create();

    if (obj != nullptr)
    {
        obj->onContactFilter = onContactFilter;
        obj->onContactBegin  = onContactBegin;
        obj->onContactEnd    = onContactEnd;
        obj->onCollisionHit  = onCollisionHit;
        obj->onSensorBegin   = onSensorBegin;
        obj->onSensorEnd     = onSensorEnd;

        return obj;
    }

    AX_SAFE_DELETE(obj);
    return nullptr;
}

#    pragma endregion

}  // namespace ax
#endif  // defined(AX_ENABLE_PHYSICS_2D)
