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

#include "b2_contact_solver.h"
#include "b2_island.h"

#include "box2d/b2_body.h"
#include "box2d/b2_broad_phase.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_collision.h"
#include "box2d/b2_contact.h"
#include "box2d/b2_draw.h"
#include "box2d/b2_edge_shape.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_pulley_joint.h"
#include "box2d/b2_time_of_impact.h"
#include "box2d/b2_timer.h"
#include "box2d/b2_world.h"

#include <new>

b2World::b2World(const b2Vec2& gravity)
{
	m_destructionListener = nullptr;
	m_debugDraw = nullptr;

	m_bodyListHead = nullptr;
	m_bodyListTail = nullptr;
	m_jointList = nullptr;
	m_particleSystemList = nullptr;
	
	m_bodyCount = 0;
	m_jointCount = 0;

	m_warmStarting = true;
	m_continuousPhysics = true;
	m_subStepping = false;

	m_stepComplete = true;

	m_allowSleep = true;
	m_gravity = gravity;

  m_removedBodies = false;
	m_newContacts = false;
	m_locked = false;
	m_clearForces = true;

	m_inv_dt0 = 0.0f;

	m_contactManager.m_allocator = &m_blockAllocator;

	memset(&m_profile, 0, sizeof(b2Profile));
}

b2World::~b2World()
{
	// Some shapes allocate using b2Alloc.
	b2Body* b = m_bodyListHead;
	while (b)
	{
		b2Body* bNext = b->m_next;

		b2Fixture* f = b->m_fixtureList;
		while (f)
		{
			b2Fixture* fNext = f->m_next;
			//f->m_proxyCount = 0;
			f->Destroy(&m_blockAllocator);
			f = fNext;
		}
		
		b2Free(b->m_contactList);

		b = bNext;
	}
	
#ifdef ENABLE_LIQUID
	while (m_particleSystemList) {
		DestroyParticleSystem(m_particleSystemList);
	}
#endif // ENABLE_LIQUID
}

void b2World::SetDestructionListener(b2DestructionListener* listener)
{
	m_destructionListener = listener;
}

void b2World::SetContactFilter(b2ContactFilter* filter)
{
	m_contactManager.m_contactFilter = filter;
}

void b2World::SetContactListener(b2ContactListener* listener)
{
	m_contactManager.m_contactListener = listener;
}

void b2World::SetDebugDraw(b2Draw* debugDraw)
{
	m_debugDraw = debugDraw;
}

void b2World::RemoveDeadContacts() {
	// Contacts that were not persisted must be removed from the body lists
	for (b2Body* b = m_bodyListHead; b; b = b->m_next) {
		for (int32 i = 0; i < b->GetContactCount(); ++i) {
			b2Contact* contact = b->GetContact(i);

			if ((contact->m_flags & b2Contact::e_persistFlag) == 0) {
				b->m_contactCount--;
				b->m_contactList[i] = b->m_contactList[b->m_contactCount];
				i--;
			}
		}
	}
}

b2Body* b2World::CreateBody(const b2BodyDef* def)
{
	b2Assert(IsLocked() == false);
	if (IsLocked())
	{
		return nullptr;
	}

	void* mem = m_blockAllocator.Allocate(sizeof(b2Body));
	b2Body* b = new (mem) b2Body(def, this);

	// Add to world doubly linked list.
	
	if (b->GetType() == b2_staticBody) {
	  b->m_prev = m_bodyListTail;
	  b->m_next = nullptr;
	  if (m_bodyListTail) {
		  m_bodyListTail->m_next = b;
	  } else {
	    m_bodyListHead = b;
	  }
	  m_bodyListTail = b;
	} else {
	  b->m_prev = nullptr;
	  b->m_next = m_bodyListHead;
	  if (m_bodyListHead) {
		  m_bodyListHead->m_prev = b;
	  } else {
	    m_bodyListTail = b;
	  }
	  m_bodyListHead = b;
	}

	++m_bodyCount;

	return b;
}

void b2World::DestroyBody(b2Body* b)
{
	b2Assert(m_bodyCount > 0);
	b2Assert(IsLocked() == false);
	if (IsLocked())
	{
		return;
	}

  m_removedBodies = true;

	// Delete the attached joints.
	b2JointEdge* je = b->m_jointList;
	while (je)
	{
		b2JointEdge* je0 = je;
		je = je->next;

		if (m_destructionListener)
		{
			m_destructionListener->SayGoodbye(je0->joint);
		}

		DestroyJoint(je0->joint);

		b->m_jointList = je;
	}
	b->m_jointList = nullptr;

	// Delete the attached contacts.
	for (int32 i = 0; i < b->GetContactCount(); ++i) {
		b2Contact* c = b->GetContact(i);
		c->m_flags &= ~b2Contact::e_persistFlag;
		m_contactManager.Destroy(c);
	}
	
	b2Free(b->m_contactList);
	
	b->m_contactCount = 0;

	// Delete the attached fixtures. This destroys broad-phase proxies.
	b2Fixture* f = b->m_fixtureList;
	while (f)
	{
		b2Fixture* f0 = f;
		f = f->m_next;

		if (m_destructionListener)
		{
			m_destructionListener->SayGoodbye(f0);
		}

		//f0->DestroyProxies(&m_contactManager.m_broadPhase);
		b2BroadPhase* broadphase = &m_contactManager.m_broadPhase;
		broadphase->Remove(f0);
		
		f0->Destroy(&m_blockAllocator);
		f0->~b2Fixture();
		m_blockAllocator.Free(f0, sizeof(b2Fixture));

		b->m_fixtureList = f;
		b->m_fixtureCount -= 1;
	}
	b->m_fixtureList = nullptr;
	b->m_fixtureCount = 0;

	// Remove world body list.
	if (b->m_prev)
	{
		b->m_prev->m_next = b->m_next;
	}

	if (b->m_next)
	{
		b->m_next->m_prev = b->m_prev;
	}

	if (b == m_bodyListHead) {
		m_bodyListHead = b->m_next;
	}

	if (b == m_bodyListTail) {
		m_bodyListTail = b->m_prev;
	}

	--m_bodyCount;
	b->~b2Body();
	m_blockAllocator.Free(b, sizeof(b2Body));
}

