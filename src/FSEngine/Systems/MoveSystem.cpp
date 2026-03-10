#include "pch.h"

void MoveSystem::Update(float dt)
{
	auto& transforms = m_ecs->GetAllFromType<TransformComponent>();
	auto& velocities = m_ecs->GetAllFromType<VelocityComponent>();

	for (int i = 0; i < transforms.size(); i++)
	{
		TransformComponent& trs = transforms[i];
		VelocityComponent& vel = velocities[i];
		if (trs.IsActive() == false || vel.IsActive() == false)
			continue;
		trs.Move(dt * vel.GetVelocity());
		//std::cout << "speed " << (vel.GetVelocity()) << " dir " << trs.GetForward().x << ", " << trs.GetForward().y << ", " << trs.GetForward().z << std::endl;
		//std::cout << "Pos " << trs.GetPosition().x << ", " << trs.GetPosition().y << ", " << trs.GetPosition().z << std::endl;
	}
}
