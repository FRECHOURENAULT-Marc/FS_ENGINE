#pragma once

#define ENTITES_MAX 128

//Settings send to ECS when Run method is call, can be completed
struct ECSSettings
{
	int winWidth = 800;
	int winHeight = 600;
	bool spriteEnable = true;
	bool particuleEnable = true;
};

class ECS
{
	FS_Device* m_device = nullptr;
	FS_Window* m_window = nullptr;
	FS_3DCamera* m_camera = nullptr;
	

	ObjectBuffer<Entity>* m_entities = nullptr;
	std::vector<int> m_entitiesFree = std::vector<int>();

	ObjectBuffer<TransformComponent>* m_transforms = nullptr;
	ObjectBuffer<MeshComponent>* m_meshs = nullptr;
	ObjectBuffer<VelocityComponent>* m_velocities = nullptr;
	ObjectBuffer<StateMachineComponent>* m_stateMachines = nullptr;
	ObjectBuffer<EmitterComponent>* m_emitters = nullptr;
	ObjectBuffer<CameraComponent>* m_cameras = nullptr;
	ObjectBuffer<ColliderComponent>* m_colliders = nullptr;
	ObjectBuffer<LightComponent>* m_lights = nullptr;

	std::vector<void*> m_componentArrays = std::vector<void*>();

	std::vector<System*> m_systems = std::vector<System*>();

	std::vector<Script*> m_scripts = std::vector<Script*>();
	std::vector<Script*> m_scriptsAdded = std::vector<Script*>();

	static inline ECS* m_pInstance = nullptr;

	FS_Clock m_clock = FS_Clock();
	float m_deltaTime = 0.0f;
	int m_fpsLock = 0;

	bool m_particuleEnabled = true;

	ECS();
	~ECS();

	template<typename O>
	ObjectBuffer<O>* InitComponentBuffer();

	void CallScriptsOnStart();
	void CreateSystems();
	void CreateManagers();
	void IncreaseVectorsCapacity();

	void UpdateDeltaTime();
	void UpdateSystems();
	void UpdateScripts();

	// Add a script to the ECS
	// It will be added to a temporary vector and moved to active scripts at the Run of ECS (for calling OnStart())
	void AddScript(Script* script) { m_scriptsAdded.push_back(script); };

	friend class Script;

public:

	// Create the ECS instance (no render init) if it doesnt exist and return it, else return the existing one
	static ECS& Create();
	static void Close();

	// Get the ECS instance, if it doesnt exist create it and return it, else return the existing one
	static ECS& Get();
	static ECS* GetPtr();
	static float GetDeltaTime() { return ECS::Get().m_deltaTime; };
	static int GetFPS() { return (int) 1.0f / ECS::Get().m_deltaTime; };

	// Run the main loop of the ECS (also create render part) and update systems/scripts
	void Run(HINSTANCE hInstance, ECSSettings settings = ECSSettings()); // Ajouter des parametres pour le window etc..

	// Lock Frame Rate (some time instable)
	void LockFPS(int fps);
	// Set the speed of time
	void SetClockSpeed(double timeSpeed) { m_clock.SetSpeed(timeSpeed); };
	double GetClockSpeed() { return m_clock.GetSpeed(); };
	bool IsParticuleEnabled() { return m_particuleEnabled; };

	// Add a sprite to the UI and return it (keep pointer for RemoveSprite)
	// /!\ Texture must be loaded before in TextureManager
	FS_Sprite* AddSprite(std::string texName) { return m_device->AddSprite(texName); };
	// Remove a sprite from the UI
	void RemoveSprite(FS_Sprite* sprite) { m_device->RemoveSprite(sprite); };

	// Get an entity from its ID
	Entity* GetEntity(int entityID);
	// Create an entity and return its ID 
	// An entity is empty by default
	int CreateEntity();
	// Remove (SetActive(false)) an entity and all its components
	void RemoveEntity(int entityID);

	// Get all components of a type
	template <typename C>
	std::vector<C>& GetAllFromType();
	// Get all components of an entity (one of each type)
	std::vector<Component*> GetAllFromEntity(int entityID);

	template <typename C>
	C* GetComponent(int entityID);
	// Add (SetActive(true) and Reset) a component to an entity and return it
	template <typename C>
	C* AddComponent(int entityID);
	// Remove (SetActive(false) and Reset) a component of an entity
	template <typename C>
	void RemoveComponent(int entityID);

	template <typename S>
	static S* CreateScript();
};

#include "ECS.inl"

