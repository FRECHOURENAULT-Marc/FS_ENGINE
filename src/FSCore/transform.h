#pragma once

namespace FSC
{

	struct Transform
	{
		XMFLOAT3 m_Pos;

		XMFLOAT3 m_Scale;

		XMFLOAT3 m_Forward;
		XMFLOAT3 m_Right;
		XMFLOAT3 m_Up;
		XMFLOAT4 m_Quat;
		XMFLOAT4X4 m_Rot;

		float m_Yaw;
		float m_Pitch;
		float m_Roll;

	private:
		bool m_IsWorldUpdated;
		XMFLOAT4X4 m_World;
		bool m_IsInvWorldUpdated;
		XMFLOAT4X4 m_InvWorld;

	public:
		Transform();

		XMFLOAT4X4 GetWorld();
		XMFLOAT4X4 GetInvWorld();
		XMFLOAT3 GetYPR();
		void SetWorld(XMFLOAT4X4 _world);

		void UpdateWorld();
		void UpdateInvWorld();

		bool IsUpdated() { return m_IsWorldUpdated && m_IsInvWorldUpdated; };

		void Identity();
		//void SetScaling(float scale);
		void SetScaling(float sx, float sy, float sz);
		void Scale(float scale);
		void Scale(float sx, float sy, float sz);
		void SetPosition(float x, float y, float z);
		void Move(float dist);
		void ResetRotation();
		void SetRotation(Transform& trs);
		void SetRotationFromQuaternion();
		void OrbitAroundAxis(XMFLOAT3& center, XMFLOAT3& axis, float radius, float angle);
		void SetYPR(float yaw, float pitch, float roll);
		void AddYPR(float yaw, float pitch, float roll);
		void AddYaw(float yaw);
		void AddPitch(float pitch);
		void AddRoll(float roll);
	};

}