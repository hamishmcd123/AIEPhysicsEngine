#define NOMINMAX

#include "ContactConstraint.h"  
#include "PhysicsObject.h"
#include "CollisionInfo.h"
#include "Maths.h"
#include <algorithm>

void ContactConstraint::Setup(CollisionInfo& info, float delta)
{
	A = info.A;
	B = info.B;

	collisionPoint = info.collisionPoint;
	collisionNormal = info.collisionNormal;

	rA = collisionPoint - A->GetPosition();
	rB = collisionPoint - B->GetPosition();

	accumulatedVelocityImpulse = 0.0f;

	penetrationdepth = info.penetrationDepth;

	rACrossN = PseudoCross(rA, collisionNormal);
	rBCrossN = PseudoCross(rB, collisionNormal);

	effectiveMass = 1.0f / 
					(A->GetInverseMass() + B->GetInverseMass()
					+ (rACrossN * rACrossN * A->GetInverseMoment()) 
					+ (rBCrossN * rBCrossN * B->GetInverseMoment()));

	Vec2 tangent = Vec2(-collisionNormal.y, collisionNormal.x);

	rACrossT = PseudoCross(rA, tangent);
	rBCrossT = PseudoCross(rB, tangent);

	effectiveMassTangent = 1.0f /
		(A->GetInverseMass() + B->GetInverseMass()
		+ (rACrossT * rACrossT * A->GetInverseMoment())
		+ (rBCrossT * rBCrossT * B->GetInverseMoment()));

	const float slop = 0.005f;
	const float baumgarte = 0.3f;
	bias = -baumgarte / delta * std::max(penetrationdepth - slop, 0.0f);
}



void ContactConstraint::SolveVelocity()
{

	Vec2 vA = A->GetVelocity() + PseudoCross(rA, A->GetAngularVelocity());
	Vec2 vB = B->GetVelocity() + PseudoCross(rB, B->GetAngularVelocity());
	
	Vec2 relativeVelocity = vA - vB;

	float normalVelocity = Dot(relativeVelocity, collisionNormal);

	float lambda = effectiveMass * (-(1 + elasticity) * normalVelocity - bias);

	float oldAccumulated = accumulatedVelocityImpulse;
	accumulatedVelocityImpulse = std::max(oldAccumulated + lambda, 0.0f);

	lambda = accumulatedVelocityImpulse - oldAccumulated;

	A->ApplyImpulse(lambda * collisionNormal, collisionPoint);
	B->ApplyImpulse(-lambda * collisionNormal, collisionPoint);

}

void ContactConstraint::SolveFriction()
{
	Vec2 tangent = Vec2(-collisionNormal.y, collisionNormal.x);

	Vec2 vA = A->GetVelocity() + PseudoCross(rA, A->GetAngularVelocity());
	Vec2 vB = B->GetVelocity() + PseudoCross(rB, B->GetAngularVelocity());

	Vec2 relativeVelocity = vA - vB;
	float tangentVelocity = Dot(relativeVelocity, tangent);

	float lambda = effectiveMassTangent * (-tangentVelocity);

	float maxFriction = 0.5f * accumulatedVelocityImpulse;
	float oldAccumulated = accumulatedFrictionImpulse;
	accumulatedFrictionImpulse = std::clamp(oldAccumulated + lambda, -maxFriction, maxFriction);
	lambda = accumulatedFrictionImpulse - oldAccumulated;

	A->ApplyImpulse(lambda * tangent, collisionPoint);
	B->ApplyImpulse(-lambda * tangent, collisionPoint);

}


//void PhysicsScene::ResolveCollisions(PhysicsObject* A, PhysicsObject* B, const CollisionInfo& info) { 
//	float e = 0.5f;
//	Vec2 rA = info.collisionPoint - A->GetPosition();
//	Vec2 rB = info.collisionPoint - B->GetPosition();
//
//	float rACrossN = PseudoCross(rA, info.collisionNormal);
//	float rBCrossN = PseudoCross(rB, info.collisionNormal);
//
//	Vec2 relativeVelocity = (A->GetVelocity() + PseudoCross(rA, A->GetAngularVelocity()))
//		- (B->GetVelocity() + PseudoCross(rB, B->GetAngularVelocity()));
//
//	if (Dot(relativeVelocity, info.collisionNormal) < 0) {
//		if (m_debugShowContactPoints) lines->DrawCircle(info.collisionPoint, 0.05f, Colour::RED);
//		float impulseMagnitude = -(1 + elasticity) * (Dot(relativeVelocity, info.collisionNormal)) /
//			(A->GetInverseMass() + B->GetInverseMass() + (rACrossN * rACrossN) * A->GetInverseMoment() + (rBCrossN * rBCrossN) * B->GetInverseMoment());
//		A->ApplyImpulse(impulseMagnitude * info.collisionNormal, info.collisionPoint);
//		B->ApplyImpulse(-1.0f * impulseMagnitude * info.collisionNormal, info.collisionPoint);
//	}
//
//	const float totalInverseMass = A->GetInverseMass() + B->GetInverseMass();
//	if (totalInverseMass > 0.0f) {
//		const Vec2 correction = (info.penetrationDepth / totalInverseMass) * info.collisionNormal;
//		A->SetPosition(A->GetPosition() + A->GetInverseMass() * correction);
//		B->SetPosition(B->GetPosition() - B->GetInverseMass() * correction);
//	}
//
//}

