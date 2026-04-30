#include "pch.h"

namespace FSE
{

	void LightSystem::ResetLights(LIGHT_TYPE type)
	{
		auto lights = ECS::Get().GetAllFromType<LightComponent>();

		for (int i = 0; i < MAX_COMPONENTS; i++)
		{
			LightComponent& light = lights[i];
			if (light.IsActive() == false)
				continue;
			if (light.m_Light == nullptr)
				continue;
			if (light.m_Type != type)
				continue;
			light.Reset();
		}

		switch (type)
		{
		case LIGHT_TYPE::POINT:
			Device::GetRenderer()->ClearPointLights();
			break;
		case LIGHT_TYPE::DIRECTIONAL:
			Device::GetRenderer()->ClearDirectionalLights();
			break;
		case LIGHT_TYPE::SPOT:
			Device::GetRenderer()->ClearSpotLights();
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
		auto lights = m_Ecs->GetAllFromType<LightComponent>();
		auto transforms = m_Ecs->GetAllFromType<TransformComponent>();

		for (int i = 0; i < MAX_COMPONENTS; i++)
		{
			TransformComponent& trs = transforms[i];
			LightComponent& light = lights[i];

			if (light.IsActive() == false || trs.IsActive() == false)
				continue;
			if (light.m_Light == nullptr)
				continue;
			light.m_Light->m_Position = trs.GetWorldPosition();
			light.m_Light->m_Direction = trs.GetForward();
		}
	}
}

