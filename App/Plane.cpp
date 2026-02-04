#include "Plane.h"

Plane::Plane() : PhysicsObject(ShapeType::PLANE)
{
	m_distanceToOrigin = 0;
	m_normal = { 0, 1 };
}

Plane::Plane(Vec2 normal, float distance) : PhysicsObject(ShapeType::PLANE), m_normal(normal), m_distanceToOrigin(distance)
{
	// Normalise normal just in case
	m_normal.Normalise();
}

void Plane::FixedUpdate(Vec2 gravity, float timeStep)
{

}

void Plane::Draw()
{
	Vec2 PlaneCenter = m_distanceToOrigin * m_normal;
	
	// Get line directions
	Vec2 dir1 = m_normal.GetRotatedBy90();
	Vec2 dir2 = m_normal.GetRotatedBy270();
	
	lines->DrawLineSegment(PlaneCenter, PlaneCenter + 25 * dir1);
	lines->DrawLineSegment(PlaneCenter, PlaneCenter + 25 * dir2);
}

void Plane::ResetPosition()
{
	m_distanceToOrigin = 0;
}
