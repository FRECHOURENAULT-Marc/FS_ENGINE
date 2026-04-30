#pragma once

namespace FSC
{

	enum class COLLIDER_TYPE
	{
		NONE,
		SPHERE,
		AABB,
	};

	class Collider
	{
	protected:
		XMFLOAT3 m_Pos = { 0, 0, 0 };
		XMFLOAT3 m_Scale = { 1.0f, 1.0f, 1.0f };
		COLLIDER_TYPE m_Type = COLLIDER_TYPE::NONE;

	public:

		Collider() {};
		virtual bool IsColliding(Collider* other) { return false; }; //override this
		XMFLOAT3 GetPosition() { return m_Pos; };
		XMFLOAT3 GetScale() { return m_Scale; };
		COLLIDER_TYPE GetColliderType() { return m_Type; };

		void SetPosition(XMFLOAT3 pos) { m_Pos = pos; };
		void SetScale(XMFLOAT3 scale) { m_Scale = scale; };
		void SetType(COLLIDER_TYPE type) { m_Type = type; };

		virtual void OnUpdate(const XMFLOAT3& _scale) {}

		static bool IsCollidingSphereSphere(Collider* sphere1, Collider* sphere2);
		static bool IsCollidingAABB(Collider* _box1, Collider* _box2);
		static bool IsCollidingAABBSphere(Collider* _box, Collider* _sphere);
	};

	/////////////////////
	/// SPHERE COLLIDER
	/////////////////////
	class ColliderSphere : public Collider
	{
	protected:

		float m_Radius = 0.5f;

	public:
		ColliderSphere();

		virtual bool IsColliding(Collider* other) override;
		float GetRadius() { return m_Radius; };

		void OnUpdate(const XMFLOAT3& _scale) override;
		void SetRadius(float radius) { m_Radius = radius; };

	};

	///////////////////
	/// AABB COLLIDER
	///////////////////
	class ColliderAABB : public Collider
	{
	protected:

		float baseLength = 1.0f;
		float width = 1.0f;
		float height = 1.0f;
		float depth = 1.0f;

	public:

		ColliderAABB();

		float GetWidth() { return width; }
		float GetHeight() { return height; }
		float GetDepth() { return depth; }

		void OnUpdate(const XMFLOAT3& _scale) override;
		virtual bool IsColliding(Collider* other) override;
	};
}