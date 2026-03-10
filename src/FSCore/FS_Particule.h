#pragma once

class FS_Particule : public FS_3DObject
{
	XMFLOAT3 m_dir = { 0, 0, 0 };
	XMFLOAT4 m_colorToAdd = { 0, 0, 0, 0 };

	XMFLOAT3 m_gravity = { 0, 0, 0 };
	XMFLOAT3 m_gravityForce = { 0, 0, 0 };
	float m_maxGravityForce = 0;

	float m_lifetime = 0.0f;
	float m_maxLifetime = 0.0f;
	float m_speed = 0.0f;

	bool m_isDead = false;

	void UpdateGravityForce(float dt);
	void UpdateWorld(float dt);
	void UpdateColor(float dt);

	FS_Particule() = default;

public:

	FS_Particule(MeshGeometry* geo, XMFLOAT4 startColor = { 1, 1, 1, 1 }, XMFLOAT4 endColor = { 0, 0, 0, 0 });
	~FS_Particule();

	bool IsDead() { return m_isDead; };
	void Update(float dt);

	friend class EmitterComponent;
};

