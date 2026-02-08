#include "Polygon.h"
#include "Colour.h"
#include "PhysicsObject.h"
#include <cmath>

Polygon::Polygon(Vec2 pos, Vec2 velocity, float mass, int sides, float sidelength, Colour colour) : RigidBody(ShapeType::POLYGON, pos, velocity, 0, mass, colour), m_sides(sides), m_sideLength(sidelength) {
    m_inverseMass = 1 / m_mass;
    m_localVertices.reserve(m_sides);

    // Angle offset so that facing correct way

    float offset = PI / 2;
    for (int i = 0; i < m_sides; i++) {
        float R = sidelength / (2 * sin(PI / m_sides));
        m_localVertices.push_back(Vec2{R * cos(2 * i * PI/m_sides + offset),R * sin(2 * i * PI / m_sides + offset)});
    }

}

void Polygon::Draw() {
    
    for (int i = 0; i < m_sides; i++) {
        const int next = (i + 1) % m_sides;
        lines->DrawLineSegment(m_position + m_localVertices[i], m_position + m_localVertices[next]);
    }

}


