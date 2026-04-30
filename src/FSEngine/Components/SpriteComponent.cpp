#include "pch.h"

namespace FSE
{

	void SpriteComponent::CheckIsHovered(XMINT2 _MousePos, XMFLOAT3 scale)
	{
		m_IsHovered = false;

		if (m_Sprite == nullptr)
			return;

		XMINT2 size = m_Sprite->GetSize();
		XMFLOAT4 mask = m_Sprite->GetMask();

		size.x *= scale.x;
		size.y *= scale.y;

		mask.z *= scale.x;
		mask.w *= scale.y;

		XMFLOAT4X4 world = m_Sprite->GetWorld();
		XMFLOAT3 worldPos = { world._41, world._42, world._43 };
		XMINT2 screenPos = WorldToScreen(worldPos, Device::GetWindow()->Size());

		//Position relative
		bool isHoverWithoutMask = false;
		{
			bool isInLeft = _MousePos.x >= screenPos.x - size.x / 2.0f;
			bool isInRight = _MousePos.x <= screenPos.x + size.x / 2.0f;
			bool isHoverX = isInRight && isInLeft;

			bool isInTop = _MousePos.y >= screenPos.y - size.y / 2.0f;
			bool isInBottom = _MousePos.y <= screenPos.y + size.y / 2.0f;
			bool isHoverY = isInTop && isInBottom;

			if (isHoverX && isHoverY)
				isHoverWithoutMask = true;
				
		}
		//Mask relative
		//Check if mouse is in the mask of the sprite
		//Convert mask from UV to pixels
		XMINT2 maskPos = UVToScreenLocal({ mask.x, mask.y }, size);
		XMINT2 maskSize = UVToScreenLocal({ mask.z, mask.w }, size);
		bool isHoverWithMask = false;
		{
			float left = screenPos.x - size.x / 2.0f;
			bool isInLeft = _MousePos.x >= left + maskPos.x;
			bool isInRight = _MousePos.x <= left + maskPos.x + maskSize.x;
			bool isHoverX = isInRight && isInLeft;

			float top = screenPos.y - size.y / 2.0f;
			bool isInTop = _MousePos.y >= top + maskPos.y;
			bool isInBottom = _MousePos.y <= top + maskPos.y + maskSize.y;
			bool isHoverY = isInTop && isInBottom;

			if (isHoverX && isHoverY)
				isHoverWithMask = true;
		}

		if (isHoverWithoutMask && isHoverWithMask)
			m_IsHovered = true;
	}
	void SpriteComponent::CheckIsClicked()
	{
		m_IsClicked = InputsManager::GetButtonDown(Mouse::LEFT) && m_IsHovered;
	}
	void SpriteComponent::RemoveObject2D()
	{
		if (m_Sprite != nullptr)
			Device::Get()->RemoveSprite(m_Sprite);
		m_Sprite = nullptr;
	}
	void SpriteComponent::Reset()
	{
		RemoveObject2D();
		m_Text->ClearString();
	}


	SpriteComponent::SpriteComponent() : Component()
	{
		m_Text = new UIString();
	}

	std::wstring SpriteComponent::GetTextWstr()
	{
		return m_TextWstr;
	}

	bool SpriteComponent::SetTexture(std::string texName)
	{
		RemoveObject2D();

		int textureID = TextureManager::GetTextureIndex(texName);
		if (textureID == -1)
			return false;

		m_TextureName = texName;
		m_Sprite = Device::Get()->AddSprite(texName, false);
		SetZIndex(UI_ZINDEX);
		return true;
	}

	void SpriteComponent::SetText(std::wstring text)
	{
		m_Text->SetString(text);
		m_TextWstr = text;
	}

	void SpriteComponent::SetZIndex(int _z)
	{
		if(m_Sprite)
			m_Sprite->SetZIndex(_z);
		m_Text->SetZIndex(_z + 1);
		Device::GetRenderer()->SetReorderSprites(true);
	}

	void SpriteComponent::Hide()
	{
		SetActive(false);
		HideSprite();
		HideText();
	}
	void SpriteComponent::HideText()
	{
		if (m_Text)
			m_Text->SetActive(false);
	}
	void SpriteComponent::HideSprite()
	{
		if (m_Sprite)
			m_Sprite->SetActive(false);
	}

	void SpriteComponent::Show()
	{
		SetActive(true);
		ShowSprite();
		ShowText();
	}
	void SpriteComponent::ShowText()
	{
		if (m_Text)
			m_Text->SetActive(true);
		ECS::Get().GetComponent<TransformComponent>(m_ID)->SetUpdatedThisFrame(true);
	}
	void SpriteComponent::ShowSprite()
	{
		if (m_Sprite)
			m_Sprite->SetActive(true);
		ECS::Get().GetComponent<TransformComponent>(m_ID)->SetUpdatedThisFrame(true);
	}
}

