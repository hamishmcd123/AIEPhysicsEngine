#include "Circle.h"

Circle::Circle(Vec2 position, Vec2 velocity, float mass, float radius, Colour colour) : RigidBody(ShapeType::CIRCLE, position, velocity, 0, mass)
{
	m_radius = radius;
	m_colour = colour;
}

void Circle::Draw()
{
	lines->DrawCircle(m_position, m_radius, m_colour);
}
