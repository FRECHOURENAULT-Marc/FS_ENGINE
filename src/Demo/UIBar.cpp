#include "pch.h"

UIBar::UIBar() : UIImage("RedTex")
{
	XMINT2 pos = XMINT2(200, 850);
	FS_Texture* tex = TextureManager::GetTexture("RedTex");
	m_maskPosY = tex->size.y / 2.0f - tex->size.y / 8.0f;
	m_maskHeight = tex->size.y / 8.0f;
	m_maskWidth = tex->size.x;
	m_spr->SetScaling(5.0f);
	m_spr->SetPosition(pos.x, pos.y);
	m_spr->SetMask(0, m_maskPosY, m_maskWidth, m_maskHeight);
}

void UIBar::OnUpdate()
{
	m_ratio = m_loading / m_capacity;
	m_spr->SetMask(0, m_maskPosY, m_maskWidth * m_ratio, m_maskHeight);
}