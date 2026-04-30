#include "pch.h"

namespace FSE
{
	void UpdateMeshWorldSystem::Update(float dt)
	{
		auto transforms = m_Ecs->GetAllFromType<TransformComponent>();
		auto meshs = m_Ecs->GetAllFromType<MeshComponent>();

		for (int i = 0; i < MAX_COMPONENTS; i++)
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
}


