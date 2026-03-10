#pragma once

struct FS_Device
{
	FS_Device(HINSTANCE hInstance);
	~FS_Device();

	static FS_Device* Get() { return m_Device; }
	static FS_Renderer* Renderer() { return Get()->m_FSRenderer; }
	static FS_Command* CommandList() { return Get()->m_FSCmd; }
	static FS_3DCamera* Camera() { return Get()->m_FSCamera; };
	static FS_SwapChain* SwapChain() { return Get()->m_FSSwapChain; };
	static FS_Window* Window() { return Get()->m_FSWindow; };

	bool InitDirect3D();
	FS_Texture* UploadingTexture(std::string _name, std::wstring _path);
	void CreateRtvAndDsvDescriptorHeaps();

	FS_Sprite* AddSprite(std::string texName);
	void RemoveSprite(FS_Sprite* sprite);

	void AddObject(FS_3DObject* object);
	bool RemoveObject(FS_3DObject* object);

	bool Initialize();
	static void Destroy() { delete(m_Device); }

	LRESULT ProcessWindowMessages(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	inline static FS_Device* m_Device = nullptr;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_Geometries;
	std::unordered_map<std::string, std::unique_ptr<FS_Material>> m_Materials;


	//FS_RENDER_PART
	TextureManager* m_FSTextures;
	FS_Command* m_FSCmd;
	FS_Window* m_FSWindow;
	FS_Renderer* m_FSRenderer;
	FS_3DCamera* m_FSCamera;
	FS_SwapChain* m_FSSwapChain;
	MaterialManager* m_FSMaterial;

	//<Device----
	Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory; // créé dans InitDirect3D() ### utilisé dans pour mSwapChain CreateSwapChain()

	Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice;// créé dans InitDirect3D() ### utilisé dans presque tous les Create() et Build(), aussi dans OnResize() et MsgProc()

	bool      m_4xMsaaState = false; 
	UINT      m_4xMsaaQuality = 0;      

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvHeap; // crée dans CreateRtvAndDsvDescriptorHeaps() ### utilisé pour la commande liste dans OnResize() !!! possède un get
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DsvHeap; // crée dans CreateRtvAndDsvDescriptorHeaps() ### utilisé pour la commande liste dans OnResize() !!! possède un get
	UINT m_RtvDescriptorSize = 0; // créé dans InitDirect3D() ### utilisé dans OnResize() et CurrentBackBufferView()
	UINT m_DsvDescriptorSize = 0; // créé dans InitDirect3D()
	UINT m_CbvSrvUavDescriptorSize = 0; // créé dans InitDirect3D()
	
	//----Device>
};
