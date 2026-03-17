#include "pch.h"

void ActionTest_Idle::OnStart(Entity* pOwner)
{
	ECS& ecs = ECS::Get();
	ecs.GetComponent<VelocityComponent>(pOwner->GetID())->SetVelocity(0.0f);
	ecs.GetComponent<TransformComponent>(pOwner->GetID())->SetScale(1.25f);
}

void ActionTest_Moving::OnStart(Entity* pOwner)
{
	ECS& ecs = ECS::Get();
	ecs.GetComponent<VelocityComponent>(pOwner->GetID())->SetVelocity(1.0f);
	ecs.GetComponent<TransformComponent>(pOwner->GetID())->SetScale(1);
}

void ActionTest_Reverse::OnStart(Entity* pOwner)
{
	ECS& ecs = ECS::Get();
	ecs.GetComponent<VelocityComponent>(pOwner->GetID())->SetVelocity(-1.0f);
	TransformComponent* trs = ecs.GetComponent<TransformComponent>(pOwner->GetID());
	trs->SetScale(1);
}
