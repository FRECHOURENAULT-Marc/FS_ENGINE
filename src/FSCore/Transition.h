#pragma once

namespace FSC
{

	template<typename T>
	class Transition
	{
	protected:
		std::vector<Condition<T>*> m_Conditions;
		int m_TransitionState;

	public:
		Transition(int transitionState) : m_TransitionState(transitionState) {}

		template<typename U>
		U* AddCondition(bool m_IsExpected = true);

		bool Try(T* owner);
		int GetTransitionState() { return m_TransitionState; }
	};

}

#include "Transition.inl"