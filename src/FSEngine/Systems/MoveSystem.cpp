#include "pch.h"

namespace FSE
{
	void MoveSystem::Update(float dt)
	{
		auto transforms = m_Ecs->GetAllFromType<TransformComponent>();
		auto velocities = m_Ecs->GetAllFromType<VelocityComponent>();

		for (int i = 0; i < MAX_COMPONENTS; i++)
		{
			TransformComponent& trs = transforms[i];
			VelocityComponent& vel = velocities[i];
			if (trs.IsActive() == false || vel.IsActive() == false)
				continue;
			float velocity = vel.GetVelocity();
			if (velocity == 0.0f)
				continue;
			trs.Move(dt * velocity);
		}
	}
}


