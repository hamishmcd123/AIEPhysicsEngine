#pragma once 
#include "Colour.h"
#include "Reflection.h"
#include "RigidBody.h"
#include "Vec2.h"

class Box : public RigidBody {
public:
    Box(const Vec2 pos, const Vec2 velocity, const float mass, const float halfWidth, const float halfHeight, const float orientation, const Colour colour);
    void Draw() override;
    void UpdateLocalAxes();
    void RefreshMoment() override;
    [[nodiscard]] float GetHalfWidth() const {return m_halfWidth;}
    [[nodiscard]] float GetHalfHeight() const {return m_halfHeight;}
    [[nodiscard]] Vec2 GetLocalXAxis() const {return m_localXAxis;}
    [[nodiscard]] Vec2 GetLocalYAxis() const {return m_localYAxis;}
private:
    float m_halfWidth;
    float m_halfHeight;
    Vec2 m_localXAxis;
    Vec2 m_localYAxis;

public:
    BEGIN_REFLECTION(Box)
        REFLECT(m_halfHeight)
        REFLECT(m_halfWidth)
    END_REFLECTION
};
