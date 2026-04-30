#pragma once

namespace FSE
{

	class CameraSystem : public System
	{

	public:
		CameraSystem() : System() {};

		virtual void Update(float dt) override;
	};

}




