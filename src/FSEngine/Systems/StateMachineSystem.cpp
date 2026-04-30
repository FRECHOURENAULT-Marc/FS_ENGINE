#include "pch.h"

namespace FSE
{
	void StateMachineSystem::Update(float dt)
	{
		auto stateMachines = m_Ecs->GetAllFromType<StateMachineComponent>();

		for (int i = 0; i < MAX_COMPONENTS; i++)
		{
			StateMachineComponent& stateMachine = stateMachines[i];
			if (stateMachine.IsActive() == false)
				continue;
			stateMachine.Update();
		}
	}
}

