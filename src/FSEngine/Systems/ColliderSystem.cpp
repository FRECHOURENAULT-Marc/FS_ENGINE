#include "pch.h"

void ColliderSystem::Update(float dt)
{
	auto& allColliders = m_ecs->GetAllFromType<ColliderComponent>();

	std::vector<ColliderComponent*> activeCollider;
	//remove inactive component
	for (int i = 0; i < allColliders.size(); i++)
	{
		auto& collider = allColliders[i];
		if (collider.IsActive() == false)
			continue;
		activeCollider.push_back(&collider);
	}
	for (auto collider : activeCollider)
	{
		collider->Update();
		collider->ResetColliding();
	}
	//last to first : previous element (if deleted) does'nt affect the next verification
	int vectorLastIndex = activeCollider.size() - 1;
	for(int i = vectorLastIndex; i >= 0; i--)
	{
		auto collider = activeCollider[i];
		for (auto other : activeCollider)
		{
			if (collider == other)
				continue;
			if (collider->IsColliding(other) == false)
				continue;

			collider->AddColliding(other);
			other->AddColliding(collider);
		}
		activeCollider.erase(activeCollider.begin() + i);
	}
}
