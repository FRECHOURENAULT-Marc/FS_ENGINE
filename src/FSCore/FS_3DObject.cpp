#include "pch.h"

FS_3DObject::FS_3DObject(MeshGeometry* geo)
{
	mWorld = Maths::Identity4x4();
	mGeo = geo;
}
