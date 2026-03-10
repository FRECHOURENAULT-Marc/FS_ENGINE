#include "pch.h"

TransformSystem::TransformSystem() : System()
{
}

void TransformSystem::Update(float dt)
{
	auto& transforms = m_ecs->GetAllFromType<TransformComponent>();

	for (int i = 0; i < transforms.size(); i++)
	{
		TransformComponent& trs = transforms[i];
		if (trs.IsActive() == false)
			continue;
		//trs.UpdateWorld();
	}
}