#include "RigidBody.h"
#include "Colour.h"
#include "Vec2.h"


RigidBody::RigidBody(const ShapeType shapeID, const Vec2 position, const Vec2 velocity, const float orientation, const float mass, const Colour colour) : PhysicsObject(shapeID), m_orientation(orientation), m_colour(colour), m_position(position), m_velocity(velocity), m_mass(mass)
{
	m_invMass = 1.0f / mass;
}

void RigidBody::FixedUpdate(const Vec2 gravity, const float timeStep)
{
	// Apply gravity
	ApplyForce(gravity * m_mass);

    // Linear
	const Vec2 acceleration = ResolveForces();
	m_velocity += acceleration * timeStep;
	m_position += m_velocity * timeStep;

    // Rotational
    const float angularAcceleration = ResolveAngular();
    m_angularVelocity += angularAcceleration * timeStep;
    m_orientation += angularAcceleration * timeStep;

}

void RigidBody::ApplyForce(const Vec2 force)
{
	m_forceAccumulated += force;
}

void RigidBody::ApplyForceAtPoint(const Vec2 force, const Vec2 pos) {

    m_forceAccumulated += force;

    // Arm length
    Vec2 r = pos - m_position;

    // Add torque
    m_torqueAccumulated += PseudoCross(r, force);

}
void RigidBody::ResetPosition()
{
	m_position = { 0,0 };
}

Vec2 RigidBody::ResolveForces()
{
	const Vec2 acceleration = m_forceAccumulated * m_invMass;
	m_forceAccumulated = { 0, 0 };
	return acceleration;
}

void RigidBody::ApplyImpulse(const Vec2 impulse)
{
	m_velocity += impulse * m_invMass;
}

float RigidBody::ResolveAngular() {
	const float angularAcceleration = m_torqueAccumulated * m_invMoment;
    m_torqueAccumulated = 0;
    return angularAcceleration;
}
