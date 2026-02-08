#pragma once 
#include "json.hpp"
#include <vector>
#include "PhysicsObject.h"

using json = nlohmann::json;


class Serialiser {
public:

// NOTE: Might be worthwhile later to have a struct which contains other save data.
// Right now, we only need to save the actors, so we will just pass in the m_actors vector.
//
json Save(const std::vector<PhysicsObject*>& actors);

};








