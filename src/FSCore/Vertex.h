#pragma once

struct Vertex
{
	XMFLOAT3 Pos;								// 12 bytes
	XMFLOAT2 UV;								// 8 bytes
	XMFLOAT3 Normal = { 0.0f, 0.0f, 0.0f };		// 12 bytes
	XMFLOAT3 Tangent = { 0.0f, 0.0f, 0.0f };	// 12 bytes
};