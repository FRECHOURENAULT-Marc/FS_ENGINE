#pragma once

namespace FSE
{

	class ColliderSystem : public System
	{

	public:
		ColliderSystem() : System() {};

		virtual void Update(float dt) override;
	};

}



