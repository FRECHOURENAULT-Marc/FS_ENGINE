#pragma once

enum class LIGHT_TYPE
{
	NONE,
	POINT,
	SPOT,
	DIRECTIONAL
};

class LightComponent : public Component
{
	virtual void Reset() override;

	Light* m_light = nullptr;
	LIGHT_TYPE m_type = LIGHT_TYPE::NONE;

	bool IsLightSet();

public:
	LightComponent();

	void SetPointLight(PointLight* light);
	void SetSpotLight(SpotLight* light);
	void SetDirectionalLight(DirectionalLight* light);

	friend class ECS;
	friend class LightSystem;
};


