#pragma once

#define TEXTURE_FONT_NAME "FontTexture"
#define MATERIAL_FONT_NAME "FontMat"
#define TEXTURE_FONT_PATH L"../../res/Textures/Font1920p.dds"

#define UISTRING_LINE_SPACING 6
#define UISTRING_MASK_SIZE 64

struct FS_Text
{
	FS_Sprite* m_sprite = nullptr;
	wchar_t m_letter = L' ';
	XMINT2 m_offset = XMINT2(0, 0);

	FS_Text(wchar_t c);
	void SetLetter(wchar_t c);
};

class FS_TextManager
{
	inline static XMINT2 majLetters[26] = {};
	inline static XMINT2 minLetters[26] = {};
	inline static XMINT2 numLetters[26] = {};

public:
	FS_TextManager();
	static XMINT2 GetLetter(wchar_t m_letter);
};


class FS_UIString
{
	std::vector<FS_Text*> m_letters;
	std::wstring m_text;

	XMINT2 m_position = XMINT2(0, 0);
public:
	void SetString(std::wstring text);
	void SetPosition(float x, float y);

	//Default font size is 30.0f
	void SetFontSize(float size) = delete; //TO DO

	std::wstring GetString() { return m_text; }
};