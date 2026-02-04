#pragma once

#include "Application.h"
#include "Colour.h"
#include "Maths.h"
#include "PhysicsObject.h"
#include "TextStream.h"
#include <vector>

class PhysicsObject;
struct CollisionInfo;

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

	typedef CollisionInfo (*CollisionFunction)(PhysicsObject*, PhysicsObject*);
    //index = (A->m_ShapeID * N) + B
	CollisionFunction CollisionFunctions[9] = {Plane2Plane, Plane2Sphere, nullptr,
                                               Sphere2Plane, Sphere2Sphere, nullptr,
                                                nullptr, nullptr, nullptr};

    static CollisionInfo Sphere2Sphere(PhysicsObject* A, PhysicsObject* B);
    static CollisionInfo Plane2Sphere(PhysicsObject* A, PhysicsObject* B);
    static CollisionInfo Sphere2Plane(PhysicsObject* A, PhysicsObject* B);
    static CollisionInfo Plane2Plane(PhysicsObject* A, PhysicsObject* B);
   
    // NOTE: Only supports linear collisions for now.
    void ResolveCollisions(PhysicsObject* A, PhysicsObject* B, const CollisionInfo& info);

};
