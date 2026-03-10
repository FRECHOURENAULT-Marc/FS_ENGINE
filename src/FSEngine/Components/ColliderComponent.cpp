#include "pch.h"

void ColliderComponent::Reset()
{
	mCollidingWith.clear();
	if (mCollider != nullptr)
	{
		delete(mCollider);
		mCollider = nullptr;
	}
}

bool ColliderComponent::IsColliding(ColliderComponent* other)
{
	return mCollider->IsColliding(other->GetCollider());
}

void ColliderComponent::EnsureTransform()
{
	if (mCollider != nullptr)
		return;

	//Need a transform to use a collider
	TransformComponent* trs = ECS::Get().GetComponent<TransformComponent>(m_id);

	if (trs == nullptr)
		ThrowIfFailed(E_FAIL);
	if (trs->IsActive() == false)
		ThrowIfFailed(E_FAIL);
}

void ColliderComponent::Update()
{
	TransformComponent* trs = ECS::Get().GetComponent<TransformComponent>(m_id);
	mCollider->SetPosition(trs->GetWorldPosition());

	XMFLOAT3 scale = trs->GetScale();
	int maximum = max(max(scale.x, scale.y), trs->GetScale().z);
	mCollider->SetScale(maximum);
}

bool ColliderComponent::IsCollidingWith(ColliderComponent* other)
{
	return utils::VectorContain(mCollidingWith, other);
}

bool ColliderComponent::IsCollidingWith(int other)
{
	return IsCollidingWith(ECS::Get().GetComponent<ColliderComponent>(other));
}

