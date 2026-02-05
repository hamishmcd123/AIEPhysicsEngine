#pragma once
#include "Colour.h"
#include "RigidBody.h"
#include "Vec2.h"
#include <vector>

class Polygon : public RigidBody {
public:

    Polygon(Vec2 pos, Vec2 velocity, float mass, int sides, float radius, Colour colour);
    void Draw() override;

private:
    int m_sides;
    float m_inverseMass;
    Colour m_colour;
    float m_sideLength;
    std::vector<Vec2> m_localVertices;
};
