/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#include "axmol/physics/3d/ContactEvent3D.h"

#if defined(AX_ENABLE_PHYSICS_3D)

#    include <utility>

namespace ax
{
constexpr std::string_view CONTACT_3D_EVENT_NAME = "contact-3d"sv;

ContactEvent3D* ContactEvent3D::obtain(ContactInfo3D& info)
{
    ContactEvent3D* contact = new ContactEvent3D();
    if (contact->init(info))
        return contact;

    AX_SAFE_DELETE(contact);
    return nullptr;
}

ContactEvent3D::ContactEvent3D() : CustomEvent(CONTACT_3D_EVENT_NAME) {}

bool ContactEvent3D::init(ContactInfo3D& info)
{
    if (!info.actorA || !info.actorB)
        return false;

    _contactInfo = std::move(info);
    return true;
}

ContactEventListener3D* ContactEventListener3D::create()
{
    auto obj = new ContactEventListener3D();
    if (obj->init())
    {
        obj->autorelease();
        return obj;
    }

    delete obj;
    return nullptr;
}

bool ContactEventListener3D::checkAvailable()
{
    if (!onContactFilter || (!onContactBegin && !onContactEnd && !onCollisionHit && !onSensorBegin && !onSensorEnd))
    {
        AXASSERT(false, "Invalid ContactEventListener3D.");
        return false;
    }

    return true;
}

ContactEventListener3D* ContactEventListener3D::clone()
{
    ContactEventListener3D* obj = ContactEventListener3D::create();
    if (obj)
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

void ContactEventListener3D::onEvent(CustomEvent* event)
{
    auto* contactEvent = static_cast<ContactEvent3D*>(event);
    if (!contactEvent)
        return;

    switch (contactEvent->getEventCode())
    {
    case ContactEvent3D::EventCode::ContactBegin:
        if (onContactBegin && onContactFilter(contactEvent))
            onContactBegin(contactEvent);
        break;
    case ContactEvent3D::EventCode::ContactEnd:
        if (onContactEnd && onContactFilter(contactEvent))
            onContactEnd(contactEvent);
        break;
    case ContactEvent3D::EventCode::CollisionHit:
        if (onCollisionHit && onContactFilter(contactEvent))
            onCollisionHit(contactEvent);
        break;
    case ContactEvent3D::EventCode::SensorBegin:
        if (onSensorBegin && onContactFilter(contactEvent))
            onSensorBegin(contactEvent);
        break;
    case ContactEvent3D::EventCode::SensorEnd:
        if (onSensorEnd && onContactFilter(contactEvent))
            onSensorEnd(contactEvent);
        break;
    default:
        break;
    }
}

bool ContactEventListener3D::init()
{
    auto func = [this](CustomEvent* event) { onEvent(event); };
    return CustomEventListener::init(CONTACT_3D_EVENT_NAME, func);
}

}  // namespace ax

#endif
