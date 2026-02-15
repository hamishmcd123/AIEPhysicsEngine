#pragma once

#include "Application.h"
#include "PhysicsObject.h"
#include <vector>
#include "Serialiser.h"

class PhysicsObject;
struct CollisionInfo;


// These are the default values for objects being created
struct ObjectCreatorInfo {

    ShapeType shapetype = ShapeType::BOX;
    Vec2 velocity = { 0.0f, 0.0f };
    float mass = 1.0f;
    float orientation = 0.0f;
    Colour colour = Colour::RED;

    // Specific to circle 
    float radius = 0.25f;

    // Specific to box
    float halfwidth = 0.25f;
    float halfheight = 0.25f;

    // Specific to plane
    float distance = 0.0f;
    Vec2 normal = { 0.0f, 0.0f };
};


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
    Serialiser serialiser;
    float elasticity = 0.3f;

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
    void ClearAllActor();
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
    void DrawSceneGraph();
    void DrawDebugOptions();

    // NOTE: Only supports linear collisions for now.
    void ResolveCollisions(PhysicsObject* A, PhysicsObject* B, const CollisionInfo& info);


    static void SDLCALL OnLoadFileSelected(void* userdata, const char* const* filelist, int filter); 
    static void SDLCALL SaveFile(void* userdata, const char* const* filelist, int filter);

    void OpenLoadFileDialogue(void* reference);
    void OpenSaveFileDialogue(json& data);

    void OnKeyPress(Key key) override;
    void DrawObjectCreator();
    ObjectCreatorInfo creatorInfo;

};
