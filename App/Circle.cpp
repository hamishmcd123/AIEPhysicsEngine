#include "Circle.h"

Circle::Circle(const Vec2 position, const Vec2 velocity, const float mass, const float radius, const Colour colour) : RigidBody(ShapeType::CIRCLE, position, velocity, 0, mass, colour), m_radius(radius)
{
}

void Circle::Draw()
{
	lines->DrawCircle(m_position, m_radius, m_colour);
}
