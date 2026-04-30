#include "pch.h"

namespace FSR 
{
	Material* MaterialManager::CreateMaterial(std::string shaderName, std::string materialName, std::string textureName, float roughness, XMFLOAT3 metallic, XMFLOAT4 diffuseAlbedo)
	{
		Material* mat = new Material();
		mat->Shader = Device::GetRenderer()->GetShader(shaderName);
		int textureIndex = TextureManager::GetTextureIndex(textureName);
		if (textureIndex != -1)
			mat->TextureIndex = textureIndex;

		mat->m_Roughness = roughness;
		mat->m_Metallic = metallic;
		mat->m_DiffuseAlbedo = diffuseAlbedo;

		instance->m_MaterialLibrary.push_back(mat);
		instance->m_MaterialNames.push_back(materialName);
		return mat;
	}

	int MaterialManager::GetMaterialIndex(std::string materialName)
	{
		auto& matNames = Get()->m_MaterialNames;
		for (int i = 0; i < matNames.size(); i++)
		{
			std::string& name = matNames[i];
			if (name != materialName)
				continue;
			return i;
		}
		return -1;
	}

	int MaterialManager::GetMaterialIndexFromTextureID(int textureID)
	{
		auto& materials = Get()->m_MaterialLibrary;
		for (int i = 0; i < materials.size(); i++)
		{
			auto& mat = materials[i];
			if (mat->TextureIndex != textureID)
				continue;
			return i;
		}
		return -1;
	}

	Material* MaterialManager::GetMaterial(std::string materialName)
	{
		int index = GetMaterialIndex(materialName);
		if (index == -1)
			return nullptr;

		return Get()->m_MaterialLibrary[index];
	}

	Material* MaterialManager::GetMaterial(int materialIndex)
	{
		return Get()->m_MaterialLibrary[materialIndex];
	}
}

