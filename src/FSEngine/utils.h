#pragma once

namespace utils
{
	XMINT2 WorldToScreen(float x, float y, float z);
	XMINT2 WorldToScreen(XMFLOAT3 pos);
	XMFLOAT3 ScreenToWorld(int x, int y);
	XMFLOAT3 ScreenToWorld(XMINT2 pos);
}