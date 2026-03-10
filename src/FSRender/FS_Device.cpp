#include "pch.h"

FS_Texture* FS_Device::UploadingTexture(std::string _name, std::wstring _path)
{
	ThrowIfFailed(
		m_FSCmd->mCommandList->Reset(
			m_FSCmd->mDirectCmdListAlloc.Get(),
			nullptr
		)
	);

	// Load texture (the loader/DirectX helper records COPY and transitions on the same command list)
	auto tex = m_FSTextures->LoadTextureInMemory(_name, _path);

	// The loader already recorded COPY_DEST -> COPY_DEST on the command list, so no need to do it again here.

	// close/execute/flush
	ThrowIfFailed(m_FSCmd->mCommandList->Close());

	ID3D12CommandList* cmdsLists[] =
	{
		m_FSCmd->mCommandList.Get()
	};

	m_FSCmd->mCommandQueue->ExecuteCommandLists(
		_countof(cmdsLists),
		cmdsLists);

	m_FSCmd->FlushCommandQueue();

	m_FSRenderer->BuildTextureSRV(tex->Resource.Get());

	return tex;
}

//FS_Device* FS_Device::m_Device = nullptr;

FS_Device::FS_Device(HINSTANCE hInstance)
{
	assert(m_Device == nullptr);	
	m_Device = this;
	m_FSWindow = new FS_Window(hInstance);
}

FS_Device::~FS_Device()
{
	delete(m_FSCmd);
	delete(m_FSSwapChain);
	delete(m_FSTextures);
	delete(m_FSCamera);
	delete(m_FSRenderer);
	delete(m_FSWindow);

	m_dxgiFactory.Reset();
	m_d3dDevice.Reset();

	m_RtvHeap.Reset();
	m_DsvHeap.Reset();

	for(auto& nameAndGeo : m_Geometries) 
		nameAndGeo.second.reset(); // détruit l'objet et met unique_ptr = nullptr
	m_Geometries.clear();
	for (auto& nameAndMat : m_Materials)
		nameAndMat.second.reset(); // détruit l'objet et met unique_ptr = nullptr
	m_Materials.clear();
}

bool FS_Device::InitDirect3D()
{
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory)));

	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,            
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&m_d3dDevice));

	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_d3dDevice)));
	}

	m_FSCmd = new FS_Command();

	ThrowIfFailed(m_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&m_FSCmd->m_Fence)));

	m_RtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_DsvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_CbvSrvUavDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = m_FSSwapChain->m_BackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(m_d3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));

	m_4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(m_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

	m_FSCmd->CreateCommandObjects(m_d3dDevice.Get());
	m_FSSwapChain->CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();

	return true;
}

void FS_Device::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(m_RtvHeap.GetAddressOf())));


	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(m_DsvHeap.GetAddressOf())));
}

FS_Sprite* FS_Device::AddSprite(std::string texName)
{
	return m_FSRenderer->AddSprite(texName);
}
void FS_Device::RemoveSprite(FS_Sprite* sprite)
{
	m_FSRenderer->RemoveSprite(sprite);
}

void FS_Device::AddObject(FS_3DObject* object)
{
	m_FSRenderer->AddObject(object);
}
bool FS_Device::RemoveObject(FS_3DObject* object)
{
	return m_FSRenderer->RemoveObject(object);
}

bool FS_Device::Initialize()
{
	if (!m_FSWindow->InitMainWindow())
		return false;

	m_FSSwapChain = new FS_SwapChain();

	if (!InitDirect3D())
		return false;

	m_FSCamera = new FS_3DCamera();
	m_FSTextures = new TextureManager();
	m_FSRenderer = new FS_Renderer();
	m_FSMaterial = new MaterialManager();

	m_FSRenderer->BuildSrvHeap();
	m_FSSwapChain->OnResize();


	// ----- CONSTANT BUFFERS -----
	//

	m_FSRenderer->CreateCBCameraAndPass();

	//
	// ----- ROOT + SHADERS + PSO -----
	//

	m_FSRenderer->BuildRootSignature();

	//SHADER TEXTURE
	std::vector<D3D12_INPUT_ELEMENT_DESC> LayoutTextureShader =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	m_FSRenderer->BuildShaderAndPSO(SHADER_TEXTURE_NAME, L"..\\..\\res\\Shaders\\texture.hlsl", LayoutTextureShader, false);

	m_FSRenderer->BuildShaderAndPSO(SHADER_TEXTURE_A_NAME, L"..\\..\\res\\Shaders\\texture.hlsl", LayoutTextureShader, true);

	m_FSRenderer->BuildShaderAndPSO(SHADER_SPRITE_NAME, L"..\\..\\res\\Shaders\\sprite.hlsl", LayoutTextureShader, true, true);

	//SHADER COLOR
	std::vector<D3D12_INPUT_ELEMENT_DESC> LayoutColorShader =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	m_FSRenderer->BuildShaderAndPSO(SHADER_COLOR_NAME, L"..\\..\\res\\Shaders\\color.hlsl", LayoutColorShader, false);
	m_FSRenderer->BuildShaderAndPSO(SHADER_COLOR_A_NAME, L"..\\..\\res\\Shaders\\color.hlsl", LayoutColorShader, true);

	// SHADER COLORED TEXTURE
	std::vector<D3D12_INPUT_ELEMENT_DESC> LayoutColorTextureShader =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	m_FSRenderer->BuildShaderAndPSO(SHADER_COLOREDTEXTURE_NAME, L"..\\..\\res\\Shaders\\textureColor.hlsl", LayoutColorTextureShader, false);
	
	// DEFAULT COLOR MATERIAL
	MaterialManager::CreateMaterial(SHADER_COLOR_NAME, MATERIAL_DEFAULT_COLOR_NAME);
	MaterialManager::CreateMaterial(SHADER_COLOR_A_NAME, MATERIAL_DEFAULT_COLOR_A_NAME);
	return true;
}

LRESULT FS_Device::ProcessWindowMessages(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return m_FSWindow->MsgProc(hwnd, msg, wParam, lParam);
}

