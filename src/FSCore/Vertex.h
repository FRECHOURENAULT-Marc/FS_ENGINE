#pragma once

namespace FSC
{

	struct Vertex
	{
		XMFLOAT3 m_Pos;								// 12 bytes
		XMFLOAT2 m_UV;								// 8 bytes
		XMFLOAT3 m_Normal = { 0.0f, 0.0f, 0.0f };	// 12 bytes
		XMFLOAT3 m_Tangent = { 0.0f, 0.0f, 0.0f };	// 12 bytes
	};

}