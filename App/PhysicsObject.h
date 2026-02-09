#pragma once
#include "math.h"
#include "LineRenderer.h"

enum class ShapeType : int {
	PLANE = 0,
	CIRCLE = 1,
	BOX = 2,
};

class PhysicsObject {
protected:
	PhysicsObject(const ShapeType shapeType) : m_ShapeID(shapeType) {}
public:

    // NOTE: Marked as virtual since we are deleting through this base type.
    virtual ~PhysicsObject() = default;

	virtual void FixedUpdate(Vec2 gravity, float timeStep) = 0;
	virtual void ResetPosition() = 0;
	virtual void Draw() = 0;
	ShapeType m_ShapeID;

    // For collision resolution
    virtual float GetInverseMass() const = 0;
    virtual Vec2 GetVelocity() const = 0;
	virtual void ApplyImpulse(const Vec2 impulse, const Vec2 contactpoint) = 0;
	virtual void ApplyImpulse(const Vec2 impulse) = 0;
    virtual void SetPosition(const Vec2 position) = 0;
    virtual Vec2 GetPosition() const = 0;
	virtual float GetAngularVelocity() const = 0;
	virtual float GetInverseMoment() const = 0;
	static LineRenderer* lines;
};
