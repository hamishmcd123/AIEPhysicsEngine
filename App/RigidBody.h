#pragma once

#include "PhysicsObject.h"
#include "Maths.h"

class RigidBody : public PhysicsObject {
public:
	RigidBody(ShapeType shapeID, Vec2 position, Vec2 velocity, float orientation, float mass);
	void FixedUpdate(Vec2 gravity, float timeStep) override;
	void ApplyForce(Vec2 force);
    void ApplyForceAtPoint(Vec2 force, Vec2 pos);
	void ResetPosition() override;
	float GetOrientation() { return m_orientation; }
	Vec2 GetVelocity() override { return m_velocity; }
    void SetVelocity(const Vec2 velocity) override {m_velocity = velocity;}
	float GetMass() { return m_mass; }
    float GetInverseMass() override {return m_invMass;}
	Vec2 ResolveForces();
    float ResolveAngular();
	void ApplyImpulse(Vec2 impulse);
    Vec2 GetPosition() override {return m_position;}
    void SetPosition(const Vec2 position) override {m_position = position; }
protected:
	Vec2 m_position;
	Vec2 m_velocity;
	float m_mass;
	float m_invMass;
	float m_orientation;
    float m_angularVelocity;
    float m_moment;
    float m_invMoment;
	Vec2 m_forceAccumulated;
    float m_torqueAccumulated;
};
