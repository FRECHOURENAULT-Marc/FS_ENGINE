#include "pch.h"

namespace FSE
{
	void TransformSystem::AfterUpdate(float dt)
	{
		auto transforms = m_Ecs->GetAllFromType<TransformComponent>();

		for (int i = 0; i < MAX_COMPONENTS; i++)
		{
			TransformComponent& trs = transforms[i];
			if (trs.IsActive() == false)
				continue;
			trs.SetUpdatedThisFrame(false);
		}
	}
}


