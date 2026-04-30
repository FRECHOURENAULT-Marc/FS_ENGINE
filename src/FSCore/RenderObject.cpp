#include "pch.h"

namespace FSC
{
	void RenderObject::MakeDirty()
	{
		m_IsDirty = true;
	}

	void RenderObject::SetGeo(MeshGeometry* geo)
	{
		m_Geo = geo;
		MakeDirty();
	}
	void RenderObject::SetWorld(XMFLOAT4X4 world)
	{
		m_World = world;
		MakeDirty();
	}
	void RenderObject::SetMaterialIndex(int matIndex)
	{
		m_MaterialIndex = (UINT)matIndex;
		MakeDirty();
	}
}