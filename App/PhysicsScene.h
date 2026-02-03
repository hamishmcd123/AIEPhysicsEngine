#pragma once

#include "Application.h"
#include "Colour.h"
#include "Maths.h"
#include <vector>

class PhysicsObject;

class PhysicsScene : public Application
{
private:
	//Any data that should persist for the duration of your program,
	//declare it here.
	Vec2 m_gravity;
	float m_timestep;
	std::vector<PhysicsObject*> m_actors;

public:
	PhysicsScene();
	~PhysicsScene();
	PhysicsScene(const PhysicsScene& other) = delete;
	PhysicsScene& operator=(const PhysicsScene& other) = delete;
	void Initialise() override;
	void Update(float delta) override;
	void AddActor(PhysicsObject* actor);
	void RemoveActor(PhysicsObject* actor);
	void OnLeftClick() override;
	void SetGravity(const Vec2 gravity) {}
};
