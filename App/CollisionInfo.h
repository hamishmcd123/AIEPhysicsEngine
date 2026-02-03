#pragma once 
#include "Vec2.h"


struct CollisionInfo {
    bool isColliding = false;
    float penetrationDepth;
    Vec2 collisionNormal;
};
