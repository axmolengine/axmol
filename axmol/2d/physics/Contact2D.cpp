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
#include "axmol/2d/physics/Contact2D.h"
#if defined(AX_ENABLE_PHYSICS_2D)

#    include "axmol/2d/physics/Rigidbody2D.h"
#    include "axmol/2d/physics/PhysicsUtility2D.h"
#    include "axmol/base/EventCustom.h"

namespace ax
{

const char* CONTACT_2D_EVENT_NAME = "contact-2d";

Contact2D::Contact2D()
    : EventCustom(CONTACT_2D_EVENT_NAME)
    , _colliderA(nullptr)
    , _colliderB(nullptr)
    , _eventCode(EventCode::None)
    , _result(true)
{}

Contact2D::~Contact2D() {}

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
    if (!_colliderA)
        return;

    // simple get 1 contact from bodyA
    auto body = _colliderA->getAttachedBody();
    if (!body->isAttached())
        return;

    auto bodyId       = body->internalHandle();
    auto contactCount = b2Body_GetContactCapacity(bodyId);
    if (contactCount <= 0)
        return;

    b2ContactData contactData;
    if (b2Body_GetContactData(bodyId, &contactData, 1) != 1)
        return;

    _contactInfo.normal     = PhysicsUtility2D::toVec2(contactData.manifold.normal);
    _contactInfo.pointCount = contactData.manifold.pointCount;
    for (auto i = 0; i < contactData.manifold.pointCount && i < Contact2DInfo::POINT_MAX; ++i)
    {
        _contactInfo.points[i].point          = PhysicsUtility2D::toVec2(contactData.manifold.points[i].clipPoint);
        _contactInfo.points[i].normalImpulse  = contactData.manifold.points[i].normalImpulse;
        _contactInfo.points[i].tangentImpulse = contactData.manifold.points[i].tangentImpulse;
        _contactInfo.points[i].normalVelocity = contactData.manifold.points[i].normalVelocity;
    }
}

#    pragma region contact 2d listeners
Contact2DListener::Contact2DListener() {}

bool Contact2DListener::init()
{
    auto func = [this](EventCustom* event) -> void { onEvent(event); };

    return EventListenerCustom::init(CONTACT_2D_EVENT_NAME, func);
}

void Contact2DListener::onEvent(EventCustom* event)
{
    Contact2D* contact = dynamic_cast<Contact2D*>(event);

    if (contact == nullptr)
    {
        return;
    }

    switch (contact->getEventCode())
    {
    case Contact2D::EventCode::PreSolve:
    {
        bool ret = true;

        if (onPreSolve != nullptr && hitTest(contact->getColliderA(), contact->getColliderB()))
        {
            ret = onPreSolve(contact);
        }

        contact->setResult(ret);
        break;
    }
    case Contact2D::EventCode::ContactBegin:
    {
        if (onContactBegin != nullptr && hitTest(contact->getColliderA(), contact->getColliderB()))
        {
            contact->generateContactData();
            onContactBegin(contact);
        }
        break;
    }
    case Contact2D::EventCode::ContactEnd:
    {
        if (onContactEnd != nullptr && hitTest(contact->getColliderA(), contact->getColliderB()))
        {
            contact->generateContactData();
            onContactEnd(contact);
        }
        break;
    }
    case Contact2D::EventCode::CollisionHit:
    {
        if (onCollisionHit != nullptr && hitTest(contact->getColliderA(), contact->getColliderB()))
        {
            contact->generateContactData();
            onCollisionHit(contact);
        }
        break;
    }
    case Contact2D::EventCode::SensorBegin:
    {
        if (onContactEnd != nullptr && hitTest(contact->getColliderA(), contact->getColliderB()))
        {
            onContactEnd(contact);
        }
        break;
    }
    case Contact2D::EventCode::SensorEnd:
    {
        if (onCollisionHit != nullptr && hitTest(contact->getColliderA(), contact->getColliderB()))
        {
            onCollisionHit(contact);
        }
        break;
    }
    default:
        break;
    }
}

Contact2DListener::~Contact2DListener() {}

Contact2DListener* Contact2DListener::create()
{
    Contact2DListener* obj = new Contact2DListener();

    if (obj->init())
    {
        obj->autorelease();
        return obj;
    }

    AX_SAFE_DELETE(obj);
    return nullptr;
}

bool Contact2DListener::hitTest(Collider2D* /*shapeA*/, Collider2D* /*shapeB*/)
{
    return true;
}

bool Contact2DListener::checkAvailable()
{
    if (!onPreSolve && !onContactBegin && !onContactEnd && !onCollisionHit && !onSensorBegin && !onSensorEnd)
    {
        AXASSERT(false, "Invalid Contact2DListener.");
        return false;
    }

    return true;
}

