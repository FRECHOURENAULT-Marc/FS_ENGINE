#include "pch.h"


void MeshComponent::Reset()
{
	if (m_3dObject != nullptr)
		FS_Device::Get()->RemoveObject(m_3dObject);
	m_3dObject = nullptr;
}

void MeshComponent::MoveObjectToCorrectRenderVector(FS_Material* mat)
{
	if (mat->Shader->mIsUI)
		return;

	if (mat->Shader->m_isTransparent)
	{
		FS_Device::Renderer()->MoveToAlphaRender(m_3dObject);
		return;
	}
	if (mat->Shader->m_isTransparent == false)
	{
		FS_Device::Renderer()->MoveToRender(m_3dObject);
		return;
	}
}

MeshComponent::MeshComponent() : Component()
{
}

bool MeshComponent::SetGeometry(MeshGeometry* mesh)
{
	if (ECS::Get().GetComponent<TransformComponent>(m_id)->IsActive() == false)
		return false;

	if(m_3dObject != nullptr)
		FS_Device::Get()->RemoveObject(m_3dObject);

	m_3dObject = new FS_3DObject(mesh);
	FS_Device::Get()->AddObject(m_3dObject);
	return true;
}

bool MeshComponent::SetMaterial(std::string name)
{
	int matIndex = MaterialManager::GetMaterialIndex(name);
	if (matIndex == -1)
	{
		std::cout << "MeshComponent::SetMaterial : Material " << name << " not found." << std::endl;
		return false;
	}
	m_3dObject->SetMaterialIndex(matIndex);

	FS_Material* mat = MaterialManager::GetMaterial(name);
	MoveObjectToCorrectRenderVector(mat);
	
	return true;
}
