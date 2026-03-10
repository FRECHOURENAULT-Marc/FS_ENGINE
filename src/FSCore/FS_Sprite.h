#pragma once

class FS_Sprite
{
protected:
	Transform m_transform;

	XMFLOAT4 mMask = { 0.0f, 0.0f, 1000000.0f, 1000000.0f };

	MeshGeometry* mGeo = nullptr; 

	XMINT2 winSize = XMINT2(1, 1);
	XMINT2 mSize = XMINT2(1, 1);

	UINT mMaterialIndex = 0;

	bool misActive = true;

	FS_Sprite();
public:
	~FS_Sprite() {};

	bool IsActive() { return misActive; };
	MeshGeometry* GetMesh() { return mGeo; }
	XMFLOAT4X4 GetWorld() { return m_transform.GetWorld(); }
	XMFLOAT4 GetMask() { return mMask; }
	UINT GetMaterialIndex() { return mMaterialIndex; };

	// Set the position of the sprite on the screen (in pixels)
	void SetPosition(float x, float y);
	// Set the scaling of the sprite
	// Scale assumed the sprite is square
	void SetScaling(float scale);
	// Set the mask for the sprite (in pixels)
	// (x,y) is the top-left corner of the mask
	void SetMask(float x, float y, float width, float height);

	void SetActive(bool isActive) { misActive = isActive; };

	friend class SpriteGenerator;
	friend class FS_Text;
};

