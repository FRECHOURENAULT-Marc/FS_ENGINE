#pragma once

namespace FSE
{

	class TransformSystem : public System
	{

	public:
		TransformSystem() : System() {};

		virtual void AfterUpdate(float dt) override;
	};

}



