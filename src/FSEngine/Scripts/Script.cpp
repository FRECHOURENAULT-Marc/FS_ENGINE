#include "pch.h"

namespace FSE
{

	Script::Script()
	{
		m_Ecs = &ECS::Get();
	}

	void Script::Update()
	{
		OnUpdate();
	}
	void Script::Start()
	{
		OnStart();
		m_IsInit = true;
	}

}
