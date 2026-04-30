#include "pch.h"

namespace FSR
{

	SpriteGenerator::SpriteGenerator()
	{
	}

	SpriteGenerator* SpriteGenerator::Get()
	{
		if (m_Instance == nullptr)
			m_Instance = new SpriteGenerator();

		return m_Instance;
	}

	Object2D* SpriteGenerator::CreateSprite(std::string textureName, bool isMat)
	{
		Device* device = Device::Get();
		RendererManager* renderer = Device::GetRenderer();
		Object2D* sprite = new Object2D();
		auto materialManager = MaterialManager::Get();
		auto textureManager = TextureManager::Get();
		std::string materialName = textureName + "Mat";

		if (isMat)
			materialName = textureName;
		if (isMat == false)
		{
			int textureIndex = TextureManager::GetTextureIndex(textureName);
			if(materialManager->GetMaterialIndexFromTextureID(textureIndex) == -1)
				materialManager->CreateMaterial(SHADER_SPRITE_NAME, materialName, textureName); // Create a material if texture doesnt have one
			if(materialManager->GetMaterialIndex(materialName) == -1)
				materialManager->CreateMaterial(SHADER_SPRITE_NAME, materialName, textureName); // Create a material if texture doesnt have one
		} 
			

		sprite->SetGeo(GeometryManager::Get()->GetGeometry(GEO_TYPE::PLANEUI));
		sprite->SetMaterialIndex(materialManager->GetMaterialIndex(materialName));
		int textureIndex = materialManager->GetMaterial(sprite->GetMaterialIndex())->TextureIndex;
		XMINT2 textureSize = textureManager->GetTexture(textureIndex)->m_Size;
		sprite->SetSize(textureSize.x, textureSize.y);
		sprite->SetActive(true);
		sprite->SetWinSize(device->GetWindow()->Width(), device->GetWindow()->Height());
		sprite->SetScaling(1.0f);
		sprite->SetPosition(0, 0);

		renderer->m_Sprites.push_back(sprite);
		return sprite;
	}

}
