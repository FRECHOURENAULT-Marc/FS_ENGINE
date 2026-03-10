#pragma once

class VelocityComponent : public Component
{
	virtual void Reset() override;

	float m_velocity = 0.0f;

public:
	VelocityComponent() : Component() {};

	float GetVelocity() { return m_velocity; };

	void SetVelocity(float velocity) { m_velocity = velocity; };
	void AddVelocity(float velocity) { m_velocity += velocity; };

	friend class ECS;
};
