#pragma once

#include "Colour.h"
#include "PhysicsObject.h"

class RigidBody : public PhysicsObject {
public:
	RigidBody(ShapeType shapeID, Vec2 position, Vec2 velocity, float orientation, float mass, Colour colour);
	void FixedUpdate(Vec2 gravity, float timeStep) override;
	void ApplyForce(Vec2 force);
    void ApplyForceAtPoint(Vec2 force, Vec2 pos);
	void ResetPosition() override;
	[[nodiscard]] float GetOrientation() const { return m_orientation; }
	[[nodiscard]] Vec2 GetVelocity() const override { return m_velocity; }
    void SetVelocity(const Vec2 velocity) override {m_velocity = velocity;}
	[[nodiscard]] float GetMass() const { return m_mass; }
    [[nodiscard]] float GetInverseMass() const override {return m_invMass;}
	Vec2 ResolveForces();
    float ResolveAngular();
	void ApplyImpulse(Vec2 impulse);
    [[nodiscard]] Vec2 GetPosition() const override {return m_position;}
    void SetPosition(const Vec2 position) override {m_position = position; }
    void SetColour(const Colour colour) {m_colour = colour;}
	float m_orientation;
    [[nodiscard]] Colour GetColour() const {return m_colour;}
protected:
    Colour m_colour;
	Vec2 m_position;
	Vec2 m_velocity;
	float m_mass;
	float m_invMass;
    float m_angularVelocity;
    float m_moment;
    float m_invMoment;
	Vec2 m_forceAccumulated;
    float m_torqueAccumulated;
};
