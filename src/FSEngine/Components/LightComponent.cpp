#include "pch.h"

namespace FSE
{

	void LightComponent::Reset()
	{
		if (m_Light == nullptr)
			return;
		Device::GetRenderer()->RemoveLight(m_Light);
	}

	bool LightComponent::IsLightSet()
	{
		if (m_Light != nullptr)
		{
			Device::GetRenderer()->RemoveLight(m_Light);
			return true;
		}
		return false;
	}

	LightComponent::LightComponent()
	{
	}

	void LightComponent::SetPointLight(PointLight* light)
	{
		IsLightSet();

		m_Light = light;
		m_Type = LIGHT_TYPE::POINT;
		Device::GetRenderer()->AddPointLight(light);
	}

	void LightComponent::SetSpotLight(SpotLight* light)
	{
		IsLightSet();

		m_Light = light;
		m_Type = LIGHT_TYPE::SPOT;
		Device::GetRenderer()->AddSpotLight(light);
	}

	void LightComponent::SetDirectionalLight(DirectionalLight* light)
	{
		IsLightSet();

		m_Light = light;
		m_Type = LIGHT_TYPE::DIRECTIONAL;
		Device::GetRenderer()->AddDirectionalLight(light);
	}
}

