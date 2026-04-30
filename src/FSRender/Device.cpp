#include "pch.h"

namespace FSR
{
	Texture* Device::UploadingTexture(std::string _name, std::wstring _path)
	{
		ThrowIfFailed(
			m_Cmd->m_CommandList->Reset(
				m_Cmd->m_DirectCmdListAlloc.Get(),
				nullptr
			)
		);

		// Load texture (the loader/DirectX helper records COPY and transitions on the same command list)
		bool isNewTexture = false;
		auto tex = m_TextureManager->LoadTextureInMemory(_name, _path, isNewTexture);

		// The loader already recorded COPY_DEST -> COPY_DEST on the command list, so no need to do it again here.

		// close/execute/flush
		ThrowIfFailed(m_Cmd->m_CommandList->Close());

		ID3D12CommandList* cmdsLists[] =
		{
			m_Cmd->m_CommandList.Get()
		};

		m_Cmd->m_CommandQueue->ExecuteCommandLists(
			_countof(cmdsLists),
			cmdsLists);

		m_Cmd->FlushCommandQueue();

		if (isNewTexture)
			m_Renderer->BuildTextureSRV(tex->m_Resource.Get());
		else
			std::cout << "Texture " << _name << " already loaded in memory. No need to upload to GPU." << std::endl;
			
		return tex;
	}

	Device::Device(HINSTANCE hInstance)
	{
		assert(m_Device == nullptr);
		m_Device = this;
		m_Window = new Window(hInstance);
	}

	Device::~Device()
	{
		delete(m_Cmd);
		delete(m_SwapChain);
		delete(m_TextureManager);
		delete(m_Camera);
		delete(m_Renderer);
		delete(m_Window);

		m_DxgiFactory.Reset();
		m_D3dDevice.Reset();

		m_RtvHeap.Reset();
		m_DsvHeap.Reset();
	}

	bool Device::InitDirect3D()
	{
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_DxgiFactory)));

		HRESULT hardwareResult = D3D12CreateDevice(
			nullptr,
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_D3dDevice));

		if (FAILED(hardwareResult))
		{
			ComPtr<IDXGIAdapter> pWarpAdapter;
			ThrowIfFailed(m_DxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

			ThrowIfFailed(D3D12CreateDevice(
				pWarpAdapter.Get(),
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&m_D3dDevice)));
		}

		m_Cmd = new CommandList();

		ThrowIfFailed(m_D3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&m_Cmd->m_Fence)));

		m_RtvDescriptorSize = m_D3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		m_DsvDescriptorSize = m_D3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		m_CbvSrvUavDescriptorSize = m_D3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
		msQualityLevels.Format = m_SwapChain->GetBackBufferFormat();
		msQualityLevels.SampleCount = 4;
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.NumQualityLevels = 0;
		ThrowIfFailed(m_D3dDevice->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&msQualityLevels,
			sizeof(msQualityLevels)));

		m_4xMsaaQuality = msQualityLevels.NumQualityLevels;
		assert(m_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

		m_Cmd->CreateCommandObjects(m_D3dDevice.Get());
		m_SwapChain->CreateSwapChain();
		CreateRtvAndDsvDescriptorHeaps();

		return true;
	}

	void Device::CreateRtvAndDsvDescriptorHeaps()
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		rtvHeapDesc.NumDescriptors = SWAPCHAIN_BUFFER_COUNT;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = 0;
		ThrowIfFailed(m_D3dDevice->CreateDescriptorHeap(
			&rtvHeapDesc, IID_PPV_ARGS(m_RtvHeap.GetAddressOf())));


		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeapDesc.NodeMask = 0;
		ThrowIfFailed(m_D3dDevice->CreateDescriptorHeap(
			&dsvHeapDesc, IID_PPV_ARGS(m_DsvHeap.GetAddressOf())));
	}

	Object2D* Device::AddSprite(std::string texName, bool isMat)
	{
		return m_Renderer->AddSprite(texName, isMat);
	}
	void Device::RemoveSprite(Object2D* sprite)
	{
		m_Renderer->RemoveSprite(sprite);
	}

	void Device::AddObject(Object3D* object)
	{
		m_Renderer->AddObject(object);
	}
	bool Device::RemoveObject(Object3D* object)
	{
		return m_Renderer->RemoveObject(object);
	}

	bool Device::Initialize()
	{
		if (!m_Window->InitMainWindow())
			return false;

		m_SwapChain = new SwapChain();

		if (!InitDirect3D())
			return false;

		m_Camera = new Camera3D();
		m_TextureManager = new TextureManager();
		m_Renderer = new RendererManager();
		m_MaterialManager = new MaterialManager();

		m_Renderer->Init();

		m_Renderer->BuildSrvHeap();
		m_SwapChain->OnResize();


		// ----- CONSTANT BUFFERS -----
		//

		m_Renderer->CreateCBCameraAndPass();

		//
		// ----- ROOT + SHADERS + PSO -----
		//

		m_Renderer->BuildRootSignature();

		//SHADER TEXTURE
		std::vector<D3D12_INPUT_ELEMENT_DESC> LayoutTextureShader =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		m_Renderer->BuildShaderAndPSO(SHADER_TEXTURE_NAME, L"..\\..\\res\\Shaders\\texture.hlsl", LayoutTextureShader, false);
		m_Renderer->BuildShaderAndPSO(SHADER_TEXTURE_A_NAME, L"..\\..\\res\\Shaders\\texture.hlsl", LayoutTextureShader, true);

		m_Renderer->BuildShaderAndPSO(SHADER_SPRITE_NAME, L"..\\..\\res\\Shaders\\sprite.hlsl", LayoutTextureShader, true, true);

		//SHADER COLOR
		std::vector<D3D12_INPUT_ELEMENT_DESC> LayoutColorShader =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		m_Renderer->BuildShaderAndPSO(SHADER_COLOR_NAME, L"..\\..\\res\\Shaders\\color.hlsl", LayoutColorShader, false);
		m_Renderer->BuildShaderAndPSO(SHADER_COLOR_A_NAME, L"..\\..\\res\\Shaders\\color.hlsl", LayoutColorShader, true);

		// SHADER COLORED TEXTURE
		std::vector<D3D12_INPUT_ELEMENT_DESC> LayoutColorTextureShader =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		m_Renderer->BuildShaderAndPSO(SHADER_COLOR_TEXTURE_NAME, L"..\\..\\res\\Shaders\\textureColor.hlsl", LayoutColorTextureShader, false);
		m_Renderer->BuildShaderAndPSO(SHADER_COLOR_TEXTURE_A_NAME, L"..\\..\\res\\Shaders\\textureColor.hlsl", LayoutColorTextureShader, true);

		// DEFAULT TEXTURE
		TextureManager::LoadTexture(DEFAULT_TEXTURE_NAME, DEFAULT_TEXTURE_PATH);

		// DEFAULT COLOR MATERIAL
		MaterialManager::CreateMaterial(SHADER_COLOR_NAME, MATERIAL_DEFAULT_COLOR_NAME);
		MaterialManager::CreateMaterial(SHADER_COLOR_A_NAME, MATERIAL_DEFAULT_COLOR_A_NAME);

		// DEFAULT TEXTURE MATERIAL
		MaterialManager::CreateMaterial(SHADER_TEXTURE_A_NAME, MATERIAL_DEFAULT_TEXTURE_NAME, DEFAULT_TEXTURE_NAME);

		return true;
	}

	LRESULT Device::ProcessWindowMessages(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return m_Window->MsgProc(hwnd, msg, wParam, lParam);
	}

}


