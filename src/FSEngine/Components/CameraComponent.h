#pragma once

namespace FSE
{

	class CameraComponent : public Component
	{
		virtual void Reset() override;

		XMFLOAT3 m_Rotation = { 0,0,0 };
		XMFLOAT3 m_LimitMin = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
		XMFLOAT3 m_LimitMax = { FLT_MAX, FLT_MAX, FLT_MAX };
		bool m_IsMainCamera = false;

	public:
		CameraComponent();

		void Update(XMFLOAT4X4 worldMtraix);

		bool IsMainCamera() { return m_IsMainCamera; }

		void RotateRad(float yaw, float pitch, float roll);
		void SetRotationRad(float yaw, float pitch, float roll);

		void SetRotationLimitYaw(float min, float max) { m_LimitMin.x = min; m_LimitMax.x = max; }
		void SetRotationLimitPitch(float min, float max) { m_LimitMin.y = min; m_LimitMax.y = max; }
		void SetRotationLimitRoll(float min, float max) { m_LimitMin.z = min; m_LimitMax.z = max; }

		void SetMainCamera(bool isMain);
		void SetLens(float fovY, float aspect, float zn, float zf) = delete;

		friend class ECS;
	};


}


