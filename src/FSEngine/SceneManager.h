#pragma once

#define MAX_COMPONENTS 4096
#define BUFFERC_IN_USE BufferC4096

#define BASE_COLLIDERS_LAYER "BaseColliders"
#define COMBAT_COLLIDERS_LAYER "CombatColliders"
#define GROUND_LAYER "GroundCollider"
#define PLACEMENT_COLLIDERS_LAYER "PlacementColliders"
#define SELECTION_LAYER "SelectionColliders"


namespace FSE
{
	class Scene
	{
	private:
		bool m_IsInit = false;

		void Init();
		void Load();
		void Quit();
		void Update(float dt);

		void SetSceneActive(bool isActive);

	protected:
		std::vector<int> m_EntitiesFree = std::vector<int>();

		BUFFERC_IN_USE<Entity>* m_Entities;

		BUFFERC_IN_USE<TransformComponent>* m_Transforms;
		BUFFERC_IN_USE<MeshComponent>* m_Meshs;
		BUFFERC_IN_USE<VelocityComponent>* m_Velocities;
		BUFFERC_IN_USE<StateMachineComponent>* m_StateMachines;
		BUFFERC_IN_USE<EmitterComponent>* m_Emitters;
		BUFFERC_IN_USE<CameraComponent>* m_Cameras;
		BUFFERC_IN_USE<ColliderComponent>* m_Colliders;
		BUFFERC_IN_USE<LightComponent>* m_Lights;
		BUFFERC_IN_USE<SpriteComponent>* m_Sprites;

		std::map<std::string, std::vector<ColliderComponent*>> m_Layers;

		std::vector<void*> m_ComponentArrays = std::vector<void*>();

		std::vector<System*> m_Systems = std::vector<System*>();

		std::vector<Script*> m_Scripts = std::vector<Script*>();
		std::vector<Script*> m_ScriptsAdded = std::vector<Script*>();

		ECS* m_ECS = nullptr;

		virtual void OnInit() {};
		virtual void OnLoad() {};
		virtual void OnQuit() {};
		virtual void OnUpdate() {};

	public:
		std::string name;

		void CreateLayer(std::string _name);
		void AddToLayer(ColliderComponent* _collider, std::string _layerName);
		void RemoveFromLayer(ColliderComponent* _collider);

		friend class SceneManager;
		friend class ECS;
	};

	class SceneManager
	{
	protected:

		std::vector<Scene*> m_Scenes = std::vector<Scene*>();
		std::vector<Scene*> m_newScenes = std::vector<Scene*>();

		static inline SceneManager* m_Instance = nullptr;

		int m_CurrentSceneIndex = -1;
		int m_nextSceneIndex = 0;

		void InitNewScenes();

		void Update(float dt);
	public:

		static SceneManager* Get();
		template<typename S>
		inline S* GetActiveScene() { return (S*)m_Scenes[m_CurrentSceneIndex]; }
		int GetActiveSceneIndex() { return m_CurrentSceneIndex; }

		template<typename S>
		S* CreateScene();
		void GoOnScene(std::string name);
		void GoOnScene(int index);

		friend class ECS;
	};

}

#include "SceneManager.inl"