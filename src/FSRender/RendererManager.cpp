#include "pch.h"

namespace FSR
{
	RendererManager::RendererManager()
	{
		m_ShaderMap[SHADER_COLOR_NAME] = new ShaderPSO();
		m_ShaderMap[SHADER_COLOR_A_NAME] = new ShaderPSO();
		m_ShaderMap[SHADER_TEXTURE_NAME] = new ShaderPSO();
		m_ShaderMap[SHADER_TEXTURE_A_NAME] = new ShaderPSO();
		m_ShaderMap[SHADER_SPRITE_NAME] = new ShaderPSO();
		m_ShaderMap[SHADER_COLOR_TEXTURE_NAME] = new ShaderPSO();
		m_ShaderMap[SHADER_COLOR_TEXTURE_A_NAME] = new ShaderPSO();

		PointLight* sun = new PointLight();
		sun->m_Position = { 0.0f, 1000.0f, 0.0f };
		sun->m_Strength = { 20.f, 20.f, 20.f };
		sun->m_FalloffStart = 1.0f;
		sun->m_FalloffEnd = 1025.0f;
		AddPointLight(sun);
	}

	void RendererManager::Init()
	{
		m_Device = Device::Get();
		m_MatManager = MaterialManager::Get();
		m_Cmd = m_Device->GetCommand();
		m_Camera = m_Device->GetCamera();
	}

	void RendererManager::ClearAllObjects()
	{
		for (auto object : m_3DObjects)
			delete(object);
		m_3DObjects.clear();
		for (auto object : m_3DAlphaObjects)
			delete(object);
		m_3DAlphaObjects.clear();
		for (auto object : m_Sprites)
			delete(object);
		m_Sprites.clear();

		for (auto light : m_DirLights)
			delete(light);
		m_DirLights.clear();
		for (auto light : m_PointLights)
			delete(light);
		m_PointLights.clear();
		for (auto light : m_SpotLights)
			delete(light);
		m_SpotLights.clear();

		for (auto cb : m_CBs)
			delete(cb);
		m_CBs.clear();
		for (auto mb : m_MBs)
			delete(mb);
		m_MBs.clear();

		for (auto cb : m_CBAlphas)
			delete(cb);
		m_CBAlphas.clear();
		for (auto mb : m_MBAlphas)
			delete(mb);
		m_MBAlphas.clear();
		for (auto cb : m_CB2Ds)
			delete(cb);
		m_CB2Ds.clear();
		for (auto mb : m_MB2Ds)
			delete(mb);
		m_MB2Ds.clear();
	}

	void RendererManager::ForceUpdate()
	{
		m_UpdateAllObjects3D = true; 
		m_UpdateAllSprites = true;
		m_Camera->SetUpdatedThisFrame(true);
	}

	RendererManager::~RendererManager()
	{
		for (auto object : m_3DObjects)
			delete(object);
		m_3DObjects.clear();
		for (auto cb : m_CBs)
			delete(cb);
		m_CBs.clear();

		for (auto light : m_DirLights)
			delete(light);
		for (auto light : m_PointLights)
			delete(light);
		for (auto light : m_SpotLights)
			delete(light);

		delete(m_CameraCB3D);
		delete(m_CameraCB2D);

		m_SrvDescriptorHeap.Reset();
		m_RootSignature.Reset();
	}

	void RendererManager::UpdateRender(float dt)
	{
		MSG msg = { 0 };

		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			m_Device->GetWindow()->SetClosed(true);

		m_DeltaTime = dt;
	
		if (dt == 0.0f)
			return;
		UpdateObjects();
		AfterUpdateObjects();
		Draw();
	}

	void RendererManager::BuildShaderAndPSO(std::string name, std::wstring path, std::vector<D3D12_INPUT_ELEMENT_DESC> _Layout, bool isTransparent, bool isUI)
	{
		SwapChain* swap = m_Device->GetSwapChain();

		m_ShaderMap[name]->BuildShadersAndInputLayout(path, _Layout, isUI);
		m_ShaderMap[name]->BuildPSO(GetRootSignature(), swap->GetBackBufferFormat(), swap->GetDepthStencilFormat(), isTransparent,
			m_Device->m_4xMsaaState, m_Device->m_4xMsaaQuality);
	}

