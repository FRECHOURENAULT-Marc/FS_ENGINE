#pragma once

namespace FSR
{
	class SpriteGenerator
	{
		static inline SpriteGenerator* m_Instance = nullptr;

	public:
		SpriteGenerator();

		static SpriteGenerator* Get();
		//Texture name or Material name need to be created before creating the sprite
		static Object2D* CreateSprite(std::string name, bool isMat = false);
	};
}



