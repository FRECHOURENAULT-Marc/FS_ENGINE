#include "pch.h"

namespace FSE
{
	void Scene::Init()
	{
		m_ECS = ECS::GetPtr();
		//Components
		m_Entities = m_ECS->InitObjectBuffer<Entity>();

		m_Transforms = m_ECS->InitObjectBuffer<TransformComponent>();
		m_Meshs = m_ECS->InitObjectBuffer<MeshComponent>();
		m_Velocities = m_ECS->InitObjectBuffer<VelocityComponent>();
		m_StateMachines = m_ECS->InitObjectBuffer<StateMachineComponent>();
		m_Emitters = m_ECS->InitObjectBuffer<EmitterComponent>();
		m_Cameras = m_ECS->InitObjectBuffer<CameraComponent>();
		m_Colliders = m_ECS->InitObjectBuffer<ColliderComponent>();
		m_Lights = m_ECS->InitObjectBuffer<LightComponent>();
		m_Sprites = m_ECS->InitObjectBuffer<SpriteComponent>();

		CreateLayer(BASE_COLLIDERS_LAYER);
		CreateLayer(GROUND_LAYER);
		CreateLayer(COMBAT_COLLIDERS_LAYER);

		for (int i = 0; i < m_Entities->Capacity(); i++)
			m_EntitiesFree.push_back(i);

		//Systems
		m_Systems.push_back(new MoveSystem());
		m_Systems.push_back(new UpdateMeshWorldSystem());
		m_Systems.push_back(new StateMachineSystem());
		m_Systems.push_back(new EmitterSystem());
		m_Systems.push_back(new CameraSystem());
		m_Systems.push_back(new ColliderSystem());
		m_Systems.push_back(new LightSystem());
		m_Systems.push_back(new TransformSystem());
		m_Systems.push_back(new SpriteSystem());
	}
	void Scene::Quit()
	{
		SetSceneActive(false);
		OnQuit();
	}
	void Scene::Load() 
	{
		if(m_IsInit == false)
		{
			OnInit();
			m_IsInit = true;
		}

		SetSceneActive(true);
		Device::GetRenderer()->ForceUpdate();
		OnLoad();
	}
	void Scene::Update(float dt) 
	{
		OnUpdate();

		//Scripts
		int LastIndex = m_ScriptsAdded.size() - 1;
		for (int i = LastIndex; i >= 0; --i)
		{
			Script* script = m_ScriptsAdded[i];
			script->Start();
			m_Scripts.push_back(script);

			m_ScriptsAdded.erase(m_ScriptsAdded.begin() + i);
		}
		for (Script* script : m_Scripts)
			script->Update();

		//Systems
		for (System* sys : m_Systems)
			sys->Update(dt);
		for (System* sys : m_Systems)
			sys->AfterUpdate(dt);

	}

	void Scene::SetSceneActive(bool isActive)
	{
		if(isActive) //Update Camera
		{
			auto* transforms = m_ECS->GetAllFromType<TransformComponent>();
			for (int i = 0; i < MAX_COMPONENTS; i++)
			{
				TransformComponent& trs = transforms[i];
				if (trs.IsActive() == false)
					continue;
				trs.SetUpdatedThisFrame(true);
			}
		}

		auto* meshs = m_ECS->GetAllFromType<MeshComponent>();
		for(int i = 0; i < MAX_COMPONENTS; i++)
		{
			MeshComponent& mesh = meshs[i];
			if (mesh.IsActive() == false)
				continue;
			Object3D* obj = mesh.Get3DObject();
			if (obj == nullptr)
				continue;
			obj->SetActive(isActive);
		}
		auto* sprites = m_ECS->GetAllFromType<SpriteComponent>();
		for(int i = 0; i < MAX_COMPONENTS; i++)
		{
			SpriteComponent& sprite = sprites[i];
			if (sprite.IsActive() == false)
				continue;
			Object2D* spr = sprite.GetSprite();
			UIString* text = sprite.GetText();

			if (spr != nullptr)
				spr->SetActive(isActive);
			if (text != nullptr)
				text->SetActive(isActive);
		}
		if(isActive == false)
		{
			auto* emitters = m_ECS->GetAllFromType<EmitterComponent>();
			for (int i = 0; i < MAX_COMPONENTS; i++)
			{
				EmitterComponent& emitter = emitters[i];
				if (emitter.IsActive() == false)
					continue;
				emitter.HideAllParticules();
			}
		}
	}

	void Scene::CreateLayer(std::string _name)
	{
		m_Layers[_name] = std::vector<ColliderComponent*>();
	}

	void Scene::AddToLayer(ColliderComponent* _collider, std::string _layerName)
	{
		std::vector<ColliderComponent*>& base = m_Layers[BASE_COLLIDERS_LAYER];

		for (int i = 0; i < base.size(); i++)
		{
			ColliderComponent* collider = base[i];

			if (collider == _collider)
			{
				base.erase(base.begin() + i);
				break;
			}
		}

		std::vector<ColliderComponent*>& goodLayer = m_Layers[_layerName];

		for (int i = 0; i < goodLayer.size(); i++)
		{
			ColliderComponent* collider = goodLayer[i];

			if (collider == _collider)
			{
				return;
			}
		}

		goodLayer.push_back(_collider);
	}

	void Scene::RemoveFromLayer(ColliderComponent* _collider)
	{
		for(auto layer : m_Layers)
		{
			std::vector<ColliderComponent*>& colliders = layer.second;
			for (auto it = colliders.begin(); it != colliders.end(); ++it)
			{
				ColliderComponent* colldier = *it;
				if (colldier != _collider)
					continue;

				colliders.erase(it);
				break;
			}
		}
	}

	void SceneManager::InitNewScenes()
	{
		int oldIndex = m_CurrentSceneIndex;
		for (int i = 0; i < m_newScenes.size(); i++)
		{
			m_CurrentSceneIndex = m_Scenes.size();
			Scene* scene = m_newScenes[i];
			m_Scenes.push_back(scene);
			scene->Init();
		}
		m_newScenes.clear();
		m_CurrentSceneIndex = oldIndex;
	}

	void SceneManager::Update(float dt)
	{
		if (m_newScenes.size() > 0)
			InitNewScenes();

		if(m_CurrentSceneIndex >= 0)
			m_Scenes[m_CurrentSceneIndex]->Update(dt);

		if (m_nextSceneIndex == -1 || m_nextSceneIndex == m_CurrentSceneIndex)
			return;

		if (m_CurrentSceneIndex != -1)
			m_Scenes[m_CurrentSceneIndex]->Quit();

		m_CurrentSceneIndex = m_nextSceneIndex;
		m_Scenes[m_CurrentSceneIndex]->Load();
		m_nextSceneIndex = -1;
	}


	SceneManager* SceneManager::Get()
	{
		if (m_Instance == nullptr)
			m_Instance = new SceneManager();
		return m_Instance;
	}
	void SceneManager::GoOnScene(std::string name)
	{
		for (int i = 0; i < m_Scenes.size(); i++)
		{
			Scene* scene = m_Scenes[i];
			if (scene->name != name)
				continue;
			m_nextSceneIndex = i;
			return;
		}
	}

	void SceneManager::GoOnScene(int index)
	{
		m_nextSceneIndex = index;
	}

}

