#include "Box.h"
#include "Colour.h"
#include "PhysicsObject.h"
#include "RigidBody.h"
#include "imgui.h"

Box::Box(Vec2 pos, Vec2 velocity, float mass, float halfWidth, float halfHeight, Colour colour) : RigidBody(ShapeType::BOX, pos, velocity, 0.0f, mass), m_halfHeight(halfHeight), m_halfWidth(halfWidth), m_colour(colour)  {
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

    Vec2 xOffset = m_localXAxis * m_halfWidth;
    Vec2 yOffset = m_localYAxis * m_halfHeight;

    Vec2 topLeft     = m_position - xOffset - yOffset;
    Vec2 topRight    = m_position + xOffset - yOffset;
    Vec2 bottomLeft  = m_position - xOffset + yOffset;
    Vec2 bottomRight = m_position + xOffset + yOffset;

    lines->DrawLineSegment(topLeft, topRight, m_colour);
    // Bottom
    lines->DrawLineSegment(bottomLeft, bottomRight, m_colour);
    // Left
    lines->DrawLineSegment(topLeft, bottomLeft, m_colour);
    // Right
    lines->DrawLineSegment(topRight, bottomRight, m_colour);}
