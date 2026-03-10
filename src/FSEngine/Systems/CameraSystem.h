#pragma once

class CameraSystem : public System
{

public:
	CameraSystem() : System() {};

	virtual void Update(float dt) override;
};


