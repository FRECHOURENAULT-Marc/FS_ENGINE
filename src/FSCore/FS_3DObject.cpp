#include "pch.h"

FS_3DObject::FS_3DObject(MeshGeometry* geo)
{
	mWorld = Maths::Identity4x4();
	mGeo = geo;
}
void FS_3DObject::SetColor(XMFLOAT4 color)
{
	mColor = color;
	m_isDirty = true;
}
void FS_3DObject::SetWorld(XMFLOAT4X4 world)
{
	mWorld = world;
	m_isDirty = true;
}

void FS_3DObject::SetMaterialIndex(int index)
{
	mMaterialIndex = index;
	m_isDirty = true;
}
