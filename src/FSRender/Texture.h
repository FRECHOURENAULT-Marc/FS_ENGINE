#pragma once

namespace FSR
{
	struct Texture
	{
		std::string m_Name;

		std::wstring m_Filename;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_UploadHeap = nullptr;

		XMINT2 m_Size = XMINT2(1, 1);

		UINT m_SrvHeapIndex = 0;
	};

	class TextureManager
	{
		static inline TextureManager* m_Instance;

		std::vector<Texture*> m_Textures;
		std::vector<std::string> m_TexturesNames;

		Texture* LoadTextureInMemory(std::string _name, std::wstring _path, bool& _isNew);

	public:

		TextureManager();

		static TextureManager* Get() { return m_Instance; }
		static Texture* LoadTexture(std::string _name, std::wstring _path);

		static int GetTextureIndex(std::string _name);
		static Texture* GetTexture(std::string _name) { return GetTexture(GetTextureIndex(_name)); }
		static Texture* GetTexture(int index) { return m_Instance->m_Textures[index]; }

		friend class Device;
		friend class RendererManager;
	};
}

