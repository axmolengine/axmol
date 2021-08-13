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

#ifndef B2_CONTACT_H
#define B2_CONTACT_H

#include "b2_api.h"
#include "b2_collision.h"
#include "b2_fixture.h"
#include "b2_math.h"
#include "b2_shape.h"

class b2Body;
class b2Contact;
class b2Fixture;
class b2World;
class b2BlockAllocator;
class b2StackAllocator;
class b2ContactListener;

/// Friction mixing law. The idea is to allow either fixture to drive the friction to zero.
/// For example, anything slides on ice.
inline float b2MixFriction(float friction1, float friction2) {
	return b2Sqrt(friction1 * friction2);
}

/// Restitution mixing law. The idea is allow for anything to bounce off an inelastic surface.
/// For example, a superball bounces on anything.
inline float b2MixRestitution(float restitution1, float restitution2) {
	return restitution1 > restitution2 ? restitution1 : restitution2;
}

typedef void b2EvaluateFunction(b2Manifold* manifold,
															const b2Shape* shapeA, const b2Transform& xfA,
															const b2Shape* shapeB, const b2Transform& xfB);

/// Restitution mixing law. This picks the lowest value.
inline float b2MixRestitutionThreshold(float threshold1, float threshold2)
{
	return threshold1 < threshold2 ? threshold1 : threshold2;
}

/// The class manages contact between two shapes. A contact exists for each overlapping
/// AABB in the broad-phase (except if filtered). Therefore a contact object may exist
/// that has no contact points.
class B2_API b2Contact
{
public:

	/// Get the contact manifold. Do not modify the manifold unless you understand the
	/// internals of Box2D.
	b2Manifold* GetManifold();
	const b2Manifold* GetManifold() const;

	/// Get the world manifold.
	void GetWorldManifold(b2WorldManifold* worldManifold) const;

	/// Is this contact touching?
	bool IsTouching() const;

	/// Enable/disable this contact. This can be used inside the pre-solve
	/// contact listener. The contact is only disabled for the current
	/// time step (or sub-step in continuous collisions).
	void SetEnabled(bool flag);

	/// Has this contact been disabled?
	bool IsEnabled() const;

	/// Get the next contact in the world's contact list.
	b2Contact* GetNext();
	const b2Contact* GetNext() const;

	/// Get fixture A in this contact.
	b2Fixture* GetFixtureA();
	const b2Fixture* GetFixtureA() const;

	/// Get fixture B in this contact.
	b2Fixture* GetFixtureB();
	const b2Fixture* GetFixtureB() const;

#ifdef ENABLE_FRICTION
	/// Override the default friction mixture. You can call this in b2ContactListener::PreSolve.
	/// This value persists until set or reset.
	void SetFriction(float friction);

	/// Get the friction.
	float GetFriction() const;

	/// Reset the friction mixture to the default value.
	void ResetFriction();
#endif // ENABLE_FRICTION

#ifdef ENABLE_RESTITUTION
	/// Override the default restitution mixture. You can call this in b2ContactListener::PreSolve.
	/// The value persists until you set or reset.
	void SetRestitution(float restitution);

	/// Get the restitution.
	float GetRestitution() const;

	/// Reset the restitution to the default value.
	void ResetRestitution();

	/// Override the default restitution velocity threshold mixture. You can call this in b2ContactListener::PreSolve.
	/// The value persists until you set or reset.
	void SetRestitutionThreshold(float threshold);

	/// Get the restitution threshold.
	float GetRestitutionThreshold() const;

	/// Reset the restitution threshold to the default value.
	void ResetRestitutionThreshold();
#endif // ENABLE_RESTITUTION

#ifdef ENABLE_TANGENT_SPEED
	/// Set the desired tangent speed for a conveyor belt behavior. In meters per second.
	void SetTangentSpeed(float speed);

	/// Get the desired tangent speed. In meters per second.
	float GetTangentSpeed() const;
#endif // ENABLE_TANGENT_SPEED

	/// Evaluate this contact with your own manifold and transforms.
	void Evaluate(b2Manifold* manifold, const b2Transform& xfA, const b2Transform& xfB);

protected:
	friend class b2ContactManager;
	friend class b2World;
	friend class b2TOIMinHeap;
	friend class b2TOIQueryWrapper;
	friend class b2ContactSolver;
	friend class b2Body;
	friend class b2Fixture;

	// Flags stored in m_flags
	enum {
		// Used when crawling contact graph when forming islands.
		e_islandFlag		= 0x0001,

		// This contact has persisted from the previous step
		e_persistFlag		= 0x0002,

		// This contact needs filtering because a fixture filter was changed.
		e_filterFlag		= 0x0004,

		// Set when the shapes are touching.
		e_touchingFlag		= 0x0008,

		// This contact can be disabled (by user)
		e_enabledFlag		= 0x0010,

		// This bullet contact had a TOI event
		e_bulletHitFlag		= 0x0020
	};

	/// Flag this contact for filtering. Filtering will occur the next time step.
	void FlagForFiltering();