b2Joint* b2World::CreateJoint(const b2JointDef* def)
{
	b2Assert(IsLocked() == false);
	if (IsLocked())
	{
		return nullptr;
	}

	b2Joint* j = b2Joint::Create(def, &m_blockAllocator);

	// Connect to the world list.
	j->m_prev = nullptr;
	j->m_next = m_jointList;
	if (m_jointList)
	{
		m_jointList->m_prev = j;
	}
	m_jointList = j;
	++m_jointCount;

	// Connect to the bodies' doubly linked lists.
	j->m_edgeA.joint = j;
	j->m_edgeA.other = j->m_bodyB;
	j->m_edgeA.prev = nullptr;
	j->m_edgeA.next = j->m_bodyA->m_jointList;
	if (j->m_bodyA->m_jointList) j->m_bodyA->m_jointList->prev = &j->m_edgeA;
	j->m_bodyA->m_jointList = &j->m_edgeA;

	j->m_edgeB.joint = j;
	j->m_edgeB.other = j->m_bodyA;
	j->m_edgeB.prev = nullptr;
	j->m_edgeB.next = j->m_bodyB->m_jointList;
	if (j->m_bodyB->m_jointList) j->m_bodyB->m_jointList->prev = &j->m_edgeB;
	j->m_bodyB->m_jointList = &j->m_edgeB;

	b2Body* bodyA = def->bodyA;
	b2Body* bodyB = def->bodyB;

	// If the joint prevents collisions, then flag any contacts for filtering.
	if (def->collideConnected == false)
	{
		for (int32 i = 0; i < bodyB->GetContactCount(); ++i) {
			b2Contact* c = bodyB->GetContact(i);

			// One of the bodies is bodyB, we want the other to be bodyA but we don't know which one is where so we check both
			if (c->GetFixtureA()->GetBody() == bodyA || c->GetFixtureB()->GetBody() == bodyA) {
				// Flag the contact for filtering at the next time step (where either
				// body is awake).
				c->FlagForFiltering();
			}
		}
	}

	// Note: creating a joint doesn't wake the bodies.

	return j;
}

void b2World::DestroyJoint(b2Joint* j)
{
	b2Assert(IsLocked() == false);
	if (IsLocked())
	{
		return;
	}

	bool collideConnected = j->m_collideConnected;

	// Remove from the doubly linked list.
	if (j->m_prev)
	{
		j->m_prev->m_next = j->m_next;
	}

	if (j->m_next)
	{
		j->m_next->m_prev = j->m_prev;
	}

	if (j == m_jointList)
	{
		m_jointList = j->m_next;
	}

	// Disconnect from island graph.
	b2Body* bodyA = j->m_bodyA;
	b2Body* bodyB = j->m_bodyB;

	// Wake up connected bodies.
  SET_AWAKE_OR_NONE(bodyA);
  SET_AWAKE_OR_NONE(bodyB);

	// Remove from body 1.
	if (j->m_edgeA.prev)
	{
		j->m_edgeA.prev->next = j->m_edgeA.next;
	}

	if (j->m_edgeA.next)
	{
		j->m_edgeA.next->prev = j->m_edgeA.prev;
	}

	if (&j->m_edgeA == bodyA->m_jointList)
	{
		bodyA->m_jointList = j->m_edgeA.next;
	}

	j->m_edgeA.prev = nullptr;
	j->m_edgeA.next = nullptr;

	// Remove from body 2
	if (j->m_edgeB.prev)
	{
		j->m_edgeB.prev->next = j->m_edgeB.next;
	}

	if (j->m_edgeB.next)
	{
		j->m_edgeB.next->prev = j->m_edgeB.prev;
	}

	if (&j->m_edgeB == bodyB->m_jointList)
	{
		bodyB->m_jointList = j->m_edgeB.next;
	}

	j->m_edgeB.prev = nullptr;
	j->m_edgeB.next = nullptr;

	b2Joint::Destroy(j, &m_blockAllocator);

	b2Assert(m_jointCount > 0);
	--m_jointCount;

	// If the joint prevents collisions, then flag any contacts for filtering.
	if (collideConnected == false)
	{
		for (int32 i = 0; i < bodyB->GetContactCount(); ++i) {
			b2Contact* c = bodyB->GetContact(i);

			// One of the bodies is bodyB, we want the other to be bodyA but we don't know which one is where so we check both
			if (c->GetFixtureA()->GetBody() == bodyA || c->GetFixtureB()->GetBody() == bodyA) {
				// Flag the contact for filtering at the next time step (where either
				// body is awake).
				c->FlagForFiltering();
			}
		}
	}
}

