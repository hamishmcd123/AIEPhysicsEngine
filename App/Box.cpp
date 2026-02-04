#include "Box.h"
#include "Colour.h"
#include "PhysicsObject.h"
#include "RigidBody.h"
#include "imgui.h"

Box::Box(Vec2 pos, Vec2 velocity, float mass, float halfWidth, float halfHeight, Colour colour) : RigidBody(ShapeType::BOX, pos, velocity, 0, mass), m_halfHeight(halfHeight), m_halfWidth(halfWidth), m_colour(colour)  {
    m_inverseMass = 1 / m_mass;
}

void Box::Draw() {
    
    // Top left to top right
    lines->DrawLineSegment(
        { m_position.x - m_halfWidth,  m_position.y - m_halfHeight },
        { m_position.x + m_halfWidth,  m_position.y - m_halfHeight },
        m_colour
    );

    // Bottom left to bottom right
    lines->DrawLineSegment(
        { m_position.x - m_halfWidth,  m_position.y + m_halfHeight },
        { m_position.x + m_halfWidth,  m_position.y + m_halfHeight },
        m_colour
    );

    // Top left to bottom left
    lines->DrawLineSegment(
        { m_position.x - m_halfWidth,  m_position.y - m_halfHeight },
        { m_position.x - m_halfWidth,  m_position.y + m_halfHeight },
        m_colour
    );

    // Top right to bottom right
    lines->DrawLineSegment(
        { m_position.x + m_halfWidth,  m_position.y - m_halfHeight },
        { m_position.x + m_halfWidth,  m_position.y + m_halfHeight },
        m_colour
    );
}
