// MIT License

// Copyright (c) 2019 Erin Catto

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "box2d/b2_body.h"
#include "box2d/b2_contact.h"
#include "box2d/b2_contact_manager.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_world_callbacks.h"

b2ContactFilter b2_defaultFilter;

b2ContactManager::b2ContactManager() {
	m_contactCount = 0;
	m_contactFilter = &b2_defaultFilter;
	m_contactListener = nullptr;
	m_allocator = nullptr;
	
	m_contactList = (b2Contact*) b2Alloc(1 * sizeof(b2Contact));
  m_contactList->m_next = m_contactList;
  m_contactList->m_prev = m_contactList;
  m_contactList->m_fixtureA = nullptr;
  m_contactList->m_fixtureB = nullptr;
}

b2ContactManager::~b2ContactManager() {
  b2Free(m_contactList);
}

void b2ContactManager::Destroy(b2Contact* c) {
	if (m_contactListener && c->IsTouching()) {
		m_contactListener->EndContact(c);
	}

  b2Assert(c != m_contactList);

	// Remove from the world.
	c->m_prev->m_next = c->m_next;
	c->m_next->m_prev = c->m_prev;

	b2Contact::Destroy(c, m_allocator);
	--m_contactCount;
}

// This is the top level collision call for the time step. Here
// all the narrow phase collision is processed for the world
// contact list.
void b2ContactManager::Collide() {
	// Update awake contacts.
	b2Contact* c = Start();
	while (c != End()) {
		b2Fixture* fixtureA = c->GetFixtureA();
		b2Fixture* fixtureB = c->GetFixtureB();
		b2Body* bodyA = fixtureA->GetBody();
		b2Body* bodyB = fixtureB->GetBody();

		// Is this contact flagged for filtering?
		if (c->m_flags & b2Contact::e_filterFlag) {
			// Should these bodies collide?
			if (bodyB->ShouldCollide(bodyA) == false) {
				b2Contact* cNuke = c;
				c = cNuke->GetNext();
				Destroy(cNuke);
				continue;
			}

			// Check user filtering.
			if (m_contactFilter && m_contactFilter->ShouldCollide(fixtureA, fixtureB) == false) {
				b2Contact* cNuke = c;
				c = cNuke->GetNext();
				Destroy(cNuke);
				continue;
			}
		}

		// Clear the island flag; Prepare to build islands in b2World::Solve
		// Clear the persist flag; Prepare to add/remove new contacts from the broadphase 
		// Clear the filtering flag
		c->m_flags &= ~(b2Contact::e_islandFlag | b2Contact::e_persistFlag | b2Contact::e_filterFlag);

		// TODO this checks can probably be removed with some extra code in b2_body
#ifdef ENABLE_SLEEPING
		bool activeA = bodyA->IsAwake() && bodyA->m_type != b2_staticBody;
		bool activeB = bodyB->IsAwake() && bodyB->m_type != b2_staticBody;
    bool active = (activeA || activeB);
#else
    bool active = (bodyA->m_type != b2_staticBody) || (bodyB->m_type != b2_staticBody);
#endif // ENABLE_SLEEPING

		// At least one body must be awake and it must be dynamic or kinematic.
		if (active) {
			// The contact persists.
			c->Update(m_contactListener);
		}

		c = c->GetNext();
	}
}

void b2ContactManager::FindNewContacts() {
	m_broadPhase.UpdateAndQuery(this);
  RemoveDeadContacts();	
}

void b2ContactManager::RemoveDeadContacts() {
  b2Contact* c = Start();

	while (c != End()) {
		b2Contact* cNuke = c;
		c = c->GetNext();

		if ((cNuke->m_flags & b2Contact::e_persistFlag) == 0) {
			Destroy(cNuke);
		}
	}
}

b2Contact* b2ContactManager::QueryCallback(b2Fixture* fixtureA, b2Fixture* fixtureB) {
	b2Body* bodyA = fixtureA->GetBody();
	b2Body* bodyB = fixtureB->GetBody();
	
	// Are the fixtures on the same body?
	if (bodyA == bodyB) {
		return nullptr;
	}
	
	// Does a contact already exist?
	for (int32 i = 0; i < bodyB->GetContactCount(); ++i) {
		b2Contact* c = bodyB->GetContact(i);

		b2Fixture* fA = c->GetFixtureA();
		b2Fixture* fB = c->GetFixtureB();
		
		if ((fA == fixtureA && fB == fixtureB) || (fA == fixtureB && fB == fixtureA)) {
			// persist the contact
			c->m_flags |= b2Contact::e_persistFlag;

			return c;
		}
	}

	// Does a joint override collision? Is at least one body dynamic?
	if (bodyB->ShouldCollide(bodyA) == false) {
		return nullptr;
	}

	// Check user filtering.
	if (m_contactFilter && m_contactFilter->ShouldCollide(fixtureA, fixtureB) == false) {
		return nullptr;
	}

	// Call the factory.
	b2Contact* c = b2Contact::Create(fixtureA, fixtureB, m_allocator);
	if (c == nullptr) {
		return nullptr;
	}

	// Insert into the world.
	c->m_prev = m_contactList;
	c->m_next = m_contactList->m_next;
	m_contactList->m_next->m_prev = c;
	m_contactList->m_next = c;

	bodyA->AddContact(c);
	bodyB->AddContact(c);

	++m_contactCount;
	
	return c;
}
