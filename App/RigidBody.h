#pragma once

#include "PhysicsObject.h"
#include "Maths.h"

class RigidBody : public PhysicsObject {
public:
	RigidBody(ShapeType shapeID, Vec2 position, Vec2 velocity, float orientation, float mass);
	void FixedUpdate(Vec2 gravity, float timeStep) override;
	void ApplyForce(Vec2 force);
	void ApplyForceToActor(RigidBody* otherActor, Vec2 force);
	void ResetPosition() override;
	Vec2 GetPosition() { return m_position; };
	float GetOrientation() { return m_orientation; }
	Vec2 GetVelocity() { return m_velocity; }
	float GetMass() { return m_mass; }
	Vec2 ResolveForces();
	void ApplyImpulse(Vec2 impulse);
protected:
	Vec2 m_position;
	Vec2 m_velocity;
	float m_mass;
	float m_invMass;
	float m_orientation;
	Vec2 m_forceAccumulated;
};