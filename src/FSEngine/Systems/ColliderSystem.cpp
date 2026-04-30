#include "pch.h"

namespace FSE
{
	void ColliderSystem::Update(float dt)
	{
		auto allLayers = m_Ecs->GetLayersMap();

		auto allColliders = ECS::Get().GetAllFromType<ColliderComponent>();
		for(int i = 0; i < MAX_COMPONENTS; i++)
		{
			ColliderComponent* collider = allColliders + i; //C array
			if (collider->IsActive() == false)
				continue;

			collider->Update();
			collider->ResetColliding();
		}

		for (auto layer : allLayers)
		{
			std::vector<ColliderComponent*> colliders = layer.second;

			int vectorLastIndex = colliders.size() - 1;
			for (int i = vectorLastIndex; i >= 0; i--)
			{
				auto me = colliders[i];

				for (auto other : colliders)
				{
					if (me->IsActive() == false)
						continue;

					if (me == other)
						continue;

					if (me->IsColliding(other) == false)
						continue;

					me->AddColliding(other);
					other->AddColliding(me);
				}

				colliders.erase(colliders.begin() + i);
			}
		}
	}
}
