#include "pch.h"

namespace utils
{
	XMINT2 WorldToScreen(float x, float y, float z)
	{ 
		return WorldToScreen({ x, y, z }); 
	}
	XMINT2 WorldToScreen(XMFLOAT3 pos)
	{
		XMINT2 winSize = Device::GetWindow()->Size();
		return XMINT2(pos.x + (winSize.x / 2.0f), pos.y + (winSize.y / 2.0f));
	}
	XMFLOAT3 ScreenToWorld(int x, int y)
	{ 
		return ScreenToWorld({ x, y });
	}
	XMFLOAT3 ScreenToWorld(XMINT2 pos)
	{
		XMINT2 winSize = Device::GetWindow()->Size();
		return XMFLOAT3(pos.x - (winSize.x / 2.0f), pos.y - (winSize.y / 2.0f), 1.0f);
	}
}