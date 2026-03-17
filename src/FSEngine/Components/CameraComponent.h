#pragma once

class CameraComponent : public Component
{
	virtual void Reset() override;

	XMFLOAT3 m_rotation = { 0,0,0 };
	XMFLOAT3 m_limitMin = { -FLOAT_MAX, -FLOAT_MAX, -FLOAT_MAX };
	XMFLOAT3 m_limitMax = { FLOAT_MAX, FLOAT_MAX, FLOAT_MAX };
	bool m_IsMainCamera = false;

public:
	CameraComponent();

	void Update(XMFLOAT4X4 worldMtraix);

	bool IsMainCamera() { return m_IsMainCamera; }

	void RotateRad(float yaw, float pitch, float roll);
	void SetRotationRad(float yaw, float pitch, float roll);

	void SetRotationLimitYaw(float min, float max) { m_limitMin.x = min; m_limitMax.x = max; }
	void SetRotationLimitPitch(float min, float max) { m_limitMin.y = min; m_limitMax.y = max; }
	void SetRotationLimitRoll(float min, float max) { m_limitMin.z = min; m_limitMax.z = max; }

	void SetMainCamera(bool isMain);
	void SetLens(float fovY, float aspect, float zn, float zf) = delete;

	friend class ECS;
};


