#pragma once

class FS_3DObject
{
protected:

	XMFLOAT4X4 mWorld = Maths::Identity4x4();
	MeshGeometry* mGeo = nullptr; 

	XMFLOAT4 mColor = {1.0f, 1.0f, 1.0f, 1.0f};
	UINT mMaterialIndex = 0;

	float m_distanceToCamera = 0.0f; //For sorting transparent objects

	bool m_isDirty = false;
	bool m_isActive = true;

	FS_3DObject() = default;
public:
	FS_3DObject(MeshGeometry* geo);
	~FS_3DObject() {};

	bool IsActive() { return m_isActive; };
	bool IsDirty() { return m_isDirty; };
	MeshGeometry* GetMesh() { return mGeo; }
	XMFLOAT4X4 GetWorld() { return mWorld; }
	XMFLOAT4 GetColor() { return mColor; }
	float GetDistanceToCamera() { return m_distanceToCamera; };
	UINT GetMaterialIndex() { return mMaterialIndex; };

	void SetColor(XMFLOAT4 color);
	void SetWorld(XMFLOAT4X4 world);
	void SetMaterialIndex(int index);
	void SetDistanceToCamera(float distance) { m_distanceToCamera = distance; };
	void SetActive(bool isActive) { m_isActive = isActive; };
	void SetDirty(bool isDirty) { m_isDirty = isDirty; };
};

