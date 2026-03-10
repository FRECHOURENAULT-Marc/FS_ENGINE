#include "pch.h"

void StateMachineSystem::Update(float dt)
{
	auto& stateMachines = m_ecs->GetAllFromType<StateMachineComponent>();

	for (int i = 0; i < stateMachines.size(); i++)
	{
		StateMachineComponent& stateMachine = stateMachines[i];
		if (stateMachine.IsActive() == false)
			continue;
		stateMachine.Update();
	}
}