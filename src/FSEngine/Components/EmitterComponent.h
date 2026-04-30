#pragma once

#define PARTICULE_SIZE_FACTOR 0.1f

namespace FSE
{
	enum class EMITER_SHAPE
	{
		CONE,
		HALF_SPHERE,
		SPHERE
	};

	//Settings send to EmitterComponent (SetSettings) to manage particules depending on preferences
	struct EmitterSettings
	{
		//Color when particule spawn
		XMFLOAT4 m_PartStartColor = { 1, 1, 1, 1 };
		//Color to reach at the end of lifetime
		XMFLOAT4 m_PartEndColor = { 0, 0, 0, 0 };

		//Direction of particule (randomized), ignore if EMITER_SHAPE == SPHERE
		XMFLOAT3 m_PartDir = { 0, 0, 0 };
		//Gravity vector apply each frame
		XMFLOAT3 m_PartGravity = { 0, 0, 0 };
		//Max value of gravity vector
		float m_PartMaxGravityForce = 9.81f;
		//Particule speed (multiply with dir for move)
		float m_PartSpeed = 1.0f;

		//Size of the geometry (multiply by PARTICULE_SIZE_FACTOR)
		float m_PartSize = 1.0f;
		//Duration of life's particule
		float m_PartLifetime = 3.0f;

		//Maximum of particule alive
		int m_EmitterMaxPart = 100;
		//Spawn rate of particule
		int m_EmitterDensity = 10;

		//Geometry of particule
		FSR::GEO_TYPE m_PartGeoType = FSR::GEO_TYPE::PLANE;
		//Emission cone of emitter
		EMITER_SHAPE m_EmitterShape = EMITER_SHAPE::SPHERE;
		//
		std::string m_PartMaterialName = "";
	};

	class EmitterComponent : public Component
	{
		virtual void Reset() override;

		EmitterSettings m_partSettings;
		std::vector<Particule*> m_particules;
		std::vector<Particule*> m_inactiveParticules;

		//To avoid GetComponent each frame, not store here just pointer
		TransformComponent* m_trs = nullptr;

		float m_timerForSpawn = 0.0f;
		int m_partSpawned = 0;

		void Update(float dt);
		void ParticulesUpdate(float dt);
		void HandleParticulesCreation(float dt);

		Particule* GetParticuleForSpawn();
		void CreateParticule();
		void SetInactiveParticule(int index);
		void SetAllParticulesInactive();

		XMFLOAT3 GenerateSphereDirection();
		XMFLOAT3 GenerateHalfSphereDirection(XMFLOAT3 settingsDir);
		XMFLOAT3 GenerateConeDirection(XMFLOAT3 settingsDir, float angleRad = 3.140f / 4.0f);

	public:
		EmitterComponent() : Component() {};
		~EmitterComponent();

		//Apply settings for emission of particules
		void SetSettings(EmitterSettings particulesSettings) { m_partSettings = particulesSettings; };
		EmitterSettings GetSettings() { return m_partSettings; }

		void HideAllParticules();

		friend class ECS;
		friend class EmitterSystem;
	};
}
