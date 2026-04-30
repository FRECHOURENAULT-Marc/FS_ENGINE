#include "pch.h"

namespace FSE
{
	template<typename U>
	U* StateMachineComponent::CreateAction(int state)
	{
		return m_StateMachine.CreateAction<U>(state);
	}
}

