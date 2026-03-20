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
#include "axmol/physics2d/Contact2D.h"
#if defined(AX_ENABLE_PHYSICS_2D)

#    include "axmol/physics2d/Rigidbody2D.h"
#    include "axmol/physics2d/PhysicsUtility2D.h"
#    include "axmol/base/EventCustom.h"

namespace ax
{

const char* PHYSICSCONTACT_EVENT_NAME = "PhysicsContactEvent";

Contact2D::Contact2D()
    : EventCustom(PHYSICSCONTACT_EVENT_NAME)
    , _world(nullptr)
    , _colliderA(nullptr)
    , _colliderB(nullptr)
    , _eventCode(EventCode::NONE)
    , _notificationEnable(true)
    , _result(true)
    , _data(nullptr)
    , _contactInfo(nullptr)
    , _contactData(nullptr)
    , _preContactData(nullptr)
{}

Contact2D::~Contact2D()
{
    AX_SAFE_DELETE(_contactData);
    AX_SAFE_DELETE(_preContactData);
}

Contact2D* Contact2D::obtain(Collider2D* a, Collider2D* b)
{
    Contact2D* contact = new Contact2D();
    if (contact->init(a, b))
    {
        return contact;
    }

    AX_SAFE_DELETE(contact);
    return nullptr;
}

bool Contact2D::init(Collider2D* a, Collider2D* b)
{
    do
    {
        AX_BREAK_IF(a == nullptr || b == nullptr);

        _colliderA = a;
        _colliderB = b;

        return true;
    } while (false);

    return false;
}

void Contact2D::generateContactData()
{
    if (_contactInfo == nullptr)
    {
        return;
    }

    auto count         = b2Body_GetContactCapacity(b2_nullBodyId);  // FIXME
    b2ContactData* arb = static_cast<b2ContactData*>(_contactInfo);
    AX_SAFE_DELETE(_preContactData);
    _preContactData     = _contactData;
    _contactData        = new PhysicsContactData();
    _contactData->count = count;
    for (int i = 0; i < _contactData->count && i < PhysicsContactData::POINT_MAX; ++i)
    {
        _contactData->points[i] = PhysicsUtility2D::toVec2(arb->manifold.points[i].point);
    }

    _contactData->normal = _contactData->count > 0 ? PhysicsUtility2D::toVec2(arb->manifold.normal) : Vec2::ZERO;
}

// PhysicsContactPreSolve implementation
PhysicsContactPreSolve::PhysicsContactPreSolve(void* contactInfo) : _contactInfo(contactInfo) {}

PhysicsContactPreSolve::~PhysicsContactPreSolve() {}

float PhysicsContactPreSolve::getRestitution() const
{
    // return cpArbiterGetRestitution(static_cast<cpArbiter*>(_contactInfo));

    return 0;  // FIXME
}

float PhysicsContactPreSolve::getFriction() const
{
    // return cpArbiterGetFriction(static_cast<cpArbiter*>(_contactInfo));
    return 0;  // FIXME
}

Vec2 PhysicsContactPreSolve::getSurfaceVelocity() const
{
    // return PhysicsUtility2D::cpv2vec2(cpArbiterGetSurfaceVelocity(static_cast<cpArbiter*>(_contactInfo)));
    return 0;  // FIXME
}

void PhysicsContactPreSolve::setRestitution(float restitution)
{
    // FIXME
    // cpArbiterSetRestitution(static_cast<cpArbiter*>(_contactInfo), restitution);
}

void PhysicsContactPreSolve::setFriction(float friction)
{
    // FIXME
    // cpArbiterSetFriction(static_cast<cpArbiter*>(_contactInfo), friction);
}

void PhysicsContactPreSolve::setSurfaceVelocity(const Vec2& velocity)
{
    // FIXME
    // cpArbiterSetSurfaceVelocity(static_cast<cpArbiter*>(_contactInfo), PhysicsUtility2D::vec22cpv(velocity));
}

void PhysicsContactPreSolve::ignore()
{
    // FIXME
    // cpArbiterIgnore(static_cast<cpArbiter*>(_contactInfo));
}

// PhysicsContactPostSolve implementation
PhysicsContactPostSolve::PhysicsContactPostSolve(void* contactInfo) : _contactInfo(contactInfo) {}

PhysicsContactPostSolve::~PhysicsContactPostSolve() {}

float PhysicsContactPostSolve::getRestitution() const
{
    // FIXME
    // return cpArbiterGetRestitution(static_cast<cpArbiter*>(_contactInfo));
    return 0;
}

float PhysicsContactPostSolve::getFriction() const
{
    // FIXME
    // return cpArbiterGetFriction(static_cast<cpArbiter*>(_contactInfo));
    return 0;
}

Vec2 PhysicsContactPostSolve::getSurfaceVelocity() const
{
    // FIXME
    // return PhysicsUtility2D::cpv2vec2(cpArbiterGetSurfaceVelocity(static_cast<cpArbiter*>(_contactInfo)));
    return Vec2::ZERO;
}

EventListenerPhysicsContact::EventListenerPhysicsContact()
    : onContactBegin(nullptr), onContactPreSolve(nullptr), onContactPostSolve(nullptr), onContactSeparate(nullptr)
{}

bool EventListenerPhysicsContact::init()
{
    auto func = [this](EventCustom* event) -> void { onEvent(event); };

    return EventListenerCustom::init(PHYSICSCONTACT_EVENT_NAME, func);
}

void EventListenerPhysicsContact::onEvent(EventCustom* event)
{
    Contact2D* contact = dynamic_cast<Contact2D*>(event);

    if (contact == nullptr)
    {
        return;
    }

    switch (contact->getEventCode())
    {
    case Contact2D::EventCode::BEGIN:
    {
        bool ret = true;

        if (onContactBegin != nullptr && hitTest(contact->getColliderA(), contact->getColliderB()))
        {
            contact->generateContactData();
            ret = onContactBegin(*contact);
        }

        contact->setResult(ret);
        break;
    }
    case Contact2D::EventCode::PRESOLVE:
    {
        bool ret = true;

        if (onContactPreSolve != nullptr && hitTest(contact->getColliderA(), contact->getColliderB()))
        {
            PhysicsContactPreSolve solve(contact->_contactInfo);
            contact->generateContactData();

            ret = onContactPreSolve(*contact, solve);
        }

        contact->setResult(ret);
        break;
    }
    case Contact2D::EventCode::POSTSOLVE:
    {
        if (onContactPostSolve != nullptr && hitTest(contact->getColliderA(), contact->getColliderB()))
        {
            PhysicsContactPostSolve solve(contact->_contactInfo);
            onContactPostSolve(*contact, solve);
        }
        break;
    }
    case Contact2D::EventCode::SEPARATE:
    {
        if (onContactSeparate != nullptr && hitTest(contact->getColliderA(), contact->getColliderB()))
        {
            onContactSeparate(*contact);
        }
        break;
    }
    default:
        break;
    }
}

EventListenerPhysicsContact::~EventListenerPhysicsContact() {}

EventListenerPhysicsContact* EventListenerPhysicsContact::create()
{
    EventListenerPhysicsContact* obj = new EventListenerPhysicsContact();

    if (obj->init())
    {
        obj->autorelease();
        return obj;
    }

    AX_SAFE_DELETE(obj);
    return nullptr;
}

bool EventListenerPhysicsContact::hitTest(Collider2D* /*shapeA*/, Collider2D* /*shapeB*/)
{
    return true;
}

bool EventListenerPhysicsContact::checkAvailable()
{
    if (onContactBegin == nullptr && onContactPreSolve == nullptr && onContactPostSolve == nullptr &&
        onContactSeparate == nullptr)
    {
        AXASSERT(false, "Invalid PhysicsContactListener.");
        return false;
    }

    return true;
}

EventListenerPhysicsContact* EventListenerPhysicsContact::clone()
{
    EventListenerPhysicsContact* obj = EventListenerPhysicsContact::create();

    if (obj != nullptr)
    {
        obj->onContactBegin     = onContactBegin;
        obj->onContactPreSolve  = onContactPreSolve;
        obj->onContactPostSolve = onContactPostSolve;
        obj->onContactSeparate  = onContactSeparate;

        return obj;
    }

    AX_SAFE_DELETE(obj);
    return nullptr;
}

EventListenerPhysicsContactWithBodies* EventListenerPhysicsContactWithBodies::create(Rigidbody2D* bodyA,
                                                                                     Rigidbody2D* bodyB)
{
    EventListenerPhysicsContactWithBodies* obj = new EventListenerPhysicsContactWithBodies();

    if (obj->init())
    {
        obj->_a = bodyA;
        obj->_b = bodyB;
        obj->autorelease();
        return obj;
    }

    AX_SAFE_DELETE(obj);
    return nullptr;
}

EventListenerPhysicsContactWithBodies::EventListenerPhysicsContactWithBodies() : _a(nullptr), _b(nullptr) {}

EventListenerPhysicsContactWithBodies::~EventListenerPhysicsContactWithBodies() {}

bool EventListenerPhysicsContactWithBodies::hitTest(Collider2D* shapeA, Collider2D* shapeB)
{
    if ((shapeA->getAttachedBody() == _a && shapeB->getAttachedBody() == _b) ||
        (shapeA->getAttachedBody() == _b && shapeB->getAttachedBody() == _a))
    {
        return true;
    }

    return false;
}

EventListenerPhysicsContactWithBodies* EventListenerPhysicsContactWithBodies::clone()
{
    EventListenerPhysicsContactWithBodies* obj = EventListenerPhysicsContactWithBodies::create(_a, _b);

    if (obj != nullptr)
    {
        obj->onContactBegin     = onContactBegin;
        obj->onContactPreSolve  = onContactPreSolve;
        obj->onContactPostSolve = onContactPostSolve;
        obj->onContactSeparate  = onContactSeparate;

        return obj;
    }

    AX_SAFE_DELETE(obj);
    return nullptr;
}

EventListenerPhysicsContactWithShapes::EventListenerPhysicsContactWithShapes() : _a(nullptr), _b(nullptr) {}

EventListenerPhysicsContactWithShapes::~EventListenerPhysicsContactWithShapes() {}

EventListenerPhysicsContactWithShapes* EventListenerPhysicsContactWithShapes::create(Collider2D* shapeA,
                                                                                     Collider2D* shapeB)
{
    EventListenerPhysicsContactWithShapes* obj = new EventListenerPhysicsContactWithShapes();

    if (obj->init())
    {
        obj->_a = shapeA;
        obj->_b = shapeB;
        obj->autorelease();
        return obj;
    }

    AX_SAFE_DELETE(obj);
    return nullptr;
}

bool EventListenerPhysicsContactWithShapes::hitTest(Collider2D* shapeA, Collider2D* shapeB)
{
    if ((shapeA == _a && shapeB == _b) || (shapeA == _b && shapeB == _a))
    {
        return true;
    }

    return false;
}

EventListenerPhysicsContactWithShapes* EventListenerPhysicsContactWithShapes::clone()
{
    EventListenerPhysicsContactWithShapes* obj = EventListenerPhysicsContactWithShapes::create(_a, _b);

    if (obj != nullptr)
    {
        obj->onContactBegin     = onContactBegin;
        obj->onContactPreSolve  = onContactPreSolve;
        obj->onContactPostSolve = onContactPostSolve;
        obj->onContactSeparate  = onContactSeparate;

        return obj;
    }

    AX_SAFE_DELETE(obj);
    return nullptr;
}

EventListenerPhysicsContactWithGroup::EventListenerPhysicsContactWithGroup() : _group(0) {}

EventListenerPhysicsContactWithGroup::~EventListenerPhysicsContactWithGroup() {}

EventListenerPhysicsContactWithGroup* EventListenerPhysicsContactWithGroup::create(int group)
{
    EventListenerPhysicsContactWithGroup* obj = new EventListenerPhysicsContactWithGroup();

    if (obj->init())
    {
        obj->_group = group;
        obj->autorelease();
        return obj;
    }

    AX_SAFE_DELETE(obj);
    return nullptr;
}

bool EventListenerPhysicsContactWithGroup::hitTest(Collider2D* shapeA, Collider2D* shapeB)
{
    if (shapeA->getGroup() == _group || shapeB->getGroup() == _group)
    {
        return true;
    }

    return false;
}

EventListenerPhysicsContactWithGroup* EventListenerPhysicsContactWithGroup::clone()
{
    EventListenerPhysicsContactWithGroup* obj = EventListenerPhysicsContactWithGroup::create(_group);

    if (obj != nullptr)
    {
        obj->onContactBegin     = onContactBegin;
        obj->onContactPreSolve  = onContactPreSolve;
        obj->onContactPostSolve = onContactPostSolve;
        obj->onContactSeparate  = onContactSeparate;

        return obj;
    }

    AX_SAFE_DELETE(obj);
    return nullptr;
}

}  // namespace ax
#endif  // defined(AX_ENABLE_PHYSICS_2D)
