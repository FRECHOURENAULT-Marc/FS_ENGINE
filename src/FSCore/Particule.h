#pragma once

namespace FSC
{

	class Particule : public Object3D
	{
		XMFLOAT3 m_Dir = { 0, 0, 0 };
		XMFLOAT4 m_ColorToAdd = { 0, 0, 0, 0 };

		XMFLOAT3 m_Gravity = { 0, 0, 0 };
		XMFLOAT3 m_GravityForce = { 0, 0, 0 };
		float m_MaxGravityForce = 0;

		float m_Lifetime = 0.0f;
		float m_MaxLifetime = 0.0f;
		float m_Speed = 0.0f;

		bool m_IsDead = false;

		void UpdateGravityForce(float dt);
		void UpdateWorld(float dt);
		void UpdateColor(float dt);

	public:
		Particule() = default;
		Particule(MeshGeometry* geo, XMFLOAT4 startColor = { 1, 1, 1, 1 }, XMFLOAT4 endColor = { 0, 0, 0, 0 });
		~Particule();

		bool IsDead() { return m_IsDead; };

		void SetIsDead(bool isDead) { m_IsDead = isDead; };
		void SetLifetime(float lifetime) { m_Lifetime = lifetime; };
		void SetMaxLifetime(float lifetime) { m_MaxLifetime = lifetime; };
		void SetSpeed(float speed) { m_Speed = speed; };
		void SetColorToAdd(XMFLOAT4 colorToAdd) { m_ColorToAdd = colorToAdd; };
		void SetDir(XMFLOAT3 dir) { m_Dir = dir; };
		void SetGravity(XMFLOAT3 gravity) { m_Gravity = gravity; };
		void SetGravityForce(XMFLOAT3 gravityForce) { m_GravityForce = gravityForce; };
		void SetMaxGravityForce(float maxGravityForce) { m_MaxGravityForce = maxGravityForce; };

		void Update(float dt);
	};

}