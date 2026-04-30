#include "pch.h"

namespace FSR 
{
	void Camera3D::MakeDirty()
	{
		m_IsDirty = true;
		m_isUpdatedThisFrame = true;
	}

	Camera3D::Camera3D()
	{
	}

	XMFLOAT3 Camera3D::GetPosition()
	{
		return m_Transform.m_Pos;
	}
	void Camera3D::Update()
	{
		UpdateMatrix();

		XMMATRIX viewCamera = XMLoadFloat4x4(&m_View3D);
		XMMATRIX projCamera = XMLoadFloat4x4(&m_Proj);
		XMMATRIX viewProj = viewCamera * projCamera;
		XMStoreFloat4x4(&m_viewProj, viewProj);

		// Get viewport size
		int height = Device::GetWindow()->GetViewPort().Height;
		int width = Device::GetWindow()->GetViewPort().Width;

		XMMATRIX projCamera2D = XMLoadFloat4x4(&m_Ortho);

		XMMATRIX viewProj2D = projCamera2D;
		XMStoreFloat4x4(&m_viewProj2D, viewProj2D);
	}

	void Camera3D::AfterUpdate()
	{
		SetUpdatedThisFrame(false);
	}

	void Camera3D::SetPosition(float x, float y, float z)
	{
		m_Transform.m_Pos = XMFLOAT3(x, y, z);
		MakeDirty();
	}

	void Camera3D::SetPosition(const XMFLOAT3& v)
	{
		m_Transform.m_Pos = v;
		MakeDirty();
	}

	void Camera3D::SetWorld(XMFLOAT4X4 world)
	{
		m_Transform.m_Right = XMFLOAT3(world.m[0][0], world.m[0][1], world.m[0][2]);
		m_Transform.m_Up = XMFLOAT3(world.m[1][0], world.m[1][1], world.m[1][2]);
		m_Transform.m_Forward = XMFLOAT3(world.m[2][0], world.m[2][1], world.m[2][2]);
		m_Transform.m_Pos = XMFLOAT3(world.m[3][0], world.m[3][1], world.m[3][2]);

		m_Transform.UpdateWorld();
		m_Transform.UpdateInvWorld();
		MakeDirty();
	}

	void Camera3D::SetLens(float fovY, float aspect, float zn, float zf)
	{
		// cache properties
		m_FovY = fovY;
		m_Aspect = aspect;
		m_NearZ = zn;
		m_FarZ = zf;

		m_NearWindowHeight = 2.0f * m_NearZ * tanf(0.5f * m_FovY);
		m_FarWindowHeight = 2.0f * m_FarZ * tanf(0.5f * m_FovY);

		XMMATRIX P = XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearZ, m_FarZ);
		XMStoreFloat4x4(&m_Proj, P);

		// Get viewport size
		int height = Device::GetWindow()->GetViewPort().Height;
		int width = Device::GetWindow()->GetViewPort().Width;

		XMMATRIX p2D = XMMatrixOrthographicLH(width, height, m_NearZ, m_FarZ);
		XMStoreFloat4x4(&m_Ortho, p2D);

		MakeDirty();
	}


	void Camera3D::UpdateMatrix()
	{
		if (m_IsDirty)
		{
			XMVECTOR R = XMLoadFloat3(&m_Transform.m_Right);
			XMVECTOR U = XMLoadFloat3(&m_Transform.m_Up);
			XMVECTOR L = XMLoadFloat3(&m_Transform.m_Forward);
			XMVECTOR P = XMLoadFloat3(&m_Transform.m_Pos);

			L = XMVector3Normalize(L);
			U = XMVector3Normalize(XMVector3Cross(L, R));
			R = XMVector3Cross(U, L);

			float x = -XMVectorGetX(XMVector3Dot(P, R));
			float y = -XMVectorGetX(XMVector3Dot(P, U));
			float z = -XMVectorGetX(XMVector3Dot(P, L));

			XMStoreFloat3(&m_Transform.m_Right, R);
			XMStoreFloat3(&m_Transform.m_Up, U);
			XMStoreFloat3(&m_Transform.m_Forward, L);

			m_View3D(0, 0) = m_Transform.m_Right.x;
			m_View3D(1, 0) = m_Transform.m_Right.y;
			m_View3D(2, 0) = m_Transform.m_Right.z;
			m_View3D(3, 0) = x;

			m_View3D(0, 1) = m_Transform.m_Up.x;
			m_View3D(1, 1) = m_Transform.m_Up.y;
			m_View3D(2, 1) = m_Transform.m_Up.z;
			m_View3D(3, 1) = y;

			m_View3D(0, 2) = m_Transform.m_Forward.x;
			m_View3D(1, 2) = m_Transform.m_Forward.y;
			m_View3D(2, 2) = m_Transform.m_Forward.z;
			m_View3D(3, 2) = z;

			m_View3D(0, 3) = 0.0f;
			m_View3D(1, 3) = 0.0f;
			m_View3D(2, 3) = 0.0f;
			m_View3D(3, 3) = 1.0f;

			m_IsDirty = false;
		}
	}
}