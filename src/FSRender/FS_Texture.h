#pragma once

struct FS_Texture
{
	std::string Name;

	std::wstring Filename;

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;

	XMINT2 size = XMINT2(1, 1);

	UINT SrvHeapIndex = 0;
};

class TextureManager
{
	static inline TextureManager* instance;

	std::vector<FS_Texture*> m_Textures;
	std::vector<std::string> m_TexturesNames;

	FS_Texture* LoadTextureInMemory(std::string _name, std::wstring _path);

public:

	TextureManager();

	static TextureManager* Get() { return instance; }
	static FS_Texture* LoadTexture(std::string _name, std::wstring _path);

	static int GetTextureIndex(std::string _name);
	static FS_Texture* GetTexture(std::string _name) { return GetTexture(GetTextureIndex(_name)); }
	static FS_Texture* GetTexture(int index) { return instance->m_Textures[index]; }

	friend class FS_Device;
	friend class FS_Renderer;
};

