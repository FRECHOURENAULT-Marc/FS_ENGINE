#include "pch.h"

void EmitterComponent::Reset()
{
	m_partSettings = EmitterSettings();
	for (FS_Particule* part : m_particules)
		FS_Device::Get()->RemoveObject(part);
	for (FS_Particule* part : m_inactiveParticules)
		FS_Device::Get()->RemoveObject(part);
	m_particules.clear();
	m_inactiveParticules.clear();
	m_timerForSpawn = 0.0f;
	m_partSpawned = 0;
	m_partSettings = EmitterSettings();
}

void EmitterComponent::Update(float dt)
{
	if(m_trs == nullptr)
		m_trs = ECS::Get().GetComponent<TransformComponent>(m_id);

	ParticulesUpdate(dt);
	HandleParticulesCreation(dt);
}

void EmitterComponent::ParticulesUpdate(float dt)
{
	for (int i = 0; i < m_particules.size(); i++)
	{
		auto particule = m_particules[i];
		particule->Update(dt);
		if (particule->IsDead())
			SetInactiveParticule(i);
	}
}

void EmitterComponent::HandleParticulesCreation(float dt)
{
	if (m_trs->IsActive() == false)
		return;
	if (m_partSpawned >= m_partSettings.emitterMaxPart)
		return;

	m_timerForSpawn += dt;
	float spawnTime = 1.0f / m_partSettings.emitterDensity;
	if (m_timerForSpawn < spawnTime)
		return;

	while(m_timerForSpawn > spawnTime)
	{
		m_partSpawned++;
		m_timerForSpawn -= spawnTime;
		CreateParticule();

		if (m_partSpawned >= m_partSettings.emitterMaxPart)
			return;
	}
}

FS_Particule* EmitterComponent::GetParticuleForSpawn()
{
	if(m_inactiveParticules.size() == 0)
	{
		FS_Particule* part = new FS_Particule();
		//Add to vectors
		FS_Device::Get()->AddObject(part);
		m_particules.push_back(part);
		return part;
	}
		

	FS_Particule* part = m_inactiveParticules[m_inactiveParticules.size() - 1];
	m_inactiveParticules.pop_back();
	m_particules.push_back(part);
	part->SetActive(true);
	part->m_isDead = false;
	part->m_lifetime = 0.0f;
	part->m_gravity = { 0, 0, 0 };
	return part;
}

void EmitterComponent::CreateParticule()
{
	//New particule
	FS_Particule* particule = GetParticuleForSpawn();
	//Geometry
	particule->mGeo = GeometryManager::GetGeometry(m_partSettings.partGeotype);
	//Material
	int matIndex = MaterialManager::GetMaterialIndex(m_partSettings.partMaterialName);
	if(matIndex != -1)
		particule->mMaterialIndex = matIndex;
	//Color
	XMVECTOR _colorToAdd = XMLoadFloat4(&m_partSettings.partEndColor) - XMLoadFloat4(&m_partSettings.partStartColor);
	XMStoreFloat4(&particule->m_colorToAdd, _colorToAdd);
	particule->SetColor(m_partSettings.partStartColor);
	//Lifetime
	particule->m_maxLifetime = m_partSettings.partLifetime;
	//World
	particule->SetWorld(m_trs->GetLocalTransform().GetWorld());
	//Gravity
	particule->m_speed = m_partSettings.partSpeed;
	particule->m_gravityForce = m_partSettings.partGravity;
	particule->m_maxGravityForce = m_partSettings.partMaxGravityForce;
	//Direction (move)
	switch (m_partSettings.emitterShape)
	{
	case EMITER_SHAPE::SPHERE:
		particule->m_dir = GenerateSphereDirection();
		break;
	case EMITER_SHAPE::HALF_SPHERE:
		particule->m_dir = GenerateHalfSphereDirection(m_partSettings.partDir);
		break;
	case EMITER_SHAPE::CONE:
		particule->m_dir = GenerateConeDirection(m_partSettings.partDir);
		break;
	default:
		break;
	}
	//Size (with world matrix)
	float size = m_partSettings.partSize * PARTICULE_SIZE_FACTOR;
	XMMATRIX _world = XMLoadFloat4x4(&particule->GetWorld());
	_world.r[0] = _world.r[0] * size;
	_world.r[1] = _world.r[1] * size;
	_world.r[2] = _world.r[2] * size;
	XMFLOAT4X4 world = Maths::Identity4x4();
	XMStoreFloat4x4(&world, _world);
	particule->SetWorld(world);
}

