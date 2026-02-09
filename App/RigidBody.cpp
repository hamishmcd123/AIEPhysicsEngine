#include "RigidBody.h"
#include "Colour.h"
#include "Vec2.h"


RigidBody::RigidBody(const ShapeType shapeID, const Vec2 position, const Vec2 velocity, const float orientation, const float mass, const Colour colour) : PhysicsObject(shapeID), m_orientation(orientation), m_colour(colour), m_position(position), m_velocity(velocity), m_mass(mass)
{
	m_invMass = 1.0f / mass;

	// Change this later
	m_torqueAccumulated = 0.0f;
	m_angularVelocity = 0.0f;
}

void RigidBody::FixedUpdate(Vec2 gravity, float timeStep)
{
	// Apply gravity
	ApplyForce(gravity * m_mass);

    // Linear
	const Vec2 acceleration = ResolveForces();
	m_velocity += acceleration * timeStep;
	m_velocity *= 0.99f;
	m_position += m_velocity * timeStep;

    // Rotational
    const float angularAcceleration = ResolveAngular();
    m_angularVelocity += angularAcceleration * timeStep;
	m_angularVelocity *= 0.99f;
    m_orientation += m_angularVelocity * timeStep;

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

void RigidBody::ApplyImpulse(const Vec2 impulse) //This is assumed to be through the centre of mass, so won't impart torque
{
	m_velocity += impulse * m_invMass;
}

void RigidBody::ApplyImpulse(const Vec2 impulse, const Vec2 contactpoint)
{
	m_velocity += impulse * m_invMass;

	Vec2 relativePos = contactpoint - this->m_position;
	float leverArmLength = Dot(relativePos, impulse.GetNormalised().GetRotatedBy270());
	m_angularVelocity += impulse.GetMagnitude() * leverArmLength * m_invMoment;
}

float RigidBody::ResolveAngular() {
	const float angularAcceleration = m_torqueAccumulated * m_invMoment;
    m_torqueAccumulated = 0;
    return angularAcceleration;
}
