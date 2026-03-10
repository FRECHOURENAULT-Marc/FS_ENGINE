#include "pch.h"
#include <algorithm>

FS_Sprite::FS_Sprite()
{
	m_transform = Transform();
}

void FS_Sprite::SetPosition(float x, float y)
{
	float nx = x - (winSize.x /2.0f);
	float ny = -y + (winSize.y /2.0f);
	m_transform.SetPosition(nx, ny, 1.0f);
}

void FS_Sprite::SetScaling(float scale)
{
	float nScale = scale * mSize.x;
	m_transform.SetScaling(nScale);
}

void FS_Sprite::SetMask(float x, float y, float width, float height)
{
	// /!\ Parameters are in pixels
	// /!\ Results are in UVs (0..1)

	// Guard against zero texture size
	float spriteW = (mSize.x > 0 ? mSize.x : 1);
	float spriteH = (mSize.y > 0 ? mSize.y : 1);

	float left = x;
	float top = y;

	// Convert pixels coordinates to UV (0..1)
	float nx = left / spriteW;
	float ny = top / spriteH;
	float nWidth = width / spriteW;
	float nHeight = height / spriteH;

	// Clamp to [0,1] to avoid invalid UVs
	nx = std::clamp(nx,0.0f,1.0f);
	ny = std::clamp(ny,0.0f,1.0f);
	nWidth = std::clamp(nWidth,0.0f,1.0f - nx);
	nHeight = std::clamp(nHeight,0.0f,1.0f - ny);

	mMask = { nx, ny, nWidth, nHeight };
}