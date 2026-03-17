#include "pch.h"

void CameraSystem::Update(float dt)
{
	auto& cameras = m_ecs->GetAllFromType<CameraComponent>();
	auto& transforms = m_ecs->GetAllFromType<TransformComponent>();

	for (int i = 0; i < cameras.size(); i++)
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
