#include "pch.h"

void LivingEntity::SetupCollider()
{
	ColliderComponent* collider = m_ecs->AddComponent<ColliderComponent>(m_entityID);
	collider->SetType<FS_SphereCollider>();
	FS_SphereCollider* fsCollider = (FS_SphereCollider*)collider->GetCollider();
	fsCollider->SetRadius(m_colliderRadius);
}

FS_SphereCollider* LivingEntity::GetFSCollider()
{
	ColliderComponent* collider = m_ecs->GetComponent<ColliderComponent>(m_entityID);
	if (collider == nullptr)
		return nullptr;
	return (FS_SphereCollider*)collider->GetCollider();
}

LivingEntity::LivingEntity(bool haveMesh)
{
	m_ecs = &ECS::Get();
	m_entityID = m_ecs->CreateEntity();
	m_ecs->AddComponent<TransformComponent>(m_entityID);
	if(haveMesh)
		m_ecs->AddComponent<MeshComponent>(m_entityID)->SetGeometry(GeometryManager::GetGeometry(GEO_TYPE::BOX));
	SetupCollider();
}

void LivingEntity::SetHealth(float health)
{
	m_health = health;
	if(m_health > m_maxHealth)
		m_health = m_maxHealth;
	//Dead
	if (m_isAlive && m_health <= 0.0f)
	{
		m_isAlive = false;
		m_health = 0.0f;
		
		m_ecs->RemoveComponent<ColliderComponent>(m_entityID);
	}
	//Alive
	else if (m_isAlive == false && m_health > 0.0f)
	{
		m_isAlive = true;
		SetupCollider();
	}
}

void LivingEntity::SetColliderRadius(float radius)
{
	m_colliderRadius = radius;
	FS_SphereCollider* fsCollider = GetFSCollider();
	if (fsCollider == nullptr)
		return;
	fsCollider->SetRadius(m_colliderRadius);
}

void LivingEntity::SetGeometry(GEO_TYPE type)
{
	m_ecs->GetComponent<MeshComponent>(m_entityID)->SetGeometry(GeometryManager::GetGeometry(type));
}

bool LivingEntity::IsCollidingWith(int other)
{
	return m_ecs->GetComponent<ColliderComponent>(m_entityID)->IsCollidingWith(other);
}
