#include "pch.h"

UIString::UIString() : UI()
{
	m_string = FS_UIString();
	m_string.SetString(L"empty");
}

void UIString::SetString(std::wstring str)
{
	m_string.SetString(str);
}