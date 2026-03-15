#include "pch.h"

#include <thread>
#include <chrono>

ECS::ECS()
{
	m_pInstance = this;

	m_entities = new ObjectBuffer<Entity>(ENTITES_MAX);

	m_transforms = InitComponentBuffer<TransformComponent>();
	m_meshs = InitComponentBuffer<MeshComponent>();
	m_velocities = InitComponentBuffer<VelocityComponent>();
	m_stateMachines = InitComponentBuffer<StateMachineComponent>();
	m_emitters = InitComponentBuffer<EmitterComponent>();
	m_cameras = InitComponentBuffer<CameraComponent>();
	m_colliders = InitComponentBuffer<ColliderComponent>();
	m_lights = InitComponentBuffer<LightComponent>();

	for (int i = 0; i < m_entities->Capacity(); i++)
		m_entitiesFree.push_back(i);

	CreateSystems();
}

ECS::~ECS()
{
	for(auto components : m_componentArrays)
		delete(components);
	m_componentArrays.clear();
	for (auto system : m_systems)
		delete(system);
	m_systems.clear();
	for (auto script : m_scripts)
		delete(script);
	m_scripts.clear();

	FS_Device::Destroy();
}


void ECS::IncreaseVectorsCapacity()
{
	int oldCapacity = m_entities->Capacity();
	m_entities->IncreaseSize();

	m_transforms->IncreaseSize(); 
	m_meshs->IncreaseSize(); 
	m_velocities->IncreaseSize();
	m_stateMachines->IncreaseSize();
	m_emitters->IncreaseSize();
	m_cameras->IncreaseSize();
	m_colliders->IncreaseSize();
	m_lights->IncreaseSize();

	for (int i = oldCapacity; i < m_entities->Capacity(); i++)
		m_entitiesFree.push_back(i);
}

void ECS::CallScriptsOnStart()
{
	int LastIndex = m_scriptsAdded.size() - 1;
	for (int i = LastIndex; i >= 0 ; --i)
	{
		Script* script = m_scriptsAdded[i];
		script->OnStart();
		m_scripts.push_back(script);

		m_scriptsAdded.erase(m_scriptsAdded.begin() + i);
	}
}

void ECS::CreateSystems()
{
	m_systems.push_back(new MoveSystem());
	m_systems.push_back(new UpdateMeshWorldSystem());
	m_systems.push_back(new StateMachineSystem());
	m_systems.push_back(new EmitterSystem());
	m_systems.push_back(new CameraSystem());
	m_systems.push_back(new ColliderSystem());
	m_systems.push_back(new LightSystem());
}

void ECS::CreateManagers()
{
	FS_InputsManager::Get();

	FS_InputsManager::s_windowHandle = FS_Device::Get()->m_FSWindow->GetWindowHandle();

	GeometryManager::Get();

	new FS_TextManager();
}

void ECS::UpdateDeltaTime() 
{
	m_clock.Tick();
	m_deltaTime = m_clock.DeltaTime();
	m_clock.Reset();
	if (m_fpsLock <= 0)
		return;
	float targetFrameTime = (1.0f / m_fpsLock);
	if (m_deltaTime > targetFrameTime)
		return;

	float timeToSleep = targetFrameTime - m_deltaTime;
	Sleep(timeToSleep * 1000);
	m_deltaTime = targetFrameTime;
}

void ECS::UpdateSystems()
{
	for (System* sys : m_systems)
		sys->Update(m_deltaTime);
}

void ECS::UpdateScripts()
{
	CallScriptsOnStart();
	for (Script* script : m_scripts) 
		script->OnUpdate();	
}

ECS& ECS::Create()
{
	if (m_pInstance != nullptr)
		return *m_pInstance;
		
	return *new ECS();
}
void ECS::Close()
{
	delete(m_pInstance);
}

ECS& ECS::Get()
{
	if (m_pInstance == nullptr)
		Create();

	return *m_pInstance;
}

ECS* ECS::GetPtr()
{
	if (m_pInstance == nullptr)
		Create();

	return m_pInstance;
}

void ECS::Run(HINSTANCE hInstance, ECSSettings settings)
{
	ENALBLE_D3D12_DEBUG_LAYER
	try
	{
		m_device = new FS_Device(hInstance);
		if (!m_device->Initialize())
			return;

		m_camera = m_device->Camera();
		m_window = m_device->Window();
		m_window->SetDimension(settings.winWidth, settings.winHeight);
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return;
	}

	m_particuleEnabled = settings.particuleEnable;
	m_device->Renderer()->SetSpriteRender(settings.spriteEnable);

	CreateManagers();

	m_clock.Start();
	float updateWindowNameTimer = 0.0f;
	while (!m_window->IsClosed())
	{
		UpdateDeltaTime();

		FS_InputsManager::HandleInputs();

		UpdateScripts();
		UpdateSystems();
		m_device->Renderer()->UpdateRender(m_deltaTime);//Update Render
		debug::Update(m_deltaTime);

		updateWindowNameTimer += m_deltaTime;
		if (updateWindowNameTimer < 1.0f)
			continue;
		updateWindowNameTimer = 0.0f;
		m_window->SetWindowName(L"** FS-ENGINE ** FPS: " + std::to_wstring((int)(1.0f / m_deltaTime)) + L" dt : " + std::to_wstring(m_deltaTime));
	}

	Close();
}

void ECS::LockFPS(int fps)
{
	m_fpsLock = fps;
}

Entity* ECS::GetEntity(int entityID)
{
	return m_entities->Get(entityID);
}

int ECS::CreateEntity()
{
	int freeIndex = 0;
	if (m_entitiesFree.size() != 0) 
	{
		freeIndex = m_entitiesFree.back();
		m_entitiesFree.pop_back();
	}
	else
	{
		freeIndex = m_entities->Capacity();
		IncreaseVectorsCapacity();
	}	

	Entity* e = m_entities->Get(freeIndex);
	e->Reset();
	e->SetActive(true);
	return e->GetID();
}

void ECS::RemoveEntity(int entityID)
{
	if (GetEntity(entityID)->IsActive() == false)
	{
		std::cout << "ECS::RemoveEntity : Entity " << entityID << " is already inactive" << std::endl;
		return;
	}
	
	m_entitiesFree.push_back(entityID);

	auto compos = GetAllFromEntity(entityID);

	m_entities->Get(entityID)->SetActive(false);
	for (Component* c : compos) 
	{
		if(c == nullptr)
			continue;
		c->SetActive(false);
		c->Reset();
	}
}

std::vector<Component*> ECS::GetAllFromEntity(int entityID)
{
	std::vector<Component*> compos;
	compos.push_back(GetComponent<TransformComponent>(entityID));
	compos.push_back(GetComponent<MeshComponent>(entityID));
	return compos;
}