#ifdef ENABLE_LIQUID
b2ParticleSystem* b2World::CreateParticleSystem(const b2ParticleSystemDef* def)
{
	b2Assert(IsLocked() == false);
	if (IsLocked())
	{
		return NULL;
	}

	void* mem = m_blockAllocator.Allocate(sizeof(b2ParticleSystem));
	b2ParticleSystem* p = new (mem) b2ParticleSystem(def, this);

	// Add to world doubly linked list.
	p->m_prev = NULL;
	p->m_next = m_particleSystemList;
	if (m_particleSystemList)
	{
		m_particleSystemList->m_prev = p;
	}
	m_particleSystemList = p;

	return p;
}

void b2World::DestroyParticleSystem(b2ParticleSystem* p)
{
	b2Assert(m_particleSystemList != NULL);
	b2Assert(IsLocked() == false);
	if (IsLocked())
	{
		return;
	}

	// Remove world particleSystem list.
	if (p->m_prev)
	{
		p->m_prev->m_next = p->m_next;
	}

	if (p->m_next)
	{
		p->m_next->m_prev = p->m_prev;
	}

	if (p == m_particleSystemList)
	{
		m_particleSystemList = p->m_next;
	}

	p->~b2ParticleSystem();
	m_blockAllocator.Free(p, sizeof(b2ParticleSystem));
}
#endif // ENABLE_LIQUID

//
void b2World::SetAllowSleeping(bool flag)
{
	if (flag == m_allowSleep)
	{
		return;
	}

	m_allowSleep = flag;
	if (m_allowSleep == false)
	{
		for (b2Body* b = m_bodyListHead; b; b = b->m_next)
		{
			SET_AWAKE_OR_NONE(b);
		}
	}
}

// Find islands, integrate and solve constraints, solve position constraints
void b2World::Solve(const b2TimeStep& step)
{
	// Size the island for the worst case.
	b2Island island(m_bodyCount,
					m_contactManager.m_contactCount,
					m_jointCount,
					&m_stackAllocator,
					m_contactManager.m_contactListener);

	// Clear all the island flags.
	for (b2Body* b = m_bodyListHead; b; b = b->m_next) {
		b->m_xf0 = b->m_xf;

		// Store positions for continuous collision.
		b->m_sweep.c0 = b->m_sweep.c;
		b->m_sweep.a0 = b->m_sweep.a;

		b->m_flags &= ~b2Body::e_islandFlag;
	  
	  if (b->GetType() == b2_staticBody) {
	    // Reminder: after the first static body in the body list all subsequent are static as well
	    // Static bodies do not move hence we need not update xf0, c0, a0
	    break;
	  }
	}
	
	for (b2Joint* j = m_jointList; j; j = j->m_next) {
		j->m_islandFlag = false;
	}

	// Build and simulate all awake islands.
	// at this point all contacts have the e_islandFlag cleared by b2ContactManager::Collide;
	int32 stackSize = m_bodyCount;
	b2Body** stack = (b2Body**)m_stackAllocator.Allocate(stackSize * sizeof(b2Body*));
	for (b2Body* seed = m_bodyListHead; seed; seed = seed->m_next) {
		if (seed->m_flags & b2Body::e_islandFlag) {
			continue;
		}

    if (seed->IsEnabled() == false) {
      continue;
    }

#ifdef ENABLE_SLEEPING
		if (seed->IsAwake() == false) {
			continue;
		}
#endif // ENABLE_SLEEPING

		// The seed can be dynamic or kinematic.
		if (seed->GetType() == b2_staticBody) {
	    // Reminder: after the first static body in the body list all subsequent are static as well
			break;
		}

		if (seed->GetContactCount() == 0 && seed->GetJointList() == nullptr) {
		  island.SolveOrphan(seed, step, m_gravity, m_allowSleep);
		  seed->m_flags |= (b2Body::e_islandFlag | b2Body::e_awakeFlag);
		  continue;
		}

		// Reset island and stack.
		island.Clear();
		int32 stackCount = 0;
		stack[stackCount++] = seed;
		seed->m_flags |= b2Body::e_islandFlag;

		// Perform a depth first search (DFS) on the constraint graph.
		while (stackCount > 0) {
			// Grab the next body off the stack and add it to the island.
			b2Body* b = stack[--stackCount];
			b2Assert(b->IsEnabled() == true);
			island.Add(b);

			// To keep islands as small as possible, we don't
			// propagate islands across static bodies.
			if (b->GetType() == b2_staticBody)
			{
				continue;
			}

			// Make sure the body is awake (without resetting sleep timer).
			b->m_flags |= b2Body::e_awakeFlag;

			// Search all contacts connected to this body.
			for (int32 i = 0; i < b->GetContactCount(); ++i) {
				b2Contact* contact = b->GetContact(i);
			
				// Has this contact already been added to an island?
				if (contact->m_flags & b2Contact::e_islandFlag)
				{
					continue;
				}

				// Is this contact solid and touching?
				if (contact->IsEnabled() == false ||
					contact->IsTouching() == false)
				{
					continue;
				}

				// Skip sensors.
				bool sensorA = contact->m_fixtureA->m_isSensor;
				bool sensorB = contact->m_fixtureB->m_isSensor;
				if (sensorA || sensorB)
				{
					continue;
				}

				island.Add(contact);
				contact->m_flags |= b2Contact::e_islandFlag;

				b2Body* bA = contact->GetFixtureA()->GetBody();
				b2Body* bB = contact->GetFixtureB()->GetBody();
				b2Body* other = (bA == b)? bB : bA;

				// Was the other body already added to this island?
				if (other->m_flags & b2Body::e_islandFlag)
				{
					continue;
				}

				b2Assert(stackCount < stackSize);
				stack[stackCount++] = other;
				other->m_flags |= b2Body::e_islandFlag;
			}

			// Search all joints connect to this body.
			for (b2JointEdge* je = b->m_jointList; je; je = je->next)
			{
				if (je->joint->m_islandFlag == true)
				{
					continue;
				}

				b2Body* other = je->other;

				// Don't simulate joints connected to diabled bodies.
				if (other->IsEnabled() == false)
				{
					continue;
				}

				island.Add(je->joint);
				je->joint->m_islandFlag = true;

				if (other->m_flags & b2Body::e_islandFlag)
				{
					continue;
				}

				b2Assert(stackCount < stackSize);
				stack[stackCount++] = other;
				other->m_flags |= b2Body::e_islandFlag;
			}
		}

		island.Solve(step, m_gravity, m_allowSleep);

		// Post solve cleanup.
		for (int32 i = 0; i < island.m_bodyCount; ++i) {
			// Allow static bodies to participate in other islands.
			b2Body* b = island.m_bodies[i];
			if (b->GetType() == b2_staticBody) {
				b->m_flags &= ~b2Body::e_islandFlag;
			}
		}
	}

	m_stackAllocator.Free(stack);
	
	{
		b2Timer timer;
		// Look for new contacts.
		m_contactManager.FindNewContacts();
		RemoveDeadContacts();
		m_profile.broadphase = timer.GetMilliseconds();
	}
}

