#include "RigidBody.h"


RigidBody::RigidBody(ShapeType shapeID, Vec2 position, Vec2 velocity, float orientation, float mass) : PhysicsObject(shapeID), m_position(position), m_velocity(velocity), m_orientation(orientation), m_mass(mass)
{
	m_invMass = 1.0f / mass;
}

void RigidBody::FixedUpdate(Vec2 gravity, float timeStep)
{
	// Apply gravity
	ApplyForce(gravity * m_mass);

	Vec2 acceleration = ResolveForces();

	m_velocity += acceleration * timeStep;
	m_position += m_velocity * timeStep;
}

void RigidBody::ApplyForce(Vec2 force)
{
	m_forceAccumulated += force;
}

void RigidBody::ApplyForceToActor(RigidBody* otherActor, Vec2 force)
{
	otherActor->ApplyForce(force);
	ApplyForce(-1.0f * force);
}

void RigidBody::ResetPosition()
{
	m_position = { 0,0 };
}

Vec2 RigidBody::ResolveForces()
{
	Vec2 acceleration;
	acceleration = m_forceAccumulated * m_invMass;
	m_forceAccumulated = { 0, 0 };
	return acceleration;
}

void RigidBody::ApplyImpulse(Vec2 impulse)
{
	m_velocity += impulse * m_invMass;
}


