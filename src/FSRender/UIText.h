#pragma once

#define TEXTURE_FONT_NAME "FontTexture"
#define MATERIAL_FONT_NAME "FontMat"
#define TEXTURE_FONT_PATH L"../../res/Textures/Font1920p.dds"

#define UISTRING_LINE_SPACING 6
#define UISTRING_MASK_SIZE 64

namespace FSR
{
	struct UIText
	{
		Object2D* m_Sprite = nullptr;
		wchar_t m_Letter = L' ';
		XMINT2 m_Offset = XMINT2(0, 0);
		float m_Scale = 1.0f;

		UIText(wchar_t c);
		void SetLetter(wchar_t c);
		void SetScale(float scale);
	};

	class UITextManager
	{
		inline static XMINT2 m_MajLetters[26] = {};
		inline static XMINT2 m_MinLetters[26] = {};
		inline static XMINT2 m_NumLetters[26] = {};
		inline static XMINT2 m_SpecialChar[1] =
		{
			{12 * UISTRING_MASK_SIZE, (UISTRING_MASK_SIZE + UISTRING_LINE_SPACING) * 6}
		};

	public:
		UITextManager();
		static XMINT2 GetLetter(wchar_t m_letter);
	};


	class UIString
	{
		std::vector<UIText*> m_Letters;
		std::wstring m_Text;

		XMINT2 m_Position = XMINT2(0, 0);
		float m_Scale = 1.0f;

		int m_ZIndex = 0;

	public:
		UIString() = default;
		~UIString() { ClearString(); };

		void SetActive(bool active);
		void SetString(std::wstring text);
		void SetPosition(float x, float y);
		void SetZIndex(int z);
		void SetScale(float scale);
		//Default font size is 30.0f
		void SetFontSize(float size) = delete; //TO DO


		void ClearString();

		std::wstring GetString() { return m_Text; }
	};
}