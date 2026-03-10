#include "pch.h"

Entity::Entity()
{
}

void Entity::AddChild(int id)
{
	ECS& ecs = ECS::Get();
	auto otherTrs = ecs.GetComponent<TransformComponent>(id);
	ecs.GetComponent<TransformComponent>(m_id)->AddChild(otherTrs);
}

void Entity::RemoveChild(int id)
{
	ECS& ecs = ECS::Get();
	auto otherTrs = ecs.GetComponent<TransformComponent>(id);
	ecs.GetComponent<TransformComponent>(m_id)->RemoveChild(otherTrs);
}
