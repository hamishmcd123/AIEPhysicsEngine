#pragma once 
#include "Vec2.h"
#include <vector>

class PhysicsObject;

struct CollisionInfo {

    PhysicsObject* A;
    PhysicsObject* B;

    bool isColliding = false;
    float penetrationDepth;
    float elasticity;

    Vec2 collisionNormal;
    Vec2 collisionPoint;
};
