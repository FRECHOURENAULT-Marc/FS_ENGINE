#pragma once

#include <vector>

#include "Action.h"

template<typename T>
class FS_StateMachine
{
	std::vector<Action<T>*> mActions;
	int mCurrentState;

	T* mOwner;

public:
	FS_StateMachine();
	~FS_StateMachine();

	void Initialize(T* owner, int stateCount);

	void Update();
	void SetState(int state);
	int GetCurrentState() const { return mCurrentState; }

	template<typename U>
	U* CreateAction(int state);
};

#include "FS_StateMachine.inl"