struct b2HeapNode {
	float toi;
	int toiCount;
	b2Contact* c;
};

class b2TOIMinHeap {
	friend class b2World;
	friend class b2TOIQueryWrapper;

	b2TOIMinHeap(int32 initialCapacity);
	~b2TOIMinHeap();

	b2HeapNode& Min();
	b2HeapNode DeleteMin();
	void Insert(b2Contact* c);
	void Update(b2Contact* c);
	b2HeapNode Delete(int32 idx);
	void SetKey(int32 idx, float toi);
	void HeapifyDown(int32 idx);
	void HeapifyUp(int32 idx);
	void Build();
	bool IsEmpty();

	b2HeapNode& operator [] (int32 idx) {
		return m_heap[idx];
	}
	
	int32 m_count;
	int32 m_capacity;
	b2HeapNode* m_heap;
};

b2TOIMinHeap::b2TOIMinHeap(int32 initialCapacity) {
	m_count = 0;
	m_capacity = initialCapacity;
	m_heap = (b2HeapNode*) b2Alloc(m_capacity * sizeof(b2HeapNode));
}

b2TOIMinHeap::~b2TOIMinHeap() {
	b2Free(m_heap);
}

bool b2TOIMinHeap::IsEmpty() {
	return m_count == 0;
}

b2HeapNode& b2TOIMinHeap::Min() {
	b2Assert(m_count > 0);
	return m_heap[0];
}

b2HeapNode b2TOIMinHeap::DeleteMin() {
	return Delete(0);
}

void b2TOIMinHeap::Insert(b2Contact* c) {
	float toi = c->CalculateTOI();

	if (toi < 1.0f) {
	  if (m_count == m_capacity) {
    	m_capacity *= 2;
	    b2HeapNode* oldHeap = m_heap;
    	m_heap = (b2HeapNode*) b2Alloc(m_capacity * sizeof(b2HeapNode));
    	memcpy(m_heap, oldHeap, m_count * sizeof(b2HeapNode));
    	b2Free(oldHeap);
	  }

	  int32 idx = m_count;
	  m_heap[idx].c = c;
	  m_heap[idx].toi = toi;
	  m_heap[idx].toiCount = 0;
	  c->m_toiIndex = idx;
	  m_count++;

	  HeapifyUp(idx);
	}
}

b2HeapNode b2TOIMinHeap::Delete(int32 idx) {
	b2Assert(m_count > 0);
	b2Assert(idx >= 0 && idx < m_count);
	
	b2HeapNode ret = m_heap[idx];
	m_count--;
	
	if (idx != m_count) {
		m_heap[idx] = m_heap[m_count];
		int32 parent = (idx - 1) / 2;
		
		if (idx == 0 || m_heap[parent].toi < m_heap[idx].toi) {
			HeapifyDown(idx);
		} else {
			HeapifyUp(idx);
		}
	}

	return ret;
}

void b2TOIMinHeap::Update(b2Contact* c) {
  int32 idx = c->m_toiIndex;
  float toi;
  
  if (m_heap[idx].toiCount > b2_maxSubSteps) {
    toi = 1.0f;
  } else {
    toi = c->CalculateTOI();
  }

  SetKey(idx, toi);
}

void b2TOIMinHeap::SetKey(int32 idx, float toi) {
	b2Assert(idx >= 0 && idx < m_count);

	bool up = toi < m_heap[idx].toi;
	bool down = toi > m_heap[idx].toi;
	
	m_heap[idx].toi = toi;

	if (up) {
		HeapifyUp(idx);
	} else if (down) {
		HeapifyDown(idx);
	}
}

void b2TOIMinHeap::HeapifyDown(int32 idx) {
	while (true) {
		int32 minIdx = idx;
		int32 left = 2 * idx + 1;
		int32 right = 2 * idx + 2;
		
		if (left < m_count && m_heap[left].toi < m_heap[minIdx].toi) {
			minIdx = left;
		}
		
		if (right < m_count && m_heap[right].toi < m_heap[minIdx].toi) {
			minIdx = right;
		}
		
		if (minIdx != idx) {
			b2Swap(m_heap[idx].c->m_toiIndex, m_heap[minIdx].c->m_toiIndex);
			b2Swap(m_heap[idx], m_heap[minIdx]);
			idx = minIdx;
		} else {
			break;
		}
	}
}

