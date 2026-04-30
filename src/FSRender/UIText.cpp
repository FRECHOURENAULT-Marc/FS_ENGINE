#include "pch.h"

namespace FSR
{
	UIText::UIText(wchar_t c) : m_Letter(c)
	{
		m_Sprite = Device::GetRenderer()->AddSprite(MATERIAL_FONT_NAME, true);
		m_Sprite->SetScaling(1.0f);
		SetLetter(c);
	}

	void UIText::SetLetter(wchar_t c)
	{
		m_Letter = c;
		XMINT2 pos = UITextManager::GetLetter(m_Letter);

		m_Sprite->SetMask(pos.x, pos.y, UISTRING_MASK_SIZE, UISTRING_MASK_SIZE + UISTRING_LINE_SPACING);

		XMINT2 spriteSize = m_Sprite->GetSize();
		float centerXMask = spriteSize.x / 2.0f;
		float centerYMask = spriteSize.y / 2.0f;

		XMINT2 diff = XMINT2(centerXMask - pos.x, centerYMask - pos.y);

		m_Offset = diff;
	}
	void UIText::SetScale(float scale)
	{
		m_Scale = scale;
		m_Sprite->SetScaling(scale);
	}

	UITextManager::UITextManager()
	{
		TextureManager::LoadTexture(TEXTURE_FONT_NAME, TEXTURE_FONT_PATH);
		MaterialManager::CreateMaterial(SHADER_SPRITE_NAME, MATERIAL_FONT_NAME, TEXTURE_FONT_NAME);
		for (int i = 0; i < 26; i++)
		{
			m_MajLetters[i] = XMINT2(i * UISTRING_MASK_SIZE, -UISTRING_LINE_SPACING);
			m_MinLetters[i] = XMINT2(i * UISTRING_MASK_SIZE, (UISTRING_MASK_SIZE + UISTRING_LINE_SPACING) * 2);
			m_NumLetters[i] = XMINT2(i * UISTRING_MASK_SIZE, (UISTRING_MASK_SIZE + UISTRING_LINE_SPACING) * 4);
		}
	}

	XMINT2 UITextManager::GetLetter(wchar_t m_letter)
	{
		if (m_letter >= L'A' && m_letter <= L'Z')
			return m_MajLetters[m_letter - L'A'];
		else if (m_letter >= L'a' && m_letter <= L'z')
			return m_MinLetters[m_letter - L'a'];
		else if (m_letter >= L'0' && m_letter <= L'9')
			return m_NumLetters[m_letter - L'0'];
		else
		{
			switch (m_letter)
			{
			case L'/':
				return m_SpecialChar[0];
			default:
				break;
			}
		}

		return XMINT2(10000, 10000);
	}


	void UIString::SetActive(bool active)
	{
		for(auto letter : m_Letters)
			letter->m_Sprite->SetActive(active);
	}

	void UIString::SetString(std::wstring text)
	{
		Device* dev = Device::Get();

		int sizeDiff = text.size() - m_Letters.size();
		if (text.size() <= m_Letters.size())
		{
			int i = 0;
			for (; i < text.size(); i++)
			{
				if (m_Letters[i]->m_Letter == text[i])
					continue;
				m_Letters[i]->SetLetter(text[i]);
				
			}
			for (; i < m_Letters.size(); i++)
			{
				if (m_Letters[i]->m_Letter == L' ')
					continue;
				m_Letters[i]->SetLetter(L' ');
			}
				
		}
		else if (text.size() > m_Letters.size())
		{
			int i = 0;
			for (; i < m_Letters.size(); i++)
			{
				if (m_Letters[i]->m_Letter == text[i])
					continue;
				m_Letters[i]->SetLetter(text[i]);
			}
			for (; i < text.size(); i++)
			{
				UIText* newText = new UIText(text[i]);
				newText->SetScale(m_Scale);
				m_Letters.push_back(newText);
			}
				
		}

		m_Text = text;

		SetZIndex(m_ZIndex);
		SetPosition(m_Position.x, m_Position.y);
	}
	void UIString::SetPosition(float x, float y)
	{
		float offsetX = 0.0f;
		for (UIText* fs_text : m_Letters)
		{
			fs_text->m_Sprite->SetPosition(x + fs_text->m_Offset.x * fs_text->m_Scale + offsetX, y + fs_text->m_Offset.y * fs_text->m_Scale);
			offsetX += UISTRING_MASK_SIZE * 0.5f * fs_text->m_Scale; // advance by scaled mask width
		}
		m_Position = XMINT2(x, y);
	}
	void UIString::SetZIndex(int z)
	{
		for (UIText* fs_text : m_Letters)
			fs_text->m_Sprite->SetZIndex(z);
		Device::GetRenderer()->SetReorderSprites(true);
		m_ZIndex = z;
	}
	void UIString::SetScale(float scale)
	{
		m_Scale = scale;
		for (UIText* fs_text : m_Letters)
			fs_text->SetScale(m_Scale);
		SetPosition(m_Position.x, m_Position.y);
	}

	void UIString::ClearString()
	{
		int lastIndex = m_Letters.size() - 1;	
		for(int i = lastIndex; i >= 0; i--)
		{
			UIText* fs_text = m_Letters[i];
			Device::Get()->RemoveSprite(fs_text->m_Sprite);
			m_Letters.erase(m_Letters.begin() + i);
			delete(fs_text);
		}	
		m_Letters.clear();
		m_Text = L"";
	}
}