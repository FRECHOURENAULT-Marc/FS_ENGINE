#include "pch.h"


Transform::Transform() 
{
	Identity();
}
void Transform::Identity()
{
	pos = { 0, 0, 0 };
	sca = { 1, 1, 1 };
	ResetRotation();
}
XMFLOAT4X4 Transform::GetWorld()
{
	if (isWorldUpdated == false)
		UpdateWorld();

	return world;
}
XMFLOAT4X4 Transform::GetInvWorld()
{
	if (isInvWorldUpdated == false)
		UpdateInvWorld();
	return invWorld;
}

XMFLOAT3 Transform::GetYPR()
{
	return XMFLOAT3(yaw, pitch, roll);
}

void Transform::SetWorld(XMFLOAT4X4 _world)
{ 
	world = _world; 
	isWorldUpdated = false; 
	isInvWorldUpdated = false; 
}

void Transform::UpdateWorld()
{
	XMVECTOR _scale = XMLoadFloat3(&sca);
	XMVECTOR _pos	= XMLoadFloat3(&pos);

	XMVECTOR sx = XMVectorSplatX(_scale);
	XMVECTOR sy = XMVectorSplatX(_scale);
	XMVECTOR sz = XMVectorSplatX(_scale);

	XMMATRIX _world = XMLoadFloat4x4(&rot);
	_world.r[0] = XMVectorMultiply(_world.r[0], sx);
	_world.r[1] = XMVectorMultiply(_world.r[1], sy);
	_world.r[2] = XMVectorMultiply(_world.r[2], sz);
	_world.r[3] = XMVectorSetW(_pos, 1.0f);

	XMStoreFloat4x4(&world, _world);
	isWorldUpdated		= true;
	isInvWorldUpdated	= false;
}
void Transform::UpdateInvWorld()
{
	XMFLOAT4X4 _world = GetWorld();
	XMMATRIX _invWorld = XMMatrixInverse(nullptr, XMLoadFloat4x4(&_world));
	XMStoreFloat4x4(&invWorld, _invWorld);

	isInvWorldUpdated = true;
}

void Transform::SetScaling(float scale)
{
	sca.x = scale;
	sca.y = scale; 		
	sca.z = scale;
	isWorldUpdated = false;
}
void Transform::SetScaling(float sx, float sy, float sz)
{
	sca.x = sx;
	sca.y = sy;
	sca.z = sz;
	isWorldUpdated = false;
}
void Transform::Scale(float scale)
{
	sca.x *= scale; 
	sca.y *= scale; 
	sca.z *= scale;
	isWorldUpdated = false;
}
void Transform::Scale(float sx, float sy, float sz)
{
	sca.x *= sx;
	sca.y *= sy;
	sca.z *= sz;
	isWorldUpdated = false;
}

void Transform::SetPosition(float x, float y, float z)
{
	pos.x = x;
	pos.y = y;
	pos.z = z;
	isWorldUpdated = false;
}

void Transform::Move(float dist)
{
	pos.x += forward.x * dist;
	pos.y += forward.y * dist;
	pos.z += forward.z * dist;
	isWorldUpdated = false;
}

void Transform::ResetRotation()
{
	yaw		= 0.0f;
	pitch	= 0.0f;
	roll	= 0.0f;
	forward	= { 0.0f, 0.0f, 1.0f };
	right	= { 1.0f, 0.0f, 0.0f };
	up		= { 0.0f, 1.0f, 0.0f };
	quat	= { 0.0f, 0.0f, 0.0f, 1.0f };
	XMStoreFloat4x4(&rot, XMMatrixIdentity());
	isWorldUpdated = false;
}
void Transform::SetRotation(Transform& trs)
{
	yaw		= trs.yaw;
	pitch	= trs.pitch;
	roll	= trs.roll;
	forward	= trs.forward;
	right	= trs.right;
	up		= trs.up;
	quat	= trs.quat;
	rot		= trs.rot;
	isWorldUpdated = false;
}
void Transform::SetRotationFromQuaternion()
{
	XMStoreFloat4x4(&rot, XMMatrixRotationQuaternion(XMLoadFloat4(&quat)));

	right.x		= rot.m[0][0]; // _11, _12, _13
	right.y		= rot.m[0][1];
	right.z		= rot.m[0][2];

	up.x		= rot.m[1][0]; // _21, _22, _23
	up.y		= rot.m[1][1];
	up.z		= rot.m[1][2];

	forward.x	= rot.m[2][0]; // _31, _32, _33
	forward.y	= rot.m[2][1];
	forward.z	= rot.m[2][2];

	isWorldUpdated = false;
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
	isWorldUpdated = false;
	isInvWorldUpdated = false;
}

void Transform::SetYPR(float yaw, float pitch, float roll)
{
	ResetRotation();
	AddYPR(yaw, pitch, roll);

}
void Transform::AddYPR(float _yaw, float _pitch, float _roll)
{
	if (_yaw == 0.0f && _pitch == 0.0f && _roll == 0.0f)
		return;

	XMVECTOR axisForward= XMLoadFloat3(&forward);
	XMVECTOR axisRight = XMLoadFloat3(&right);
	XMVECTOR axisUp = XMLoadFloat3(&up);
	XMVECTOR quatRot = XMLoadFloat4(&quat);

	if (_roll)
	{
		roll += _roll;
		quatRot = XMQuaternionMultiply(quatRot, XMQuaternionRotationAxis(axisForward, _roll));
	}
	if (_pitch)
	{
		pitch += _pitch;
		quatRot = XMQuaternionMultiply(quatRot, XMQuaternionRotationAxis(axisRight, _pitch));
	}
	if (_yaw)
	{
		yaw += _yaw;
		quatRot = XMQuaternionMultiply(quatRot, XMQuaternionRotationAxis(axisUp, _yaw));
	}

	quatRot = XMQuaternionNormalize(quatRot);
	XMStoreFloat4(&quat, quatRot);

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
