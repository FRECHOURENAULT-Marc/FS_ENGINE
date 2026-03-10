#include "pch.h"


void MeshComponent::Reset()
{
	if (m_3dObject != nullptr)
		FS_Device::Get()->RemoveObject(m_3dObject);
	m_3dObject = nullptr;
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
		return false;
	m_3dObject->mMaterialIndex = matIndex;
	return true;
}
