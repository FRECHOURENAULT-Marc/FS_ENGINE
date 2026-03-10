#include "pch.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,PSTR cmdLine, int showCmd)
{
	DEBUG_MEMORY_START_RECORD
		DEBUG_ALLOC_CONSOLE

		ECSSettings settings;
	settings.winHeight = 900;
	settings.winWidth = 1200;
	ECS& ecs = ECS::Create();

	ecs.CreateScript<GameManager>();

	//ecs.LockFPS(60);
	ecs.Run(hInstance, settings);

	DEBUG_MEMORY_END_RECORD
}

