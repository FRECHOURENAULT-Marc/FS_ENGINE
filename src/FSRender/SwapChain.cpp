#include "pch.h"

namespace FSR
{
	ID3D12Resource* SwapChain::CurrentBackBuffer() const
	{
		return m_SwapChainBuffer[m_CurrBackBuffer].Get();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::CurrentBackBufferView() const
	{
		Device* device = Device::Get();

		return CD3DX12_CPU_DESCRIPTOR_HANDLE(device->GetRtvHeap()->GetCPUDescriptorHandleForHeapStart(), m_CurrBackBuffer, device->GetRtvDescriptorSize());
	}

	D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::DepthStencilView() const
	{
		return Device::Get()->GetDsvHeap()->GetCPUDescriptorHandleForHeapStart();
	}

	void SwapChain::CreateSwapChain()
	{
		m_SwapChain.Reset();

		Device* device = Device::Get();

		DXGI_SWAP_CHAIN_DESC sd;
		sd.BufferDesc.Width = device->GetWindow()->Width();
		sd.BufferDesc.Height = device->GetWindow()->Height();
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferDesc.Format = m_BackBufferFormat;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.SampleDesc.Count = device->m_4xMsaaState ? 4 : 1;
		sd.SampleDesc.Quality = device->m_4xMsaaState ? (device->m_4xMsaaQuality - 1) : 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = SWAPCHAIN_BUFFER_COUNT;
		sd.OutputWindow = device->GetWindow()->GetWindowHandle();
		sd.Windowed = true;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		ThrowIfFailed(device->GetDxgiFactory()->CreateSwapChain(device->GetCommand()->m_CommandQueue.Get(), &sd, m_SwapChain.GetAddressOf()));
	}

	void SwapChain::OnResize()
	{
		Device* device = Device::Get();
		auto command = device->GetCommand();
		auto window = device->GetWindow();

		assert(device->GetD3DDevice());
		assert(m_SwapChain);
		assert(command->m_DirectCmdListAlloc);

		command->FlushCommandQueue();

		ThrowIfFailed(command->m_CommandList->Reset(command->m_DirectCmdListAlloc.Get(), nullptr));

		for (int i = 0; i < SWAPCHAIN_BUFFER_COUNT; ++i)
			m_SwapChainBuffer[i].Reset();
		m_DepthStencilBuffer.Reset();

		ThrowIfFailed(m_SwapChain->ResizeBuffers(SWAPCHAIN_BUFFER_COUNT, window->Width(), window->Height(), m_BackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

		m_CurrBackBuffer = 0;

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(device->GetRtvHeap()->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < SWAPCHAIN_BUFFER_COUNT; i++)
		{
			ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffer[i])));
			device->GetD3DDevice()->CreateRenderTargetView(m_SwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
			rtvHeapHandle.Offset(1, device->GetRtvDescriptorSize());
		}

		D3D12_RESOURCE_DESC depthStencilDesc;
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = window->Width();
		depthStencilDesc.Height = window->Height();
		depthStencilDesc.DepthOrArraySize = 1;
		depthStencilDesc.MipLevels = 1;

		depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

		depthStencilDesc.SampleDesc.Count = device->m_4xMsaaState ? 4 : 1;
		depthStencilDesc.SampleDesc.Quality = device->m_4xMsaaState ? (device->m_4xMsaaQuality - 1) : 0;
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = m_DepthStencilFormat;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;

		CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

		ThrowIfFailed(device->GetD3DDevice()->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			IID_PPV_ARGS(m_DepthStencilBuffer.GetAddressOf())));

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = m_DepthStencilFormat;
		dsvDesc.Texture2D.MipSlice = 0;

		device->GetD3DDevice()->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &dsvDesc, DepthStencilView());

		CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		command->m_CommandList->ResourceBarrier(1, &resourceBarrier);

		ThrowIfFailed(command->m_CommandList->Close());
		ID3D12CommandList* cmdsLists[] = { command->m_CommandList.Get() };
		command->m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		command->FlushCommandQueue();

		window->UpdateScreenViewport();

		device->GetCamera()->SetLens(0.25f * Maths::Pi, window->AspectRatio(), 1.0f, 1000.0f);

		device->GetRenderer()->UpdateWinSizeForSprites(window->Width(), window->Height());
	}
}
