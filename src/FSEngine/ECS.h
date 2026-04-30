#pragma once

namespace FSE
{
	class SceneManager;

	//Settings send to ECS when Run method is call, can be completed
	struct ECSSettings
	{
		std::wstring m_WinName = L"FS-ENGINE";
		int m_FPSLock = -1;
		int m_WinWidth = 800;
		int m_WinHeight = 600;
		bool m_IsSpriteEnable = true;
		bool m_IsParticuleEnable = true;
		bool m_ShowFPSInWindowName = true;
	};

	class ECS
	{
		std::vector<Script*> m_PermanentScripts = std::vector<Script*>();
		std::vector<Script*> m_PermanentScriptsAdded = std::vector<Script*>();

		Device* m_Device = nullptr;
		Window* m_Window = nullptr;
		Camera3D* m_Camera = nullptr;

		SceneManager* m_ScMgr = nullptr;

		static inline ECS* m_Instance = nullptr;

		Clock m_Clock = Clock();
		float m_DeltaTime = 0.0f;
		float m_UpdateWindowNameTimer = 0.0f;
		int m_FpsLock = 0;

		bool m_IsParticuleEnabled = true;
		bool m_IsStarted = false;

		ECS();
		~ECS();

		void InitManagers();

		void UpdateDeltaTime();
		void UpdatePermanentScript();
		void UpdateWindowName(const ECSSettings& settings);

		// Add a script to the ECS
		// It will be added to a temporary vector and moved to active scripts at the Run of ECS (for calling OnStart())
		void AddScript(Script* script);
		void AddPermanentScript(Script* script);

		friend class Script;

	public:

		// Create the ECS instance (no render init) if it doesnt exist and return it, else return the existing one
		static ECS& Create();
		static void Close();

		// Get the ECS instance, if it doesnt exist create it and return it, else return the existing one
		static ECS& Get();
		static ECS* GetPtr();
		static float GetDeltaTime() { return ECS::Get().m_DeltaTime; };
		static int GetFPS() { return (int)1.0f / ECS::Get().m_DeltaTime; };

		// Run the main loop of the ECS (also create render part) and update systems/scripts
		void Run(HINSTANCE hInstance, ECSSettings settings = ECSSettings()); // Ajouter des parametres pour le window etc..

		// Lock Frame Rate (some time instable)
		void LockFPS(int fps);
		// Set the speed of time
		void SetClockSpeed(double timeSpeed) { m_Clock.SetSpeed(timeSpeed); };

		double GetClockSpeed() { return m_Clock.GetSpeed(); };
		bool IsParticuleEnabled() { return m_IsParticuleEnabled; };
		bool IsStarted() { return m_IsStarted; };

		// Get an entity from its ID
		Entity* GetEntity(int entityID);
		// Create an entity and return its ID 
		// An entity is empty by default
		int CreateEntity();
		// Remove (SetActive(false)) an entity and all its components
		void RemoveEntity(int entityID);

		template <typename C>
		C* GetAllFromType();
		// Get all components of an entity (one of each type)
		std::vector<Component*> GetAllFromEntity(int entityID);
		template<typename O>
		BUFFERC_IN_USE<O>* InitObjectBuffer();

		std::vector<ColliderComponent*>& GetLayer(std::string _name);
		std::map<std::string, std::vector<ColliderComponent*>>& GetLayersMap();

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
		template <typename S>
		static S* CreatePermanentScript();
	};

}

#include "ECS.inl"