void b2TOIMinHeap::HeapifyUp(int32 idx) {
	int32 parent;
	
	while (idx > 0 && m_heap[idx].toi < m_heap[parent = (idx - 1) / 2].toi) {
		b2Swap(m_heap[idx].c->m_toiIndex, m_heap[parent].c->m_toiIndex);
		b2Swap(m_heap[idx], m_heap[parent]);
		idx = parent;
	}
}

void b2TOIMinHeap::Build() {
	for (int32 i = m_count / 2 - 1; i >= 0; --i) {
		HeapifyDown(i);
	}

	for (int32 i = 0; i < m_count; ++i) {
		m_heap[i].c->m_toiIndex = i;
	}
}

struct b2TOIQueryWrapper {
	void QueryCallback(b2Fixture* fixture) {
		b2Contact* c = m_contactManager->QueryCallback(m_currentQueryFixture, fixture);
	  
	  if (c != nullptr && c->m_toiIndex == -1) {
	    m_heap->Insert(c);
  	}
	}

  b2Fixture* m_currentQueryFixture;
	b2ContactManager* m_contactManager;
	b2TOIMinHeap* m_heap;
};

// Find TOI contacts and solve them.
void b2World::SolveTOI(const b2TimeStep& step) {
  if (m_contactManager.m_contactCount == 0) {
		return;
	}

	b2Island island(2 * b2_maxTOIContacts, b2_maxTOIContacts, 0, &m_stackAllocator, m_contactManager.m_contactListener);

	if (m_stepComplete) {
		for (b2Body* b = m_bodyListHead; b; b = b->m_next) {
			b->m_flags &= ~b2Body::e_islandFlag;
			b->m_sweep.alpha0 = 0.0f;
		}

  	for (b2Contact* c = m_contactManager.Start(); c != m_contactManager.End(); c = c->GetNext()) {
			// Invalidate TOI
			c->m_flags &= ~b2Contact::e_islandFlag;
			c->m_toiIndex = -1;
		}
	}

	b2TOIMinHeap heap(m_contactManager.m_contactCount * 3 / 2);
	int32 i = 0;

	for (b2Contact* c = m_contactManager.Start(); c != m_contactManager.End(); c = c->GetNext()) {
    if (c->IsEnabled()) {
		  float toi = c->CalculateTOI();

      if (toi < 1.0f) {
		    heap[i].c = c;
	      heap[i].toi = toi;
	      heap[i].toiCount = 0;

		    ++i;
		  }
		}
	}

  heap.m_count = i;
	heap.Build();

  b2TOIQueryWrapper toiCallback;
  toiCallback.m_contactManager = &m_contactManager;
  toiCallback.m_heap = &heap;

	// Find TOI events and solve them.
	while (!heap.IsEmpty()) {
		// Find the first TOI.
		b2HeapNode& min = heap.Min();
		++min.toiCount;
		b2Contact* minContact = min.c;
		float minAlpha = min.toi;

		if (minContact == nullptr || 1.0f - 10.0f * b2_epsilon < minAlpha) {
			// No more TOI events. Done!
			m_stepComplete = true;
			break;
		}

		// Advance the bodies to the TOI.
		b2Fixture* fA = minContact->GetFixtureA();
		b2Fixture* fB = minContact->GetFixtureB();
		b2Body* bA = fA->GetBody();
		b2Body* bB = fB->GetBody();

		b2Sweep backup1 = bA->m_sweep;
		b2Sweep backup2 = bB->m_sweep;

		bA->Advance(minAlpha);
		bB->Advance(minAlpha);

		// The TOI contact likely has some new contact points.
		minContact->Update(m_contactManager.m_contactListener);
		heap.Update(minContact);

		// Is the contact solid?
		if (minContact->IsEnabled() == false || minContact->IsTouching() == false) {
			// Restore the sweeps.
			minContact->SetEnabled(false);
			bA->m_sweep = backup1;
			bB->m_sweep = backup2;
			bA->SynchronizeTransform();
			bB->SynchronizeTransform();
			heap.Update(minContact);
			
			continue;
		}

    SET_AWAKE_OR_NONE(bA);
    SET_AWAKE_OR_NONE(bB);

		// Build the island
		island.Clear();
		island.Add(bA);
		island.Add(bB);
		island.Add(minContact);

		bA->m_flags |= b2Body::e_islandFlag;
		bB->m_flags |= b2Body::e_islandFlag;
		minContact->m_flags |= b2Contact::e_islandFlag;

		// Get contacts on bodyA and bodyB.
		b2Body* bodies[2] = {bA, bB};
		for (int32 i = 0; i < 2; ++i) {
			b2Body* body = bodies[i];
			if (body->m_type == b2_dynamicBody) {
				for (int32 i = 0; i < body->GetContactCount(); ++i) {
					b2Contact* contact = body->GetContact(i);

					if (island.m_bodyCount == island.m_bodyCapacity) {
						break;
					}

					if (island.m_contactCount == island.m_contactCapacity) {
						break;
					}

					// Has this contact already been added to the island?
					if (contact->m_flags & b2Contact::e_islandFlag) {
						continue;
					}
					
			    if ((contact->m_flags & b2Contact::e_persistFlag) == 0) {
			      continue;
			    }

					// Only add static, kinematic, or bullet bodies.
					b2Body* bA = contact->GetFixtureA()->GetBody();
					b2Body* bB = contact->GetFixtureB()->GetBody();
					b2Body* other = (bA == body)? bB : bA;

					if (other->m_type == b2_dynamicBody &&
						body->IsBullet() == false && other->IsBullet() == false) {
						continue;
					}

					// Skip sensors.
					bool sensorA = contact->m_fixtureA->m_isSensor;
					bool sensorB = contact->m_fixtureB->m_isSensor;
					if (sensorA || sensorB) {
						continue;
					}

					// Tentatively advance the body to the TOI.
					b2Sweep backup = other->m_sweep;
					if ((other->m_flags & b2Body::e_islandFlag) == 0) {
						other->Advance(minAlpha);
					}

					// Update the contact points
					contact->Update(m_contactManager.m_contactListener);

					// Was the contact disabled by the user?
					if (contact->IsEnabled() == false) {
						other->m_sweep = backup;
						other->SynchronizeTransform();
						continue;
					}

					// Are there contact points?
					if (contact->IsTouching() == false) {
						other->m_sweep = backup;
						other->SynchronizeTransform();
						continue;
					}

					// Add the contact to the island
					contact->m_flags |= b2Contact::e_islandFlag;
					island.Add(contact);

					// Has the other body already been added to the island?
					if (other->m_flags & b2Body::e_islandFlag) {
						continue;
					}
					
					// Add the other body to the island.
					other->m_flags |= b2Body::e_islandFlag;

					if (other->m_type != b2_staticBody) {
						SET_AWAKE_OR_NONE(other);
					}

					island.Add(other);
				}
			}
		}

		b2TimeStep subStep;
		subStep.dt = (1.0f - minAlpha) * step.dt;
		subStep.inv_dt = 1.0f / subStep.dt;
		subStep.dtRatio = 1.0f;
		subStep.positionIterations = 20;
		subStep.velocityIterations = step.velocityIterations;
		subStep.particleIterations = step.particleIterations;
		subStep.warmStarting = false;
		island.SolveTOI(subStep, bA->m_islandIndex, bB->m_islandIndex);

		// Reset island flags and synchronize broad-phase proxies.
		for (int32 i = 0; i < island.m_bodyCount; ++i) {
			b2Body* body = island.m_bodies[i];
			body->m_flags &= ~b2Body::e_islandFlag;

			if (body->m_type != b2_dynamicBody) {
				continue;
			}
			
			// Synchronize fixtures
			for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
				m_contactManager.m_broadPhase.UpdateNoRebuild(f);
			}

			// Invalidate all contact TOIs on this displaced body.
			for (int32 i = 0; i < body->GetContactCount(); ++i) {
				b2Contact* contact = body->GetContact(i);
        
        if ((contact->m_flags & b2Contact::e_persistFlag) != 0) {
          if (contact->m_toiIndex == -1) {
            heap.Insert(contact);
          } else {
            heap.Update(contact);
          }
        }
        
				contact->m_flags &= ~(b2Contact::e_islandFlag | b2Contact::e_persistFlag);
			}
		}

		// Commit fixture proxy movements to the broad-phase so that new contacts are created.
		// Also, some contacts can be destroyed.
		for (int32 i = 0; i < island.m_bodyCount; ++i) {
			b2Body* body = island.m_bodies[i];

			if (body->m_type == b2_dynamicBody) {
			  for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
          toiCallback.m_currentQueryFixture = f;
			    m_contactManager.m_broadPhase.Query(&toiCallback, f->GetAABB());
			  }
			}
    }

		if (m_subStepping) {
			m_stepComplete = false;
			break;
		}
	}
	
	RemoveDeadContacts();
	m_contactManager.RemoveDeadContacts();
}

