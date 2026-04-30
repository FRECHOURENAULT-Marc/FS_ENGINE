#include "pch.h"

namespace FSR
{
	TextureManager::TextureManager()
	{
		if (m_Instance)
			return;
		m_Instance = this;
	};

	Texture* TextureManager::LoadTextureInMemory(std::string _name, std::wstring _path, bool& _isNew)
	{
		//If Texture already exist, send the one who already exist
		_isNew = true;
		for (int i = 0; i < m_Instance->m_TexturesNames.size(); i++)
		{
			if (m_Instance->m_TexturesNames[i] != _name)
				continue;
			_isNew = false;
			return m_Instance->m_Textures[i];
		}

		auto NewTexture = new Texture;

		NewTexture->m_Name = _name;
		NewTexture->m_Filename = _path;
		SIZE sizeOut = {};

		ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(
			Device::GetD3DDevice().Get(),
			Device::GetCommand()->m_CommandList.Get(), NewTexture->m_Filename.c_str(),
			NewTexture->m_Resource, NewTexture->m_UploadHeap, 0, nullptr, &sizeOut));

		NewTexture->m_Size = { sizeOut.cx, sizeOut.cy };

		m_Instance->m_Textures.push_back(NewTexture);
		m_Instance->m_TexturesNames.push_back(NewTexture->m_Name);

		return NewTexture;
	}

	Texture* TextureManager::LoadTexture(std::string _name, std::wstring _path)
	{
		return Device::Get()->UploadingTexture(_name, _path);
	}

	int TextureManager::GetTextureIndex(std::string _name)
	{
		auto& texturesNames = Get()->m_TexturesNames;
		for (int i = 0; i < texturesNames.size(); i++)
		{
			auto& name = texturesNames[i];
			if (name != _name)
				continue;
			return i;
		}
		return -1;
	}
}