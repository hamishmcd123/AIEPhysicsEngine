#pragma once

#include "Application.h"
#include "PhysicsObject.h"
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
    bool m_debugShowContactPoints = false;
    bool m_isPhysicsSimulating = false;

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
    void OnRightClick() override;

	typedef CollisionInfo (*CollisionFunction)(PhysicsObject*, PhysicsObject*);
    //index = (A->m_ShapeID * N) + B
	CollisionFunction CollisionFunctions[9] = {Plane2Plane, Plane2Sphere, Plane2Box,
                                               Sphere2Plane, Sphere2Sphere, Sphere2Box,
                                                Box2Plane, Box2Sphere, Box2Box};

    static CollisionInfo Sphere2Sphere(PhysicsObject* A, PhysicsObject* B);
    static CollisionInfo Sphere2Plane(PhysicsObject* A, PhysicsObject* B);
    static CollisionInfo Sphere2Box(PhysicsObject* A, PhysicsObject* B);

    static CollisionInfo Plane2Sphere(PhysicsObject* A, PhysicsObject* B);
    static CollisionInfo Plane2Plane(PhysicsObject* A, PhysicsObject* B);
    static CollisionInfo Plane2Box(PhysicsObject* A, PhysicsObject* B);

    static CollisionInfo Box2Plane(PhysicsObject* A, PhysicsObject* B);
    static CollisionInfo Box2Sphere(PhysicsObject* A, PhysicsObject* B);
    static CollisionInfo Box2Box(PhysicsObject* A, PhysicsObject* B);

    void DisplayActor(PhysicsObject* Actor);
    void SetUpImGUItheme();
   
    // NOTE: Only supports linear collisions for now.
    void ResolveCollisions(PhysicsObject* A, PhysicsObject* B, const CollisionInfo& info);

};
