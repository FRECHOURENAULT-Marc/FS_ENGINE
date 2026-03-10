#include "pch.h"

UIImage::UIImage(std::string texName) : UI()
{
	FS_Texture* tex = TextureManager::GetTexture(texName);
	m_spr = m_ecs->AddSprite(texName);
	m_spr->SetScaling(1.0f);
}