void b2World::Step(float dt, int32 velocityIterations, int32 positionIterations, int32 particleIterations)
{
	b2Timer stepTimer;
	m_locked = true;

	// If new fixtures were added, we need to find the new contacts.
	if (m_newContacts) {
		m_contactManager.FindNewContacts();
	}

  if (m_newContacts || m_removedBodies) {
    RemoveDeadContacts();
		m_newContacts = false;
    m_removedBodies = false;
  }

	b2TimeStep step;
	step.dt = dt;
	step.inv_dt = (dt > 0.0f)? 1.0f / dt : 0.0f;
  step.velocityIterations	= velocityIterations;
	step.positionIterations = positionIterations;
	step.particleIterations = particleIterations;
	step.dtRatio = m_inv_dt0 * dt;
	step.warmStarting = m_warmStarting;

	// Update contacts. This is where some contacts are destroyed.
  {
	  b2Timer timer;
	  m_contactManager.Collide();
	  m_profile.collide = timer.GetMilliseconds();
  }

	if (dt > 0.0f) {
		// Integrate velocities, solve velocity constraints, and integrate positions.
	  if (m_stepComplete) {
		  b2Timer timer;

#ifdef ENABLE_LIQUID
		  for (b2ParticleSystem* p = m_particleSystemList; p; p = p->GetNext()) {
			  p->Solve(step); // Particle Simulation
		  }
#endif // ENABLE_LIQUID

		  Solve(step);
		  m_profile.solve = timer.GetMilliseconds();
	  }

	  // Handle TOI events.
	  if (m_continuousPhysics) {
		  b2Timer timer;
		  SolveTOI(step);
		  m_profile.solveTOI = timer.GetMilliseconds();
	  }

		m_inv_dt0 = step.inv_dt;
	}

	if (m_clearForces) {
		ClearForces();
	}

	m_locked = false;
	m_profile.step = stepTimer.GetMilliseconds();
}

