#include "pch.h"

namespace FSE
{
	template<typename C>
	inline C* ColliderComponent::SetType()
	{
		if (m_Collider != nullptr)
			delete(m_Collider);

		C* collider = new C();
		m_Collider = collider;

		AddToDefaultLayer();

		return collider;
	}
}



