#pragma once 
#include "PhysicsObject.h"


class Plane : public PhysicsObject {
public:
	Plane();
	Plane(Vec2 normal, float distance);
	void FixedUpdate(Vec2 gravity, float timeStep) override;
	void Draw() override;
	void ResetPosition() override;
    [[nodiscard]] float GetInverseMass() const override {return 0.0f;}
    [[nodiscard]] Vec2 GetVelocity() const override {return {0.0f, 0.0f};}
	[[nodiscard]] Vec2 GetNormal() const { return m_normal; }
    void SetVelocity(const Vec2 velocity) override {}
	[[nodiscard]] float GetDistance() const { return m_distanceToOrigin; }
    void SetPosition(const Vec2 position) override { }
    [[nodiscard]] Vec2 GetPosition() const override {return {0,0};}
protected:
	Vec2 m_normal;
	float m_distanceToOrigin;
	
};