Contact2DListener* Contact2DListener::clone()
{
    Contact2DListener* obj = Contact2DListener::create();

    if (obj != nullptr)
    {
        obj->onPreSolve     = onPreSolve;
        obj->onContactBegin = onContactBegin;
        obj->onContactEnd   = onContactEnd;
        obj->onCollisionHit = onCollisionHit;
        obj->onSensorBegin  = onSensorBegin;
        obj->onSensorEnd    = onSensorEnd;

        return obj;
    }

    AX_SAFE_DELETE(obj);
    return nullptr;
}

Contact2DListenerWithBodies* Contact2DListenerWithBodies::create(Rigidbody2D* bodyA, Rigidbody2D* bodyB)
{
    Contact2DListenerWithBodies* obj = new Contact2DListenerWithBodies();

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

Contact2DListenerWithBodies::Contact2DListenerWithBodies() : _a(nullptr), _b(nullptr) {}

Contact2DListenerWithBodies::~Contact2DListenerWithBodies() {}

bool Contact2DListenerWithBodies::hitTest(Collider2D* shapeA, Collider2D* shapeB)
{
    if ((shapeA->getAttachedBody() == _a && shapeB->getAttachedBody() == _b) ||
        (shapeA->getAttachedBody() == _b && shapeB->getAttachedBody() == _a))
    {
        return true;
    }

    return false;
}

Contact2DListenerWithBodies* Contact2DListenerWithBodies::clone()
{
    Contact2DListenerWithBodies* obj = Contact2DListenerWithBodies::create(_a, _b);

    if (obj != nullptr)
    {
        obj->onPreSolve     = onPreSolve;
        obj->onContactBegin = onContactBegin;
        obj->onContactEnd   = onContactEnd;
        obj->onCollisionHit = onCollisionHit;
        obj->onSensorBegin  = onSensorBegin;
        obj->onSensorEnd    = onSensorEnd;

        return obj;
    }

    AX_SAFE_DELETE(obj);
    return nullptr;
}

Contact2DListenerWithShapes::Contact2DListenerWithShapes() : _a(nullptr), _b(nullptr) {}

Contact2DListenerWithShapes::~Contact2DListenerWithShapes() {}

Contact2DListenerWithShapes* Contact2DListenerWithShapes::create(Collider2D* shapeA, Collider2D* shapeB)
{
    Contact2DListenerWithShapes* obj = new Contact2DListenerWithShapes();

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

bool Contact2DListenerWithShapes::hitTest(Collider2D* shapeA, Collider2D* shapeB)
{
    if ((shapeA == _a && shapeB == _b) || (shapeA == _b && shapeB == _a))
    {
        return true;
    }

    return false;
}

Contact2DListenerWithShapes* Contact2DListenerWithShapes::clone()
{
    Contact2DListenerWithShapes* obj = Contact2DListenerWithShapes::create(_a, _b);

    if (obj != nullptr)
    {
        obj->onPreSolve     = onPreSolve;
        obj->onContactBegin = onContactBegin;
        obj->onContactEnd   = onContactEnd;
        obj->onCollisionHit = onCollisionHit;
        obj->onSensorBegin  = onSensorBegin;
        obj->onSensorEnd    = onSensorEnd;

        return obj;
    }

    AX_SAFE_DELETE(obj);
    return nullptr;
}

Contact2DListenerWithGroup::Contact2DListenerWithGroup() : _group(0) {}

Contact2DListenerWithGroup::~Contact2DListenerWithGroup() {}

Contact2DListenerWithGroup* Contact2DListenerWithGroup::create(int group)
{
    Contact2DListenerWithGroup* obj = new Contact2DListenerWithGroup();

    if (obj->init())
    {
        obj->_group = group;
        obj->autorelease();
        return obj;
    }

    AX_SAFE_DELETE(obj);
    return nullptr;
}

bool Contact2DListenerWithGroup::hitTest(Collider2D* shapeA, Collider2D* shapeB)
{
    if (shapeA->getGroup() == _group || shapeB->getGroup() == _group)
    {
        return true;
    }

    return false;
}

Contact2DListenerWithGroup* Contact2DListenerWithGroup::clone()
{
    Contact2DListenerWithGroup* obj = Contact2DListenerWithGroup::create(_group);

    if (obj != nullptr)
    {
        obj->onPreSolve     = onPreSolve;
        obj->onContactBegin = onContactBegin;
        obj->onContactEnd   = onContactEnd;
        obj->onCollisionHit = onCollisionHit;
        obj->onSensorBegin  = onSensorBegin;
        obj->onSensorEnd    = onSensorEnd;

        return obj;
    }

    AX_SAFE_DELETE(obj);
    return nullptr;
}

#    pragma endregion

}  // namespace ax
#endif  // defined(AX_ENABLE_PHYSICS_2D)
