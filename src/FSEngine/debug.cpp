#include "pch.h"

#ifdef _DEBUG

debug* debug::Get() 
{
	if (instance == nullptr)
		instance = new debug();
	return instance;
}

void debug::Draw(float scale, XMFLOAT3 pos, GEO_TYPE geo)
{
	FS_3DObject* obj = new FS_3DObject(GeometryManager::GetGeometry(geo));
	auto world = obj->GetWorld();
	world.m[0][0] = scale;
	world.m[1][1] = scale;
	world.m[2][2] = scale;
	world.m[3][0] = pos.x;
	world.m[3][1] = pos.y;
	world.m[3][2] = pos.z;
	obj->SetWorld(world);
	obj->SetColor({ 1, 0, 0, 1 });

	FS_Device::Get()->AddObject(obj);
	objects.push_back(obj);
}

void debug::ClearObjects()
{
	int objectsLastIndex = objects.size() - 1;
	for (int i = objectsLastIndex; i >= 0; i--)
	{
		auto obj = objects[i];
		FS_Device::Get()->RemoveObject(obj);
	}	
	objects.clear();
}

#endif