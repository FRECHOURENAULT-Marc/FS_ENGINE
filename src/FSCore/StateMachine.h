#pragma once

#include <vector>

#include "Action.h"

namespace FSC
{

	template<typename T>
	class StateMachine
	{
	protected:
		std::vector<Action<T>*> m_Actions;
		int m_CurrentState;

		T* m_Owner;

	public:
		StateMachine();
		~StateMachine();

		void Initialize(T* owner, int stateCount);

		void Update();
		void SetState(int state);
		int GetCurrentState() const { return m_CurrentState; }

		template<typename U>
		U* CreateAction(int state);
	};

}

#include "StateMachine.inl"