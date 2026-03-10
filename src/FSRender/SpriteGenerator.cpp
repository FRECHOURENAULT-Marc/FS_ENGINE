#include "pch.h"

SpriteGenerator::SpriteGenerator()
{
}

SpriteGenerator* SpriteGenerator::Get()
{
	if (m_instance == nullptr)
		m_instance = new SpriteGenerator();

	return m_instance;
}

FS_Sprite* SpriteGenerator::CreateSprite(std::string name, bool isMat)
{
	FS_Device* device = FS_Device::Get();
	FS_Renderer* renderer = FS_Device::Renderer();
	FS_Sprite* sprite = new FS_Sprite();
	auto matManager = device->m_FSMaterial;
	auto texManager = device->m_FSTextures;
	std::string matName = name + "Mat";

	if (isMat)
		matName = name;
	if (isMat == false) // Create a material if texture is using only by this sprite
		matManager->CreateMaterial(SHADER_SPRITE_NAME, matName, name);

	sprite->mGeo = GeometryManager::Get()->GetGeometry(GEO_TYPE::PLANE);
	sprite->mMaterialIndex = matManager->GetMaterialIndex(matName);
	int texIndex = matManager->GetMaterial(sprite->mMaterialIndex)->TextureIndex;
	sprite->mSize = texManager->GetTexture(texIndex)->size;
	sprite->misActive = true;
	sprite->winSize = { device->m_FSWindow->Width(), device->m_FSWindow->Height() };
	sprite->SetScaling(1.0f);
	sprite->SetPosition(0, 0);
	sprite->m_transform.UpdateWorld();
	sprite->m_transform.UpdateInvWorld();

	renderer->m_Sprites.push_back(sprite);
	return sprite;
}
