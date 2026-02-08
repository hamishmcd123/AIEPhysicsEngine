#include "Box.h"
#include "Colour.h"
#include "PhysicsObject.h"
#include "RigidBody.h"

Box::Box(const Vec2 pos, const Vec2 velocity, const float mass, const float halfWidth, const float halfHeight, const Colour colour) : RigidBody(ShapeType::BOX, pos, velocity, 0.0f, mass, colour), m_halfHeight(halfHeight), m_halfWidth(halfWidth)  {
    m_moment = 1.0f / 12.0f * m_mass * (2 * halfWidth) + (2 * halfHeight);
    m_invMoment = 1 / m_moment;
    m_localXAxis = Vec2{1.0f, 0.0}.RotateBy(m_orientation);    
    m_localYAxis = Vec2{0.0f, 1.0f}.RotateBy(m_orientation);    
}

void Box::UpdateLocalAxes() {
    m_localXAxis = Vec2{1.0f, 0.0}.RotateBy(m_orientation);
    m_localYAxis = Vec2{0.0f, 1.0f}.RotateBy(m_orientation);
}

void Box::Draw() {
    UpdateLocalAxes();

    const Vec2 xOffset = m_localXAxis * m_halfWidth;
    const Vec2 yOffset = m_localYAxis * m_halfHeight;

    const Vec2 topLeft     = m_position - xOffset - yOffset;
    const Vec2 topRight    = m_position + xOffset - yOffset;
    const Vec2 bottomLeft  = m_position - xOffset + yOffset;
    const Vec2 bottomRight = m_position + xOffset + yOffset;

    lines->DrawLineSegment(topLeft, topRight, m_colour);
    // Bottom
    lines->DrawLineSegment(bottomLeft, bottomRight, m_colour);
    // Left
    lines->DrawLineSegment(topLeft, bottomLeft, m_colour);
    // Right
    lines->DrawLineSegment(topRight, bottomRight, m_colour);}
