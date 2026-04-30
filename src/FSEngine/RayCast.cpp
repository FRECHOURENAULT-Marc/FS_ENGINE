#include "pch.h"

bool RayCast::RayIntersectAABB(const Collider& _collider, Hit& _hit, const Ray& _ray)
{
    ColliderAABB* aabb = (ColliderAABB*)(&_collider);

    XMFLOAT3 pos = aabb->GetPosition();

    float hx = aabb->GetWidth() * 0.5f;
    float hy = aabb->GetHeight() * 0.5f;
    float hz = aabb->GetDepth() * 0.5f;

    XMFLOAT3 bmin = { pos.x - hx, pos.y - hy, pos.z - hz };
    XMFLOAT3 bmax = { pos.x + hx, pos.y + hy, pos.z + hz };

    float tMin = 0.0f;
    float tMax = FLT_MAX;

    int hitAxis = -1;

    for (int i = 0; i < 3; i++)
    {
        float origin = ((float*)&_ray.m_Origin)[i];
        float dir = ((float*)&_ray.m_Dir)[i];
        float min = ((float*)&bmin)[i];
        float max = ((float*)&bmax)[i];

        if (fabs(dir) < 1e-6f)
        {
            if (origin < min || origin > max)
                return false;
        }
        else
        {
            float ood = 1.0f / dir;
            float t1 = (min - origin) * ood;
            float t2 = (max - origin) * ood;

            if (t1 > t2) std::swap(t1, t2);

            if (t1 > tMin)
            {
                tMin = t1;
                hitAxis = i;
            }

            tMax = min(tMax, t2);

            if (tMin > tMax)
                return false;
        }
    }

    _hit.m_Dist = tMin;
    _hit.m_Point = 
    {
        _ray.m_Origin.x + _ray.m_Dir.x * tMin,
        _ray.m_Origin.y + _ray.m_Dir.y * tMin,
        _ray.m_Origin.z + _ray.m_Dir.z * tMin
    };

    return true;
}

bool RayCast::RayIntersectSphere(const Collider& _collider, Hit& _hit, const Ray& _ray)
{
    ColliderSphere* sphere = (ColliderSphere*)(&_collider);

    XMFLOAT3 center = sphere->GetPosition();
    float radius = sphere->GetRadius() * sphere->GetScale().x;

    XMFLOAT3 rayLength = { _ray.m_Origin.x - center.x, _ray.m_Origin.y - center.y, _ray.m_Origin.z - center.z };

    float a = _ray.m_Dir.x * _ray.m_Dir.x + _ray.m_Dir.y * _ray.m_Dir.y + _ray.m_Dir.z * _ray.m_Dir.z;
    float b = 2.0f * ( rayLength.x * _ray.m_Dir.x + rayLength.y * _ray.m_Dir.y + rayLength.z * _ray.m_Dir.z );
    float c = rayLength.x * rayLength.x + rayLength.y * rayLength.y + rayLength.z * rayLength.z - radius * radius;

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0.0f)
        return false;

    float sqrtD = sqrtf(discriminant);

    float t1 = (-b - sqrtD) / (2.0f * a);
    float t2 = (-b + sqrtD) / (2.0f * a);
    float t = t1;

    if (t < 0.0f)
        t = t2;

    if (t < 0.0f)
        return false;

    _hit.m_Dist = t;
    _hit.m_Point =
    {
        _ray.m_Origin.x + _ray.m_Dir.x * t,
        _ray.m_Origin.y + _ray.m_Dir.y * t,
        _ray.m_Origin.z + _ray.m_Dir.z * t
    };

    return true;
}

Hit RayCast::CastRay(const XMINT2& _mousePos, std::string _layerName)
{
	Window* window = Device::Get()->GetWindow();

	float x = (2.0f * _mousePos.x) / window->Width() - 1.0f;
	float y = 1.0f - (2.0f * _mousePos.y) / window->Height();

	XMVECTOR rayNds = XMVectorSet(x, y, 1.0f, 1.0f);
    XMFLOAT4X4 proj = Device::GetCamera()->GetProj();
	XMMATRIX invProjection = XMMatrixInverse(nullptr, XMLoadFloat4x4(&proj));
	XMVECTOR rayView = XMVector3TransformNormal(rayNds, invProjection);
	rayView = XMVectorSetZ(rayView, 1.0f);
	rayView = XMVector3Normalize(rayView);
    XMFLOAT4X4 view = Device::GetCamera()->GetView();
	XMMATRIX invView = XMMatrixInverse(nullptr, XMLoadFloat4x4(&view));
	XMVECTOR rayWorld = XMVector3TransformNormal(rayView, invView);
	rayWorld = XMVector3Normalize(rayWorld);

	Ray ray;
	ray.m_Origin = Device::GetCamera()->GetPosition();
	XMStoreFloat3(&ray.m_Dir, rayWorld);

    std::vector<ColliderComponent*>& layer = ECS::Get().GetLayer(_layerName);

	float closestDist = FLT_MAX;
	Hit bestHit;

	for( int i = 0; i < layer.size(); i++)
	{
		ColliderComponent* obj = layer[i];

        if (obj->IsActive() == false)
            continue;

        Collider& collider = *obj->GetCollider();

		Hit hit;
		COLLIDER_TYPE type = collider.GetColliderType();

        if (type == COLLIDER_TYPE::AABB)
        {
            if (RayCast::RayIntersectAABB(collider, hit, ray))
            {
                if (hit.m_Dist < closestDist)
                {
                    closestDist = hit.m_Dist;
                    hit.m_CollidingID = obj->GetID();
                    bestHit = hit;

                }
            }
        }
        else if (type == COLLIDER_TYPE::SPHERE)
        {
            if (RayCast::RayIntersectSphere(collider, hit, ray))
            {
                if (hit.m_Dist < closestDist)
                {
                    closestDist = hit.m_Dist;
                    hit.m_CollidingID = obj->GetID();
                    bestHit = hit;
                }
            }
		}
	}

    return bestHit;
}
