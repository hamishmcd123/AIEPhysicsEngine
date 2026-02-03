#pragma once 
#include "RigidBody.h"
#include "Colour.h"

class Circle : public RigidBody {
public:
	Circle(Vec2 position, Vec2 velocity, float mass, float radius, Colour colour);
	void Draw() override;
    float GetRadius() {return m_radius;}
	
protected:
	float m_radius;
	Colour m_colour;
};
