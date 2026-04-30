#pragma once

namespace FSE
{
	class VelocityComponent : public Component
	{
		virtual void Reset() override;

		float m_Velocity = 0.0f;

	public:
		VelocityComponent() : Component() {};

		float GetVelocity() { return m_Velocity; };

		void SetVelocity(float velocity) { m_Velocity = velocity; };
		void AddVelocity(float velocity) { m_Velocity += velocity; };

		friend class ECS;
	};
}
