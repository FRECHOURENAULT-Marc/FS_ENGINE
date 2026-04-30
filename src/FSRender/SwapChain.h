#pragma once

#define SWAPCHAIN_BUFFER_COUNT 2 

namespace FSR
{
	class SwapChain
	{
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain; // créé dans CreateSwapChain() ### utilisé dans OnResize() et Draw()
		int m_CurrBackBuffer = 0; // assigné dans OnResize() et Draw() ### utilisé dans CurrentBackBuffer() et CurrentBackBufferView()

		Microsoft::WRL::ComPtr<ID3D12Resource> m_SwapChainBuffer[SWAPCHAIN_BUFFER_COUNT]; // utilisé dans OnResize() !!! possède un get
		Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer; // utilisé dans OnResize()
		DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM; // utilisé dans CreateSwapChain(), BuildPSO(), InitDirect3D() et OnResize()
		DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; // utilisé dans BuildPSO() et OnResize()

	public:
		void CreateSwapChain();

		Microsoft::WRL::ComPtr<IDXGISwapChain> GetSwapChain()const { return m_SwapChain; }
		int GetCurrBackBuffer()const { return m_CurrBackBuffer; }
		DXGI_FORMAT GetBackBufferFormat()const { return m_BackBufferFormat; }
		DXGI_FORMAT GetDepthStencilFormat()const { return m_DepthStencilFormat; }
		ID3D12Resource* CurrentBackBuffer()const;
		D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;

		void SetCurrBackBuffer(int curr) { m_CurrBackBuffer = curr; }

		void OnResize();
	};

}