	struct Compare3DObjectsByDistance
	{
		Object3D* obj;
		UploadBuffer<ObjectConstants>* CB;
		UploadBuffer<MaterialConstants>* MB;

		bool operator()(Object3D* a, Object3D* b) {
			return a->GetDistanceToCamera() > b->GetDistanceToCamera(); // du plus loin au plus proche
		}
	};

	void RendererManager::Reorder3DAlphaObjects()
	{
		m_ReorderTimer += m_DeltaTime;
		if (m_ReorderTimer < REORDER_3D_ALPHA_TIME)
			return;
		m_ReorderTimer = 0.0f;

		std::vector<Compare3DObjectsByDistance> objectsToSort;

		for (int i = 0; i < m_3DAlphaObjects.size(); i++)
		{
			Compare3DObjectsByDistance toSort;
			toSort.obj = m_3DAlphaObjects[i];
			toSort.CB = m_CBAlphas[i];
			toSort.MB = m_MBAlphas[i];
			objectsToSort.push_back(toSort);
		}

		//Trier les objets par distance à la caméra (depth)
		std::sort(objectsToSort.begin(), objectsToSort.end(),
			[](Compare3DObjectsByDistance a, Compare3DObjectsByDistance b)
			{
				return a.obj->GetDistanceToCamera() > b.obj->GetDistanceToCamera(); // du plus loin au plus proche
			}
		);

		for (int i = 0; i < m_3DAlphaObjects.size(); i++)
		{
			Compare3DObjectsByDistance sorted = objectsToSort[i];
			m_3DAlphaObjects[i] = sorted.obj;
			m_CBAlphas[i] = sorted.CB;
			m_MBAlphas[i] = sorted.MB;
		}

		m_UpdateAllObjects3D = true;
	}

	void RendererManager::ReorderSprites()
	{
		m_ReorderSprites = false;
		std::sort(m_Sprites.begin(), m_Sprites.end(),
			[](Object2D* a, Object2D* b)
			{
				return a->GetZIndex() < b->GetZIndex();
			}
		);
		m_UpdateAllSprites = true;
	}

	void RendererManager::UpdateObjects()
	{
		UpdateCameras();
		Update3DObjects();
		Update3DAlphaObjects();
		UpdateSprites();
		m_UpdateAllObjects3D = false;
		m_UpdateAllSprites = false;
		// Update main pass constants (lights, ambient, eye pos) before drawing
		UpdateMainPassCB();
	}

	void RendererManager::UpdateCameras()
	{
		m_Camera->Update();
		if (m_Camera->IsUpdatedThisFrame() == false)
			return;

		//Camera3D
		XMFLOAT4X4 cameraViewProj = m_Device->Get()->GetCamera()->GetViewProj();
		XMMATRIX cameraMatrix = XMLoadFloat4x4(&cameraViewProj);

		CameraOC3D cameraObjConstants;
		XMStoreFloat4x4(&cameraObjConstants.m_ViewProj, XMMatrixTranspose(cameraMatrix));
		m_CameraCB3D->CopyData(0, cameraObjConstants);
		//Camera2D
		XMFLOAT4X4 cameraViewProj2D = m_Camera->GetViewProj2D();
		XMMATRIX cameraMatrix2D = XMLoadFloat4x4(&cameraViewProj2D);

		CameraOC2D cameraObjConstants2D;
		XMStoreFloat4x4(&cameraObjConstants2D.m_ViewProj, XMMatrixTranspose(cameraMatrix2D));
		m_CameraCB2D->CopyData(0, cameraObjConstants2D);
	}

	void RendererManager::BuildRootSignature()
	{
		CD3DX12_ROOT_PARAMETER slotRootParameters[6];

		CD3DX12_DESCRIPTOR_RANGE texTable;
		texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);//t0

