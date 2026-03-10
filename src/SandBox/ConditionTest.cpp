#include "pch.h"

bool ConditionTest_SpacePressed::OnTest(Entity* pOwner)
{
	if(FS_InputsManager::Get()->GetKeyDown(Keyboard::SPACE))
	{
		return true;
	}

	return false;
}

bool ConditionTest_EnterPressed::OnTest(Entity* pOwner)
{
	if (FS_InputsManager::Get()->GetKeyDown(Keyboard::RETURN))
	{
		return true;
	}

	return false;
}
