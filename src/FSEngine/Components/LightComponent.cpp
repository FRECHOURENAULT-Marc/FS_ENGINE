#include "pch.h"
#include "LightComponent.h"

void LightComponent::Reset()
{
	if (m_light == nullptr)
		return;
	FS_Device::Renderer()->RemoveLight(m_light);
}

bool LightComponent::IsLightSet()
{
	if (m_light != nullptr)
	{
		FS_Device::Renderer()->RemoveLight(m_light);
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

	m_light = light;
	m_type = LIGHT_TYPE::POINT;
	FS_Device::Renderer()->AddPointLight(light);
}

void LightComponent::SetSpotLight(SpotLight* light)
{
	IsLightSet();

	m_light = light;
	m_type = LIGHT_TYPE::SPOT;
	FS_Device::Renderer()->AddSpotLight(light);
}

void LightComponent::SetDirectionalLight(DirectionalLight* light)
{
	IsLightSet();

	m_light = light;
	m_type = LIGHT_TYPE::DIRECTIONAL;
	FS_Device::Renderer()->AddDirectionalLight(light);
}
