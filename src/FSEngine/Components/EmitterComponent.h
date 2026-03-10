#pragma once

#define PARTICULE_SIZE_FACTOR 0.1f

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
	XMFLOAT4 partStartColor = { 1, 1, 1, 1 };
	//Color to reach at the end of lifetime
	XMFLOAT4 partEndColor = { 0, 0, 0, 0 };

	//Direction of particule (randomized), ignore if EMITER_SHAPE == SPHERE
	XMFLOAT3 partDir = { 0, 0, 0 };
	//Gravity vector apply each frame
	XMFLOAT3 partGravity = { 0, 0, 0 };
	//Max value of gravity vector
	float partMaxGravityForce = 9.81f;
	//Particule speed (multiply with dir for move)
	float partSpeed = 1.0f;

	//Size of the geometry (multiply by PARTICULE_SIZE_FACTOR)
	float partSize = 1.0f;
	//Duration of life's particule
	float partLifetime = 3.0f;

	//Maximum of particule alive
	int emitterMaxPart = 100;
	//Spawn rate of particule
	int emitterDensity = 10;
	
	//Geometry of particule
	GEO_TYPE partGeotype = GEO_TYPE::PLANE;
	//Emission cone of emitter
	EMITER_SHAPE emitterShape = EMITER_SHAPE::SPHERE;
	//
	std::string partMaterialName = "";
};

class EmitterComponent : public Component
{
	virtual void Reset() override;

	EmitterSettings m_partSettings;
	std::vector<FS_Particule*> m_particules;
	std::vector<FS_Particule*> m_inactiveParticules;

	//To avoid GetComponent each frame, not store here just pointer
	TransformComponent* m_trs = nullptr;
	
	float m_timerForSpawn = 0.0f;
	int m_partSpawned = 0;

	void Update(float dt);
	void ParticulesUpdate(float dt);
	void HandleParticulesCreation(float dt);

	FS_Particule* GetParticuleForSpawn();
	void CreateParticule();
	void SetInactiveParticule(int index);

	XMFLOAT3 GenerateSphereDirection();
	XMFLOAT3 GenerateHalfSphereDirection(XMFLOAT3 settingsDir);
	XMFLOAT3 GenerateConeDirection(XMFLOAT3 settingsDir, float angleRad = 3.140f/4.0f);

public:
	EmitterComponent() : Component() {};
	~EmitterComponent();

	//Apply settings for emission of particules
	void SetSettings(EmitterSettings particulesSettings) { m_partSettings = particulesSettings; };

	friend class ECS;
	friend class EmitterSystem;
};
