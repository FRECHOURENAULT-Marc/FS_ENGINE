#pragma once

class StateMachineComponent : public Component
{
	virtual void Reset() override;

	FS_StateMachine<Entity> m_StateMachine;

public:
	StateMachineComponent();

	void Update() { m_StateMachine.Update(); }
	void SetState(int state);
	void InitializeStateMachine(int stateCount);

	template<typename U>
	U* CreateAction(int state);

	friend class ECS;
};

#include "StateMachineComponent.inl"

