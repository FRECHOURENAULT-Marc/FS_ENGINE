#include "pch.h"

namespace FSC
{

	Transform::Transform()
	{
		Identity();
	}
	void Transform::Identity()
	{
		m_Pos = { 0, 0, 0 };
		m_Scale = { 1, 1, 1 };
		m_Yaw = 0.0f;
		m_Pitch = 0.0f;
		m_Roll = 0.0f;
		ResetRotation();
	}
	XMFLOAT4X4 Transform::GetWorld()
	{
		if (m_IsWorldUpdated == false)
			UpdateWorld();

		return m_World;
	}
	XMFLOAT4X4 Transform::GetInvWorld()
	{
		if (m_IsInvWorldUpdated == false)
			UpdateInvWorld();
		return m_InvWorld;
	}

	XMFLOAT3 Transform::GetYPR()
	{
		return XMFLOAT3(m_Yaw, m_Pitch, m_Roll);
	}

	void Transform::SetWorld(XMFLOAT4X4 _world)
	{
		m_World = _world;
		m_IsWorldUpdated = false;
		m_IsInvWorldUpdated = false;
	}

	void Transform::UpdateWorld()
	{
		XMVECTOR _scale = XMLoadFloat3(&m_Scale);
		XMVECTOR _pos = XMLoadFloat3(&m_Pos);

		XMVECTOR sx = XMVectorSplatX(_scale);
		XMVECTOR sy = XMVectorSplatY(_scale);
		XMVECTOR sz = XMVectorSplatZ(_scale);

		XMMATRIX _world = XMLoadFloat4x4(&m_Rot);
		_world.r[0] = XMVectorMultiply(_world.r[0], sx);
		_world.r[1] = XMVectorMultiply(_world.r[1], sy);
		_world.r[2] = XMVectorMultiply(_world.r[2], sz);
		_world.r[3] = XMVectorSetW(_pos, 1.0f);

		XMStoreFloat4x4(&m_World, _world);
		m_IsWorldUpdated = true;
		m_IsInvWorldUpdated = false;
	}
	void Transform::UpdateInvWorld()
	{
		XMFLOAT4X4 _world = GetWorld();
		XMMATRIX _invWorld = XMMatrixInverse(nullptr, XMLoadFloat4x4(&_world));
		XMStoreFloat4x4(&m_InvWorld, _invWorld);

		m_IsInvWorldUpdated = true;
	}

	/*void Transform::SetScaling(float scale)
	{
		m_Scale.x = scale;
		m_Scale.y = scale;
		m_Scale.z = scale;
		m_IsWorldUpdated = false;
	}*/
	void Transform::SetScaling(float sx, float sy, float sz)
	{
		m_Scale.x = sx;
		m_Scale.y = sy;
		m_Scale.z = sz;
		m_IsWorldUpdated = false;
	}
	void Transform::Scale(float scale)
	{
		m_Scale.x *= scale;
		m_Scale.y *= scale;
		m_Scale.z *= scale;
		m_IsWorldUpdated = false;
	}
	void Transform::Scale(float sx, float sy, float sz)
	{
		m_Scale.x *= sx;
		m_Scale.y *= sy;
		m_Scale.z *= sz;
		m_IsWorldUpdated = false;
	}

	void Transform::SetPosition(float x, float y, float z)
	{
		m_Pos.x = x;
		m_Pos.y = y;
		m_Pos.z = z;
		m_IsWorldUpdated = false;
	}

	void Transform::Move(float dist)
	{
		m_Pos.x += m_Forward.x * dist;
		m_Pos.y += m_Forward.y * dist;
		m_Pos.z += m_Forward.z * dist;
		m_IsWorldUpdated = false;
	}

