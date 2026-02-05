#pragma once 
#include "Colour.h"
#include "RigidBody.h"
#include "Vec2.h"

class Box : public RigidBody {
public:
    Box(Vec2 pos, Vec2 velocity, float mass, float halfWidth, float halfHeight, Colour colour);
    void Draw() override;
    float GetHalfWidth() {return m_halfWidth;}
    float GetHalfHeight() {return m_halfHeight;}
private:
    float m_halfWidth;
    float m_halfHeight;
    float m_inverseMass;
    Colour m_colour;
};
