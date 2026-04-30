#include "pch.h"

namespace FSE
{
	void EmitterComponent::Reset()
	{
		m_partSettings = EmitterSettings();
		for (Particule* part : m_particules)
			Device::Get()->RemoveObject(part);
		for (Particule* part : m_inactiveParticules)
			Device::Get()->RemoveObject(part);
		m_particules.clear();
		m_inactiveParticules.clear();
		m_timerForSpawn = 0.0f;
		m_partSpawned = 0;
		m_partSettings = EmitterSettings();
	}

	void EmitterComponent::Update(float dt)
	{
		if (m_trs == nullptr)
			m_trs = ECS::Get().GetComponent<TransformComponent>(m_ID);

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
		if (m_partSpawned >= m_partSettings.m_EmitterMaxPart)
			return;

		m_timerForSpawn += dt;
		float spawnTime = 1.0f / m_partSettings.m_EmitterDensity;
		if (m_timerForSpawn < spawnTime)
			return;

		while (m_timerForSpawn > spawnTime)
		{
			m_partSpawned++;
			m_timerForSpawn -= spawnTime;
			CreateParticule();

			if (m_partSpawned >= m_partSettings.m_EmitterMaxPart)
				return;
		}
	}

	Particule* EmitterComponent::GetParticuleForSpawn()
	{
		if (m_inactiveParticules.size() == 0)
		{
			Particule* part = new Particule();
			//Add to vectors
			Device::Get()->AddObject(part);
			m_particules.push_back(part);
			return part;
		}


		Particule* part = m_inactiveParticules[m_inactiveParticules.size() - 1];
		m_inactiveParticules.pop_back();
		m_particules.push_back(part);
		part->SetActive(true);
		part->SetIsDead(false);
		part->SetLifetime(0.0f);
		part->SetGravity({ 0, 0, 0 });
		return part;
	}

	void EmitterComponent::CreateParticule()
	{
		//New particule
		Particule* particule = GetParticuleForSpawn();
		//Geometry
		particule->SetGeo(GeometryManager::GetGeometry(m_partSettings.m_PartGeoType));
		//Material
		int matIndex = MaterialManager::GetMaterialIndex(m_partSettings.m_PartMaterialName);
		if (matIndex != -1)
			particule->SetMaterialIndex(matIndex);
		//Color
		XMFLOAT4 colorToAdd = { 0, 0, 0, 0 };
		XMVECTOR _colorToAdd = XMLoadFloat4(&m_partSettings.m_PartEndColor) - XMLoadFloat4(&m_partSettings.m_PartStartColor);
		XMStoreFloat4(&colorToAdd, _colorToAdd);
		particule->SetColorToAdd(colorToAdd);
		particule->SetColor(m_partSettings.m_PartStartColor);
		//Lifetime
		particule->SetMaxLifetime(m_partSettings.m_PartLifetime);
		//World
		particule->SetWorld(m_trs->GetWorldMatrix());
		//Gravity
		particule->SetSpeed(m_partSettings.m_PartSpeed);
		particule->SetGravityForce(m_partSettings.m_PartGravity);
		particule->SetMaxGravityForce(m_partSettings.m_PartMaxGravityForce);
		//Direction (move)
		switch (m_partSettings.m_EmitterShape)
		{
		case EMITER_SHAPE::SPHERE:
			particule->SetDir(GenerateSphereDirection());
			break;
		case EMITER_SHAPE::HALF_SPHERE:
			particule->SetDir(GenerateHalfSphereDirection(m_partSettings.m_PartDir));
			break;
		case EMITER_SHAPE::CONE:
			particule->SetDir(GenerateConeDirection(m_partSettings.m_PartDir));
			break;
		default:
			break;
		}
		//Size (with world matrix)
		float size = m_partSettings.m_PartSize * PARTICULE_SIZE_FACTOR;
		XMFLOAT4X4 world = particule->GetWorld();
		XMMATRIX _world = XMLoadFloat4x4(&world);
		_world.r[0] = _world.r[0] * size;
		_world.r[1] = _world.r[1] * size;
		_world.r[2] = _world.r[2] * size;
		XMStoreFloat4x4(&world, _world);
		particule->SetWorld(world);
	}

	void EmitterComponent::SetInactiveParticule(int index)
	{
		Particule* part = m_particules[index];
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
		for (int i = 0; i < m_particules.size(); i++)
		{
			Particule* part = m_particules[i];
			Device::Get()->RemoveObject(part);
		}
		m_particules.clear();
		for (int i = 0; i < m_inactiveParticules.size(); i++)
		{
			Particule* part = m_inactiveParticules[i];
			Device::Get()->RemoveObject(part);
		}
		m_inactiveParticules.clear();
	}

	void EmitterComponent::SetAllParticulesInactive()
	{
		int lastIndex = m_particules.size() - 1;
		for(int i = lastIndex; i >= 0; i--)
			SetInactiveParticule(i);
	}

	void EmitterComponent::HideAllParticules()
	{
		SetAllParticulesInactive();
	}

}


