#pragma once

//(en faire variable globale / get ? )
constexpr int SwapChainBufferCount = 2; // utilis dans OnResize(), CreateSwapChain(), CreateRtvAndDsvDescriptorHeaps() et Draw()

class FS_SwapChain
{
public:

	Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain; // créé dans CreateSwapChain() ### utilisé dans OnResize() et Draw()
	int m_CurrBackBuffer = 0; // assigné dans OnResize() et Draw() ### utilisé dans CurrentBackBuffer() et CurrentBackBufferView()

	Microsoft::WRL::ComPtr<ID3D12Resource> m_SwapChainBuffer[SwapChainBufferCount]; // utilisé dans OnResize() !!! possède un get
	Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer; // utilisé dans OnResize()
	DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM; // utilisé dans CreateSwapChain(), BuildPSO(), InitDirect3D() et OnResize()
	DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; // utilisé dans BuildPSO() et OnResize()

	void CreateSwapChain();

	void OnResize();

	ID3D12Resource* CurrentBackBuffer()const;

	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;

	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;
};

