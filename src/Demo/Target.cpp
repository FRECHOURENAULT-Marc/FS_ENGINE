#include "pch.h"

void Target::Create(XMFLOAT3 pos)
{
	ECS& ecs = ECS::Get();

	entityID = ecs.CreateEntity();

	TransformComponent* transf = ecs.AddComponent<TransformComponent>(entityID);
	transf->SetLocalPosition(pos);

	MeshComponent* mesh = ecs.AddComponent<MeshComponent>(entityID);
	mesh->SetGeometry(GeometryManager::Get()->GetGeometry(GEO_TYPE::BOX));
  mesh->SetMaterial("TargetMat");

	ColliderComponent* CC = ecs.AddComponent<ColliderComponent>(entityID);
	CC->SetType<FS_SphereCollider>()->SetRadius(1);
}

void Target::Update()
{
	ECS& ecs = ECS::Get();

	auto& colliders = ecs.GetAllFromType<ColliderComponent>();

	for (auto collider : colliders)
	{
		if (collider.IsActive() == false)
			continue;

		if (collider.GetID() == entityID)
			continue;

		if (ecs.GetComponent<ColliderComponent>(entityID)->IsCollidingWith(collider.GetID()))
			CurrentLife -= 10;
	}

	if (CurrentLife <= 0)
	{
		IsDead = true;
		ecs.RemoveEntity(entityID);
		return;
	}
}