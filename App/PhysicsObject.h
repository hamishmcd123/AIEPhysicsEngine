#pragma once
#include "math.h"
#include "LineRenderer.h"

enum class ShapeType {
	PLANE,
	CIRCLE,
	BOX
};


class PhysicsObject {
protected:
	PhysicsObject(ShapeType shapeType) : m_ShapeID(shapeType) {}
public:
	virtual void FixedUpdate(Vec2 gravity, float timeStep) = 0;
	virtual void ResetPosition() = 0;
	virtual void Draw() = 0;
	ShapeType m_ShapeID;
	static LineRenderer* lines;
};