void EmitterComponent::SetInactiveParticule(int index)
{
	//FS_Device::Get()->RemoveObject(m_particules[index]);
	//m_particules.erase(m_particules.begin() + index);

	FS_Particule* part = m_particules[index];
	part->SetActive(false);
	m_particules.erase(m_particules.begin() + index);
	m_inactiveParticules.push_back(part);
	m_partSpawned--;
}

XMFLOAT3 EmitterComponent::GenerateSphereDirection()
{
	float u = (rand() % 100) / 100.f;	// [0.0f, 1.0f]
	float v = (rand() % 100) / 100.f;   // [0.0f, 1.0f]

	float theta = 2.0f * XM_PI * u; // horizontal angle
	float z = 2.0f * v - 1.0f;      // [-1.0f, 1.0f]
	float r = sqrtf(1.0f - z * z);

	float x = r * cosf(theta);
	float y = r * sinf(theta);

	return XMFLOAT3(x, y, z);
}

XMFLOAT3 EmitterComponent::GenerateHalfSphereDirection(XMFLOAT3 settingsDir)
{
	XMVECTOR dir = XMVector3Normalize(XMLoadFloat3(&settingsDir));

	XMFLOAT3 random = GenerateSphereDirection();
	XMVECTOR randVec = XMLoadFloat3(&random);

	// when wrong side, mirror (apply minus XMVECTOR)
	if (XMVectorGetX(XMVector3Dot(randVec, dir)) < 0.0f)
		randVec = XMVectorNegate(randVec);

	XMFLOAT3 result;
	XMStoreFloat3(&result, randVec);
	return result;
}

XMFLOAT3 EmitterComponent::GenerateConeDirection(XMFLOAT3 settingsDir, float angleRad)
{
	float u = (rand() % 100) / 100.f;	// [0.0f, 1.0f]
	float v = (rand() % 100) / 100.f;   // [0.0f, 1.0f]

	float cosTheta = (1 - u) + u * cosf(angleRad);
	float sinTheta = sqrtf(1 - cosTheta * cosTheta);
	float phi = 2.0f * XM_PI * v;

	float x = sinTheta * cosf(phi);
	float y = sinTheta * sinf(phi);
	float z = cosTheta;

	// local vector
	XMVECTOR localVec = XMVectorSet(x, y, z, 0.0f);
	// Create a base with dir
	XMVECTOR dir = XMVector3Normalize(XMLoadFloat3(&settingsDir));

	XMVECTOR worldUp = XMVectorSet(0, 1, 0, 0);
	//Magnitude (if ~1 or ~-1 parallel : bad for cross product)
	float dot = XMVectorGetX(XMVector3Dot(dir, worldUp));
	XMVECTOR up;
	if (abs(dot) > 0.99f) // if dot near from Y axis, take x
		up = XMVectorSet(1, 0, 0, 0);
	else
		up = worldUp;

	XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, dir));
	XMVECTOR forward = XMVector3Cross(dir, right);

	// Rotation from matrix
	XMMATRIX basis = XMMATRIX(right, forward, dir, XMVectorSet(0, 0, 0, 1));
	XMVECTOR rotated = XMVector3TransformNormal(localVec, basis);

	XMFLOAT3 result;
	XMStoreFloat3(&result, rotated);
	return result;
}

EmitterComponent::~EmitterComponent()
{
	for(int i = 0; i < m_particules.size(); i++)
	{
		FS_Particule* part = m_particules[i];
		FS_Device::Get()->RemoveObject(part);
	}
	m_particules.clear();
	for (int i = 0; i < m_inactiveParticules.size(); i++)
	{
		FS_Particule* part = m_inactiveParticules[i];
		FS_Device::Get()->RemoveObject(part);
	}
	m_inactiveParticules.clear();
}
