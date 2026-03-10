#include "pch.h"

bool FS_Collider::IsCollidingSphereSphere(FS_Collider* sphere1, FS_Collider* sphere2)
{
	FS_SphereCollider* s1 = (FS_SphereCollider*)sphere1;
	FS_SphereCollider* s2 = (FS_SphereCollider*)sphere2;
	float r1 = s1->GetRadius() * s1->GetScale();
	float r2 = s2->GetRadius() * s2->GetScale();
	float r1r2Squared = pow(r1 + r2, 2);
	XMFLOAT3 p1 = s1->GetPosition();
	XMFLOAT3 p2 = s2->GetPosition();

	XMVECTOR v1 = XMLoadFloat3(&p1);
	XMVECTOR v2 = XMLoadFloat3(&p2);
	XMVECTOR vResult = v1 - v2;
	XMVECTOR vDist = XMVector3LengthSq(vResult);

	if(XMVector3Less(vDist, XMVectorReplicate(r1r2Squared)))
		return true;
	return false;
}

FS_SphereCollider::FS_SphereCollider()
{
	mType = COLLIDER_TYPE::SPHERE;
}

bool FS_SphereCollider::IsColliding(FS_Collider* other)
{
	switch (other->GetColliderType())
	{
	case COLLIDER_TYPE::SPHERE:
		return FS_Collider::IsCollidingSphereSphere(this, other);
	default:
		return false;
	}
}
	
