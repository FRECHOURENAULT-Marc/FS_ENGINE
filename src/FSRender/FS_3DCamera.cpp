#include "pch.h"

FS_3DCamera::FS_3DCamera()
{
}

XMFLOAT3 FS_3DCamera::GetPosition()
{
	return m_transform.pos;
}
void FS_3DCamera::Update()
{
	XMMATRIX viewCamera = XMLoadFloat4x4(&m_View3D);
	XMMATRIX projCamera = XMLoadFloat4x4(&m_Proj);
	XMMATRIX viewProj = viewCamera * projCamera;
	XMStoreFloat4x4(&m_viewProj, viewProj);

	// Get viewport size
	int height = FS_Device::Window()->GetViewPort().Height;
	int width = FS_Device::Window()->GetViewPort().Width;

	XMMATRIX projCamera2D = XMLoadFloat4x4(&m_Ortho);

	XMMATRIX viewProj2D = projCamera2D ;
	XMStoreFloat4x4(&m_viewProj2D, viewProj2D);
}

void FS_3DCamera::SetPosition(float x, float y, float z)
{
	m_transform.pos = XMFLOAT3(x, y, z);
	m_IsDirty = true;
}

void FS_3DCamera::SetPosition(const XMFLOAT3& v)
{
	m_transform.pos = v;
	m_IsDirty = true;
}

void FS_3DCamera::SetWorld(XMFLOAT4X4 world)
{
	m_transform.right = XMFLOAT3(world.m[0][0], world.m[0][1], world.m[0][2]);
	m_transform.up = XMFLOAT3(world.m[1][0], world.m[1][1], world.m[1][2]);
	m_transform.forward = XMFLOAT3(world.m[2][0], world.m[2][1], world.m[2][2]);
	m_transform.pos = XMFLOAT3(world.m[3][0], world.m[3][1], world.m[3][2]);

	m_transform.UpdateWorld();
	m_transform.UpdateInvWorld();
	m_IsDirty = true;
}

void FS_3DCamera::SetLens(float fovY, float aspect, float zn, float zf)
{
	// cache properties
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = zn;
	mFarZ = zf;

	mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f * mFovY);
	mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f * mFovY);

	XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	XMStoreFloat4x4(&m_Proj, P);

	// Get viewport size
	int height = FS_Device::Window()->GetViewPort().Height;
	int width = FS_Device::Window()->GetViewPort().Width;

	XMMATRIX p2D = XMMatrixOrthographicLH(width, height, mNearZ, mFarZ);
	XMStoreFloat4x4(&m_Ortho, p2D);
}

void FS_3DCamera::UpdateMatrix()
{
	if (m_IsDirty)
	{
		XMVECTOR R = XMLoadFloat3(&m_transform.right);
		XMVECTOR U = XMLoadFloat3(&m_transform.up);
		XMVECTOR L = XMLoadFloat3(&m_transform.forward);
		XMVECTOR P = XMLoadFloat3(&m_transform.pos);

		L = XMVector3Normalize(L);
		U = XMVector3Normalize(XMVector3Cross(L, R));
		R = XMVector3Cross(U, L);

		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));

		XMStoreFloat3(&m_transform.right, R);
		XMStoreFloat3(&m_transform.up, U);
		XMStoreFloat3(&m_transform.forward, L);

		m_View3D(0, 0) = m_transform.right.x;
		m_View3D(1, 0) = m_transform.right.y;
		m_View3D(2, 0) = m_transform.right.z;
		m_View3D(3, 0) = x;

		m_View3D(0, 1) = m_transform.up.x;
		m_View3D(1, 1) = m_transform.up.y;
		m_View3D(2, 1) = m_transform.up.z;
		m_View3D(3, 1) = y;

		m_View3D(0, 2) = m_transform.forward.x;
		m_View3D(1, 2) = m_transform.forward.y;
		m_View3D(2, 2) = m_transform.forward.z;
		m_View3D(3, 2) = z;

		m_View3D(0, 3) = 0.0f;
		m_View3D(1, 3) = 0.0f;
		m_View3D(2, 3) = 0.0f;
		m_View3D(3, 3) = 1.0f;

		m_IsDirty = false;
	}
}