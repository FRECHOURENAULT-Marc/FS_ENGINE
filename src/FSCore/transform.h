#pragma once

struct Transform
{
	XMFLOAT3 pos;

	XMFLOAT3 sca;

	XMFLOAT3 forward;
	XMFLOAT3 right;
	XMFLOAT3 up;
	XMFLOAT4 quat;
	XMFLOAT4X4 rot;

	float yaw;
	float pitch;
	float roll;

private:
	bool isWorldUpdated;
	XMFLOAT4X4 world;
	bool isInvWorldUpdated;
	XMFLOAT4X4 invWorld;

public:
	Transform();

	XMFLOAT4X4 GetWorld();
	XMFLOAT4X4 GetInvWorld();
	XMFLOAT3 GetYPR();
	void SetWorld(XMFLOAT4X4 _world);

	void UpdateWorld();
	void UpdateInvWorld();

	bool IsUpdated() { return isWorldUpdated && isInvWorldUpdated; };

	void Identity();
	void SetScaling(float scale);
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

