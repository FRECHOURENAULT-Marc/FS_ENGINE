#pragma once

namespace FSE
{

	class StateMachineSystem : public System
	{
	public:
		StateMachineSystem() : System() {};

		virtual void Update(float dt) override;

	};

}

