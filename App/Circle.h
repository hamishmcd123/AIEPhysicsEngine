#pragma once 
#include "RigidBody.h"
#include "Colour.h"

class Circle : public RigidBody {
public:
	Circle(const Vec2 position, const Vec2 velocity, const float mass, const float radius, const float orientation, const Colour colour);
	void Draw() override;
    [[nodiscard]] float GetRadius() const {return m_radius;}
	
protected:
	float m_radius;
};
