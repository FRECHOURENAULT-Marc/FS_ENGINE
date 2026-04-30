#include "pch.h"

namespace FSE
{
	void ColliderComponent::Reset()
	{
		m_CollidingWith.clear();
		if (m_Collider != nullptr)
		{
			delete(m_Collider);
			m_Collider = nullptr;
		}
		m_Tag = "";
		SceneManager::Get()->GetActiveScene<Scene>()->RemoveFromLayer(this);
	}

	bool ColliderComponent::IsColliding(ColliderComponent* other)
	{
		if (other->GetCollider() == nullptr)
			return false;
		return m_Collider->IsColliding(other->GetCollider());
	}

	void ColliderComponent::AddToDefaultLayer()
	{
		SceneManager::Get()->GetActiveScene<Scene>()->AddToLayer(this, BASE_COLLIDERS_LAYER);
	}

	void ColliderComponent::EnsureTransform()
	{
		if (m_Collider != nullptr)
			return;

		//Need a transform to use a collider
		TransformComponent* trs = ECS::Get().GetComponent<TransformComponent>(m_ID);

		if (trs == nullptr)
			ThrowIfFailed(E_FAIL);
		if (trs->IsActive() == false)
			ThrowIfFailed(E_FAIL);
	}

	void ColliderComponent::Update()
	{
		TransformComponent* trs = ECS::Get().GetComponent<TransformComponent>(m_ID);
		m_Collider->SetPosition(trs->GetWorldPosition());

		if (trs->IsUpdatedThisFrame() == false)
			return;

		XMFLOAT3 scale = trs->GetScale();
		m_Collider->OnUpdate(scale);

	}

	void ColliderComponent::AddToLayer(std::string _layerName)
	{
		SceneManager::Get()->GetActiveScene<Scene>()->AddToLayer(this, _layerName);
	}

	ColliderComponent* ColliderComponent::IsCollidingWith(int otherEntityID)
	{
		for (auto* collider : m_CollidingWith)
		{
			if (collider->m_ID == otherEntityID)
				return collider;
		}
		return nullptr;
	}
	ColliderComponent* ColliderComponent::IsCollidingWith(ColliderComponent* other)
	{
		for (auto* collider : m_CollidingWith)
		{
			if (collider == other)
				return collider;
		}
		return nullptr;
	}
	ColliderComponent* ColliderComponent::IsCollidingWith(std::string tag)
	{
		for(auto* collider : m_CollidingWith)
		{
			if (collider->GetTag() == tag)
				return collider;
		}
		return nullptr;
	}

}



