#include "Serialiser.h"
#include "PhysicsObject.h"
#include "Plane.h"
#include "Circle.h"
#include "Box.h"

json Serialiser::Save(const std::vector<PhysicsObject*>& actors) {

    json output;

    for (const auto current : actors) {
        switch(current->m_ShapeID) {

            case ShapeType::PLANE:{
                Plane* plane = static_cast<Plane*>(current);
                output["Actors"]["Planes"].push_back({
                        {"normalx", plane->GetNormal().x},
                        {"normaly", plane->GetNormal().y},
                        {"origindistance", plane->GetDistance()}
                });
            }
            break;

            case ShapeType::CIRCLE:{
                Circle* circle = static_cast<Circle*>(current);
                output["Actors"]["Circle"].push_back({
                        {"positionx", circle->GetPosition().x},
                            {"positiony", circle->GetPosition().y},
                            {"velocityx", circle->GetVelocity().x},
                            {"velocityy", circle->GetVelocity().y},
                            {"mass", circle->GetMass()},
                            {"orientation", circle->GetOrientation()},
                            {"radius", circle->GetRadius()}
                        });
            }
            break;


            case ShapeType::BOX:{
                Box* box = static_cast<Box*>(current);
                output["Actors"]["Box"].push_back({
                            {"positionx", box->GetPosition().x},
                            {"positiony", box->GetPosition().y},
                            {"velocityx", box->GetVelocity().x},
                            {"velocityy", box->GetVelocity().y},
                            {"mass", box->GetMass()},
                            {"orientation", box->GetOrientation()},
                            {"halfwidth", box->GetHalfWidth()},
                            {"halfheight", box->GetHalfHeight()},
                    });
            }
            break;

            default:
                break;
        }
    }
    return output;
}
