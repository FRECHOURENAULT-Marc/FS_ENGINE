#include "pch.h"

void LightSystem::ResetLights(LIGHT_TYPE type)
{
	auto& lights = ECS::Get().GetAllFromType<LightComponent>();

	for (int i = 0; i < lights.size(); i++)
	{
		LightComponent& light = lights[i];
		if (light.IsActive() == false)
			continue;
		if (light.m_light == nullptr)
			continue;
		if(light.m_type != type)
			continue;
		light.Reset();
	}

	switch (type)
	{
	case LIGHT_TYPE::POINT:
		FS_Device::Renderer()->ClearPointLights();
		break;
	case LIGHT_TYPE::DIRECTIONAL:
		FS_Device::Renderer()->ClearDirectionalLights();
		break;
	case LIGHT_TYPE::SPOT:
		FS_Device::Renderer()->ClearSpotLights();
		break;
	}
}

void LightSystem::ResetPointLights()
{
	ResetLights(LIGHT_TYPE::POINT);
}

void LightSystem::ResetSpotLights()
{
	ResetLights(LIGHT_TYPE::SPOT);
}

void LightSystem::ResetDirectionalLights()
{
	ResetLights(LIGHT_TYPE::DIRECTIONAL);
}

void LightSystem::Update(float dt)
{
	auto& lights = m_ecs->GetAllFromType<LightComponent>();
	auto& transforms = m_ecs->GetAllFromType<TransformComponent>();

	for (int i = 0; i < lights.size(); i++)
	{
		TransformComponent& trs = transforms[i];
		LightComponent& light = lights[i];

		if (light.IsActive() == false || trs.IsActive() == false)
			continue;
		if (light.m_light == nullptr)
			continue;
		light.m_light->Position = trs.GetWorldPosition();
		light.m_light->Direction = trs.GetForward();
	}
}
