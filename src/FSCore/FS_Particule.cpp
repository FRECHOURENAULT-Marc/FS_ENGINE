#include "pch.h"

void FS_Particule::UpdateGravityForce(float dt)
{
	XMVECTOR _gravityToAdd = XMLoadFloat3(&m_gravityForce);
	XMVECTOR _gravity = XMLoadFloat3(&m_gravity);
	_gravity += _gravityToAdd * dt;
	XMStoreFloat3(&m_gravity, _gravity);

	if (m_gravity.x > m_maxGravityForce)
		m_gravity.x = m_maxGravityForce;
	if (m_gravity.y > m_maxGravityForce)
		m_gravity.y = m_maxGravityForce;
	if (m_gravity.z > m_maxGravityForce)
		m_gravity.z = m_maxGravityForce;
}

void FS_Particule::UpdateWorld(float dt)
{
	mWorld._41 += (m_dir.x + m_gravity.x) * m_speed * dt;
	mWorld._42 += (m_dir.y + m_gravity.y) * m_speed * dt;
	mWorld._43 += (m_dir.z + m_gravity.z) * m_speed * dt;
}

void FS_Particule::UpdateColor(float dt)
{
	XMFLOAT4 color = mColor;
	XMVECTOR _color = XMLoadFloat4(&color);
	XMVECTOR _colorToAdd = XMLoadFloat4(&m_colorToAdd) * (dt / m_maxLifetime);
	XMFLOAT4 nColor = { 0, 0, 0, 0 };
	XMStoreFloat4(&nColor, (_color + _colorToAdd));
	SetColor(nColor);
}

FS_Particule::FS_Particule(MeshGeometry* geo, XMFLOAT4 startColor, XMFLOAT4 endColor) : FS_3DObject(geo)
{
	XMVECTOR vec = XMLoadFloat4(&endColor) - XMLoadFloat4(&startColor);
	XMStoreFloat4(&m_colorToAdd, vec);
}
FS_Particule::~FS_Particule()
{
	int i = 0;
}

void FS_Particule::Update(float dt)
{
	m_lifetime += dt;
	if (m_lifetime >= m_maxLifetime)
	{
		m_isDead = true;
		return;
	}

	UpdateGravityForce(dt);
	UpdateWorld(dt);

	UpdateColor(dt);
}
