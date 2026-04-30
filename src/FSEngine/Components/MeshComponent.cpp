#include "pch.h"

namespace FSE
{

	void MeshComponent::Reset()
	{
		if (m_Object3D != nullptr)
			Device::Get()->RemoveObject(m_Object3D);
		m_Object3D = nullptr;
	}

	void MeshComponent::MoveObjectToCorrectRenderVector(Material* mat)
	{
		if (mat->Shader->IsUI())
			return;

		if (mat->Shader->IsTransparent())
		{
			Device::GetRenderer()->MoveToAlphaRender(m_Object3D);
			return;
		}
		if (mat->Shader->IsTransparent()== false)
		{
			Device::GetRenderer()->MoveToRender(m_Object3D);
			return;
		}
	}

	MeshComponent::MeshComponent() : Component()
	{
	}

	bool MeshComponent::SetGeometry(MeshGeometry* mesh)
	{
		if (ECS::Get().GetComponent<TransformComponent>(m_ID)->IsActive() == false)
			return false;

		if (m_Object3D != nullptr)
			Device::Get()->RemoveObject(m_Object3D);

		m_MeshData = mesh;
		m_Object3D = new Object3D(mesh);
		Device::Get()->AddObject(m_Object3D);
		return true;
	}

	bool MeshComponent::SetMaterial(std::string name)
	{
		int matIndex = MaterialManager::GetMaterialIndex(name);
		if (matIndex == -1)
		{
			D::Cout("MeshComponent::SetTexture : Material " + name + " not found.");
			return false;
		}
		m_Object3D->SetMaterialIndex(matIndex);

		Material* mat = MaterialManager::GetMaterial(name);
		MoveObjectToCorrectRenderVector(mat);

		return true;
	}
}

