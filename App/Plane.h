#pragma once 
#include "PhysicsObject.h"
#include "Maths.h"


class Plane : public PhysicsObject {
public:
	Plane();
	Plane(Vec2 normal, float distance);
	void FixedUpdate(Vec2 gravity, float timeStep) override;
	void Draw() override;
	void ResetPosition() override;
    float GetInverseMass() override {return 0.0f;}
    Vec2 GetVelocity() override {return {0.0f, 0.0f};}
	Vec2 GetNormal() { return m_normal; }
    void SetVelocity(const Vec2 velocity) override {return;}
	float GetDistance() { return m_distanceToOrigin; }
    void SetPosition(const Vec2 position) override { }
    Vec2 GetPosition() override {return {0,0};}
protected:
	Vec2 m_normal;
	float m_distanceToOrigin;
	
};

