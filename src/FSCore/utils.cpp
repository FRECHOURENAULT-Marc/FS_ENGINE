#include "pch.h"

namespace utils
{
    bool MatrixDecompose(XMFLOAT4X4 matrix, XMFLOAT3* _scale, XMFLOAT3* _rotation, XMFLOAT3* _pos)
    {
		XMMATRIX M = XMLoadFloat4x4(&matrix);
		XMVECTOR scale;
		XMVECTOR rotation;
		XMVECTOR translation;
		bool isDecomposed = XMMatrixDecompose(&scale, &rotation, &translation, M);

		if (isDecomposed == false)
		{
			std::cout << "utils::MatrixDecompose : Failed to decompose m_World matrix" << std::endl;
			return false;
		}

		XMFLOAT4 q;
		XMStoreFloat4(&q, rotation);

		float ysqr = q.y * q.y;

		float t0 = 2.0f * (q.w * q.z + q.x * q.y);
		float t1 = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
		float roll = atan2f(t0, t1);

		float t2 = 2.0f * (q.w * q.x - q.z * q.y);
		t2 = std::clamp(t2, -1.0f, 1.0f);
		float pitch = asinf(t2);

		float t3 = 2.0f * (q.w * q.y + q.x * q.z);
		float t4 = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
		float yaw = atan2f(t3, t4);

		_rotation->x = yaw;
		_rotation->y = pitch;
		_rotation->z = roll;

		XMFLOAT3 pos;
		XMStoreFloat3(&pos, translation);
		_pos->x = pos.x;
		_pos->y = pos.y;
		_pos->z = pos.z;

		XMFLOAT3 scl;
		XMStoreFloat3(&scl, scale);
		_scale->x = scl.x;
		_scale->y = scl.y;
		_scale->z = scl.z;

		return true;
    }
	bool MatrixDecomposeRot(XMFLOAT4X4 matrix, XMFLOAT3* _rotation)
	{
		XMMATRIX M = XMLoadFloat4x4(&matrix);
		XMVECTOR scale;
		XMVECTOR rotation;
		XMVECTOR translation;
		bool isDecomposed = XMMatrixDecompose(&scale, &rotation, &translation, M);

		if (isDecomposed == false)
		{
			std::cout << "utils::MatrixDecomposeRot : Failed to decompose m_World matrix" << std::endl;
			return false;
		}

		XMFLOAT4 q;
		XMStoreFloat4(&q, rotation);

		float ysqr = q.y * q.y;

		float t0 = 2.0f * (q.w * q.z + q.x * q.y);
		float t1 = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
		float roll = atan2f(t0, t1);

		float t2 = 2.0f * (q.w * q.x - q.z * q.y);
		t2 = std::clamp(t2, -1.0f, 1.0f);
		float pitch = asinf(t2);

		float t3 = 2.0f * (q.w * q.y + q.x * q.z);
		float t4 = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
		float yaw = atan2f(t3, t4);

		_rotation->x = yaw;
		_rotation->y = pitch;
		_rotation->z = roll;

		return true;
	}
}