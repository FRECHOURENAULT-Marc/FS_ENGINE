#include "pch.h"

#ifdef _DEBUG

debug::debug()
{
	for (int i = 0; i < 10; i++)
	{	
		UIDebug* uiD = new UIDebug();
		uiD->m_IsActive = false;
		uiD->m_Str = new UIString();
		uis.push_back(uiD);
	}
		
}

debug* debug::Get()
{
	if (instance == nullptr)
		instance = new debug();
	return instance;
}

void debug::Draw(XMFLOAT3 scale, XMFLOAT3 pos, GEO_TYPE geo)
{
	Object3D* obj = new Object3D(GeometryManager::GetGeometry(geo));
	auto world = obj->GetWorld();
	world.m[0][0] = scale.x;
	world.m[1][1] = scale.y;
	world.m[2][2] = scale.z;
	world.m[3][0] = pos.x;
	world.m[3][1] = pos.y;
	world.m[3][2] = pos.z;
	obj->SetWorld(world);
	obj->SetColor({ 1, 0, 0, 1 });

	Device::Get()->AddObject(obj);
	objects.push_back(obj);
}

void debug::DrawUI(std::string text, XMINT2 pos)
{
	for (int i = 0; i < uis.size(); i++)
	{
		UIDebug* uiD = uis[i];
		if (uiD->m_IsActive)
			continue;

		uiD->m_IsActive = true;
		uiD->m_Str->SetActive(uiD->m_IsActive);
		uiD->m_Str->SetString(std::wstring(text.begin(), text.end()));
		uiD->m_Str->SetPosition(pos.x, pos.y);
	}
}

void debug::Update(float dt)
{
	Get()->ClearObjects();
}

void debug::ClearObjects()
{
	int objectsLastIndex = objects.size() - 1;
	for (int i = objectsLastIndex; i >= 0; i--)
	{
		auto obj = objects[i];
		Device::Get()->RemoveObject(obj);
	}
	objects.clear();
	int uisLastIndex = uis.size() - 1;
	for (int i = uisLastIndex; i >= 0; i--)
	{
		auto ui = uis[i];
		ui->m_IsActive = false;
		ui->m_Str->SetActive(ui->m_IsActive);
	}	
}

#endif