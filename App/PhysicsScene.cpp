#include "PhysicsScene.h"
#include "Colour.h"
#include "RigidBody.h"
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

    AddActor(new Plane({0.0f, 1.0f}, 0.0f));
    AddActor(new Plane({1.0f, 0.0f}, 5.0f));
    AddActor(new Plane({1.0f, 0.0f}, -5.0f));
    SetUpImGUItheme();
}

void PhysicsScene::Update(float delta)
{

	ImGui::Begin("Window");
    if (ImGui::TreeNode("Scene")) {
            for (auto actor : m_actors) {
                DisplayActor(actor);
            }
            ImGui::TreePop();
    }
    ImGui::ShowDemoWindow();
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

    for (auto actor : m_actors) {
                if (actor->m_ShapeID == ShapeType::BOX) {
                    dynamic_cast<Box*>(actor)->ApplyForceAtPoint({1.0f, -1.0f}, {0.2f, 0.0f});
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
    AddActor(new Circle(cursorPos, {0.0f, 0.0f}, 10.0f, 0.25f, Colour::BLUE));

}


void PhysicsScene::OnRightClick() {
    AddActor(new Box(cursorPos, {0.0f, 0.0f}, 20.0f, 0.25f, 0.25f, Colour::RED));
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
            info.collisionPoint = CircleA->GetPosition() + CircleA->GetRadius() * info.collisionNormal;
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
                info.collisionPoint = CircleB->GetPosition() + CircleB->GetRadius() * info.collisionNormal;
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
        info.collisionPoint = CircleB->GetPosition() + CircleB->GetRadius() * info.collisionNormal;
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

            // Getting collision point -> find signed distance of vertices to plane. Then, set the collision point to be the one with the lowest signed distance.
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
    
    BoxA->UpdateLocalAxes();

    //NOTE: Transform the circle's position so that its in the OBB's local axes, where the OBB is centered at 0,0
    Vec2 RelativePos = CircleB->GetPosition() - BoxA->GetPosition();

    Vec2 CirclePos;

    CirclePos.x = Dot(RelativePos, BoxA->GetLocalXAxis());
    CirclePos.y = Dot(RelativePos, BoxA->GetLocalYAxis());

    // Get position on box that is closet to circle.
    Vec2 closest = {Clamp<float>(CirclePos.x, -BoxA->GetHalfWidth(),BoxA->GetHalfWidth()), 
    Clamp<float>(CirclePos.y,-BoxA->GetHalfHeight(),BoxA->GetHalfHeight())};

    float distance = (closest - CirclePos).GetMagnitude();
    
    if (distance <= CircleB->GetRadius()) {
        info.isColliding = true;
        info.penetrationDepth = CircleB->GetRadius() - distance;
        Vec2 collisionNormalLocal = (closest - CirclePos).Normalise();
        info.collisionNormal = (BoxA->GetLocalXAxis() * collisionNormalLocal.x) + (BoxA->GetLocalYAxis() * collisionNormalLocal.y);
        info.collisionPoint = (CircleB->GetPosition() + CircleB->GetRadius() * info.collisionNormal);
    }
    return info;
}

CollisionInfo PhysicsScene::Sphere2Box(PhysicsObject* A, PhysicsObject *B) {

    CollisionInfo info;
    Box* BoxB = static_cast<Box*>(B);
    Circle* CircleA = static_cast<Circle*>(A);

    BoxB->UpdateLocalAxes();
    Vec2 RelativePos = CircleA->GetPosition() - BoxB->GetPosition();

    Vec2 CirclePos;

    CirclePos.x = Dot(RelativePos, BoxB->GetLocalXAxis());
    CirclePos.y = Dot(RelativePos, BoxB->GetLocalYAxis());

    // Get position on box that is closet to circle.
    Vec2 closest = {Clamp<float>(CirclePos.x, -BoxB->GetHalfWidth(),BoxB->GetHalfWidth()), 
    Clamp<float>(CirclePos.y,-BoxB->GetHalfHeight(),BoxB->GetHalfHeight())};

    float distance = (closest - CirclePos).GetMagnitude();
    
    if (distance <= CircleA->GetRadius()) {
        info.isColliding = true;
        info.penetrationDepth = CircleA->GetRadius() - distance;
        Vec2 collisionNormalLocal = (CirclePos - closest).Normalise();
        info.collisionNormal = (BoxB->GetLocalXAxis() * collisionNormalLocal.x) + (BoxB->GetLocalYAxis() * collisionNormalLocal.y);
        info.collisionPoint = (CircleA->GetPosition() - CircleA->GetRadius() * info.collisionNormal);
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
        lines->DrawCircle(info.collisionPoint, 0.05f, Colour::RED);
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
            // In case the last actor was deleted.
            if (SelectedActor) {
                SelectedActor->SetColour(SelectedActorPrevColour);
            }
            SelectedActor = CastedActor;
            SelectedActorPrevColour = CastedActor->GetColour();
            SelectedActor->SetColour(Colour::GREEN);
        }

        if (isOpen) {
            if(ImGui::BeginTable("Properties", 2,ImGuiTableFlags_SizingStretchProp)) {

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn(); ImGui::Text("Shape Type");

                    ImGui::TableNextColumn();
                    switch(CastedActor->m_ShapeID) {
                        case ShapeType::BOX:
                            ImGui::Text("Box");
                            break;
                        case ShapeType::CIRCLE:
                            ImGui::Text("Circle");
                            break;
                        default:
                            ImGui::Text("Unknown");
                    }
                    
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::Text("Position");  
                        ImGui::TableNextColumn(); ImGui::Text("(%.2f, %.2f)", CastedActor->GetPosition().x, CastedActor->GetPosition().y);

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::Text("Orientation");
                        ImGui::TableNextColumn(); ImGui::Text("%.2f", CastedActor->GetOrientation());
                        
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::Text("Mass");
                        ImGui::TableNextColumn(); ImGui::Text("%.2f", CastedActor->GetMass());

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::Text("Velocity");
                        ImGui::TableNextColumn(); ImGui::Text("(%.2f, %.2f)", CastedActor->GetVelocity().x, CastedActor->GetVelocity().y);
                    }

                    ImGui::EndTable();
                    if (ImGui::Button("Delete Actor##")) {
                        RemoveActor(Actor);
                    }
            ImGui::TreePop();
        }
            ImGui::PopID();
    }
}

void PhysicsScene::SetUpImGUItheme() {
	ImGuiStyle& style = ImGui::GetStyle();
	
	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.1f;
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.WindowRounding = 10.0f;
	style.WindowBorderSize = 0.0f;
	style.WindowMinSize = ImVec2(30.0f, 30.0f);
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Right;
	style.ChildRounding = 5.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 10.0f;
	style.PopupBorderSize = 0.0f;
	style.FramePadding = ImVec2(5.0f, 3.5f);
	style.FrameRounding = 5.0f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(5.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(5.0f, 5.0f);
	style.CellPadding = ImVec2(4.0f, 2.0f);
	style.IndentSpacing = 5.0f;
	style.ColumnsMinSpacing = 5.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize = 15.0f;
	style.GrabRounding = 5.0f;
	style.TabRounding = 5.0f;
	style.TabBorderSize = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
	
	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(1.0f, 1.0f, 1.0f, 0.360515f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09803922f, 0.09803922f, 0.09803922f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.09803922f, 0.09803922f, 0.09803922f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.42352942f, 0.38039216f, 0.57254905f, 0.5493562f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15686275f, 0.15686275f, 0.15686275f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38039216f, 0.42352942f, 0.57254905f, 0.54901963f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.61960787f, 0.5764706f, 0.76862746f, 0.54901963f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.09803922f, 0.09803922f, 0.09803922f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.09803922f, 0.09803922f, 0.09803922f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.25882354f, 0.25882354f, 0.25882354f, 0.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.15686275f, 0.15686275f, 0.15686275f, 0.0f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.15686275f, 0.15686275f, 0.15686275f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.23529412f, 0.23529412f, 0.23529412f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.29411766f, 0.29411766f, 0.29411766f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.29411766f, 0.29411766f, 0.29411766f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.61960787f, 0.5764706f, 0.76862746f, 0.54901963f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.8156863f, 0.77254903f, 0.9647059f, 0.54901963f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.61960787f, 0.5764706f, 0.76862746f, 0.54901963f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.7372549f, 0.69411767f, 0.8862745f, 0.54901963f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.8156863f, 0.77254903f, 0.9647059f, 0.54901963f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.61960787f, 0.5764706f, 0.76862746f, 0.54901963f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.7372549f, 0.69411767f, 0.8862745f, 0.54901963f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.8156863f, 0.77254903f, 0.9647059f, 0.54901963f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.61960787f, 0.5764706f, 0.76862746f, 0.54901963f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.7372549f, 0.69411767f, 0.8862745f, 0.54901963f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.8156863f, 0.77254903f, 0.9647059f, 0.54901963f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.61960787f, 0.5764706f, 0.76862746f, 0.54901963f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.7372549f, 0.69411767f, 0.8862745f, 0.54901963f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.8156863f, 0.77254903f, 0.9647059f, 0.54901963f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.61960787f, 0.5764706f, 0.76862746f, 0.54901963f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.7372549f, 0.69411767f, 0.8862745f, 0.54901963f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.8156863f, 0.77254903f, 0.9647059f, 0.54901963f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0f, 0.4509804f, 1.0f, 0.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13333334f, 0.25882354f, 0.42352942f, 0.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.29411766f, 0.29411766f, 0.29411766f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.7372549f, 0.69411767f, 0.8862745f, 0.54901963f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.61960787f, 0.5764706f, 0.76862746f, 0.54901963f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.7372549f, 0.69411767f, 0.8862745f, 0.54901963f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882353f, 0.1882353f, 0.2f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.42352942f, 0.38039216f, 0.57254905f, 0.54901963f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.42352942f, 0.38039216f, 0.57254905f, 0.2918455f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.03433478f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.7372549f, 0.69411767f, 0.8862745f, 0.54901963f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.9f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
}
