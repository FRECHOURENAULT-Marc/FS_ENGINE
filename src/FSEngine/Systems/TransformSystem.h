#pragma once

class TransformSystem : public System
{

public:
	TransformSystem() : System() {};

	virtual void Update(float dt) override;
};

