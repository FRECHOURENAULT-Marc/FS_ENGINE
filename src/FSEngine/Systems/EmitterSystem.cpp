#include "pch.h"

EmitterSystem::EmitterSystem() : System()
{
	srand(time(nullptr));
}

void EmitterSystem::Update(float dt)
{
	if (m_ecs->IsParticuleEnabled() == false)
		return;

	auto& emitters = m_ecs->GetAllFromType<EmitterComponent>();

	for (int i = 0; i < emitters.size(); i++)
	{
		EmitterComponent& emitter = emitters[i];
		if (emitter.IsActive() == false)
			continue;
		emitter.Update(dt);
	}
}