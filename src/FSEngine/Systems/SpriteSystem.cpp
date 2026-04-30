#include "pch.h"

namespace FSE
{
	void SpriteSystem::Update(float dt)
	{
		auto transforms = m_Ecs->GetAllFromType<TransformComponent>();
		auto sprites = m_Ecs->GetAllFromType<SpriteComponent>(); 
		
		XMINT2 mousePos = InputsManager::GetMousePosition();
		XMINT2 winSize = Device::GetWindow()->Size();

		for (int i = 0; i < MAX_COMPONENTS; i++)
		{
			TransformComponent& trs = transforms[i];
			SpriteComponent& sprite = sprites[i];

			if (trs.IsActive() == false || sprite.IsActive() == false)
				continue;

			XMFLOAT3 scale = trs.GetScale();
			sprite.CheckIsHovered(mousePos, scale);
			sprite.CheckIsClicked();

			if (trs.IsUpdatedThisFrame() == false)
				continue;

			XMFLOAT4X4 worldTrs = trs.GetWorldMatrix();
			XMFLOAT3 worldPos = { worldTrs._41, worldTrs._42, worldTrs._43 };
			XMINT2 screenPos = WorldToScreen(worldPos, Device::GetWindow()->Size());

			// Sprite
			XMFLOAT2 scaledSize = { scale.x, scale.y };
			if(sprite.m_Sprite)
			{
				sprite.m_Sprite->SetPosition(screenPos.x, screenPos.y);

				sprite.m_Sprite->SetScaling(scale.x);

				XMFLOAT2 scaleSpr = sprite.m_Sprite->GetScale();
				XMINT2 size = sprite.m_Sprite->GetSize();
				scaledSize = XMFLOAT2(scaleSpr.x / (float)size.x, scaleSpr.y / (float)size.y);
			}

			// Texte
			XMFLOAT2 offset = XMFLOAT2(sprite.m_TextOffSet.x * scaledSize.x, sprite.m_TextOffSet.y * scaledSize.y);

			sprite.m_Text->SetPosition(screenPos.x + offset.x, screenPos.y + offset.y);
			sprite.m_Text->SetScale(scale.x);
		}
	}
}


