#include "pch.h"

TextureManager::TextureManager()
{
	if (instance) 
		return; 
	instance = this;
};

FS_Texture* TextureManager::LoadTextureInMemory(std::string _name, std::wstring _path)
{
	auto NewTexture = new FS_Texture;

	NewTexture->Name = _name;
	NewTexture->Filename = _path;
	SIZE sizeOut = {};

	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(
		FS_Device::Get()->m_d3dDevice.Get(),
		FS_Device::Get()->m_FSCmd->mCommandList.Get(), NewTexture->Filename.c_str(),
		NewTexture->Resource, NewTexture->UploadHeap, 0, nullptr, &sizeOut));

	NewTexture->size = { sizeOut.cx, sizeOut.cy };

	instance->m_Textures.push_back(NewTexture);
	instance->m_TexturesNames.push_back(NewTexture->Name);

	return NewTexture;
}

FS_Texture* TextureManager::LoadTexture(std::string _name, std::wstring _path)
{
	return FS_Device::Get()->UploadingTexture(_name, _path);
}

int TextureManager::GetTextureIndex(std::string _name)
{
	auto& texturesNames = Get()->m_TexturesNames;
	for(int  i = 0; i < texturesNames.size(); i++)
	{
		auto& name = texturesNames[i];
		if (name != _name)
			continue;
		return i;
	}
	return -1;
}