		slotRootParameters[0].InitAsConstantBufferView(0); // b0
		slotRootParameters[1].InitAsConstantBufferView(1); // b1
		slotRootParameters[2].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);

		slotRootParameters[3].InitAsConstantBufferView(2); // b2 (material)
		slotRootParameters[4].InitAsConstantBufferView(3); // b3
		slotRootParameters[5].InitAsConstantBufferView(4); // b4

		// Static sampler (s0)
		CD3DX12_STATIC_SAMPLER_DESC sampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP);

		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(6, slotRootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> serializedRootSig = nullptr;
		ComPtr<ID3DBlob> errorBlob = nullptr;

		ThrowIfFailed(D3D12SerializeRootSignature(
			&rootSigDesc,
			D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(),
			errorBlob.GetAddressOf()));

		ThrowIfFailed(m_Device->GetD3DDevice()->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(&m_RootSignature)));
	}
	void RendererManager::BuildSrvHeap()
	{
		auto device = m_Device->GetD3DDevice().Get();

		m_CbvSrvDescriptorSize =
			device->GetDescriptorHandleIncrementSize(
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 10;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		ThrowIfFailed(device->CreateDescriptorHeap(
			&srvHeapDesc,
			IID_PPV_ARGS(&m_SrvDescriptorHeap)));

		m_CurrentSrvIndex = 0;
	}
	void RendererManager::BuildTextureSRV(ID3D12Resource* texture)
	{
		auto device = m_Device->GetD3DDevice().Get();

		CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(
			m_SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			m_CurrentSrvIndex,
			m_CbvSrvDescriptorSize);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping =
			D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = texture->GetDesc().Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = texture->GetDesc().MipLevels;

		device->CreateShaderResourceView(
			texture,
			&srvDesc,
			hDescriptor);

		m_CurrentSrvIndex++;
	}

	void RendererManager::CreateCBObject()
	{

		auto CB = new UploadBuffer<ObjectConstants>(
			m_Device->GetD3DDevice().Get(), 1, true);
		m_CBs.push_back(CB);

		auto MB = new UploadBuffer<MaterialConstants>(
			m_Device->GetD3DDevice().Get(), 1, true);
		m_MBs.push_back(MB);
	}
	void RendererManager::CreateCBSprite()
	{
		auto CB = new UploadBuffer<ObjectConstants2D>(
			m_Device->GetD3DDevice().Get(), 1, true);
		m_CB2Ds.push_back(CB);

		auto MB = new UploadBuffer<MaterialConstants>(
			m_Device->GetD3DDevice().Get(), 1, true);
		m_MB2Ds.push_back(MB);
	}

	void RendererManager::CreateCBCameraAndPass()
	{

		m_CameraCB3D = new UploadBuffer<CameraOC3D>(
			m_Device->GetD3DDevice().Get(), 1, true);

		m_CameraCB2D = new UploadBuffer<CameraOC2D>(
			m_Device->GetD3DDevice().Get(), 1, true);

		m_PassCB = new UploadBuffer<PassConstants>(
			m_Device->GetD3DDevice().Get(), 1, true);
	}

	void RendererManager::AddObject(Object3D* object)
	{
		CreateCBObject();
		m_3DObjects.push_back(object);
		m_UpdateAllObjects3D = true;
	}
	void RendererManager::MoveToAlphaRender(Object3D* object)
	{
		for (int i = 0; i < m_3DObjects.size(); i++)
		{
			if (m_3DObjects[i] != object)
				continue;
			m_3DAlphaObjects.push_back(object);
			m_CBAlphas.push_back(m_CBs[i]);
			m_MBAlphas.push_back(m_MBs[i]);
			m_3DObjects.erase(m_3DObjects.begin() + i);
			m_MBs.erase(m_MBs.begin() + i);
			m_CBs.erase(m_CBs.begin() + i);

			m_UpdateAllObjects3D = true;
			return;
		}
		std::cout << "RendererManager::MoveToAlphaRender : Object not found." << std::endl;
	}
	void RendererManager::MoveToRender(Object3D* object)
	{
		for (int i = 0; i < m_3DAlphaObjects.size(); i++)
		{
			if (m_3DAlphaObjects[i] != object)
				continue;
			m_3DObjects.push_back(object);
			m_CBs.push_back(m_CBAlphas[i]);
			m_MBs.push_back(m_MBAlphas[i]);
			m_3DAlphaObjects.erase(m_3DAlphaObjects.begin() + i);
			m_MBAlphas.erase(m_MBAlphas.begin() + i);
			m_CBAlphas.erase(m_CBAlphas.begin() + i);

			m_UpdateAllObjects3D = true;
			return;
		}
		std::cout << "RendererManager::MoveToAlpha : Object not found." << std::endl;
	}
	bool RendererManager::RemoveObject(Object3D* object)
	{
		for (int i = 0; i < m_3DObjects.size(); i++)
		{
			if (m_3DObjects[i] != object)
				continue;
			RemoveObject(i, m_3DObjects, m_CBs, m_MBs);
			return true;
		}
		for (int i = 0; i < m_3DAlphaObjects.size(); i++)
		{
			if (m_3DAlphaObjects[i] != object)
				continue;
			RemoveObject(i, m_3DAlphaObjects, m_CBAlphas, m_MBAlphas);
			return true;
		}
		std::cout << "RendererManager::RemoveObject : Object not found." << std::endl;
		return false;
	}
	void RendererManager::RemoveObject(int index, std::vector<Object3D*>& vec, std::vector<UploadBuffer<ObjectConstants>*>& CB, std::vector<UploadBuffer<MaterialConstants>*>& MB)
	{
		auto itObject = vec.begin();
		auto itCB = CB.begin();
		auto itMB = MB.begin();
		itObject += index;
		itCB += index;
		itMB += index;

		delete(vec[index]);
		delete(CB[index]);
		delete(MB[index]);

		vec.erase(itObject);
		CB.erase(itCB);
		MB.erase(itMB);

		m_UpdateAllObjects3D = true;
	}

	Object2D* RendererManager::AddSprite(std::string texName, bool isMat)
	{
		CreateCBSprite();
		Object2D* sprite = SpriteGenerator::CreateSprite(texName, isMat);
		m_UpdateAllSprites = true;
		return sprite;
	}
	void RendererManager::RemoveSprite(Object2D* sprite)
	{
		for (int i = 0; i < m_Sprites.size(); i++)
		{
			auto spr = m_Sprites[i];
			if (spr != sprite)
				continue;
			RemoveSprite(i);
			return;
		}
		std::cout << "FS_Device::RemoveSprite : Sprite not found" << std::endl;
	}
	void RendererManager::RemoveSprite(int index)
	{
		auto itSprite = m_Sprites.begin();
		auto itCB = m_CB2Ds.begin();
		auto itMB = m_MB2Ds.begin();
		itSprite += index;
		itCB += index;
		itMB += index;

		delete(m_Sprites[index]);
		delete(m_CB2Ds[index]);
		delete(m_MB2Ds[index]);

		m_Sprites.erase(itSprite);
		m_CB2Ds.erase(itCB);
		m_MB2Ds.erase(itMB);

		m_UpdateAllSprites = true;
	}

	void RendererManager::UpdateWinSizeForSprites(int width, int height)
	{
		for(Object2D* sprite : m_Sprites)
			sprite->SetWinSize(width, height);
	}

	void RendererManager::AddPointLight(PointLight* light)
	{
		if (m_PointLights.size() >= MaxLights)
		{
			std::cout << "Max point lights reached, clear them if you want to add other." << std::endl;
			return;
		}
		m_PointLights.push_back(light);
	}
	void RendererManager::AddSpotLight(SpotLight* light)
	{
		if (m_SpotLights.size() >= MaxLights)
		{
			std::cout << "Max spot lights reached, clear them if you want to add other." << std::endl;
			return;
		}
		m_SpotLights.push_back(light);
	}
	void RendererManager::AddDirectionalLight(DirectionalLight* light)
	{
		if (m_DirLights.size() >= MaxLights)
		{
			std::cout << "Max directional lights reached, clear them if you want to add other." << std::endl;
			return;
		}
		m_DirLights.push_back(light);
	}

	void RendererManager::RemoveLight(Light* light)
	{
		RemovePointLight((PointLight*)light);
		RemoveSpotLight((SpotLight*)light);
		RemoveDirectionalLight((DirectionalLight*)light);
	}
	void RendererManager::RemovePointLight(PointLight* light)
	{
		for (int i = 0; i < m_PointLights.size(); i++)
		{
			auto l = m_PointLights[i];
			if (l != light)
				continue;
			delete(l);
			m_PointLights.erase(m_PointLights.begin() + i);
			return;
		}
		std::cout << "RendererManager::RemovePointLight : Point light not found." << std::endl;
	}
	void RendererManager::RemoveSpotLight(SpotLight* light)
	{
		for (int i = 0; i < m_SpotLights.size(); i++)
		{
			auto l = m_SpotLights[i];
			if (l != light)
				continue;
			delete(l);
			m_SpotLights.erase(m_SpotLights.begin() + i);
			return;
		}
		std::cout << "RendererManager::RemoveSpotLight : Spot light not found." << std::endl;
	}
	void RendererManager::RemoveDirectionalLight(DirectionalLight* light)
	{
		for (int i = 0; i < m_DirLights.size(); i++)
		{
			auto l = m_DirLights[i];
			if (l != light)
				continue;
			delete(l);
			m_DirLights.erase(m_DirLights.begin() + i);
			return;
		}
		std::cout << "RendererManager::RemoveDirectionalLight : Directional light not found." << std::endl;
	}

	void RendererManager::ClearPointLights()
	{
		for (auto light : m_PointLights)
			delete(light);
		m_PointLights.clear();
	}
	void RendererManager::ClearSpotLights()
	{
		for (auto light : m_SpotLights)
			delete(light);
		m_SpotLights.clear();
	}
	void RendererManager::ClearDirectionalLights()
	{
		for (auto light : m_DirLights)
			delete(light);
		m_DirLights.clear();
	}

	void RendererManager::Draw3DObjects()
	{
		for (int i = 0; i < m_3DObjects.size(); ++i)
		{
			Object3D* object = m_3DObjects[i];
			if (object->IsActive() == false)
				continue;
			MeshGeometry* meshGeo = object->GetMesh();

			auto vbv = meshGeo->VertexBufferView();
			auto ibv = meshGeo->IndexBufferView();

			Material* mat = m_MatManager->m_MaterialLibrary[object->GetMaterialIndex()];
			ShaderPSO* shader = mat->Shader;
			m_Cmd->m_CommandList->SetPipelineState(shader->GetPSO().Get());

			// b0 - Object
			m_Cmd->m_CommandList->SetGraphicsRootConstantBufferView(0, m_CBs[i]->Resource()->GetGPUVirtualAddress());
			// b2 - Material → slot 3
			m_Cmd->m_CommandList->SetGraphicsRootConstantBufferView(3, m_MBs[i]->Resource()->GetGPUVirtualAddress());
			// t0 - Texture → slot 2

			CD3DX12_GPU_DESCRIPTOR_HANDLE texHandle(m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
			texHandle.Offset(mat->TextureIndex, m_CbvSrvDescriptorSize);
			m_Cmd->m_CommandList->SetGraphicsRootDescriptorTable(2, texHandle);

			m_Cmd->m_CommandList->IASetVertexBuffers(0, 1, &vbv);
			m_Cmd->m_CommandList->IASetIndexBuffer(&ibv);
			m_Cmd->m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			m_Cmd->m_CommandList->DrawIndexedInstanced(meshGeo->m_IndexCount, 1, 0, 0, 0);
		}
	}
	void RendererManager::Draw3DAlphaObjects()
	{
		for (int i = 0; i < m_3DAlphaObjects.size(); ++i)
		{
			Object3D* object = m_3DAlphaObjects[i];
			if (object->IsActive() == false)
				continue;
			MeshGeometry* meshGeo = object->GetMesh();

			auto vbv = meshGeo->VertexBufferView();
			auto ibv = meshGeo->IndexBufferView();

			Material* mat = m_MatManager->m_MaterialLibrary[object->GetMaterialIndex()];
			ShaderPSO* shader = mat->Shader;
			m_Cmd->m_CommandList->SetPipelineState(shader->GetPSO().Get());

			// b0 - Object
			m_Cmd->m_CommandList->SetGraphicsRootConstantBufferView(0, m_CBAlphas[i]->Resource()->GetGPUVirtualAddress());
			// b2 - Material → slot 3
			m_Cmd->m_CommandList->SetGraphicsRootConstantBufferView(3, m_MBAlphas[i]->Resource()->GetGPUVirtualAddress());
			// t0 - Texture → slot 2

			CD3DX12_GPU_DESCRIPTOR_HANDLE texHandle(m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
			texHandle.Offset(mat->TextureIndex, m_CbvSrvDescriptorSize);
			m_Cmd->m_CommandList->SetGraphicsRootDescriptorTable(2, texHandle);

			m_Cmd->m_CommandList->IASetVertexBuffers(0, 1, &vbv);
			m_Cmd->m_CommandList->IASetIndexBuffer(&ibv);
			m_Cmd->m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			m_Cmd->m_CommandList->DrawIndexedInstanced(meshGeo->m_IndexCount, 1, 0, 0, 0);
		}
	}
	void RendererManager::DrawSprites()
	{
		if (m_SpriteRenderEnabled == false)
			return;

		for (int i = 0; i < m_Sprites.size(); ++i)
		{
			Object2D* sprite = m_Sprites[i];
			if (sprite->IsActive() == false)
				continue;

			MeshGeometry* meshGeo = sprite->GetMesh();

			auto vbv = meshGeo->VertexBufferView();
			auto ibv = meshGeo->IndexBufferView();

			Material* mat = m_MatManager->m_MaterialLibrary[sprite->GetMaterialIndex()];
			ShaderPSO* shader = mat->Shader;
			m_Cmd->m_CommandList->SetPipelineState(shader->GetPSO().Get());

			// b0 - Object
			m_Cmd->m_CommandList->SetGraphicsRootConstantBufferView(0, m_CB2Ds[i]->Resource()->GetGPUVirtualAddress());
			// b2 - Material → slot3
			m_Cmd->m_CommandList->SetGraphicsRootConstantBufferView(3, m_MB2Ds[i]->Resource()->GetGPUVirtualAddress());
			// t0 - Texture → slot2

			CD3DX12_GPU_DESCRIPTOR_HANDLE texHandle(m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
			texHandle.Offset(mat->TextureIndex, m_CbvSrvDescriptorSize);
			m_Cmd->m_CommandList->SetGraphicsRootDescriptorTable(2, texHandle);

			m_Cmd->m_CommandList->IASetVertexBuffers(0, 1, &vbv);
			m_Cmd->m_CommandList->IASetIndexBuffer(&ibv);
			m_Cmd->m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			m_Cmd->m_CommandList->DrawIndexedInstanced(meshGeo->m_IndexCount, 1, 0, 0, 0);
		}
	}

	void RendererManager::Update3DObjects()
	{
		for (int i = 0; i < m_3DObjects.size(); i++)
		{
			Object3D* object = m_3DObjects[i];
			if (object->IsActive() == false)
				continue;
			if (object->IsDirty() == false && m_UpdateAllObjects3D == false)
				continue;
			object->SetDirty(false);

			auto& cb = m_CBs[i];
			auto& mb = m_MBs[i];

			//<Update par objet----
			auto world = object->GetWorld();
			XMMATRIX worldObject = XMLoadFloat4x4(&world);

			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.m_World, XMMatrixTranspose(worldObject));
			cb->CopyData(0, objConstants);

			MaterialConstants matConstants;
			matConstants.m_Color = object->GetColor();
			// Fill material data from material manager so HLSL cbPerMaterial fields match
			auto matIndex = object->GetMaterialIndex();
			if (matIndex >= 0 && matIndex < (int)m_MatManager->m_MaterialLibrary.size())
			{
				Material* mat = m_MatManager->m_MaterialLibrary[matIndex];
				matConstants.m_DiffuseAlbedo = mat->m_DiffuseAlbedo;
				matConstants.m_Metallic = mat->m_Metallic;
				matConstants.m_Roughness = mat->m_Roughness;
			}
			mb->CopyData(0, matConstants);

			//----Update par objet>
		}
	}
	void RendererManager::Update3DAlphaObjects()
	{
		XMFLOAT3 camPos = m_Camera->GetPosition();
		XMVECTOR cameraPosition = XMLoadFloat3(&camPos);
		XMFLOAT3 camForward = m_Camera->GetTransform().m_Forward;
		XMVECTOR cameraForward = XMLoadFloat3(&camForward);

		Reorder3DAlphaObjects();

		for (int i = 0; i < m_3DAlphaObjects.size(); i++)
		{
			Object3D* object = m_3DAlphaObjects[i];
			if (object->IsActive() == false)
				continue;

			//Si l'objet est immobile et la camera aussi, 
			//pas besoin de recalculer la distance à la caméra ni de mettre à jour les constants buffers
			if (object->IsDirty() == false && m_Camera->IsUpdatedThisFrame() == false && m_UpdateAllObjects3D == false)
				continue;
			object->SetDirty(false);

			auto& cb = m_CBAlphas[i];
			auto& mb = m_MBAlphas[i];

			//<Update par objet----
			auto world = object->GetWorld();
			XMMATRIX worldObject = XMLoadFloat4x4(&world);

			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.m_World, XMMatrixTranspose(worldObject));
			cb->CopyData(0, objConstants);

			MaterialConstants matConstants;
			matConstants.m_Color = object->GetColor();
			// Fill material data from material manager so HLSL cbPerMaterial fields match
			auto matIndex = object->GetMaterialIndex();
			if (matIndex >= 0 && matIndex < (int)m_MatManager->m_MaterialLibrary.size())
			{
				Material* mat = m_MatManager->m_MaterialLibrary[matIndex];
				matConstants.m_DiffuseAlbedo = mat->m_DiffuseAlbedo;
				matConstants.m_Metallic = mat->m_Metallic;
				matConstants.m_Roughness = mat->m_Roughness;
			}
			mb->CopyData(0, matConstants);

			XMFLOAT3 pos = { world.m[3][0], world.m[3][1], world.m[3][2] };
			XMVECTOR objPos = XMLoadFloat3(&pos);
			float depth = XMVectorGetX(XMVector3Dot(objPos - cameraPosition, cameraForward));
			object->SetDistanceToCamera(depth);
			//----Update par objet>
		}
	}
	void RendererManager::UpdateSprites()
	{
		if (m_SpriteRenderEnabled == false)
			return;

		if(m_ReorderSprites)
			ReorderSprites();

		for (int i = 0; i < m_Sprites.size(); i++)
		{
			Object2D* sprite = m_Sprites[i];
			if (sprite->IsActive() == false)
				continue;
			if (sprite->IsDirty() == false && m_UpdateAllSprites == false)
				continue;

			sprite->SetDirty(false);
			auto& cb = m_CB2Ds[i];
			auto& mb = m_MB2Ds[i];

			//<Update par objet----
			XMFLOAT4X4 world = sprite->GetWorld();
			world.m[3][1] = -world.m[3][1]; //Reverse Y for correct orientation in screen space
			XMMATRIX worldObject = XMLoadFloat4x4(&world);

			ObjectConstants2D objConstants;
			XMStoreFloat4x4(&objConstants.m_World, XMMatrixTranspose(worldObject));
			objConstants.m_Mask = sprite->GetMask();
			cb->CopyData(0, objConstants);

			MaterialConstants matConstants;
			matConstants.m_Color = { 1, 1, 1, 1 };
			// Fill material data from material manager so HLSL cbPerMaterial fields match
			int matIndex = sprite->GetMaterialIndex();
			if (matIndex >= 0 && matIndex < m_MatManager->m_MaterialLibrary.size())
			{
				Material* mat = m_MatManager->m_MaterialLibrary[matIndex];
				matConstants.m_DiffuseAlbedo = mat->m_DiffuseAlbedo;
				matConstants.m_Metallic = mat->m_Metallic;
				matConstants.m_Roughness = mat->m_Roughness;
			}
			mb->CopyData(0, matConstants);

			//----Update par objet>
		}
	}

	void RendererManager::UpdateMainPassCB()
	{
		// Zero-initialize to ensure light arrays are not left with garbage
		PassConstants mMainPassCB = {};
		XMFLOAT3 pos = m_Camera->GetPosition();

		mMainPassCB.m_EyePosW = { pos.x, pos.y, pos.z, 1.0f };
		mMainPassCB.m_AmbientLight = m_AmbientLight;

		for (int i = 0; i < MaxLights; i++)
		{
			if (i < m_DirLights.size())
				mMainPassCB.m_DirLights[i] = *m_DirLights[i];
			else
				mMainPassCB.m_DirLights[i] = DirectionalLight();

			if (i < m_SpotLights.size())
				mMainPassCB.m_SpotLights[i] = *m_SpotLights[i];
			else
				mMainPassCB.m_SpotLights[i] = DirectionalLight();

			if (i < m_PointLights.size())
				mMainPassCB.m_PointLights[i] = *m_PointLights[i];
			else
				mMainPassCB.m_PointLights[i] = DirectionalLight();
		}

		m_PassCB->CopyData(0, mMainPassCB);
	}

	void RendererManager::AfterUpdateObjects()
	{
		AfterUpdateCameras();
	}

	void RendererManager::AfterUpdateCameras()
	{
		m_Camera->AfterUpdate();
	}

	void RendererManager::Draw()
	{
		Window* win = m_Device->GetWindow();
		SwapChain* swap = m_Device->GetSwapChain();

		m_Cmd->FlushCommandQueue();

		ThrowIfFailed(m_Cmd->m_DirectCmdListAlloc->Reset());
		ThrowIfFailed(m_Cmd->m_CommandList->Reset(m_Cmd->m_DirectCmdListAlloc.Get(), nullptr));

		D3D12_VIEWPORT viewport = win->GetViewPort();
		D3D12_RECT scissorRect = win->GetScissorRect();

		m_Cmd->m_CommandList->RSSetViewports(1, &viewport);
		m_Cmd->m_CommandList->RSSetScissorRects(1, &scissorRect);

		// Transition Present -> RenderTarget
		auto presentToTarget = CD3DX12_RESOURCE_BARRIER::Transition(
			swap->CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET);

		m_Cmd->m_CommandList->ResourceBarrier(1, &presentToTarget);

		m_Cmd->m_CommandList->ClearRenderTargetView(
			swap->CurrentBackBufferView(),
			Colors::LightSteelBlue,
			0, nullptr);

		m_Cmd->m_CommandList->ClearDepthStencilView(
			swap->DepthStencilView(),
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
			1.0f, 0,
			0, nullptr);

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swap->CurrentBackBufferView();
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = swap->DepthStencilView();

		m_Cmd->m_CommandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);

		// Root signature
		m_Cmd->m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

		// Descriptor heap
		ID3D12DescriptorHeap* descriptorHeaps[] = { m_SrvDescriptorHeap.Get() };
		m_Cmd->m_CommandList->SetDescriptorHeaps(1, descriptorHeaps);

		// Bind Light CB (b3)
		auto passCB = m_PassCB->Resource();
		m_Cmd->m_CommandList->SetGraphicsRootConstantBufferView(4, passCB->GetGPUVirtualAddress());

		// Bind camera3D CB (b1)
		m_Cmd->m_CommandList->SetGraphicsRootConstantBufferView(1, m_CameraCB3D->Resource()->GetGPUVirtualAddress());

		// Bind camera2D CB (b4)
		m_Cmd->m_CommandList->SetGraphicsRootConstantBufferView(5, m_CameraCB2D->Resource()->GetGPUVirtualAddress());

		// =========================
		// DRAW OBJECTS
		// =========================

		Draw3DObjects();
		Draw3DAlphaObjects();

		DrawSprites();

		// Transition back to Present
		auto targetToPresent = CD3DX12_RESOURCE_BARRIER::Transition(
			swap->CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT);

		m_Cmd->m_CommandList->ResourceBarrier(1, &targetToPresent);

		ThrowIfFailed(m_Cmd->m_CommandList->Close());

		ID3D12CommandList* cmdsLists[] = { m_Cmd->m_CommandList.Get() };
		m_Cmd->m_CommandQueue->ExecuteCommandLists(1, cmdsLists);

		ThrowIfFailed(swap->GetSwapChain()->Present(0, 0));

		swap->SetCurrBackBuffer((swap->GetCurrBackBuffer() + 1) % SWAPCHAIN_BUFFER_COUNT);

		m_Cmd->FlushCommandQueue();
	}
}
