#pragma once

class FS_3DCamera
{
	//<Camera----
	std::unique_ptr<UploadBuffer<CameraOC3D>> mCB = nullptr;

	Transform m_transform;

	XMFLOAT4X4 m_View3D = Maths::Identity4x4();
	XMFLOAT4X4 m_Proj = Maths::Identity4x4();
	XMFLOAT4X4 m_Ortho = Maths::Identity4x4();
	XMFLOAT4X4 m_viewProj = Maths::Identity4x4();
	XMFLOAT4X4 m_viewProj2D = Maths::Identity4x4();

	float mNearZ = 0.0f;
	float mFarZ = 0.0f;
	float mAspect = 0.0f;
	float mFovY = 0.0f;
	float mNearWindowHeight = 0.0f;
	float mFarWindowHeight = 0.0f;

	bool m_IsDirty = true;
	bool m_isUpdatedThisFrame = true;

	void MakeDirty();

public:
	FS_3DCamera();

	void Update();
	void AfterUpdate();
	XMFLOAT3 GetPosition();
	XMFLOAT4X4 GetView() { return m_View3D; }
	XMFLOAT4X4 GetProj() { return m_Proj; }
	XMFLOAT4X4 GetViewProj() { return m_viewProj; }
	XMFLOAT4X4 GetViewProj2D() { return m_viewProj2D; }
	Transform GetTransform() { return m_transform; }
	bool IsUpdatedThisFrame() { return m_isUpdatedThisFrame; }

	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& v);
	void SetWorld(XMFLOAT4X4 world);
	void SetUpdatedThisFrame(bool updated) { m_isUpdatedThisFrame = updated; }
	
	void UpdateMatrix();

	void SetLens(float fovY, float aspect, float zn, float zf);

};