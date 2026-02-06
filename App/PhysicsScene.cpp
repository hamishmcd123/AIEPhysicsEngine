#include "PhysicsScene.h"
#include "Colour.h"
#include "Vec2.h"
#include "imgui.h"
#include <algorithm>
#include <unistd.h>
#include "PhysicsObject.h"
#include "Circle.h"
#include "Plane.h"
#include "Box.h"
#include "CollisionInfo.h"

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

    AddActor(new Circle(cursorPos, {0.0f, 2.0f}, 10.0f, 0.5f, Colour::BLUE));
    AddActor(new Box({0.0f, 5.0f}, {0.0f, 0.0f}, 20.0f, 0.5f, 0.5f, Colour::RED));
    AddActor(new Plane({0.0f, 1.0f}, 0.0f));
}

void PhysicsScene::Update(float delta)
{

	ImGui::Begin("Window");
	ImGui::End();

	//Everything that your program does every frame should go here.
	//This includes rendering done with the line renderer!
	for (PhysicsObject* actor : m_actors) {
		actor->FixedUpdate(m_gravity, delta);
	}

    for (int outer = 0; outer < m_actors.size(); outer++) {
            PhysicsObject* A = m_actors[outer];
            for (int inner = outer + 1; inner <m_actors.size(); inner++) {
                PhysicsObject* B = m_actors[inner];

                //index = (A->m_ShapeID * N) + B
                int index = static_cast<int>(A->m_ShapeID) * 3 + static_cast<int>(B->m_ShapeID);
                CollisionInfo info = CollisionFunctions[index](A, B);
                if (info.isColliding) {
					ResolveCollisions(A, B, info);
                }
        }
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
    auto it = std::remove_if(m_actors.begin(), m_actors.end(), [actor](PhysicsObject* a){
            if (a == actor) {
                delete a;
                return true;
            }
            return false;
        });

    m_actors.erase(it, m_actors.end());
}

void PhysicsScene::OnLeftClick()
{
    AddActor(new Circle(cursorPos, {0.0f, 0.0f}, 10.0f, 0.5f, Colour::BLUE));

}


void PhysicsScene::OnRightClick() {

    AddActor(new Box(cursorPos, {0.0f, 0.0f}, 20.0f, 0.5f, 0.5f, Colour::RED));

}


// WE ARE DOING B->A FOR NORMALS

CollisionInfo PhysicsScene::Sphere2Plane(PhysicsObject *A, PhysicsObject *B) {

    // NOTE: For circle-plane collisions, the collision normal will be either -1 * planeNormal or the planeNormal. 
        CollisionInfo info;
        Circle* CircleA = static_cast<Circle*>(A);
        Plane* PlaneB = static_cast<Plane*>(B);

        if (abs(Dot(CircleA->GetPosition(), PlaneB->GetNormal()) - PlaneB->GetDistance()) <= CircleA->GetRadius())  {
            info.isColliding = true;

            float distanceToPlane = Dot(CircleA->GetPosition(), PlaneB->GetNormal()) - PlaneB->GetDistance();
            (distanceToPlane > 0) ? info.collisionNormal = PlaneB->GetNormal() : info.collisionNormal = -1.0f * PlaneB->GetNormal();
            info.penetrationDepth = CircleA->GetRadius() - abs(distanceToPlane);
        }
    return info;

}

CollisionInfo PhysicsScene::Plane2Sphere(PhysicsObject *A, PhysicsObject *B) {

    // NOTE: For circle-plane collisions, the collision normal will be either -1 * planeNormal or the planeNormal. 
            CollisionInfo info;
            Plane* PlaneA = static_cast<Plane*>(A);
            Circle* CircleB = static_cast<Circle*>(B);
            
            if (abs(Dot(CircleB->GetPosition(), PlaneA->GetNormal()) - PlaneA->GetDistance()) <= CircleB->GetRadius())  {
                info.isColliding = true;

                // Get normal direction
                float distanceToPlane = Dot(CircleB->GetPosition(), PlaneA->GetNormal()) - PlaneA->GetDistance();
                (distanceToPlane > 0) ? info.collisionNormal = -1.0f * PlaneA->GetNormal() : info.collisionNormal = PlaneA->GetNormal();
                info.penetrationDepth = CircleB->GetRadius() - abs(distanceToPlane);
            }
        
        return info;
}


CollisionInfo PhysicsScene::Sphere2Sphere(PhysicsObject *A, PhysicsObject *B) {

    CollisionInfo info;

    Circle* CircleA = static_cast<Circle*>(A);
    Circle* CircleB = static_cast<Circle*>(B);

    // TODO: Clean this up
    if ((CircleA->GetPosition() - CircleB->GetPosition()).GetMagnitudeSquared() < (CircleA->GetRadius() + CircleB->GetRadius()) * (CircleA->GetRadius() + CircleB->GetRadius())) {
        info.isColliding = true;
        info.collisionNormal = (CircleA->GetPosition() - CircleB->GetPosition()).Normalise();
        info.penetrationDepth = (CircleA->GetRadius() + CircleB->GetRadius()) - (CircleA->GetPosition() - CircleB->GetPosition()).GetMagnitude();
    }

    return info;
}

CollisionInfo PhysicsScene::Plane2Plane(PhysicsObject *A, PhysicsObject *B) {
    // don't do anything;
    return CollisionInfo();
}

CollisionInfo PhysicsScene::Box2Plane(PhysicsObject *A, PhysicsObject *B) {
    
    CollisionInfo info;
    Box* BoxA = static_cast<Box*>(A);
    Plane* PlaneB = static_cast<Plane*>(B);

        BoxA->UpdateLocalAxes();

        float distance = Dot(BoxA->GetPosition(), PlaneB->GetNormal()) - PlaneB->GetDistance();
        float r = BoxA->GetHalfWidth() * abs(Dot(BoxA->GetLocalXAxis(), PlaneB->GetNormal())) + BoxA->GetHalfHeight() * abs(Dot(BoxA->GetLocalYAxis(), PlaneB->GetNormal()));
        
        if (abs(distance) <= r) {
            info.isColliding = true;
            info.penetrationDepth = r - abs(distance);
            info.collisionNormal = distance > 0 ? PlaneB->GetNormal() : -1.0f * PlaneB->GetNormal();
        }

        return info;
};

CollisionInfo PhysicsScene::Plane2Box(PhysicsObject *A, PhysicsObject *B) {
    CollisionInfo info;
    Box* BoxB = static_cast<Box*>(B);
    Plane* PlaneA = static_cast<Plane*>(A);

        float distance = Dot(BoxB->GetPosition(), PlaneA->GetNormal()) - PlaneA->GetDistance();
        float r = BoxB->GetHalfWidth() * abs(Dot(BoxB->GetLocalXAxis(), PlaneA->GetNormal())) + BoxB->GetHalfHeight() * abs(Dot(BoxB->GetLocalYAxis(), PlaneA->GetNormal()));
        
        if (abs(distance )<= r) {
            info.isColliding = true;
            info.penetrationDepth = r - abs(distance);
            info.collisionNormal = distance > 0 ? -1.0f * PlaneA->GetNormal() : PlaneA->GetNormal();
        }
    return info;
}

CollisionInfo PhysicsScene::Box2Sphere(PhysicsObject *A, PhysicsObject *B) {
    CollisionInfo info;
    Box* BoxA = static_cast<Box*>(A);
    Circle* CircleB = static_cast<Circle*>(B);

    // If AABB
    if (BoxA->GetOrientation() == 0) {

        // Get position on box that is closet to circle.
        Vec2 closest = {Clamp<float>(CircleB->GetPosition().x, BoxA->GetPosition().x - BoxA->GetHalfWidth(), BoxA->GetPosition().x + BoxA->GetHalfWidth()), 
        Clamp<float>(CircleB->GetPosition().y, BoxA->GetPosition().y - BoxA->GetHalfHeight(), BoxA->GetPosition().y + BoxA->GetHalfHeight())};

        float distance = (closest - CircleB->GetPosition()).GetMagnitude();
        
        if (distance <= CircleB->GetRadius()) {
            info.isColliding = true;
            info.penetrationDepth = CircleB->GetRadius() - distance;
            info.collisionNormal = (closest - CircleB->GetPosition()).Normalise();
        }
        return info;
    }
    return info; 
}

CollisionInfo PhysicsScene::Sphere2Box(PhysicsObject* A, PhysicsObject *B) {

    CollisionInfo info;
    Box* BoxB = static_cast<Box*>(B);
    Circle* CircleA = static_cast<Circle*>(A);

    // If AABB
    if (BoxB->GetOrientation() == 0) {

        // Get position on box that is closet to circle.
        Vec2 closest = {Clamp<float>(CircleA->GetPosition().x, BoxB->GetPosition().x - BoxB->GetHalfWidth(), BoxB->GetPosition().x + BoxB->GetHalfWidth()), 
        Clamp<float>(CircleA->GetPosition().y, BoxB->GetPosition().y - BoxB->GetHalfHeight(), BoxB->GetPosition().y + BoxB->GetHalfHeight())};

        float distance = (closest - CircleA->GetPosition()).GetMagnitude();
        
        if (distance <= CircleA->GetRadius()) {
            info.isColliding = true;
            info.penetrationDepth = CircleA->GetRadius() - distance;
            info.collisionNormal = (CircleA->GetPosition() - closest).Normalise();
        }
        return info;
    }
    return info; 
}

CollisionInfo PhysicsScene::Box2Box(PhysicsObject *A, PhysicsObject *B) {

    CollisionInfo info;
    Box* BoxA = static_cast<Box*>(A);
    Box* BoxB = static_cast<Box*>(B);

   // Update local axes
    BoxA->UpdateLocalAxes();
    BoxB->UpdateLocalAxes();

   // SAT
    Vec2 axes[4] = {BoxA->GetLocalXAxis(), BoxA->GetLocalYAxis(), BoxB->GetLocalXAxis(), BoxB->GetLocalYAxis()};
    Vec2 bestAxis;
    float minOverlap = FLT_MAX; 

    for (Vec2& axis : axes) {

        float dist = Dot(BoxA->GetPosition() - BoxB->GetPosition(), axis);
        float rA = abs(Dot(BoxA->GetLocalXAxis() * BoxA->GetHalfWidth(), axis)) + abs(Dot(BoxA->GetLocalYAxis() * BoxA->GetHalfHeight(), axis));
        float rB = abs(Dot(BoxB->GetLocalXAxis() * BoxB->GetHalfWidth(), axis)) + abs(Dot(BoxB->GetLocalYAxis() * BoxB->GetHalfHeight(), axis));

        float overlap = (rA + rB) - abs(dist);

        // Separating axis found
        if (overlap <= 0) return info;

        if (overlap < minOverlap) {
            minOverlap = overlap;
            bestAxis = (dist > 0) ? axis  : -1.0f * axis;
        }
    }
    info.isColliding = true;
    info.penetrationDepth = minOverlap;
    info.collisionNormal = bestAxis;
    return info;
}


//NOTE: Only handles linear cases right now
void PhysicsScene::ResolveCollisions(PhysicsObject* A, PhysicsObject* B, const CollisionInfo& info) {
    
    Vec2 relativeVelocity = A->GetVelocity() - B->GetVelocity();
    if (Dot(relativeVelocity, info.collisionNormal) < 0) {
        float impulseMagnitude = -1.8 * (Dot(relativeVelocity, info.collisionNormal)) / (A->GetInverseMass() + B->GetInverseMass());
        Vec2 newVelocityA = A->GetVelocity() + A->GetInverseMass() * (impulseMagnitude * info.collisionNormal);
        Vec2 newVelocityB = B->GetVelocity() - B->GetInverseMass() *  (impulseMagnitude * info.collisionNormal);
        A->SetVelocity(newVelocityA);
        B->SetVelocity(newVelocityB);
    }

    float totalInverseMass = A->GetInverseMass() + B->GetInverseMass(); 
    if (totalInverseMass > 0.0f) { Vec2 correction = (info.penetrationDepth / totalInverseMass) * info.collisionNormal;
        A->SetPosition( A->GetPosition() + A->GetInverseMass() * correction );
        B->SetPosition( B->GetPosition() - B->GetInverseMass() * correction );
    }

}

