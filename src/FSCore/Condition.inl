#include "pch.h"

namespace FSC
{

	template<typename T>
	bool Condition<T>::Test(T* owner)
	{
		return m_IsExpected == OnTest(owner);
	}

}