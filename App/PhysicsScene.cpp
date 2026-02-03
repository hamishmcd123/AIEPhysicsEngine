#include "PhysicsScene.h"
#include "Colour.h"
#include "LineRenderer.h"
#include "imgui.h"
#include <SDL3/SDL_timer.h>
#include <cmath>
#include <string>
#include <algorithm>
#include "PhysicsObject.h"
#include "Circle.h"

PhysicsScene::PhysicsScene()
{
	//Use the constructor to set up the application info, because the harness
	//needs this information early so it can set the name of the window when
	//it creates it.
	appInfo.appName = "Example Program";
}

PhysicsScene::~PhysicsScene()
{
	for (PhysicsObject* actor : m_actors) {
		delete actor;
	}
}

void PhysicsScene::Initialise()
{
	//You should set up your application here. This function gets called
	//after the window is set up and the rendering context is created, so if
	//you're doing anything weird with rendering, OpenGL functions will be
	//available at this point.

	PhysicsObject::lines = lines;
	m_gravity = { 0, -9.81f };
	AddActor(new Circle(Vec2{0.0f, 5.0f}, Vec2{0.0f, 0.0f}, 1.0f, 1.0f, Colour::RED));
}

void PhysicsScene::Update(float delta)
{
	//Everything that your program does every frame should go here.
	//This includes rendering done with the line renderer!
	for (PhysicsObject* actor : m_actors) {
		actor->FixedUpdate(m_gravity, delta);
	}
	// TODO: Move this to rendering()? Only problem is that we would have to do temporal anti-aliasing.
	for (PhysicsObject* actor : m_actors) {
		actor->Draw();
	}
}

void PhysicsScene::AddActor(PhysicsObject* actor)
{
	m_actors.push_back(actor);
}

void PhysicsScene::RemoveActor(PhysicsObject* actor)
{
	auto it = std::remove(m_actors.begin(), m_actors.end(), actor);
	m_actors.erase(it, m_actors.end());
}

void PhysicsScene::OnLeftClick()
{
		for (PhysicsObject* actor : m_actors) {
			if (RigidBody* converted = dynamic_cast<RigidBody*>(actor)) {
				converted->ApplyImpulse(Vec2{0, 5});
			}
		}

}
