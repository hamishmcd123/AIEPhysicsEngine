#include "PhysicsScene.h"
#include "Colour.h"
#include "Key.h"
#include "RigidBody.h"
#include "Vec2.h"
#include <SDL3/SDL_dialog.h>
#include <algorithm>
#include "PhysicsObject.h"
#include "Circle.h"
#include "Plane.h"
#include "Box.h"
#include "CollisionInfo.h"
#include <iostream>
#include <string>
#include <fstream>
#include "imgui.h"
#include "ImGuiStuff.hpp"
#include "Reflection.h"
#include "ContactConstraint.h"

PhysicsScene::PhysicsScene()
{
	//Use the constructor to set up the application info, because the harness
	//needs this information early so it can set the name of the window when
	//it creates it.
	appInfo.appName = "Example Program";
}

PhysicsScene::~PhysicsScene()
{
	for (const PhysicsObject* actor : m_actors) {
		delete actor;
	}
}

void PhysicsScene::Initialise()
{
	//You should set up your application here. This function gets called
	//after the window is set up and the rendering context is created, so if
	//you're doing anything weird with rendering, OpenGL functions will be
	//available at this point.
    
    // For relfection
    RigidBody::RegisterClass();
    Circle::RegisterClass();
    Box::RegisterClass();

	PhysicsObject::lines = lines;
	m_gravity = { 0, -9.81f };
	AddActor(new Plane({ 0.0f, 1.0f }, 0.0f));

	SetupImGUITheme();
}