	void Transform::ResetRotation()
	{
		m_Forward = { 0.0f, 0.0f, 1.0f };
		m_Right = { 1.0f, 0.0f, 0.0f };
		m_Up = { 0.0f, 1.0f, 0.0f };
		m_Quat = { 0.0f, 0.0f, 0.0f, 1.0f };
		XMStoreFloat4x4(&m_Rot, XMMatrixIdentity());
		m_IsWorldUpdated = false;
	}
	void Transform::SetRotation(Transform& trs)
	{
		m_Yaw		= trs.m_Yaw;
		m_Pitch		= trs.m_Pitch;
		m_Roll		= trs.m_Roll;
		m_Forward	= trs.m_Forward;
		m_Right		= trs.m_Right;
		m_Up		= trs.m_Up;
		m_Quat		= trs.m_Quat;
		m_Rot		= trs.m_Rot;
		m_IsWorldUpdated = false;
	}
	void Transform::SetRotationFromQuaternion()
	{
		XMStoreFloat4x4(&m_Rot, XMMatrixRotationQuaternion(XMLoadFloat4(&m_Quat)));

		m_Right.x = m_Rot.m[0][0]; // _11, _12, _13
		m_Right.y = m_Rot.m[0][1];
		m_Right.z = m_Rot.m[0][2];

		m_Up.x = m_Rot.m[1][0]; // _21, _22, _23
		m_Up.y = m_Rot.m[1][1];
		m_Up.z = m_Rot.m[1][2];

		m_Forward.x = m_Rot.m[2][0]; // _31, _32, _33
		m_Forward.y = m_Rot.m[2][1];
		m_Forward.z = m_Rot.m[2][2];

		m_IsWorldUpdated = false;
	}

	void Transform::OrbitAroundAxis(XMFLOAT3& center, XMFLOAT3& axis, float radius, float angle)
	{
		XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMVECTOR right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

		XMVECTOR nAxis = XMVector3Normalize(XMLoadFloat3(&axis));
		float d = XMVectorGetX(XMVector3Dot(nAxis, up));
		XMVECTOR ref = fabsf(d) > 0.99f ? right : up;
		XMVECTOR radialDir = XMVector3Normalize(XMVector3Cross(nAxis, ref));
		XMVECTOR radial = XMVectorScale(radialDir, radius);
		XMMATRIX r = XMMatrixRotationAxis(nAxis, angle);
		XMVECTOR rotatedRadial = XMVector3TransformNormal(radial, r);
		XMVECTOR position = XMVectorAdd(XMLoadFloat3(&center), rotatedRadial);
		SetPosition(XMVectorGetX(position), XMVectorGetY(position), XMVectorGetZ(position));

		// Marquer la matrice monde/inverse comme à recalculer
		m_IsWorldUpdated = false;
		m_IsInvWorldUpdated = false;
	}

	void Transform::SetYPR(float _yaw, float _pitch, float _roll)
	{
		ResetRotation();
		m_Yaw = 0.0f;
		m_Pitch = 0.0f;
		m_Roll = 0.0f;
		AddYPR(_yaw, _pitch, _roll);

	}
	void Transform::AddYPR(float _yaw, float _pitch, float _roll)
	{
		if (_yaw == 0.0f && _pitch == 0.0f && _roll == 0.0f)
			return;
		ResetRotation();

		XMVECTOR axisForward = XMLoadFloat3(&m_Forward);
		XMVECTOR axisRight = XMLoadFloat3(&m_Right);
		XMVECTOR axisUp = XMLoadFloat3(&m_Up);
		XMVECTOR quatRot = XMLoadFloat4(&m_Quat);

		m_Roll += _roll;
		quatRot = XMQuaternionMultiply(quatRot, XMQuaternionRotationAxis(axisForward, m_Roll));

		m_Pitch += _pitch;
		quatRot = XMQuaternionMultiply(quatRot, XMQuaternionRotationAxis(axisRight, m_Pitch));

		m_Yaw += _yaw;
		quatRot = XMQuaternionMultiply(quatRot, XMQuaternionRotationAxis(axisUp, m_Yaw));

		quatRot = XMQuaternionNormalize(quatRot);
		XMStoreFloat4(&m_Quat, quatRot);

		SetRotationFromQuaternion();
	}

	void Transform::AddYaw(float yaw)
	{
		AddYPR(yaw, 0.0f, 0.0f);
	}
	void Transform::AddPitch(float pitch)
	{
		AddYPR(0.0f, pitch, 0.0f);
	}
	void Transform::AddRoll(float roll)
	{
		AddYPR(0.0f, 0.0f, roll);
	}

}