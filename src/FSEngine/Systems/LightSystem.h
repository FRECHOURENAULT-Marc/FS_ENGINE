#pragma once

class LightSystem : public System
{
public:
	LightSystem() : System() {};

	static void ResetLights(LIGHT_TYPE type);
	static void ResetPointLights();
	static void ResetSpotLights();
	static void ResetDirectionalLights();

	virtual void Update(float dt) override;
};


