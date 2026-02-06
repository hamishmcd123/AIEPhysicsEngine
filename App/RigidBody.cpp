#include "RigidBody.h"
#include "Vec2.h"


RigidBody::RigidBody(ShapeType shapeID, Vec2 position, Vec2 velocity, float orientation, float mass) : PhysicsObject(shapeID), m_position(position), m_velocity(velocity), m_orientation(orientation), m_mass(mass)
{
	m_invMass = 1.0f / mass;
}

void RigidBody::FixedUpdate(Vec2 gravity, float timeStep)
{
	// Apply gravity
	ApplyForce(gravity * m_mass);

    // Linear
	Vec2 acceleration = ResolveForces();
	m_velocity += acceleration * timeStep;
	m_position += m_velocity * timeStep;

    // Rotational
    float angularAcceleration = ResolveAngular();
    m_angularVelocity += angularAcceleration * timeStep;
    m_orientation += angularAcceleration * timeStep;

}

void RigidBody::ApplyForce(Vec2 force)
{
	m_forceAccumulated += force;
}

void RigidBody::ApplyForceAtPoint(Vec2 force, Vec2 pos) {

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
	Vec2 acceleration;
	acceleration = m_forceAccumulated * m_invMass;
	m_forceAccumulated = { 0, 0 };
	return acceleration;
}

void RigidBody::ApplyImpulse(Vec2 impulse)
{
	m_velocity += impulse * m_invMass;
}

float RigidBody::ResolveAngular() {
    float angularAcceleration;
    angularAcceleration = m_torqueAccumulated * m_invMoment;
    m_torqueAccumulated = 0;
    return angularAcceleration;
}
