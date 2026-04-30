#include "pch.h"

namespace FSC
{

	Object3D::Object3D(MeshGeometry* geo)
	{
		m_World = Maths::Identity4x4();
		m_Geo = geo;
		MakeDirty();
	}
	void Object3D::SetColor(XMFLOAT4 color)
	{
		m_Color = color;
		MakeDirty();
	}

}