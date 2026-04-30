#pragma once

#define UI_ZINDEX 1000.0f

namespace FSE
{

	class SpriteComponent : public Component
	{
		std::string m_TextureName = "";
		std::wstring m_TextWstr = L"";

		Object2D* m_Sprite = nullptr;
		UIString* m_Text = nullptr;

		XMINT2 m_TextOffSet = { 0,0 };

		bool m_IsClicked = false;
		bool m_IsHovered = false;

		void CheckIsHovered(XMINT2 _MousePos, XMFLOAT3 scale);
		void CheckIsClicked();
		void RemoveObject2D();
		virtual void Reset() override;

	public:
		SpriteComponent();

		Object2D* GetSprite() { return m_Sprite; };
		UIString* GetText() { return m_Text; };
		XMINT2 GetSize() { return m_Sprite->GetSize(); };
		XMFLOAT2 GetSprScale() { return m_Sprite->GetScale(); };
		std::wstring GetTextWstr();
		std::string GetTextureName() { return m_TextureName; };
		bool HaveSprite() { return m_Sprite != nullptr; };
		bool IsClicked() { return m_IsClicked; };
		bool IsHovered() { return m_IsHovered; };

		bool SetTexture(std::string texName);
		void SetText(std::wstring text);
		void SetWorld(XMFLOAT4X4 world) { m_Sprite->SetWorld(world); };
		void SetTextOffSet(XMINT2 offset) { m_TextOffSet = offset; };
		void SetZIndex(int _z);

		void Hide();
		void HideText();
		void HideSprite();
		void Show();
		void ShowText();
		void ShowSprite();

		friend class ECS;
		friend class SpriteSystem;
	};

}



