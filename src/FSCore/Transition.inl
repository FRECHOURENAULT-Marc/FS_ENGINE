#include "pch.h"

namespace FSC
{

	template<typename T>
	template<typename U>
	U* Transition<T>::AddCondition(bool m_IsExpected)
	{
		static_assert(std::is_base_of<Condition<T>, U>::value, "T must be derived from Condition");

		U* pCondition = new U();

		pCondition->m_IsExpected = m_IsExpected;

		m_Conditions.push_back(pCondition);

		return pCondition;
	}

	template<typename T>
	bool Transition<T>::Try(T* owner)
	{
		for (const auto& c : m_Conditions)
		{
			if (c->Test(owner) == false)
				return false;
		}

		return true;
	}

}