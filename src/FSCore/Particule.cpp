#include "pch.h"

namespace FSC
{

	void Particule::UpdateGravityForce(float dt)
	{
		XMVECTOR _gravityToAdd = XMLoadFloat3(&m_GravityForce);
		XMVECTOR _gravity = XMLoadFloat3(&m_Gravity);
		_gravity += _gravityToAdd * dt;
		XMStoreFloat3(&m_Gravity, _gravity);

		if (m_Gravity.x > m_MaxGravityForce)
			m_Gravity.x = m_MaxGravityForce;
		if (m_Gravity.y > m_MaxGravityForce)
			m_Gravity.y = m_MaxGravityForce;
		if (m_Gravity.z > m_MaxGravityForce)
			m_Gravity.z = m_MaxGravityForce;
	}

	void Particule::UpdateWorld(float dt)
	{
		auto world = GetWorld();
		world._41 += (m_Dir.x + m_Gravity.x) * m_Speed * dt;
		world._42 += (m_Dir.y + m_Gravity.y) * m_Speed * dt;
		world._43 += (m_Dir.z + m_Gravity.z) * m_Speed * dt;
		SetWorld(world);
	}

	void Particule::UpdateColor(float dt)
	{
		XMFLOAT4 color = m_Color;
		XMVECTOR _color = XMLoadFloat4(&color);
		XMVECTOR _colorToAdd = XMLoadFloat4(&m_ColorToAdd) * (dt / m_MaxLifetime);
		XMFLOAT4 nColor = { 0, 0, 0, 0 };
		XMStoreFloat4(&nColor, (_color + _colorToAdd));
		SetColor(nColor);
	}

	Particule::Particule(MeshGeometry* geo, XMFLOAT4 startColor, XMFLOAT4 endColor) : Object3D(geo)
	{
		XMVECTOR vec = XMLoadFloat4(&endColor) - XMLoadFloat4(&startColor);
		XMStoreFloat4(&m_ColorToAdd, vec);
	}
	Particule::~Particule()
	{
		int i = 0;
	}

	void Particule::Update(float dt)
	{
		m_Lifetime += dt;
		if (m_Lifetime >= m_MaxLifetime)
		{
			m_IsDead = true;
			return;
		}

		UpdateGravityForce(dt);
		UpdateWorld(dt);

		UpdateColor(dt);
	}

}