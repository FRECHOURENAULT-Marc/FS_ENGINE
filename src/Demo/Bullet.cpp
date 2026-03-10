#include "pch.h"

void Bullet::Create(XMFLOAT3 rot, XMFLOAT3 _pos)
{
	ECS& ecs = ECS::Get();

	entityID = ecs.CreateEntity();

	TransformComponent* TC = ecs.AddComponent<TransformComponent>(entityID);
	TC->SetForward(rot);
	//TC->SetDirection(_direct);
	TC->SetLocalPosition(_pos);
	TC->SetScale(0.5f, 0.5f, 0.5f);

	ecs.AddComponent<VelocityComponent>(entityID)->SetVelocity(50);
	MeshComponent* MC = ecs.AddComponent<MeshComponent>(entityID);
	MC->SetGeometry(GeometryManager::Get()->GetGeometry(GEO_TYPE::SPHERE));
	MC->SetColor({ 1.0f,0.f,0.f,0.f });

	EmitterSettings settings = {};
	settings.emitterMaxPart = 20;
	settings.emitterDensity = 10;
	settings.partLifetime = 0.5f;
	settings.partSize = 3.0f;
	settings.partStartColor = { 1, 0.5f, 0, 1.0f };
	settings.partGeotype = GEO_TYPE::BOX;
	ecs.AddComponent<EmitterComponent>(entityID)->SetSettings(settings);
		
	ColliderComponent* CC = ecs.AddComponent<ColliderComponent>(entityID);
	CC->SetType<FS_SphereCollider>()->SetRadius(1);
}

void Bullet::Update(GameManager* GM)
{
	ECS& ecs = ECS::Get();

	duringTime += ecs.GetDeltaTime();
	if (duringTime > lifeTime)
	{ 
		IsDead = true;
		ecs.RemoveEntity(entityID);
		return;
	}

	auto& colliders = ecs.GetAllFromType<ColliderComponent>();

	for (auto collider : colliders)
	{
		if (collider.IsActive() == false)
			continue;

		if (collider.GetID() == entityID)
			continue;

		if (ecs.GetComponent<ColliderComponent>(entityID)->IsCollidingWith(collider.GetID()))
		{
			GM->m_score += 1000;
			IsDead = true;
			ecs.RemoveEntity(entityID);
			return;
		}
	}
}