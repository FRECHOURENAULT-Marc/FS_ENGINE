#pragma once

struct Hit
{
	int m_CollidingID = -1;
	float m_Dist;
	XMFLOAT3 m_Point;
};

struct Ray
{
	XMFLOAT3 m_Origin;
	XMFLOAT3 m_Dir;
};

class RayCast
{
	static bool RayIntersectAABB(const Collider& _collider, Hit& _hit, const Ray& _ray);
	static bool RayIntersectSphere(const Collider& _collider, Hit& _hit, const Ray& _ray);
public:
	
	static Hit CastRay(const XMINT2& _mousePos, std::string _layerName);
};

