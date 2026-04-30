#pragma once

namespace FSE
{

	class SpriteSystem : public System
	{

	public:
		SpriteSystem() : System() {};

		virtual void Update(float dt) override;
	};

}



