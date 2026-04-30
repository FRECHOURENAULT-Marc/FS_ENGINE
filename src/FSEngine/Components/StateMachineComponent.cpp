#include "pch.h"

namespace FSE
{
	void StateMachineComponent::Reset()
	{
		m_StateMachine = StateMachine<Entity>(); //Work if StateMachine has a default constructor well built
	}

	StateMachineComponent::StateMachineComponent() : Component(), m_StateMachine()
	{
	}

	void StateMachineComponent::SetState(int state)
	{
		m_StateMachine.SetState(state);
	}

	void StateMachineComponent::InitializeStateMachine(int stateCount)
	{
		m_StateMachine.Initialize(ECS::Get().GetEntity(m_ID), stateCount);
	}
}


