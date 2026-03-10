#pragma once

class UpdateMeshWorldSystem : public System
{

public:
	UpdateMeshWorldSystem() : System() {};

	virtual void Update(float dt) override;
};

