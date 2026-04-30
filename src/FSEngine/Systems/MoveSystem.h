#pragma once

namespace FSE
{

	class MoveSystem : public System
	{

	public:
		MoveSystem() : System() {};

		virtual void Update(float dt) override;
	};

}



