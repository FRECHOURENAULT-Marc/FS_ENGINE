#pragma once

#define DEFAULT_TEXTURE_PATH L"../../res/Textures/WoodCrate01.dds"
#define DEFAULT_TEXTURE_NAME "woodCrateTex"

namespace FSR
{
	struct Device
	{
		Device(HINSTANCE hInstance);
		~Device();

		bool Initialize();
		static void Destroy() { delete(m_Device); }

		static Device* Get() { return m_Device; }
		static RendererManager* GetRenderer() { return Get()->m_Renderer; }
		static CommandList* GetCommand() { return Get()->m_Cmd; }
		static Camera3D* GetCamera() { return Get()->m_Camera; };
		static SwapChain* GetSwapChain() { return Get()->m_SwapChain; };
		static Window* GetWindow() { return Get()->m_Window; };
		static Microsoft::WRL::ComPtr<ID3D12Device> GetD3DDevice() { return Get()->m_D3dDevice; }
		static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetRtvHeap() { return Get()->m_RtvHeap; }
		static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDsvHeap() { return Get()->m_DsvHeap; }
		static UINT GetRtvDescriptorSize() { return Get()->m_RtvDescriptorSize; }
		static UINT GetDsvDescriptorSize() { return Get()->m_DsvDescriptorSize; }
		static Microsoft::WRL::ComPtr<IDXGIFactory4> GetDxgiFactory() { return Get()->m_DxgiFactory; }

		void AddObject(Object3D* object);
		bool RemoveObject(Object3D* object);
		Object2D* AddSprite(std::string texName, bool isMat = false);
		void RemoveSprite(Object2D* sprite);

		Texture* UploadingTexture(std::string _name, std::wstring _path);
		LRESULT ProcessWindowMessages(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		bool      m_4xMsaaState = false;
		UINT      m_4xMsaaQuality = 0;
	private:

		bool InitDirect3D();
		void CreateRtvAndDsvDescriptorHeaps();

		inline static Device* m_Device = nullptr;

		TextureManager* m_TextureManager;
		CommandList* m_Cmd;
		Window* m_Window;
		RendererManager* m_Renderer;
		Camera3D* m_Camera;
		SwapChain* m_SwapChain;
		MaterialManager* m_MaterialManager;

		Microsoft::WRL::ComPtr<IDXGIFactory4> m_DxgiFactory;

		Microsoft::WRL::ComPtr<ID3D12Device> m_D3dDevice;


		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DsvHeap;
		UINT m_RtvDescriptorSize = 0;
		UINT m_DsvDescriptorSize = 0;
		UINT m_CbvSrvUavDescriptorSize = 0;
	};

}