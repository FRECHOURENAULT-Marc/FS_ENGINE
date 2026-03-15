#include "pch.h"

Tree::Tree()
{
	ECS& ecs = ECS::Get();

	for (int i = 0; i < m_SubDivision; i++)
	{
		int id = ecs.CreateEntity();
		TransformComponent* trs = ecs.AddComponent<TransformComponent>(id);

		MeshComponent* mesh = ecs.AddComponent<MeshComponent>(id);
		mesh->SetGeometry(GeometryManager::Get()->GetGeometry(GEO_TYPE::PLANE));
		mesh->SetMaterial("TreeMat");
		trs->SetLocalPosition({ 0, 0, 0 });
		float angle = (2 * 3.141592f / m_SubDivision) * i;
		trs->SetRotation(angle,0, 0);

		m_transform.push_back(trs);

		int id2 = ecs.CreateEntity();
		TransformComponent* trs2 = ecs.AddComponent<TransformComponent>(id2);

		MeshComponent* mesh2 = ecs.AddComponent<MeshComponent>(id2);
		mesh2->SetGeometry(GeometryManager::Get()->GetGeometry(GEO_TYPE::PLANE));
		mesh2->SetMaterial("TreeMat");

		trs2->SetLocalPosition({ 0, 0, 0 });
		trs2->SetRotation(angle + 3.141592, 0, 0);

		m_transform.push_back(trs2);
	}
}

void Tree::SetPosition(XMFLOAT3 position)
{
	for (TransformComponent* transform : m_transform)
	{
		transform->SetLocalPosition(position);
	}
}

void Tree::SetScale(float scale)
{
	for (TransformComponent* transform : m_transform)
	{
		transform->SetScale(scale);
	}
}