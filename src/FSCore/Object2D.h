#pragma once

XMINT2 WorldToScreen(XMFLOAT3 pos, XMINT2 winSize);
XMFLOAT3 ScreenToWorld(XMINT2 pos, XMINT2 winSize);

XMINT2 UVToScreenLocal(XMFLOAT2 uv, XMINT2 size);
XMFLOAT2 LocalScreenToUV(XMINT2 pos, XMINT2 size);

namespace FSC
{

	class Object2D : public RenderObject
	{
	protected:

		XMFLOAT4 m_Mask = { 0.0f, 0.0f, 1000000.0f, 1000000.0f };

		XMINT2 m_WinSize = XMINT2(1, 1);
		XMINT2 m_Size = XMINT2(1, 1);

		XMFLOAT2 m_Scale = { 1.0f, 1.0f };

		int m_ZIndex = 0;

	public:
		Object2D() = default;
		~Object2D() {};

		XMFLOAT4 GetMask() { return m_Mask; }
		XMINT2 GetSize() { return m_Size; }
		XMFLOAT2 GetScale() { return m_Scale; }
		int GetZIndex() { return m_ZIndex; }

		// Set the position of the sprite on the screen (in pixels)
		void SetPosition(float x, float y, float z = 1.0f);
		void SetSize(int x, int y);
		void SetWinSize(int x, int y);
		// Set the scaling of the sprite
		void SetScaling(float scale);
		// Set the mask for the sprite (in pixels)
		// (x,y) is the top-left corner of the mask
		void SetMask(int x, int y, int width, int height);
		void SetZIndex(int z);

		//void UpdateWorld();
	};

}