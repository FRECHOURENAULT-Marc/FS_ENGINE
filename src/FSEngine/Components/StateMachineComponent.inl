#include "pch.h"

template<typename U>
U* StateMachineComponent::CreateAction(int state)
{
	return m_StateMachine.CreateAction<U>(state);
}