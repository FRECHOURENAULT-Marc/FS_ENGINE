#pragma once

namespace FSC
{

	class RenderObject
	{
	protected:
		// Transform is used here (to replace XMFLOAT4X4) because Sprite are facing modifications more frequently than 3D objects
		XMFLOAT4X4 m_World = Maths::Identity4x4();
		MeshGeometry* m_Geo = nullptr;

		UINT m_MaterialIndex = 0; // DEFAULT COLOR MATERIAL (without texture and alpha)

		bool m_IsActive = true;
		bool m_IsDirty = true;

		void MakeDirty();
	public:
		RenderObject() = default;
		~RenderObject() {};

		bool IsActive() { return m_IsActive; };
		bool IsDirty() { return m_IsDirty; };
		MeshGeometry* GetMesh() { return m_Geo; }
		virtual XMFLOAT4X4 GetWorld() { return m_World; }
		UINT GetMaterialIndex() { return m_MaterialIndex; };

		void SetGeo(MeshGeometry* geo);
		void SetWorld(XMFLOAT4X4 world);
		void SetMaterialIndex(int matIndex);
		void SetActive(bool isActive) { m_IsActive = isActive; };
		void SetDirty(bool isDirty) { m_IsDirty = isDirty; };
	};

}