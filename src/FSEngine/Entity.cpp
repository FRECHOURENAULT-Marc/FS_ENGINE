#include "pch.h"

namespace FSE
{
	Entity::Entity()
	{
	}

	void Entity::AddChild(int id)
	{
		ECS& ecs = ECS::Get();
		auto otherTrs = ecs.GetComponent<TransformComponent>(id);
		ecs.GetComponent<TransformComponent>(m_ID)->AddChild(otherTrs);
	}

	void Entity::RemoveChild(int id)
	{
		ECS& ecs = ECS::Get();
		auto otherTrs = ecs.GetComponent<TransformComponent>(id);
		ecs.GetComponent<TransformComponent>(m_ID)->RemoveChild(otherTrs);
	}
}