void b2World::ClearForces()
{
	for (b2Body* body = m_bodyListHead; body; body = body->GetNext())
	{
		body->m_force.SetZero();
		body->m_torque = 0.0f;
	}
}

struct b2WorldQueryWrapper
{
	bool QueryCallback(b2Fixture* fixture)
	{
		return callback->ReportFixture(fixture);
	}

	const b2BroadPhase* broadPhase;
	b2QueryCallback* callback;
};

void b2World::QueryAABB(b2QueryCallback* callback, const b2AABB& aabb)
{
	b2WorldQueryWrapper wrapper;
	wrapper.broadPhase = &m_contactManager.m_broadPhase;
	wrapper.callback = callback;
	m_contactManager.m_broadPhase.Query(&wrapper, aabb);

#ifdef ENABLE_LIQUID
	for (b2ParticleSystem* p = m_particleSystemList; p; p = p->GetNext()) {
		if (callback->ShouldQueryParticleSystem(p)) {
			p->QueryAABB(callback, aabb);
		}
	}
#endif // ENABLE_LIQUID
}

struct b2WorldRayCastWrapper
{
	float RayCastCallback(const b2RayCastInput& input, b2Fixture* fixture) {
		b2RayCastOutput output;
		bool hit = fixture->RayCast(&output, input);

		if (hit) {
			float fraction = output.fraction;
			b2Vec2 point = (1.0f - fraction) * input.p1 + fraction * input.p2;
			return callback->ReportFixture(fixture, point, output.normal, fraction);
		}

		return input.maxFraction;
	}

	const b2BroadPhase* broadPhase;
	b2RayCastCallback* callback;
};

void b2World::RayCast(b2RayCastCallback* callback, const b2Vec2& point1, const b2Vec2& point2)
{
	b2WorldRayCastWrapper wrapper;
	wrapper.broadPhase = &m_contactManager.m_broadPhase;
	wrapper.callback = callback;
	b2RayCastInput input;
	input.maxFraction = 1.0f;
	input.p1 = point1;
	input.p2 = point2;
	m_contactManager.m_broadPhase.RayCast(&wrapper, input);

#ifdef ENABLE_LIQUID
	for (b2ParticleSystem* p = m_particleSystemList; p; p = p->GetNext()) {
		if (callback->ShouldQueryParticleSystem(p)) {
			p->RayCast(callback, point1, point2);
		}
	}
#endif // ENABLE_LIQUID
}

void b2World::DrawShape(b2Fixture* fixture, const b2Transform& xf, const b2Color& color)
{
	switch (fixture->GetType())
	{
	case b2Shape::e_circle:
		{
			b2CircleShape* circle = (b2CircleShape*)fixture->GetShape();

			b2Vec2 center = b2Mul(xf, circle->m_p);
			float radius = circle->m_radius;
			b2Vec2 axis = b2Mul(xf.q, b2Vec2(1.0f, 0.0f));

			m_debugDraw->DrawSolidCircle(center, radius, axis, color);
		}
		break;

	case b2Shape::e_edge:
		{
			b2EdgeShape* edge = (b2EdgeShape*)fixture->GetShape();
			b2Vec2 v1 = b2Mul(xf, edge->m_vertex1);
			b2Vec2 v2 = b2Mul(xf, edge->m_vertex2);
			m_debugDraw->DrawSegment(v1, v2, color);

			if (edge->m_oneSided == false) {
				m_debugDraw->DrawPoint(v1, 4.0f, color);
				m_debugDraw->DrawPoint(v2, 4.0f, color);
			}
		}
		break;

	case b2Shape::e_polygon:
		{
			b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
			int32 vertexCount = poly->m_count;
			b2Assert(vertexCount <= b2_maxPolygonVertices);
			b2Vec2 vertices[b2_maxPolygonVertices];

			for (int32 i = 0; i < vertexCount; ++i)
			{
				vertices[i] = b2Mul(xf, poly->m_vertices[i]);
			}

			m_debugDraw->DrawSolidPolygon(vertices, vertexCount, color);
		}
		break;

	default:
	break;
	}
}

#ifdef ENABLE_LIQUID
void b2World::DrawParticleSystem(const b2ParticleSystem& system)
{
	int32 particleCount = system.GetParticleCount();
	if (particleCount)
	{
		float32 radius = system.GetRadius();
		const b2Vec2* positionBuffer = system.GetPositionBuffer();
		if (system.m_colorBuffer.data)
		{
			const b2ParticleColor* colorBuffer = system.GetColorBuffer();
			m_debugDraw->DrawParticles(positionBuffer, radius, colorBuffer, particleCount);
		}
		else
		{
			m_debugDraw->DrawParticles(positionBuffer, radius, NULL, particleCount);
		}
	}
}
#endif // ENABLE_LIQUID

