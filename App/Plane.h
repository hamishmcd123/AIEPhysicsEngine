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
	Vec2 GetNormal() { return m_normal; }
	float GetDistance() { return m_distanceToOrigin; }
protected:
	Vec2 m_normal;
	float m_distanceToOrigin;
	
};

