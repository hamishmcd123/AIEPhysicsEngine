#pragma once 
#include "RigidBody.h"
#include "Colour.h"

class Circle : public RigidBody {
public:
	Circle(Vec2 position, Vec2 velocity, float mass, float radius, Colour colour);
	void Draw() override;
    [[nodiscard]] float GetRadius() const {return m_radius;}
	
protected:
	float m_radius;
};
