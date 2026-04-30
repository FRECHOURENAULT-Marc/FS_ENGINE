#include "pch.h"

namespace FSE
{
	ECS::ECS()
	{
		m_Instance = this;
	}

	ECS::~ECS()
	{
		// TODO : Delete all Scenes
		Device::Destroy();
	}


	void ECS::InitManagers()
	{
		m_ScMgr = SceneManager::Get();
		InputsManager::Create();
		InputsManager::SetHandle(Device::GetWindow()->GetWindowHandle());

		GeometryManager::Get();

		new UITextManager();
	}

	void ECS::UpdateDeltaTime()
	{
		m_Clock.Tick();
		m_DeltaTime = m_Clock.DeltaTime();
		m_Clock.Reset();
		if (m_FpsLock <= 0)
			return;
		double targetFrameTime = (1.0f / m_FpsLock);
		if (m_DeltaTime > targetFrameTime)
			return;

		double timeToSleep = targetFrameTime - m_DeltaTime;
		Sleep(timeToSleep * 1000);
		m_DeltaTime = targetFrameTime;
	}

	void ECS::UpdatePermanentScript()
	{
		int LastIndex = m_PermanentScriptsAdded.size() - 1;
		for (int i = LastIndex; i >= 0; --i)
		{
			Script* script = m_PermanentScriptsAdded[i];
			script->Start();
			m_PermanentScripts.push_back(script);

			m_PermanentScriptsAdded.erase(m_PermanentScriptsAdded.begin() + i);
		}
		for (Script* script : m_PermanentScripts)
			script->Update();
	}

	void ECS::UpdateWindowName(const ECSSettings& settings)
	{
		m_UpdateWindowNameTimer += m_DeltaTime;
		if (m_UpdateWindowNameTimer < 1.0f)
			return;
		m_UpdateWindowNameTimer = 0.0f;
		m_Window->SetWindowName(settings.m_WinName + L" - FPS: " + std::to_wstring((int)(1.0f / m_DeltaTime)) + L" dt : " + std::to_wstring(m_DeltaTime));
	}

	void ECS::AddScript(Script* script)
	{
		m_ScMgr->GetActiveScene<Scene>()->m_ScriptsAdded.push_back(script);
	}

	void ECS::AddPermanentScript(Script* script)
	{
		m_PermanentScriptsAdded.push_back(script);
	}

	ECS& ECS::Create()
	{
		if (m_Instance != nullptr)
			return *m_Instance;

		return *new ECS();
	}
	void ECS::Close()
	{
		delete(m_Instance);
	}

	ECS& ECS::Get()
	{
		if (m_Instance == nullptr)
			Create();

		return *m_Instance;
	}

	ECS* ECS::GetPtr()
	{
		if (m_Instance == nullptr)
			Create();

		return m_Instance;
	}

	void ECS::Run(HINSTANCE hInstance, ECSSettings settings)
	{
		//ENALBLE_D3D12_DEBUG_LAYER
		try
		{
			m_Device = new Device(hInstance);
			if (!m_Device->Initialize())
				return;

			m_Camera = m_Device->GetCamera();
			m_Window = m_Device->GetWindow();
			m_Window->SetDimension(settings.m_WinWidth, settings.m_WinHeight);
		}
		catch (DxException& e)
		{
			MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
			return;
		}
		m_Device->GetRenderer()->SetSpriteRender(settings.m_IsSpriteEnable);
		m_Window->SetWindowName(settings.m_WinName);
		m_IsParticuleEnabled = settings.m_IsParticuleEnable;

		LockFPS(settings.m_FPSLock);
		InitManagers();

		m_Clock.Start();
		m_IsStarted = true;
		float updateWindowNameTimer = 0.0f;
		while (!m_Window->IsClosed())
		{
			UpdateDeltaTime(); // Update value of deltaTime

			InputsManager::HandleInputs(); // Update inputs (Mouse and Keyboard)

			debug::Update(m_DeltaTime); // Clear debug objects/UI

			UpdatePermanentScript(); // Update permanent scripts

			m_ScMgr->Update(m_DeltaTime); // Update Scene (Scripts and Systems)
			
			InputsManager::ClearInputs(); // Clear MouseWheel delta for this frame

			m_Device->GetRenderer()->UpdateRender(m_DeltaTime); // Update window messages(ex:MouseWheel/resize), Render and draw	

			if (settings.m_ShowFPSInWindowName == false)
				continue;
			UpdateWindowName(settings);
		}

		Close();
	}

	void ECS::LockFPS(int fps)
	{
		m_FpsLock = fps;
	}

	Entity* ECS::GetEntity(int entityID)
	{
		return &m_ScMgr->GetActiveScene<Scene>()->m_Entities->Get(entityID);
	}

	int ECS::CreateEntity()
	{
		Scene* sc = m_ScMgr->GetActiveScene<Scene>();

		int freeIndex = -1;
		if (sc->m_EntitiesFree.size() != 0)
		{
			freeIndex = sc->m_EntitiesFree.back();
			sc->m_EntitiesFree.pop_back();
		}
		else
		{
			D::Cout("ECS::CreateEntity : No more free entity, increase capacity before launching or remove entities.");
			return -1;
		}

		Entity& e = sc->m_Entities->Get(freeIndex);
		e.Reset();
		e.SetActive(true);
		return e.GetID();
	}

	void ECS::RemoveEntity(int entityID)
	{
		if (GetEntity(entityID)->IsActive() == false)
		{
			D::Cout("ECS::RemoveEntity : Entity " + std::to_string(entityID) + " is already inactive");
			return;
		}

		Scene* sc = m_ScMgr->GetActiveScene<Scene>();

		sc->m_EntitiesFree.push_back(entityID);
		auto compos = GetAllFromEntity(entityID);
		sc->m_Entities->Get(entityID).SetActive(false);
		for (Component* c : compos)
		{
			if (c == nullptr)
				continue;
			c->SetActive(false);
			c->Reset();
		}
	}

	std::vector<ColliderComponent*>& ECS::GetLayer(std::string _name)
	{
		return m_ScMgr->GetActiveScene<Scene>()->m_Layers[_name];
	}

	std::map<std::string, std::vector<ColliderComponent*>>& ECS::GetLayersMap()
	{
		return m_ScMgr->GetActiveScene<Scene>()->m_Layers;
	}

	std::vector<Component*> ECS::GetAllFromEntity(int entityID)
	{
		std::vector<Component*> compos;
		compos.push_back(GetComponent<TransformComponent>(entityID));
		compos.push_back(GetComponent<MeshComponent>(entityID));
		compos.push_back(GetComponent<VelocityComponent>(entityID));
		compos.push_back(GetComponent<StateMachineComponent>(entityID));
		compos.push_back(GetComponent<EmitterComponent>(entityID));
		compos.push_back(GetComponent<CameraComponent>(entityID));
		compos.push_back(GetComponent<ColliderComponent>(entityID));
		compos.push_back(GetComponent<LightComponent>(entityID));
		compos.push_back(GetComponent<SpriteComponent>(entityID));
		return compos;
	}
}



