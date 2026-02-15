#include "Circle.h"

Circle::Circle(const Vec2 position, const Vec2 velocity, const float mass, const float radius, const float orientation, const Colour colour) : RigidBody(ShapeType::CIRCLE, position, velocity, orientation, mass, colour), m_radius(radius)
{
	m_moment = 0.5f * m_mass * m_radius * m_radius;
	m_invMoment = 1 / m_moment;
}

void Circle::Draw()
{
	lines->DrawCircle(m_position, m_radius, m_colour);
}

void Circle::RefreshMoment()
{
	m_moment = 0.5f * m_mass * m_radius * m_radius;
	m_invMoment = 1 / m_moment;
}