void b2World::DebugDraw()
{
	if (m_debugDraw == nullptr)
	{
		return;
	}

	uint32 flags = m_debugDraw->GetFlags();

	if (flags & b2Draw::e_shapeBit)
	{
		for (b2Body* b = m_bodyListHead; b; b = b->GetNext())
		{
			const b2Transform& xf = b->GetTransform();
			for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
			{
				if (b->GetType() == b2_dynamicBody && b->m_mass == 0.0f)
				{
					// Bad body
					DrawShape(f, xf, b2Color(1.0f, 0.0f, 0.0f));
				}
				else if (b->IsEnabled() == false)
				{
					DrawShape(f, xf, b2Color(0.5f, 0.5f, 0.3f));
				}
				else if (b->GetType() == b2_staticBody)
				{
					DrawShape(f, xf, b2Color(0.5f, 0.9f, 0.5f));
				}
				else if (b->GetType() == b2_kinematicBody)
				{
					DrawShape(f, xf, b2Color(0.5f, 0.5f, 0.9f));
				}
				
#ifdef ENABLE_SLEEPING
				else if (b->IsAwake() == false)
				{
					DrawShape(f, xf, b2Color(0.6f, 0.6f, 0.6f));
				}
#endif // ENABLE_SLEEPING

				else
				{
					DrawShape(f, xf, b2Color(0.9f, 0.7f, 0.7f));
				}
			}
		}
	}
	
#ifdef ENABLE_LIQUID
	if (flags & b2Draw::e_particleBit)
	{
		for (b2ParticleSystem* p = m_particleSystemList; p; p = p->GetNext())
		{
			DrawParticleSystem(*p);
		}
	}
#endif // ENABLE_LIQUID
	
	if (flags & b2Draw::e_jointBit)
	{
		for (b2Joint* j = m_jointList; j; j = j->GetNext())
		{
			j->Draw(m_debugDraw);
		}
	}

	if (flags & b2Draw::e_pairBit)
	{
		b2Color color(0.3f, 0.9f, 0.9f);
  	for (b2Contact* c = m_contactManager.Start(); c != m_contactManager.End(); c = c->GetNext()) {
			b2Fixture* fixtureA = c->GetFixtureA();
			b2Fixture* fixtureB = c->GetFixtureB();
			b2Vec2 cA = fixtureA->GetAABB().GetCenter();
			b2Vec2 cB = fixtureB->GetAABB().GetCenter();

			m_debugDraw->DrawSegment(cA, cB, color);
		}
	}

	if (flags & b2Draw::e_aabbBit)
	{
		b2Color color(0.9f, 0.3f, 0.9f);
		b2BroadPhase* bp = &m_contactManager.m_broadPhase;

		for (b2Body* b = m_bodyListHead; b; b = b->GetNext())
		{
			if (b->IsEnabled() == false)
			{
				continue;
			}

			for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
			{
				b2AABB aabb = f->GetAABB();
				b2Vec2 vs[4];
				vs[0].Set(aabb.lowerBound.x, aabb.lowerBound.y);
				vs[1].Set(aabb.upperBound.x, aabb.lowerBound.y);
				vs[2].Set(aabb.upperBound.x, aabb.upperBound.y);
				vs[3].Set(aabb.lowerBound.x, aabb.upperBound.y);
					m_debugDraw->DrawPolygon(vs, 4, color);
			}
		}
	}

	if (flags & b2Draw::e_centerOfMassBit)
	{
		for (b2Body* b = m_bodyListHead; b; b = b->GetNext())
		{
			b2Transform xf = b->GetTransform();
			xf.p = b->GetWorldCenter();
			m_debugDraw->DrawTransform(xf);
		}
	}
}

int32 b2World::GetProxyCount() const
{
	return m_contactManager.m_broadPhase.GetCount();
}

int32 b2World::GetTreeHeight() const
{
	return m_contactManager.m_broadPhase.GetTreeHeight();
}

void b2World::ShiftOrigin(const b2Vec2& newOrigin)
{
	b2Assert(m_locked == false);
	if (m_locked)
	{
		return;
	}

	for (b2Body* b = m_bodyListHead; b; b = b->m_next)
	{
		b->m_xf.p -= newOrigin;
		b->m_sweep.c0 -= newOrigin;
		b->m_sweep.c -= newOrigin;
	}

	for (b2Joint* j = m_jointList; j; j = j->m_next)
	{
		j->ShiftOrigin(newOrigin);
	}

	m_contactManager.m_broadPhase.ShiftOrigin(newOrigin);
}

void b2World::Dump()
{
	if (m_locked)
	{
		return;
	}

	b2OpenDump("box2d_dump.inl");

	b2Dump("b2Vec2 g(%.9g, %.9g);\n", m_gravity.x, m_gravity.y);
	b2Dump("m_world->SetGravity(g);\n");

	b2Dump("b2Body** bodies = (b2Body**)b2Alloc(%d * sizeof(b2Body*));\n", m_bodyCount);
	b2Dump("b2Joint** joints = (b2Joint**)b2Alloc(%d * sizeof(b2Joint*));\n", m_jointCount);

	int32 i = 0;
	for (b2Body* b = m_bodyListHead; b; b = b->m_next)
	{
		b->m_islandIndex = i;
		b->Dump();
		++i;
	}

	i = 0;
	for (b2Joint* j = m_jointList; j; j = j->m_next)
	{
		j->m_index = i;
		++i;
	}

	// First pass on joints, skip gear joints.
	for (b2Joint* j = m_jointList; j; j = j->m_next)
	{
		if (j->m_type == e_gearJoint)
		{
			continue;
		}

		b2Dump("{\n");
		j->Dump();
		b2Dump("}\n");
	}

	// Second pass on joints, only gear joints.
	for (b2Joint* j = m_jointList; j; j = j->m_next)
	{
		if (j->m_type != e_gearJoint)
		{
			continue;
		}

		b2Dump("{\n");
		j->Dump();
		b2Dump("}\n");
	}

	b2Dump("b2Free(joints);\n");
	b2Dump("b2Free(bodies);\n");
	b2Dump("joints = nullptr;\n");
	b2Dump("bodies = nullptr;\n");

	b2CloseDump();
}
