#include "pch.h"


void UpdateMeshWorldSystem::Update(float dt)
{
	auto& transforms = m_ecs->GetAllFromType<TransformComponent>();
	auto& meshs = m_ecs->GetAllFromType<MeshComponent>();

	for (int i = 0; i < transforms.size(); i++)
	{
		TransformComponent& trs = transforms[i];
		MeshComponent& mesh = meshs[i];
		if (trs.IsActive() == false || mesh.IsActive() == false)
			continue;
		if (trs.IsUpdatedThisFrame() == false)
			continue;
		mesh.SetWorld(trs.GetWorldMatrix());
	}
}
