#include "pch.h"

void MaterialManager::CreateMaterial(std::string shaderName, std::string materialName, std::string textureName, float roughness, XMFLOAT3 metallic, XMFLOAT4 diffuseAlbedo)
{
	FS_Material* mat = new FS_Material();
	mat->Shader = FS_Device::Renderer()->GetShader(shaderName);
	int textureIndex = TextureManager::GetTextureIndex(textureName);
	if(textureIndex != -1)
		mat->TextureIndex = textureIndex;

	mat->Roughness = roughness;
	mat->Metallic = metallic;
	mat->DiffuseAlbedo = diffuseAlbedo;

	instance->m_MaterialLibrary.push_back(mat);
	instance->m_MaterialNames.push_back(materialName);
}

int MaterialManager::GetMaterialIndex(std::string materialName)
{
	auto& matNames = Get()->m_MaterialNames;
	for(int i = 0; i < matNames.size(); i++)
	{
		std::string& name = matNames[i];
		if (name != materialName)
			continue;
		return i;
	}
	return -1;
}

FS_Material* MaterialManager::GetMaterial(std::string materialName)
{
	int index = GetMaterialIndex(materialName);
	if(index == -1)
		return nullptr;

	return Get()->m_MaterialLibrary[index];
}

FS_Material* MaterialManager::GetMaterial(int materialIndex)
{
	return Get()->m_MaterialLibrary[materialIndex];
}
