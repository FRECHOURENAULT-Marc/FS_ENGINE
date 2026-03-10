#include "pch.h"

template<typename C>
inline C* ColliderComponent::SetType()
{
	EnsureTransform();

	if (mCollider != nullptr)
		delete(mCollider);

	C* collider = new C();
	mCollider = collider;
	return collider;
}

