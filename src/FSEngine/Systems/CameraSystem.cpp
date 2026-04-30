#include "pch.h"

namespace FSE
{
	void CameraSystem::Update(float dt)
	{
		auto cameras = m_Ecs->GetAllFromType<CameraComponent>();
		auto transforms = m_Ecs->GetAllFromType<TransformComponent>();

		for (int i = 0; i < MAX_COMPONENTS; i++)
		{
			TransformComponent& trs = transforms[i];
			CameraComponent& camera = cameras[i];

			if (camera.IsActive() == false || trs.IsActive() == false)
				continue;
			if (trs.IsUpdatedThisFrame() == false)
				continue;
			camera.Update(trs.GetWorldMatrix());
		}
	}
}


