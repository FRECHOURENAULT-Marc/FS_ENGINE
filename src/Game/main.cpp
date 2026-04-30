#include "pch.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,PSTR cmdLine, int showCmd)
{
	ECS& ecs = ECS::Create();

	SceneManager::Get()->CreateScene<Scene>();

	ECSSettings settings;
	settings.m_WinHeight = 900;
	settings.m_WinWidth = 1200;
	settings.m_ShowFPSInWindowName = false;
	settings.m_WinName = L"Game";
	
	ecs.Run(hInstance, settings);
}

