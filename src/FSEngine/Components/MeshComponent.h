#pragma once

namespace FSE
{

	class MeshComponent : public Component
	{
		virtual void Reset() override;

		Object3D* m_Object3D = nullptr;
		MeshGeometry* m_MeshData = nullptr;

		void MoveObjectToCorrectRenderVector(Material* mat);

	public:
		MeshComponent();

		Object3D* Get3DObject() { return m_Object3D; }
		MeshGeometry* GetMeshData() { return m_MeshData; }

		bool SetGeometry(MeshGeometry* mesh);
		bool SetMaterial(std::string name);
		void SetColor(XMFLOAT4 color) { m_Object3D->SetColor(color); };
		void SetWorld(XMFLOAT4X4 world) { m_Object3D->SetWorld(world); };

		friend class ECS;
	};

}

