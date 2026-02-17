#pragma once
#include "Vec2.h"

class PhysicsObject;
struct CollisionInfo;

struct ContactConstraint {

    PhysicsObject* A;
    PhysicsObject* B;
    
    Vec2 collisionNormal;
    Vec2 collisionPoint;
    Vec2 rA, rB;
    
    float rACrossN;
    float rBCrossN;
    float rACrossT;
    float rBCrossT;
    float dt;
    float effectiveMass;
    float effectiveMassTangent;
    float accumulatedVelocityImpulse = 0.0f;
    float accumulatedFrictionImpulse = 0.0f;
    float elasticity;
    float bias;
    float penetrationdepth;

	void Setup(CollisionInfo& info, float delta);
    void SolveVelocity();
    void SolveFriction();
};
