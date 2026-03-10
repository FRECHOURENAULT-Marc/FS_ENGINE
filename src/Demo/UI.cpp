#include "pch.h"

UI::UI()
{
	m_ecs = &ECS::Get();
}

void UI::Update()
{
	m_timer += m_ecs->GetDeltaTime();
	if (m_timer < m_time)
		return;
	m_timer = 0.0f;

	OnUpdate();
};