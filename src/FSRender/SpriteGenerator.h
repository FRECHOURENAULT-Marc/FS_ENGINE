#pragma once

class SpriteGenerator
{
	static inline SpriteGenerator* m_instance = nullptr;

public:
	SpriteGenerator();

	static SpriteGenerator* Get();
	//Texture name or Material name need to be created before creating the sprite
	static FS_Sprite* CreateSprite(std::string name, bool isMat = false);
};

