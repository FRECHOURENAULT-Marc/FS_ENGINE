#include "pch.h"

namespace FSC
{
	/// COLLIDER
	bool Collider::IsCollidingSphereSphere(Collider* sphere1, Collider* sphere2)
	{
		ColliderSphere* s1 = (ColliderSphere*)sphere1;
		ColliderSphere* s2 = (ColliderSphere*)sphere2;

		XMFLOAT3 scale1 = s1->GetScale();
		XMFLOAT3 scale2 = s2->GetScale();

		float maximumScale1 = max(max(scale1.x, scale1.y), scale1.z);
		float maximumScale2 = max(max(scale2.x, scale2.y), scale2.z);

		float r1 = s1->GetRadius() * maximumScale1;
		float r2 = s2->GetRadius() * maximumScale2;

		float r1r2Squared = pow(r1 + r2, 2);
		XMFLOAT3 p1 = s1->GetPosition();
		XMFLOAT3 p2 = s2->GetPosition();

		XMVECTOR v1 = XMLoadFloat3(&p1);
		XMVECTOR v2 = XMLoadFloat3(&p2);
		XMVECTOR vResult = v1 - v2;
		XMVECTOR vDist = XMVector3LengthSq(vResult);

		if (XMVector3Less(vDist, XMVectorReplicate(r1r2Squared)))
			return true;
		return false;
	}

	bool Collider::IsCollidingAABB(Collider* _box1, Collider* _box2)
	{
		ColliderAABB* box1 = (ColliderAABB*)_box1;
		ColliderAABB* box2 = (ColliderAABB*)_box2;

		XMFLOAT3 origin1 = box1->GetPosition();
		float width1 = box1->GetWidth();
		float height1 = box1->GetHeight();
		float depth1 = box1->GetDepth();
		XMFLOAT3 box1Corner = { origin1.x - width1 / 2.f, origin1.y - height1 / 2.f, origin1.z - depth1 / 2.f };

		XMFLOAT3 origin2 = box2->GetPosition();
		float width2 = box2->GetWidth();
		float height2 = box2->GetHeight();
		float depth2 = box2->GetDepth();
		XMFLOAT3 box2Corner = { origin2.x - width2 / 2.f, origin2.y - height2 / 2.f, origin2.z - depth2 / 2.f };

		if ((box2Corner.x >= box1Corner.x + width1)
			|| (box2Corner.x + width2 <= box1Corner.x)
			|| (box2Corner.y >= box1Corner.y + height1)
			|| (box2Corner.y + height2 <= box1Corner.y)
			|| (box2Corner.z >= box1Corner.z + depth1)
			|| (box2Corner.z + depth2 <= box1Corner.z))
			return false;
		else
			return true;
	}

	bool Collider::IsCollidingAABBSphere(Collider* _box, Collider* _sphere)
	{
		ColliderSphere* sphere = (ColliderSphere*)_sphere;
		ColliderAABB* box = (ColliderAABB*)_box;

		//Sphere
		XMFLOAT3 sphereScale = sphere->GetScale();
		float sphereMaximumScale = max(max(sphereScale.x, sphereScale.y), sphereScale.z);
		float sphereRadius = sphere->GetRadius() * sphereMaximumScale;
		XMFLOAT3 spherePosition = sphere->GetPosition();

		//Box
		XMFLOAT3 boxOrigin = box->GetPosition();
		float boxWidth = box->GetWidth();
		float boxHeight = box->GetHeight();
		float boxDepth = box->GetDepth();
		XMFLOAT3 boxBottomCorner = { boxOrigin.x - boxWidth / 2.f, boxOrigin.y - boxHeight / 2.f, boxOrigin.z - boxDepth / 2.f };
		XMFLOAT3 boxTopCorner = { boxOrigin.x + boxWidth / 2.f, boxOrigin.y + boxHeight / 2.f, boxOrigin.z + boxDepth / 2.f };

		XMFLOAT3 closestPoint = { 0.0f, 0.0f, 0.0f };

		//X
		if (spherePosition.x < boxBottomCorner.x)
		{
			closestPoint.x = boxBottomCorner.x;
		}
		else if (boxBottomCorner.x < spherePosition.x && spherePosition.x < boxTopCorner.x)
		{
			closestPoint.x = spherePosition.x;
		}
		else if (boxTopCorner.x < spherePosition.x)
		{
			closestPoint.x = boxTopCorner.x;
		}

		// Y
		if (spherePosition.y < boxBottomCorner.y)
		{
			closestPoint.y = boxBottomCorner.y;
		}
		else if (boxBottomCorner.y < spherePosition.y && spherePosition.y < boxTopCorner.y)
		{
			closestPoint.y = spherePosition.y;
		}
		else if (boxTopCorner.y < spherePosition.y)
		{
			closestPoint.y = boxTopCorner.y;
		}

		//Z
		if (spherePosition.z < boxBottomCorner.z)
		{
			closestPoint.z = boxBottomCorner.z;
		}
		else if (boxBottomCorner.z < spherePosition.z && spherePosition.z < boxTopCorner.z)
		{
			closestPoint.z = spherePosition.z;
		}
		else if (boxTopCorner.z < spherePosition.z)
		{
			closestPoint.z = boxTopCorner.z;
		}

		if ((pow((closestPoint.x - spherePosition.x), 2.f) + pow((closestPoint.y - spherePosition.y), 2.0f) + pow((closestPoint.z - spherePosition.z), 2.f)) < pow(sphereRadius, 2.0f))
			return true;

		return false;
	}

	/// COLLIDER SPHERE 
	ColliderSphere::ColliderSphere()
	{
		m_Type = COLLIDER_TYPE::SPHERE;
	}

	bool ColliderSphere::IsColliding(Collider* other)
	{
		switch (other->GetColliderType())
		{
		case COLLIDER_TYPE::SPHERE:

			return Collider::IsCollidingSphereSphere(this, other);

		case COLLIDER_TYPE::AABB:

			return Collider::IsCollidingAABBSphere(other, this); 

		default:

			return false;
		}
	}

	void ColliderSphere::OnUpdate(const XMFLOAT3& _scale)
	{
		SetScale(_scale);
	}

	/// COLLIDER AABB
	ColliderAABB::ColliderAABB()
	{
		m_Type = COLLIDER_TYPE::AABB;
	}

	bool ColliderAABB::IsColliding(Collider* other)
	{
		switch (other->GetColliderType())
		{
		case COLLIDER_TYPE::AABB:

			return Collider::IsCollidingAABB(this, other);

		case COLLIDER_TYPE::SPHERE:

			return Collider::IsCollidingAABBSphere(this, other);

		default:

			return false;
		}
	}

	void ColliderAABB::OnUpdate(const XMFLOAT3& _scale)
	{
		if (_scale.x == m_Scale.x && _scale.y == m_Scale.y && _scale.z == m_Scale.z)
			return;

		SetScale(_scale);
		width = baseLength * _scale.x;
		height = baseLength * _scale.y;
		depth = baseLength * _scale.z;
	}
}