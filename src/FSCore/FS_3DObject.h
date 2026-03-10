#pragma once

class FS_3DObject
{
public: //Debug

	XMFLOAT4X4 mWorld = Maths::Identity4x4();
	MeshGeometry* mGeo = nullptr; 

	XMFLOAT4 mColor = {1.0f, 1.0f, 1.0f, 1.0f};
	UINT mMaterialIndex = 0;

	bool misActive = true;

	FS_3DObject() = default;
public:
	FS_3DObject(MeshGeometry* geo);
	~FS_3DObject() {};

	bool IsActive() { return misActive; };
	MeshGeometry* GetMesh() { return mGeo; }
	XMFLOAT4X4& GetWorld() { return mWorld; }
	UINT GetMaterialIndex() { return mMaterialIndex; };

	void SetColor(XMFLOAT4 color) { mColor = color; };
	XMFLOAT4 GetColor() { return mColor; }
	void SetWorld(XMFLOAT4X4 world) { mWorld = world; };
	void SetActive(bool isActive) { misActive = isActive; };
};