	static bool InitializeRegisters();
	static b2Contact* Create(b2Fixture* fixtureA, b2Fixture* fixtureB, b2BlockAllocator* allocator);
	static void Destroy(b2Contact* contact, b2BlockAllocator* allocator);

	b2Contact(b2Fixture* fixtureA, b2Fixture* fixtureB, b2EvaluateFunction* evaluateFunction);
	
	void Update(b2ContactListener* listener);

  float CalculateTOI();

	static b2EvaluateFunction* functions[b2Shape::e_typeCount][b2Shape::e_typeCount];
	
	b2EvaluateFunction* m_evaluateFunction;

	// World pool and list pointers.
	b2Contact* m_prev;
	b2Contact* m_next;

	b2Fixture* m_fixtureA;
	b2Fixture* m_fixtureB;

	b2Manifold m_manifold;

	uint32 m_flags;

	int32 m_toiIndex;

#ifdef ENABLE_FRICTION
	float m_friction;
#endif // ENABLE_FRICTION

#ifdef ENABLE_RESTITUTION
	float m_restitution;
	float m_restitutionThreshold;
#endif // ENABLE_RESTITUTION

#ifdef ENABLE_TANGENT_SPEED
	float m_tangentSpeed;
#endif // ENABLE_TANGENT_SPEED
};

inline b2Manifold* b2Contact::GetManifold() {
	return &m_manifold;
}

inline const b2Manifold* b2Contact::GetManifold() const {
	return &m_manifold;
}

inline void b2Contact::GetWorldManifold(b2WorldManifold* worldManifold) const {
	const b2Body* bodyA = m_fixtureA->GetBody();
	const b2Body* bodyB = m_fixtureB->GetBody();
	const b2Shape* shapeA = m_fixtureA->GetShape();
	const b2Shape* shapeB = m_fixtureB->GetShape();

	worldManifold->Initialize(&m_manifold, bodyA->GetTransform(), shapeA->m_radius, bodyB->GetTransform(), shapeB->m_radius);
}

inline void b2Contact::SetEnabled(bool flag) {
	if (flag) {
		m_flags |= e_enabledFlag;
	} else {
		m_flags &= ~e_enabledFlag;
	}
}

inline bool b2Contact::IsEnabled() const {
	return (m_flags & e_enabledFlag) == e_enabledFlag;
}

inline bool b2Contact::IsTouching() const {
	return (m_flags & e_touchingFlag) == e_touchingFlag;
}

inline b2Contact* b2Contact::GetNext() {
	return m_next;
}

inline const b2Contact* b2Contact::GetNext() const {
	return m_next;
}

inline b2Fixture* b2Contact::GetFixtureA() {
	return m_fixtureA;
}

inline const b2Fixture* b2Contact::GetFixtureA() const {
	return m_fixtureA;
}

inline b2Fixture* b2Contact::GetFixtureB() {
	return m_fixtureB;
}

inline const b2Fixture* b2Contact::GetFixtureB() const {
	return m_fixtureB;
}

inline void b2Contact::FlagForFiltering() {
	m_flags |= e_filterFlag;
}

#ifdef ENABLE_FRICTION
inline void b2Contact::SetFriction(float friction) {
	m_friction = friction;
}

inline float b2Contact::GetFriction() const {
	return m_friction;
}

inline void b2Contact::ResetFriction() {
	m_friction = b2MixFriction(m_fixtureA->m_friction, m_fixtureB->m_friction);
}
#endif // ENABLE_FRICTION

#ifdef ENABLE_RESTITUTION
inline void b2Contact::SetRestitution(float restitution) {
	m_restitution = restitution;
}

inline float b2Contact::GetRestitution() const {
	return m_restitution;
}

inline void b2Contact::ResetRestitution() {
	m_restitution = b2MixRestitution(m_fixtureA->m_restitution, m_fixtureB->m_restitution);
}

inline void b2Contact::SetRestitutionThreshold(float threshold)
{
	m_restitutionThreshold = threshold;
}

inline float b2Contact::GetRestitutionThreshold() const
{
	return m_restitutionThreshold;
}

inline void b2Contact::ResetRestitutionThreshold()
{
	m_restitutionThreshold = b2MixRestitutionThreshold(m_fixtureA->m_restitutionThreshold, m_fixtureB->m_restitutionThreshold);
}
#endif // ENABLE_RESTITUTION

#ifdef ENABLE_TANGENT_SPEED
inline void b2Contact::SetTangentSpeed(float speed) {
	m_tangentSpeed = speed;
}

inline float b2Contact::GetTangentSpeed() const {
	return m_tangentSpeed;
}
#endif // ENABLE_TANGENT_SPEED

inline void b2Contact::Evaluate(b2Manifold* manifold, const b2Transform& xfA, const b2Transform& xfB) {
	m_evaluateFunction(manifold, m_fixtureA->GetShape(), xfA, m_fixtureB->GetShape(), xfB);
}

#endif
