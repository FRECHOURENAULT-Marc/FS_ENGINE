#pragma once

class StateMachineSystem : public System
{
public:
	StateMachineSystem() : System() {};

	virtual void Update(float dt) override;

};