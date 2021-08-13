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

#ifndef B2_CONTACT_MANAGER_H
#define B2_CONTACT_MANAGER_H

#include "b2_api.h"
#include "b2_broad_phase.h"
#include "b2_fixture.h"
#include "b2_contact.h"

class b2ContactFilter;
class b2ContactListener;
class b2BlockAllocator;

// Delegate of b2World.
class B2_API b2ContactManager
{
public:
	b2ContactManager();
	~b2ContactManager();

	/// Callback to process a new pair (fixtureA, fixtureB).
	b2Contact* QueryCallback(b2Fixture* fixtureA, b2Fixture* fixtureB);

	// Perform broad-phase collision detection, process the pairs into new contacts and remove dead contacts.
	void FindNewContacts();
	
	/// Remove contacts that have not persisted, ie currently are not marked with the b2Contact::e_persistFlag.
	void RemoveDeadContacts();
	
	/// Destroy a contact and remove it from the global contact list.
	/// @warning currently does not removes the contact from the body lists.
	// TODO fix removing from body lists & ensure that the persist flag is not accessed from removed contacts 
	void Destroy(b2Contact* c);
	
	/// Updates the contacts and performs narrow-phase collision detection. May remove some contacts.
	void Collide();
  
  /// The first contact in this contact manager. Used to iterate the contacts.
	b2Contact* Start() const;
  
  /// An invalid marker contact that marks the end of the contact list. Used to iterate the contacts.
	b2Contact* End() const;

  /// @return the number of contacts currently in this contact manager.
  int32 GetCount() const;

	b2BroadPhase m_broadPhase;
	int32 m_contactCount;
	b2ContactFilter* m_contactFilter;
	b2ContactListener* m_contactListener;
	b2BlockAllocator* m_allocator;

private:
	b2Contact* m_contactList;
};

inline b2Contact* b2ContactManager::Start() const {
  return m_contactList->m_next;
}

inline b2Contact* b2ContactManager::End() const {
  return m_contactList;
}

inline int32 b2ContactManager::GetCount() const {
  return m_contactCount;
}

#endif
