#include "pch.h"

ID3D12Resource* FS_SwapChain::CurrentBackBuffer() const
{
	return m_SwapChainBuffer[m_CurrBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE FS_SwapChain::CurrentBackBufferView() const
{
	FS_Device* device = FS_Device::Get();
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(device->m_RtvHeap->GetCPUDescriptorHandleForHeapStart(),m_CurrBackBuffer, device->m_RtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE FS_SwapChain::DepthStencilView() const
{
	return FS_Device::Get()->m_DsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void FS_SwapChain::CreateSwapChain()
{
	m_SwapChain.Reset();

	FS_Device* device = FS_Device::Get();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = device->m_FSWindow->Width();
	sd.BufferDesc.Height = device->m_FSWindow->Height();
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = m_BackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = device->m_4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = device->m_4xMsaaState ? (device->m_4xMsaaQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.OutputWindow = device->m_FSWindow->GetWindowHandle();
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(device->m_dxgiFactory->CreateSwapChain(device->m_FSCmd->mCommandQueue.Get(),&sd,m_SwapChain.GetAddressOf()));
}

void FS_SwapChain::OnResize()
{
	FS_Device* device = FS_Device::Get();

	assert(device->m_d3dDevice);
	assert(m_SwapChain);
	assert(device->m_FSCmd->mDirectCmdListAlloc);

	device->m_FSCmd->FlushCommandQueue();

	ThrowIfFailed(device->m_FSCmd->mCommandList->Reset(device->m_FSCmd->mDirectCmdListAlloc.Get(), nullptr));

	for (int i = 0; i < SwapChainBufferCount; ++i)
		m_SwapChainBuffer[i].Reset();
	m_DepthStencilBuffer.Reset();

	ThrowIfFailed(m_SwapChain->ResizeBuffers(SwapChainBufferCount, device->m_FSWindow->Width(), device->m_FSWindow->Height(),m_BackBufferFormat,DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	m_CurrBackBuffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(device->m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffer[i])));
		device->m_d3dDevice->CreateRenderTargetView(m_SwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, device->m_RtvDescriptorSize);
	}

	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = device->m_FSWindow->Width();
	depthStencilDesc.Height = device->m_FSWindow->Height();
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

	ThrowIfFailed(device->m_d3dDevice->CreateCommittedResource(
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

	device->m_d3dDevice->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &dsvDesc, DepthStencilView());

	CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	device->m_FSCmd->mCommandList->ResourceBarrier(1, &resourceBarrier);

	ThrowIfFailed(device->m_FSCmd->mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { device->m_FSCmd->mCommandList.Get() };
	device->m_FSCmd->mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	device->m_FSCmd->FlushCommandQueue();

	device->m_FSWindow->UpdateScreenViewport();

	device->m_FSCamera->SetLens(0.25f * Maths::Pi, device->m_FSWindow->AspectRatio(), 1.0f, 1000.0f);
}
