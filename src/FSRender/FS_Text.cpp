#include "pch.h"

FS_Text::FS_Text(wchar_t c) : m_letter(c)
{
	m_sprite = FS_Device::Renderer()->AddSprite(MATERIAL_FONT_NAME, true);
	m_sprite->SetScaling(1.0f);
	SetLetter(c);
}

void FS_Text::SetLetter(wchar_t c)
{
	m_letter = c;
	XMINT2 pos = FS_TextManager::GetLetter(m_letter);

	m_sprite->SetMask(pos.x, pos.y, UISTRING_MASK_SIZE, UISTRING_MASK_SIZE + UISTRING_LINE_SPACING);

	float centerXMask = m_sprite->mSize.x / 2.0f;
	float centerYMask = m_sprite->mSize.y / 2.0f;

	XMINT2 diff = XMINT2(centerXMask - pos.x, centerYMask - pos.y);

	m_offset = diff;
}

FS_TextManager::FS_TextManager()
{
	TextureManager::LoadTexture(TEXTURE_FONT_NAME, TEXTURE_FONT_PATH);
	MaterialManager::CreateMaterial(SHADER_SPRITE_NAME, MATERIAL_FONT_NAME, TEXTURE_FONT_NAME);
	for(int i = 0; i < 26; i++)
	{
		majLetters[i] = XMINT2(i * UISTRING_MASK_SIZE, -UISTRING_LINE_SPACING);
		minLetters[i] = XMINT2(i * UISTRING_MASK_SIZE, (UISTRING_MASK_SIZE + UISTRING_LINE_SPACING) * 2);
		numLetters[i] = XMINT2(i * UISTRING_MASK_SIZE, (UISTRING_MASK_SIZE + UISTRING_LINE_SPACING) * 4);
	}
}

XMINT2 FS_TextManager::GetLetter(wchar_t m_letter)
{
	if (m_letter >= L'A' && m_letter <= L'Z')
		return majLetters[m_letter - L'A'];
	else if (m_letter >= L'a' && m_letter <= L'z')
		return minLetters[m_letter - L'a'];
	else if (m_letter >= L'0' && m_letter <= L'9')
		return numLetters[m_letter - L'0'];

	return XMINT2(10000, 10000);
}

void FS_UIString::SetString(std::wstring text)
{
	FS_Device* dev = FS_Device::Get();
	
	int sizeDiff = text.size() - m_letters.size();
	if(text.size() <= m_letters.size())
	{
		int i = 0;
		for (; i < text.size(); i++)
			m_letters[i]->SetLetter(text[i]);
		for (; i < m_letters.size(); i++)
			m_letters[i]->SetLetter(L' ');
	}
	else if (text.size() > m_letters.size())
	{
		int i = 0;
		for (; i < m_letters.size(); i++)
			m_letters[i]->SetLetter(text[i]);
		for (; i < text.size(); i++)
			m_letters.push_back(new FS_Text(text[i]));
	}

	m_text = text;

	SetPosition(m_position.x, m_position.y);
}
void FS_UIString::SetPosition(float x, float y)
{
	float offsetX = 0.0f;
	for (FS_Text* fs_text : m_letters)
	{
		fs_text->m_sprite->SetPosition(x + fs_text->m_offset.x + offsetX, y + fs_text->m_offset.y);
		offsetX += UISTRING_MASK_SIZE * 0.5f; // advance by scaled mask width
	}
	m_position = XMINT2(x, y);
}