void PhysicsScene::Update(float delta)
{
	//Everything that your program does every frame should go here.
	//This includes rendering done with the line renderer!

	DrawSceneGraph();
	DrawDebugOptions();
	DrawObjectCreator();

	if (m_isPhysicsSimulating) {
		m_contactConstraints.clear();

		for (int outer = 0; outer < m_actors.size(); outer++) {
			PhysicsObject* A = m_actors[outer];
			for (int inner = outer + 1; inner < m_actors.size(); inner++) {
				PhysicsObject* B = m_actors[inner];

				//NOTE: The index for the function pointer array is given by: (A->m_ShapeID * N) + B, where N is the number of shape types.
				const int index = static_cast<int>(A->m_ShapeID) * 3 + static_cast<int>(B->m_ShapeID);
				CollisionInfo info = CollisionFunctions[index](A, B);
				if (info.isColliding) {

					// Create contact constraints
					ContactConstraint constraint;
					constraint.Setup(info, delta);
					constraint.elasticity = elasticity;
					m_contactConstraints.push_back(constraint);
				}
			}
		}
		
		for (PhysicsObject* actor : m_actors) {
			actor->IntegrateForces(m_gravity, delta);
		}


		for (int i = 0; i < 10; i++) {
			for (auto& constraint : m_contactConstraints) {
				if (m_debugShowContactPoints) lines->DrawCircle(constraint.collisionPoint, 0.05f, Colour::RED);
				constraint.SolveVelocity();
				constraint.SolveFriction();
			}
		}
	
		for (PhysicsObject* actor : m_actors) {
			actor->IntegrateVelocity(delta);
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

    // NOTE: This could be simplified using smart pointers.
    
	auto it = std::remove_if(m_actors.begin(), m_actors.end(), [actor](PhysicsObject* a) {
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
        switch (creatorInfo.shapetype) {
        case ShapeType::BOX:
            AddActor(new Box(
                cursorPos,
                creatorInfo.velocity,
                creatorInfo.mass,
                creatorInfo.halfwidth,
                creatorInfo.halfheight,
                creatorInfo.orientation,
                creatorInfo.colour
            ));
            break;

        case ShapeType::CIRCLE:
            AddActor(new Circle(
                cursorPos,
                creatorInfo.velocity,
                creatorInfo.mass,
                creatorInfo.radius,
                creatorInfo.orientation,
                creatorInfo.colour
            ));
            break;
		case ShapeType::PLANE:
			AddActor(new Plane(creatorInfo.normal, creatorInfo.distance));
			break;
        default:
            break;

        }
}


void PhysicsScene::ClearAllActor()
{

    // NOTE: This could be much simpler if I were using smart pointers for the actors.
	auto it = std::remove_if(m_actors.begin(), m_actors.end(), [](PhysicsObject* a) {
		delete a;
		return true;
		});

	m_actors.erase(it, m_actors.end());
}


// NOTE: These collision functions return the collision normal from B to A. This is unconvential, and I only realised this when I had finished making the first half of these functions. 
// This shouldn't cause any ununsual behaviour (the collision resolution function is consisent with this normal direction), but it is something to be aware of. 

CollisionInfo PhysicsScene::Sphere2Plane(PhysicsObject* A, PhysicsObject* B) {

	// NOTE: For circle-plane collisions, the collision normal will be either -planeNormal or planeNormal. 

	CollisionInfo info;
	const Circle* CircleA = static_cast<Circle*>(A);
	const Plane* PlaneB = static_cast<Plane*>(B);

	if (abs(Dot(CircleA->GetPosition(), PlaneB->GetNormal()) - PlaneB->GetDistance()) <= CircleA->GetRadius()) {
		info.isColliding = true;

		const float distanceToPlane = Dot(CircleA->GetPosition(), PlaneB->GetNormal()) - PlaneB->GetDistance();
		info.collisionNormal = (distanceToPlane > 0) ? PlaneB->GetNormal() : -1.0f * PlaneB->GetNormal();
		info.penetrationDepth = CircleA->GetRadius() - abs(distanceToPlane);
		info.collisionPoint = CircleA->GetPosition() + CircleA->GetRadius() * info.collisionNormal;
		info.A = A;
		info.B = B;
	}
	return info;

}

CollisionInfo PhysicsScene::Plane2Sphere(PhysicsObject* A, PhysicsObject* B) {

	// NOTE: For circle-plane collisions, the collision normal will be either -planeNormal or planeNormal. 

	CollisionInfo info;
	const Plane* PlaneA = static_cast<Plane*>(A);
	const Circle* CircleB = static_cast<Circle*>(B);

	if (abs(Dot(CircleB->GetPosition(), PlaneA->GetNormal()) - PlaneA->GetDistance()) <= CircleB->GetRadius()) {
		info.isColliding = true;

		// Get normal direction
		const float distanceToPlane = Dot(CircleB->GetPosition(), PlaneA->GetNormal()) - PlaneA->GetDistance();
		info.collisionNormal = (distanceToPlane > 0) ? -1.0f * PlaneA->GetNormal() : PlaneA->GetNormal();
		info.penetrationDepth = CircleB->GetRadius() - abs(distanceToPlane);
		info.collisionPoint = CircleB->GetPosition() + CircleB->GetRadius() * info.collisionNormal;
		info.A = A;
		info.B = B;
	}

	return info;
}


CollisionInfo PhysicsScene::Sphere2Sphere(PhysicsObject* A, PhysicsObject* B) {

	CollisionInfo info;

	const Circle* CircleA = static_cast<Circle*>(A);
	const Circle* CircleB = static_cast<Circle*>(B);

	if ((CircleA->GetPosition() - CircleB->GetPosition()).GetMagnitudeSquared() < (CircleA->GetRadius() + CircleB->GetRadius()) * (CircleA->GetRadius() + CircleB->GetRadius())) {
		info.isColliding = true;
		info.collisionNormal = (CircleA->GetPosition() - CircleB->GetPosition()).Normalise();
		info.penetrationDepth = (CircleA->GetRadius() + CircleB->GetRadius()) - (CircleA->GetPosition() - CircleB->GetPosition()).GetMagnitude();
		info.collisionPoint = CircleB->GetPosition() + CircleB->GetRadius() * info.collisionNormal;
		info.A = A;
		info.B = B;


	}

	return info;
}

CollisionInfo PhysicsScene::Plane2Plane(PhysicsObject* A, PhysicsObject* B) {
	return CollisionInfo();
}

CollisionInfo PhysicsScene::Box2Plane(PhysicsObject* A, PhysicsObject* B) {

	CollisionInfo info;
	Box* BoxA = static_cast<Box*>(A);
	const Plane* PlaneB = static_cast<Plane*>(B);

	BoxA->UpdateLocalAxes();

	const float distance = Dot(BoxA->GetPosition(), PlaneB->GetNormal()) - PlaneB->GetDistance();
	const float r = BoxA->GetHalfWidth() * abs(Dot(BoxA->GetLocalXAxis(), PlaneB->GetNormal())) + BoxA->GetHalfHeight() * abs(Dot(BoxA->GetLocalYAxis(), PlaneB->GetNormal()));

	if (abs(distance) <= r) {
		info.isColliding = true;
		info.penetrationDepth = r - abs(distance);
		info.collisionNormal = distance > 0 ? PlaneB->GetNormal() : -1.0f * PlaneB->GetNormal();
 		info.A = A;
		info.B = B;

        // NOTE: Find the corner with the lowest signed distance to the plane and use it as the collision point. 
        // This is not the most sophisticated method, as with edge-to-plane collisions we should ideally get a collision manifold and average the points.
        // However, this is beyond the scope of this project.
       
		const Vec2 vertices[4] = { BoxA->GetPosition() + BoxA->GetLocalXAxis() * BoxA->GetHalfWidth() + BoxA->GetLocalYAxis() * BoxA->GetHalfHeight(),
								   BoxA->GetPosition() + BoxA->GetLocalXAxis() * BoxA->GetHalfWidth() - BoxA->GetLocalYAxis() * BoxA->GetHalfHeight(),
								   BoxA->GetPosition() - BoxA->GetLocalXAxis() * BoxA->GetHalfWidth() + BoxA->GetLocalYAxis() * BoxA->GetHalfHeight(),
								   BoxA->GetPosition() - BoxA->GetLocalXAxis() * BoxA->GetHalfWidth() - BoxA->GetLocalYAxis() * BoxA->GetHalfHeight()
		};

		float lowest = FLT_MAX;
		Vec2 bestVertex;

		for (int i = 0; i < 4; i++) {
			float currentDistance = Dot(vertices[i], PlaneB->GetNormal()) - PlaneB->GetDistance();
			if (currentDistance < lowest) {

				lowest = currentDistance;
				bestVertex = vertices[i];
			}
			else {
				continue;
			}

			info.collisionPoint = bestVertex;
		}
	};
	return info;

}

CollisionInfo PhysicsScene::Plane2Box(PhysicsObject* A, PhysicsObject* B) {
	CollisionInfo info;
	Box* BoxB = static_cast<Box*>(B);
	const Plane* PlaneA = static_cast<Plane*>(A);

	BoxB->UpdateLocalAxes();

	const float distance = Dot(BoxB->GetPosition(), PlaneA->GetNormal()) - PlaneA->GetDistance();
	const float r = BoxB->GetHalfWidth() * abs(Dot(BoxB->GetLocalXAxis(), PlaneA->GetNormal())) + BoxB->GetHalfHeight() * abs(Dot(BoxB->GetLocalYAxis(), PlaneA->GetNormal()));

	if (abs(distance) <= r) {
		info.isColliding = true;
		info.penetrationDepth = r - abs(distance);
        info.collisionNormal = distance > 0 ? -1.0f * PlaneA->GetNormal() : PlaneA->GetNormal();
 		info.A = A;
		info.B = B;

        // NOTE: Find the corner with the lowest signed distance to the plane and use it as the collision point. 
        // This is not the most sophisticated method, as with edge-to-plane collisions we should ideally get a collision manifold and average the points.
        // However, this is beyond the scope of this project.
	
		const Vec2 vertices[4] = { BoxB->GetPosition() + BoxB->GetLocalXAxis() * BoxB->GetHalfWidth() + BoxB->GetLocalYAxis() * BoxB->GetHalfHeight(),
								   BoxB->GetPosition() + BoxB->GetLocalXAxis() * BoxB->GetHalfWidth() - BoxB->GetLocalYAxis() * BoxB->GetHalfHeight(),
								   BoxB->GetPosition() - BoxB->GetLocalXAxis() * BoxB->GetHalfWidth() + BoxB->GetLocalYAxis() * BoxB->GetHalfHeight(),
								   BoxB->GetPosition() - BoxB->GetLocalXAxis() * BoxB->GetHalfWidth() - BoxB->GetLocalYAxis() * BoxB->GetHalfHeight()
		};

		float lowest = FLT_MAX;
		Vec2 bestVertex;

		for (int i = 0; i < 4; i++) {
			float currentDistance = Dot(vertices[i], PlaneA->GetNormal()) - PlaneA->GetDistance();
			if (currentDistance < lowest) {
				lowest = currentDistance;
				bestVertex = vertices[i];
			}
			else {
				continue;
			}

			info.collisionPoint = bestVertex;
		}
	}
	return info;
}

CollisionInfo PhysicsScene::Box2Sphere(PhysicsObject* A, PhysicsObject* B) {
	CollisionInfo info;
	Box* BoxA = static_cast<Box*>(A);
	const Circle* CircleB = static_cast<Circle*>(B);

	BoxA->UpdateLocalAxes();

	//NOTE: Here we transform the Circle so that it is in OBB's local axes with the OBB centered at the origin.
	const Vec2 RelativePos = CircleB->GetPosition() - BoxA->GetPosition();

	Vec2 CirclePos;

	CirclePos.x = Dot(RelativePos, BoxA->GetLocalXAxis());
	CirclePos.y = Dot(RelativePos, BoxA->GetLocalYAxis());

	// Get position on box that is closet to circle.
	const Vec2 closest = { Clamp<float>(CirclePos.x, -BoxA->GetHalfWidth(),BoxA->GetHalfWidth()),
	Clamp<float>(CirclePos.y,-BoxA->GetHalfHeight(),BoxA->GetHalfHeight()) };

	const float distance = (closest - CirclePos).GetMagnitude();

	if (distance <= CircleB->GetRadius()) {
		info.isColliding = true;
		info.penetrationDepth = CircleB->GetRadius() - distance;
		const Vec2 collisionNormalLocal = (closest - CirclePos).Normalise();
		info.collisionNormal = (BoxA->GetLocalXAxis() * collisionNormalLocal.x) + (BoxA->GetLocalYAxis() * collisionNormalLocal.y);
		info.collisionPoint = (CircleB->GetPosition() + CircleB->GetRadius() * info.collisionNormal);
 		info.A = A;
		info.B = B;
	}
	return info;
}

CollisionInfo PhysicsScene::Sphere2Box(PhysicsObject* A, PhysicsObject* B) {

	CollisionInfo info;
	Box* BoxB = static_cast<Box*>(B);
	const Circle* CircleA = static_cast<Circle*>(A);

	//NOTE: Here we transform the Circle so that it is in OBB's local axes with the OBB centered at the origin.
	BoxB->UpdateLocalAxes();
	const Vec2 RelativePos = CircleA->GetPosition() - BoxB->GetPosition();

	Vec2 CirclePos;

	CirclePos.x = Dot(RelativePos, BoxB->GetLocalXAxis());
	CirclePos.y = Dot(RelativePos, BoxB->GetLocalYAxis());

	// Get position on box that is closet to circle.
	const Vec2 closest = { Clamp<float>(CirclePos.x, -BoxB->GetHalfWidth(),BoxB->GetHalfWidth()),
	Clamp<float>(CirclePos.y,-BoxB->GetHalfHeight(),BoxB->GetHalfHeight()) };

	const float distance = (closest - CirclePos).GetMagnitude();

	if (distance <= CircleA->GetRadius()) {
		info.isColliding = true;
		info.penetrationDepth = CircleA->GetRadius() - distance;
		const Vec2 collisionNormalLocal = (CirclePos - closest).Normalise();
		info.collisionNormal = (BoxB->GetLocalXAxis() * collisionNormalLocal.x) + (BoxB->GetLocalYAxis() * collisionNormalLocal.y);
		info.collisionPoint = (CircleA->GetPosition() - CircleA->GetRadius() * info.collisionNormal);
		info.A = A;
		info.B = B;

	}
	return info;
}

CollisionInfo PhysicsScene::Box2Box(PhysicsObject* A, PhysicsObject* B) {

	CollisionInfo info;
	Box* BoxA = static_cast<Box*>(A);
	Box* BoxB = static_cast<Box*>(B);

	// Update local axes
	BoxA->UpdateLocalAxes();
	BoxB->UpdateLocalAxes();

	// SAT
	Vec2 axes[4] = { BoxA->GetLocalXAxis(), BoxA->GetLocalYAxis(), BoxB->GetLocalXAxis(), BoxB->GetLocalYAxis() };
	Vec2 bestAxis;
	float minOverlap = FLT_MAX;
	int bestIndex;

	for (int index = 0; index < 4; index++) {

		Vec2 axis = axes[index];
		const float dist = Dot(BoxA->GetPosition() - BoxB->GetPosition(), axis);
		const float rA = abs(Dot(BoxA->GetLocalXAxis() * BoxA->GetHalfWidth(), axis)) + abs(Dot(BoxA->GetLocalYAxis() * BoxA->GetHalfHeight(), axis));
		const float rB = abs(Dot(BoxB->GetLocalXAxis() * BoxB->GetHalfWidth(), axis)) + abs(Dot(BoxB->GetLocalYAxis() * BoxB->GetHalfHeight(), axis));

		const float overlap = (rA + rB) - abs(dist);

		// Separating axis found
		if (overlap <= 0) return info;

		if (overlap < minOverlap) {
			minOverlap = overlap;
			bestAxis = (dist > 0) ? axis : -1.0f * axis;
			bestIndex = index;
		}
	}

	// NOTE: The idea here is to project all the vertices of the incident shape onto the separating axis. Then, take the one with the lowest value to
	// be the collision point.
    // Like with the Box2Plane collision, this is not the most sophisticated approach, as does not handle edge-to-edge collision nicely. Ideally we would get a collision manifold
    // and average the points.

	if (bestIndex == 1 || bestIndex == 2) {
		// BoxA owns the best axis, so project BoxB vertices onto that.
		float lowestProj = FLT_MAX;
		Vec2 bestVertex;

		Vec2 vertices[4] = {
			BoxB->GetPosition() + BoxB->GetLocalXAxis() * BoxB->GetHalfWidth() + BoxB->GetLocalYAxis() * BoxB->GetHalfHeight(),
			BoxB->GetPosition() + BoxB->GetLocalXAxis() * BoxB->GetHalfWidth() - BoxB->GetLocalYAxis() * BoxB->GetHalfHeight(),
			BoxB->GetPosition() - BoxB->GetLocalXAxis() * BoxB->GetHalfWidth() + BoxB->GetLocalYAxis() * BoxB->GetHalfHeight(),
			BoxB->GetPosition() - BoxB->GetLocalXAxis() * BoxB->GetHalfWidth() - BoxB->GetLocalYAxis() * BoxB->GetHalfHeight(),
		};

		for (int i = 0; i < 4; i++) {
			float thisProj = Dot(vertices[i], -1.0f * bestAxis);
			if (thisProj < lowestProj) {
				lowestProj = thisProj;
				bestVertex = vertices[i];
			}
		}
		info.collisionPoint = bestVertex;
	}

	else {
		float lowestProj = FLT_MAX;
		Vec2 bestVertex;
		// BoxB owns the best axis, so project BoxA vertices onto that.
		Vec2 vertices[4] = {
				BoxA->GetPosition() + BoxA->GetLocalXAxis() * BoxA->GetHalfWidth() + BoxA->GetLocalYAxis() * BoxA->GetHalfHeight(),
				BoxA->GetPosition() + BoxA->GetLocalXAxis() * BoxA->GetHalfWidth() - BoxA->GetLocalYAxis() * BoxA->GetHalfHeight(),
				BoxA->GetPosition() - BoxA->GetLocalXAxis() * BoxA->GetHalfWidth() + BoxA->GetLocalYAxis() * BoxA->GetHalfHeight(),
				BoxA->GetPosition() - BoxA->GetLocalXAxis() * BoxA->GetHalfWidth() - BoxA->GetLocalYAxis() * BoxA->GetHalfHeight(),
		};

		for (int i = 0; i < 4; i++) {
			float thisProj = Dot(vertices[i], bestAxis);
			if (thisProj < lowestProj) {
				lowestProj = thisProj;
				bestVertex = vertices[i];
			}
		}
		info.collisionPoint = bestVertex;
	}
	info.isColliding = true;
	info.penetrationDepth = minOverlap;
	info.collisionNormal = bestAxis;
	info.A = A;
	info.B = B;

	return info;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void PhysicsScene::DisplayActor(PhysicsObject* Actor) {
	static RigidBody* SelectedActor = nullptr;
	static Colour SelectedActorPrevColour;
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

	if (RigidBody* CastedActor = dynamic_cast<RigidBody*>(Actor)) {

		if (SelectedActor == CastedActor) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		ImGui::PushID(Actor);
		bool isOpen = ImGui::TreeNodeEx("PhysicsActor", flags);

		if (ImGui::IsItemClicked()) {

			// If there is already a selected actor, change its colour to the stored colour to restore it.
			if (SelectedActor) {
				SelectedActor->SetColour(SelectedActorPrevColour);
			}

			// If the selected actor is this actor (and we click on it again), unselect it.
			if (SelectedActor == CastedActor) {
				SelectedActor = nullptr;
			}

			// If the selected actor is not the selected actor, change the selected actor to this one and change its colour to green.
			else {
				SelectedActor = CastedActor;
				SelectedActorPrevColour = CastedActor->GetColour();
				SelectedActor->SetColour(Colour::GREEN);
			}

		}

		if (isOpen) {
			if (ImGui::BeginTable("Properties", 2, ImGuiTableFlags_SizingStretchProp)) {
                switch(CastedActor->m_ShapeID){
                    case(ShapeType::CIRCLE):
                        for (auto& prop : GetType<RigidBody>().properties) {
                            prop->Draw(CastedActor);
                        }
                        for (auto& prop : GetType<Circle>().properties) {
                                prop->Draw(static_cast<Circle*>(CastedActor));
                        }
                        break;

                    case(ShapeType::BOX):
                        for (auto& prop : GetType<RigidBody>().properties) {
                            prop->Draw(CastedActor);
                        }
                        for (auto& prop : GetType<Box>().properties) {
                                prop->Draw(static_cast<Box*>(CastedActor));
                        }
                        break;

                    default:
                        break;
                }
			}

			// Refresh the inverse mass, moment and inverse moment
			CastedActor->RefreshInverseMass();
			CastedActor->RefreshMoment();

			ImGui::EndTable();
			if (ImGui::Button("Delete Actor##")) {
				// This ensures the selected actor is not a dangling pointer.
				if (SelectedActor == Actor) {
					SelectedActor = nullptr;
				}
				RemoveActor(Actor);
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
	}
}

void PhysicsScene::DrawSceneGraph()
{
	ImGui::Begin("Scene Graph");
	if (ImGui::TreeNode("Scene")) {
		for (const auto actor : m_actors) {
			DisplayActor(actor);
		}
		ImGui::TreePop();
	}
	ImGui::End();
}

void PhysicsScene::DrawDebugOptions()
{
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(5.0f, 5.0f));

	ImGui::Begin("Debug Options");
	if (ImGui::BeginTable("Options", 2, ImGuiTableFlags_SizingStretchProp)) {

		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		ImGui::Checkbox("Visualise contact points", &m_debugShowContactPoints);

		ImGui::TableNextRow();

		ImGui::TableNextColumn();
		if (ImGui::Button("Simulate!")) {
			m_isPhysicsSimulating ^= 1;
		}


		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		if (ImGui::Button("Load Scene")) {
			// this should be okay??
			OpenLoadFileDialogue((void*)this);
		}

		ImGui::TableNextRow();
		ImGui::TableNextColumn();


		if (ImGui::Button("Save Scene")) {
			json savedata = serialiser.Save(m_actors);
			OpenSaveFileDialogue(savedata);
		}

		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		if (ImGui::Button("Clear Scene")) {
			ClearAllActor();
			AddActor(new Plane({ 0.0f, 1.0f }, 0.0f));
		}

		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::InputFloat2("Gravity", &m_gravity.x, "%.2f");

		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::InputFloat("Elasticity", &elasticity, 0.0f, 0.0f, " % .2f");

		ImGui::EndTable();
	}
	ImGui::PopStyleVar();
	ImGui::End();
}

void SDLCALL PhysicsScene::OnLoadFileSelected(void* userdata, const char* const* filelist, int filter) {

	if (!filelist || !filelist[0]) {
		std::cout << "No file selected...";
		return;
	}

	const char* path = *filelist;

	std::cout << "selected file: " << path << '\n';

	std::fstream file(path, std::ios::in | std::ios::binary);

	if (!file.is_open()) {
		std::cerr << "Failed to open file\n";
		return;
	}

	const std::string contents(
		(std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>()
	);

	//NOTE: Have to do this weird shit because this callback is asynchronous. 
	char* data = new char[contents.length() + 1];
	memcpy(data, contents.data(), contents.length());
	data[contents.length()] = '\0';

	PhysicsScene* ref = (PhysicsScene*)(userdata);
	ref->serialiser.Load(ref, data);
}

void SDLCALL PhysicsScene::SaveFile(void* userdata, const char* const* filelist, int filter) {

	json* data = static_cast<json*>(userdata);

	if (!filelist || !filelist[0]) {
		std::cout << "Cancelled save...";
	}
	else {
		const char* path = *filelist;
		std::ofstream file(path);
		const std::string stringified = data->dump(3);
		file.write(stringified.c_str(), stringified.size());
	}

	delete data;
}



void PhysicsScene::OpenLoadFileDialogue(void* reference) {
	SDL_DialogFileFilter filters[] = {
		{ "JSON", "json" }
	};

	SDL_ShowOpenFileDialog(
		OnLoadFileSelected,
		reference,
		nullptr,
		filters,
		SDL_arraysize(filters),
		nullptr,
		false
	);

}

void PhysicsScene::OpenSaveFileDialogue(json& data) {

	SDL_DialogFileFilter filters[] = {
			{ "JSON", "json" }
	};

	json* dataptr = new json(data);

	SDL_ShowSaveFileDialog(
		SaveFile,
		(void*)dataptr,
		nullptr,
		filters,
		SDL_arraysize(filters),
		nullptr);
}

void PhysicsScene::OnKeyPress(Key key) {
    if(!ImGui::GetIO().WantCaptureKeyboard)
    switch(key) {
        case(Key::One):
            creatorInfo.shapetype = ShapeType::BOX;
            break;
        case(Key::Two):
            creatorInfo.shapetype = ShapeType::CIRCLE;
            break;
		case (Key::Three):
			creatorInfo.shapetype = ShapeType::PLANE;
			break;
        default:
            break;
    }

}


void PhysicsScene::DrawObjectCreator()
{
	ImGui::Begin("Object Creator");
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 10.0f));
	if (ImGui::BeginTable("Properties", 2, ImGuiTableFlags_SizingStretchProp)) {

		ImGui::TableNextRow();
		ImGui::TableNextColumn(); ImGui::Text("Current Shape:");
		switch (creatorInfo.shapetype)
		{
		case ShapeType::BOX:
			ImGui::TableNextColumn(); ImGui::Text("BOX");
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); ImGui::Text("Velocity");
			ImGui::TableNextColumn(); ImGui::InputFloat2("##1", &creatorInfo.velocity.x);

			ImGui::TableNextColumn(); ImGui::Text("Mass");
			ImGui::TableNextColumn(); ImGui::InputFloat("##2", &creatorInfo.mass, 0.0f, 0.0f, "%.3f");
			if (creatorInfo.mass <= 0.0f) creatorInfo.mass = 0.001f; // If the mass is set to 0, the inverse mass with be infinite and the velocity/acceleration witll explode.

			ImGui::TableNextColumn(); ImGui::Text("Orientation");
			ImGui::TableNextColumn(); ImGui::InputFloat("##4", &creatorInfo.orientation);

			ImGui::TableNextColumn(); ImGui::Text("Half Width");
			ImGui::TableNextColumn(); ImGui::InputFloat("##5", &creatorInfo.halfwidth);

			ImGui::TableNextColumn(); ImGui::Text("Half Height");
			ImGui::TableNextColumn(); ImGui::InputFloat("##6", &creatorInfo.halfheight);
			break;

		case ShapeType::CIRCLE:
			ImGui::TableNextColumn(); ImGui::Text("CIRCLE");
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); ImGui::Text("Velocity");
			ImGui::TableNextColumn(); ImGui::InputFloat2("##1", &creatorInfo.velocity.x);

			ImGui::TableNextColumn(); ImGui::Text("Mass");
			ImGui::TableNextColumn(); ImGui::InputFloat("##2", &creatorInfo.mass, 0.0f, 0.0f, "%.3f");
			if (creatorInfo.mass <= 0.0f) creatorInfo.mass = 0.001f; // If the mass is set to 0, the inverse mass with be infinite and the velocity/acceleration witll explode.

			ImGui::TableNextColumn(); ImGui::Text("Radius");
			ImGui::TableNextColumn(); ImGui::InputFloat("##4", &creatorInfo.radius);

			ImGui::TableNextColumn(); ImGui::Text("Orientation");
			ImGui::TableNextColumn(); ImGui::InputFloat("##5", &creatorInfo.orientation);
			break;

		case ShapeType::PLANE:
			ImGui::TableNextColumn(); ImGui::Text("PLANE");
			ImGui::TableNextRow();

			ImGui::TableNextColumn(); ImGui::Text("Normal");
			ImGui::TableNextColumn(); if (ImGui::InputFloat2("##2", &creatorInfo.normal.x)) {
				creatorInfo.normal.Normalise();
			};

			ImGui::TableNextColumn(); ImGui::Text("Distance");
			ImGui::TableNextColumn(); ImGui::InputFloat("##3", &creatorInfo.distance);


			break;

		}
		ImGui::EndTable();
	}
	ImGui::PopStyleVar();
	ImGui::End();
}

