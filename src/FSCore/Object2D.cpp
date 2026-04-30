#include "pch.h"

XMINT2 WorldToScreen(XMFLOAT3 pos, XMINT2 winSize)
{
	return XMINT2(pos.x + (winSize.x / 2.0f), pos.y + (winSize.y / 2.0f));
}
XMFLOAT3 ScreenToWorld(XMINT2 pos, XMINT2 winSize)
{
	return XMFLOAT3(pos.x - (winSize.x / 2.0f), pos.y - (winSize.y / 2.0f), 1.0f);
}

XMINT2 UVToScreenLocal(XMFLOAT2 uv, XMINT2 size)
{
	float x = uv.x * size.x;
	float y = uv.y * size.y;

	return XMINT2(x, y);
}
XMFLOAT2 LocalScreenToUV(XMINT2 pos, XMINT2 size)
{
	float x = std::clamp((float)pos.x / size.x, 0.0f, 1.0f);
	float y = std::clamp((float)pos.y / size.y, 0.0f, 1.0f);

	return XMFLOAT2(x, y);
}

namespace FSC
{

	void Object2D::SetPosition(float x, float y, float z)
	{
		XMFLOAT3 pos = ScreenToWorld({ (int)x, (int)y }, m_WinSize);

		m_World.m[3][0] = pos.x;
		m_World.m[3][1] = pos.y;
		m_World.m[3][2] = z;
		MakeDirty();
	}

	void FSC::Object2D::SetSize(int x, int y)
	{
		m_Size = XMINT2(x, y);
		MakeDirty();
	}

	void Object2D::SetWinSize(int x, int y)
	{
		m_WinSize = XMINT2(x, y);;
		MakeDirty();
	}

	void Object2D::SetScaling(float scale)
	{
		//Revenir à la scale d'origine
		XMMATRIX worldMatrix = XMLoadFloat4x4(&m_World);
		XMMATRIX scaleMatrix1 = XMMatrixScaling(1 / m_Scale.x, 1 / m_Scale.y, 1 / 1.0f);
		worldMatrix = scaleMatrix1 * worldMatrix;
		//Appliquer la nouvelle scale
		m_Scale.x = scale * m_Size.x;
		m_Scale.y = scale * m_Size.y;
		XMMATRIX scaleMatrix2 = XMMatrixScaling(m_Scale.x, m_Scale.y, 1.0f);
		worldMatrix = scaleMatrix2 * worldMatrix;
		XMStoreFloat4x4(&m_World, worldMatrix);
		MakeDirty();
	}

	void Object2D::SetMask(int x, int y, int width, int height)
	{
		// /!\ Parameters are in pixels
		// /!\ Results are in UVs (0..1)

		// Guard against zero texture size
		int spriteW = (m_Size.x > 0 ? m_Size.x : 1);
		int spriteH = (m_Size.y > 0 ? m_Size.y : 1);

		// Convert pixels coordinates to UV (0..1)
		XMFLOAT2 uv = LocalScreenToUV({x, y}, { spriteW , spriteH });

		float nWidth = ((float)width) / spriteW;
		float nHeight = ((float)height) / spriteH;

		// Clamp to [0,1] to avoid invalid UVs
		nWidth = std::clamp(nWidth, 0.0f, 1.0f - uv.x);
		nHeight = std::clamp(nHeight, 0.0f, 1.0f - uv.y);

		m_Mask = { uv.x, uv.y, nWidth, nHeight };
		MakeDirty();
	}
	void Object2D::SetZIndex(int z)
	{
		m_ZIndex = z;
		MakeDirty();
	}
}