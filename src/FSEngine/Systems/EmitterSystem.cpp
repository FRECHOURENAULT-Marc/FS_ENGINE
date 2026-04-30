#include "pch.h"

namespace FSE
{
	EmitterSystem::EmitterSystem() : System()
	{
		srand(time(nullptr));
	}

	void EmitterSystem::Update(float dt)
	{
		if (m_Ecs->IsParticuleEnabled() == false)
			return;

		auto emitters = m_Ecs->GetAllFromType<EmitterComponent>();

		for (int i = 0; i < MAX_COMPONENTS; i++)
		{
			EmitterComponent& emitter = emitters[i];
			if (emitter.IsActive() == false)
				continue;
			emitter.Update(dt);
		}
	}
}

