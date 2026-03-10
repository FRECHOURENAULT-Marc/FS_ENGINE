#include "pch.h"

UIFPS::UIFPS() : UIString()
{
	m_string.SetString(L"0 FPS");
}

void UIFPS::OnUpdate()
{
	int fps = m_ecs->GetFPS();
	m_string.SetString(std::to_wstring(fps)+ L" FPS");
}