#include "pch.h"

namespace FSC
{

	template<typename T>
	StateMachine<T>::StateMachine()
	{
	}

	template<typename T>
	StateMachine<T>::~StateMachine()
	{
		for (Action<T>* pAction : m_Actions)
			delete pAction;
	}

	template<typename T>
	void StateMachine<T>::SetState(int state)
	{
		if (m_CurrentState > 0 && m_CurrentState < m_Actions.size())
			m_Actions[m_CurrentState]->OnEnd(m_Owner);

		m_CurrentState = state;

		m_Actions[m_CurrentState]->OnStart(m_Owner);
	}

	template<typename T>
	void StateMachine<T>::Initialize(T* owner, int stateCount)
	{
		m_Owner = owner;
		m_CurrentState = -1;
		m_Actions.resize(stateCount);
	}

	template<typename T>
	void StateMachine<T>::Update()
	{
		if (m_CurrentState == -1)
			return;

		int transitionState = m_Actions[m_CurrentState]->Update(m_Owner);

		if (transitionState == -1)
			return;

		SetState(transitionState);
	}

	template<typename T>
	template<typename U>
	U* StateMachine<T>::CreateAction(int state)
	{
		_ASSERT(state >= 0 && state < m_Actions.size());
		_ASSERT(m_Actions[state] == nullptr);

		static_assert(std::is_base_of<Action<T>, U>::value, "U must derive from Action<T>");

		U* pAction = new U();

		m_Actions[state] = pAction;

		return pAction;
	}

}