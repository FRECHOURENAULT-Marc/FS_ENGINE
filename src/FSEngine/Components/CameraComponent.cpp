#include "pch.h"

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

	FS_3DCamera* mainCamera = FS_Device::Get()->Camera();
	mainCamera->SetWorld(worldMtraix);
}

void CameraComponent::RotateRad(float yaw, float pitch, float roll)
{
	SetRotationRad(m_rotation.x + yaw, m_rotation.y + pitch, m_rotation.z + roll);
}

void CameraComponent::SetRotationRad(float yaw, float pitch, float roll)
{
	ECS& ecs = ECS::Get();
	m_rotation.x = yaw;
	m_rotation.y = pitch;
	m_rotation.z = roll;

	m_rotation.x = std::clamp(m_rotation.x, m_limitMin.x, m_limitMax.x);
	m_rotation.y = std::clamp(m_rotation.y, m_limitMin.y, m_limitMax.y);
	m_rotation.z = std::clamp(m_rotation.z, m_limitMin.z, m_limitMax.z);

	ecs.GetComponent<TransformComponent>(m_id)->SetRotation(m_rotation.x, m_rotation.y, m_rotation.z);
}

void CameraComponent::SetMainCamera(bool isMain)
{
	m_IsMainCamera = isMain;

	if (!m_IsMainCamera)
		return;

	ECS& ecs = ECS::Get();
	Update(ecs.GetComponent<TransformComponent>(m_id)->GetWorldMatrix());

	std::vector<CameraComponent>& cameras = ecs.GetAllFromType<CameraComponent>();

	for (int i = 0; i < cameras.size(); i++)
	{
		CameraComponent& cam = cameras[i];
		if(cam.m_id == m_id)
			continue;

		cam.m_IsMainCamera = false;
	}
}

//void CameraComponent::SetLens(float fovY, float aspect, float zn, float zf)
//{
//	XMMATRIX P = XMMatrixPerspectiveFovLH(fovY, aspect, zn, zf);
//}