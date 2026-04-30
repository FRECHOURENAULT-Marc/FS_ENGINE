#include "pch.h"

namespace FSE
{
	void CameraComponent::Reset()
	{
		//SetLens(0.25f * Maths::Pi, 1.0f, 1.0f, 1000.0f);
	}

	CameraComponent::CameraComponent() : Component()
	{
		//SetLens(0.25f * Maths::Pi, 1.0f, 1.0f, 1000.0f);
	}

	void CameraComponent::Update(XMFLOAT4X4 worldMtraix)
	{
		if (!m_IsMainCamera)
			return;

		Camera3D* mainCamera = Device::Get()->GetCamera();
		mainCamera->SetWorld(worldMtraix);
	}

	void CameraComponent::RotateRad(float yaw, float pitch, float roll)
	{
		SetRotationRad(m_Rotation.x + yaw, m_Rotation.y + pitch, m_Rotation.z + roll);
	}

	void CameraComponent::SetRotationRad(float yaw, float pitch, float roll)
	{
		ECS& ecs = ECS::Get();
		m_Rotation.x = yaw;
		m_Rotation.y = pitch;
		m_Rotation.z = roll;

		m_Rotation.x = std::clamp(m_Rotation.x, m_LimitMin.x, m_LimitMax.x);
		m_Rotation.y = std::clamp(m_Rotation.y, m_LimitMin.y, m_LimitMax.y);
		m_Rotation.z = std::clamp(m_Rotation.z, m_LimitMin.z, m_LimitMax.z);

		ecs.GetComponent<TransformComponent>(m_ID)->SetRotation(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	}

	void CameraComponent::SetMainCamera(bool isMain)
	{
		m_IsMainCamera = isMain;

		if (!m_IsMainCamera)
			return;

		ECS& ecs = ECS::Get();
		Update(ecs.GetComponent<TransformComponent>(m_ID)->GetWorldMatrix());

		CameraComponent* cameras = ecs.GetAllFromType<CameraComponent>();

		for (int i = 0; i < MAX_COMPONENTS; i++)
		{
			CameraComponent& cam = cameras[i];
			if (cam.m_ID == m_ID)
				continue;

			cam.m_IsMainCamera = false;
		}
	}

	//void CameraComponent::SetLens(float fovY, float aspect, float zn, float zf)
	//{
	//	XMMATRIX P = XMMatrixPerspectiveFovLH(fovY, aspect, zn, zf);
	//}
}

