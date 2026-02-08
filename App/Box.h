#pragma once 
#include "Colour.h"
#include "RigidBody.h"
#include "Vec2.h"

class Box : public RigidBody {
public:
    Box(Vec2 pos, Vec2 velocity, float mass, float halfWidth, float halfHeight, Colour colour);
    void Draw() override;
    void UpdateLocalAxes();
    [[nodiscard]] float GetHalfWidth() const {return m_halfWidth;}
    [[nodiscard]] float GetHalfHeight() const {return m_halfHeight;}
    [[nodiscard]] Vec2 GetLocalXAxis() const {return m_localXAxis;}
    [[nodiscard]] Vec2 GetLocalYAxis() const {return m_localYAxis;}
private:
    float m_halfWidth;
    float m_halfHeight;
    Vec2 m_localXAxis;
    Vec2 m_localYAxis;
};
