#include "pch.h"

FS_Renderer::FS_Renderer()
{
	m_ShaderMap[SHADER_COLOR_NAME] = new FS_ShaderPSO();
	m_ShaderMap[SHADER_COLOR_A_NAME] = new FS_ShaderPSO();
	m_ShaderMap[SHADER_TEXTURE_NAME] = new FS_ShaderPSO();
	m_ShaderMap[SHADER_TEXTURE_A_NAME] = new FS_ShaderPSO();
	m_ShaderMap[SHADER_SPRITE_NAME] = new FS_ShaderPSO();
	m_ShaderMap[SHADER_COLOREDTEXTURE_NAME] = new FS_ShaderPSO();

	PointLight* sun = new PointLight();
	sun->Position = { 0.0f, 50.0f, 0.0f };
	sun->Strength = { 1.5f, 1.5f, 1.5f };
	sun->FalloffStart = 1.0f;
	sun->FalloffEnd = 100.0f;
	AddPointLight(sun);
}

FS_Renderer::~FS_Renderer()
{
	for(auto object : m_3DObjects)
		delete(object);
	m_3DObjects.clear();
	for (auto cb : m_CBs)
		delete(cb);
	m_CBs.clear();

	for (auto light : m_dirLights)
		delete(light);
	for (auto light : m_pointLights)
		delete(light);
	for (auto light : m_spotLights)
		delete(light);

	delete(m_CameraCB3D);
	delete(m_CameraCB2D);

	m_SrvDescriptorHeap.Reset();
	m_RootSignature.Reset();
}

void FS_Renderer::UpdateRender(float dt)
{
	MSG msg = { 0 };
	if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	else
	{
		if (dt == 0.0f)
			return;
		UpdateObjects();
		AfterUpdateObjects();
		Draw();
	}
	if (msg.message != WM_QUIT)
		return;
	FS_Device::Get()->m_FSWindow->m_isClosed = true;
}

void FS_Renderer::BuildShaderAndPSO(std::string name, std::wstring path, std::vector<D3D12_INPUT_ELEMENT_DESC> _Layout, bool isTransparent, bool isUI)
{
	FS_Device* device = FS_Device::Get();
	FS_SwapChain* swap = FS_Device::Get()->m_FSSwapChain;

	m_ShaderMap[name]->BuildShadersAndInputLayout(path, _Layout, isUI);
	m_ShaderMap[name]->BuildPSO(GetRootSignature(), swap->m_BackBufferFormat, swap->m_DepthStencilFormat, isTransparent,
		device->m_4xMsaaState, device->m_4xMsaaQuality);
}

void FS_Renderer::UpdateObjects()
{
	UpdateCameras();
	Update3DObjects();
	Update3DAlphaObjects();
	UpdateSprites();
	// Update main pass constants (lights, ambient, eye pos) before drawing
	UpdateMainPassCB();
}

void FS_Renderer::UpdateCameras()
{
	FS_3DCamera* cam = FS_Device::Get()->m_FSCamera;
	cam->Update();
	if (cam->IsUpdatedThisFrame() == false)
		return;

	std::cout << "Camera updated this frame" << std::endl;
	//Camera3D
	XMFLOAT4X4 cameraViewProj = FS_Device::Get()->Camera()->GetViewProj();
	XMMATRIX cameraMatrix = XMLoadFloat4x4(&cameraViewProj);

	CameraOC3D cameraObjConstants;
	XMStoreFloat4x4(&cameraObjConstants.m_ViewProj, XMMatrixTranspose(cameraMatrix));
	m_CameraCB3D->CopyData(0, cameraObjConstants);
	//Camera2D
	XMFLOAT4X4 cameraViewProj2D = FS_Device::Get()->Camera()->GetViewProj2D();
	XMMATRIX cameraMatrix2D = XMLoadFloat4x4(&cameraViewProj2D);

	CameraOC2D cameraObjConstants2D;
	XMStoreFloat4x4(&cameraObjConstants2D.m_ViewProj, XMMatrixTranspose(cameraMatrix2D));
	m_CameraCB2D->CopyData(0, cameraObjConstants2D);
}

void FS_Renderer::BuildRootSignature()
{
	CD3DX12_ROOT_PARAMETER slotRootParameters[6];

	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,1,0 );//t0

	slotRootParameters[0].InitAsConstantBufferView(0); // b0
	slotRootParameters[1].InitAsConstantBufferView(1); // b1
	slotRootParameters[2].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);

	slotRootParameters[3].InitAsConstantBufferView(2); // b2 (material)
	slotRootParameters[4].InitAsConstantBufferView(3); // b3
	slotRootParameters[5].InitAsConstantBufferView(4); // b4

	// Static sampler (s0)
	CD3DX12_STATIC_SAMPLER_DESC sampler(0,D3D12_FILTER_MIN_MAG_MIP_LINEAR,D3D12_TEXTURE_ADDRESS_MODE_WRAP,D3D12_TEXTURE_ADDRESS_MODE_WRAP,D3D12_TEXTURE_ADDRESS_MODE_WRAP);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(6,slotRootParameters,1,&sampler,D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;

	ThrowIfFailed(D3D12SerializeRootSignature(
		&rootSigDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(),
		errorBlob.GetAddressOf()));

	ThrowIfFailed(FS_Device::Get()->m_d3dDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature)));
}
void FS_Renderer::BuildSrvHeap()
{
	auto device = FS_Device::Get()->m_d3dDevice.Get();

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
void FS_Renderer::BuildTextureSRV(ID3D12Resource* texture)
{
	auto device = FS_Device::Get()->m_d3dDevice.Get();

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

void FS_Renderer::CreateCBObject()
{
	FS_Device* device = FS_Device::Get();

	auto CB = new UploadBuffer<ObjectConstants>(
		device->m_d3dDevice.Get(), 1, true);
	m_CBs.push_back(CB);

	auto MB = new UploadBuffer<MaterialConstants>(
		device->m_d3dDevice.Get(), 1, true);
	m_MBs.push_back(MB);
}
void FS_Renderer::CreateCBSprite()
{
	FS_Device* device = FS_Device::Get();

	auto CB = new UploadBuffer<ObjectConstants2D>(
		device->m_d3dDevice.Get(), 1, true);
	m_CB2Ds.push_back(CB);

	auto MB = new UploadBuffer<MaterialConstants>(
		device->m_d3dDevice.Get(), 1, true);
	m_MB2Ds.push_back(MB);
}

void FS_Renderer::CreateCBCameraAndPass()
{
	FS_Device* device = FS_Device::Get();

	m_CameraCB3D = new UploadBuffer<CameraOC3D>(
			device->m_d3dDevice.Get(), 1, true);

	m_CameraCB2D = new UploadBuffer<CameraOC2D>(
			device->m_d3dDevice.Get(), 1, true);

	m_PassCB = new UploadBuffer<PassConstants>(
			device->m_d3dDevice.Get(), 1, true);
}

void FS_Renderer::AddObject(FS_3DObject* object)
{
	CreateCBObject();
	m_3DObjects.push_back(object);
}
void FS_Renderer::MoveToAlphaRender(FS_3DObject* object)
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
		return;
	}
	std::cout << "FS_Renderer::MoveToAlphaRender : Object not found." << std::endl;
}
void FS_Renderer::MoveToRender(FS_3DObject* object)
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
		return;
	}
	std::cout << "FS_Renderer::MoveToAlpha : Object not found." << std::endl;
}
bool FS_Renderer::RemoveObject(FS_3DObject* object)
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
	std::cout << "FS_Renderer::RemoveObject : Object not found." << std::endl;
	return false;
}
void FS_Renderer::RemoveObject(int index, std::vector<FS_3DObject*>& vec, std::vector<UploadBuffer<ObjectConstants>*>& CB, std::vector<UploadBuffer<MaterialConstants>*>& MB)
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
}

FS_Sprite* FS_Renderer::AddSprite(std::string texName, bool isMat)
{
	CreateCBSprite();
	return SpriteGenerator::CreateSprite(texName, isMat);
}
void FS_Renderer::RemoveSprite(FS_Sprite* sprite)
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
void FS_Renderer::RemoveSprite(int index)
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
}

void FS_Renderer::AddPointLight(PointLight* light)
{
	if (m_pointLights.size() >= MaxLights)
	{
		std::cout << "Max point lights reached, clear them if you want to add other." << std::endl;
		return;
	}
	m_pointLights.push_back(light);
}
void FS_Renderer::AddSpotLight(SpotLight* light)
{
	if (m_spotLights.size() >= MaxLights)
	{
		std::cout << "Max spot lights reached, clear them if you want to add other." << std::endl;
		return;
	}
	m_spotLights.push_back(light);
}
void FS_Renderer::AddDirectionalLight(DirectionalLight* light)
{
	if (m_dirLights.size() >= MaxLights)
	{
		std::cout << "Max directional lights reached, clear them if you want to add other." << std::endl;
		return;
	}
	m_dirLights.push_back(light);
}

void FS_Renderer::RemoveLight(Light* light)
{
	RemovePointLight((PointLight*)light);
	RemoveSpotLight((SpotLight*)light);
	RemoveDirectionalLight((DirectionalLight*)light);
}
void FS_Renderer::RemovePointLight(PointLight* light)
{
	for (int i = 0; i < m_pointLights.size(); i++)
	{
		auto l = m_pointLights[i];
		if (l != light)
			continue;
		delete(l);
		m_pointLights.erase(m_pointLights.begin() + i);
		return;
	}
	std::cout << "FS_Renderer::RemovePointLight : Point light not found." << std::endl;
}
void FS_Renderer::RemoveSpotLight(SpotLight* light)
{
	for (int i = 0; i < m_spotLights.size(); i++)
	{
		auto l = m_spotLights[i];
		if (l != light)
			continue;
		delete(l);
		m_spotLights.erase(m_spotLights.begin() + i);
		return;
	}
	std::cout << "FS_Renderer::RemoveSpotLight : Spot light not found." << std::endl;
}
void FS_Renderer::RemoveDirectionalLight(DirectionalLight* light)
{
	for (int i = 0; i < m_dirLights.size(); i++)
	{
		auto l = m_dirLights[i];
		if (l != light)
			continue;
		delete(l);
		m_dirLights.erase(m_dirLights.begin() + i);
		return;
	}
	std::cout << "FS_Renderer::RemoveDirectionalLight : Directional light not found." << std::endl;
}

void FS_Renderer::ClearPointLights()
{
	for(auto light : m_pointLights)
		delete(light);
	m_pointLights.clear();
}
void FS_Renderer::ClearSpotLights()
{
	for (auto light : m_spotLights)
		delete(light);
	m_spotLights.clear();
}
void FS_Renderer::ClearDirectionalLights()
{
	for (auto light : m_dirLights)
		delete(light);
	m_dirLights.clear();
}

void FS_Renderer::Draw3DObjects()
{
	FS_Device* device = FS_Device::Get();
	MaterialManager* fsMat = device->m_FSMaterial;
	FS_Command* cmd = device->m_FSCmd;

	for (int i = 0; i < m_3DObjects.size(); ++i)
	{
		FS_3DObject* object = m_3DObjects[i];
		if (object->IsActive() == false)
			continue;
		MeshGeometry* meshGeo = object->GetMesh();

		auto vbv = meshGeo->VertexBufferView();
		auto ibv = meshGeo->IndexBufferView();

		FS_Material* mat = fsMat->m_MaterialLibrary[object->GetMaterialIndex()];
		FS_ShaderPSO* shader = mat->Shader;
		cmd->mCommandList->SetPipelineState(shader->mPSO.Get());

		// b0 - Object
		cmd->mCommandList->SetGraphicsRootConstantBufferView(0, m_CBs[i]->Resource()->GetGPUVirtualAddress());
		// b2 - Material → slot 3
		cmd->mCommandList->SetGraphicsRootConstantBufferView(3, m_MBs[i]->Resource()->GetGPUVirtualAddress());
		// t0 - Texture → slot 2

		CD3DX12_GPU_DESCRIPTOR_HANDLE texHandle(m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		texHandle.Offset(mat->TextureIndex, m_CbvSrvDescriptorSize);
		cmd->mCommandList->SetGraphicsRootDescriptorTable(2, texHandle);

		cmd->mCommandList->IASetVertexBuffers(0, 1, &vbv);
		cmd->mCommandList->IASetIndexBuffer(&ibv);
		cmd->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		cmd->mCommandList->DrawIndexedInstanced(meshGeo->IndexCount, 1, 0, 0, 0);
	}
}

void FS_Renderer::Draw3DAlphaObjects()
{
	FS_Device* device = FS_Device::Get();
	MaterialManager* fsMat = device->m_FSMaterial;
	FS_Command* cmd = device->m_FSCmd;

	for (int i = 0; i < m_3DAlphaObjects.size(); ++i)
	{
		FS_3DObject* object = m_3DAlphaObjects[i];
		if (object->IsActive() == false)
			continue;
		MeshGeometry* meshGeo = object->GetMesh();

		auto vbv = meshGeo->VertexBufferView();
		auto ibv = meshGeo->IndexBufferView();

		FS_Material* mat = fsMat->m_MaterialLibrary[object->GetMaterialIndex()];
		FS_ShaderPSO* shader = mat->Shader;
		cmd->mCommandList->SetPipelineState(shader->mPSO.Get());

		// b0 - Object
		cmd->mCommandList->SetGraphicsRootConstantBufferView(0, m_CBAlphas[i]->Resource()->GetGPUVirtualAddress());
		// b2 - Material → slot 3
		cmd->mCommandList->SetGraphicsRootConstantBufferView(3, m_MBAlphas[i]->Resource()->GetGPUVirtualAddress());
		// t0 - Texture → slot 2

		CD3DX12_GPU_DESCRIPTOR_HANDLE texHandle(m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		texHandle.Offset(mat->TextureIndex, m_CbvSrvDescriptorSize);
		cmd->mCommandList->SetGraphicsRootDescriptorTable(2, texHandle);

		cmd->mCommandList->IASetVertexBuffers(0, 1, &vbv);
		cmd->mCommandList->IASetIndexBuffer(&ibv);
		cmd->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		cmd->mCommandList->DrawIndexedInstanced(meshGeo->IndexCount, 1, 0, 0, 0);
	}
}

void FS_Renderer::DrawSprites()
{
	if (m_spriteRenderEnabled == false)
		return;

	FS_Device* device = FS_Device::Get();
	MaterialManager* fsMat = device->m_FSMaterial;
	FS_Command* cmd = device->m_FSCmd;

	for (int i = 0; i < m_Sprites.size(); ++i)
	{
		FS_Sprite* sprite = m_Sprites[i];
		if (sprite->IsActive() == false)
			continue;

		MeshGeometry* meshGeo = sprite->GetMesh();

		auto vbv = meshGeo->VertexBufferView();
		auto ibv = meshGeo->IndexBufferView();

		FS_Material* mat = fsMat->m_MaterialLibrary[sprite->GetMaterialIndex()];
		FS_ShaderPSO* shader = mat->Shader;
		cmd->mCommandList->SetPipelineState(shader->mPSO.Get());

		// b0 - Object
		cmd->mCommandList->SetGraphicsRootConstantBufferView(0, m_CB2Ds[i]->Resource()->GetGPUVirtualAddress());
		// b2 - Material → slot3
		cmd->mCommandList->SetGraphicsRootConstantBufferView(3, m_MB2Ds[i]->Resource()->GetGPUVirtualAddress());
		// t0 - Texture → slot2

		CD3DX12_GPU_DESCRIPTOR_HANDLE texHandle(m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		texHandle.Offset(mat->TextureIndex, m_CbvSrvDescriptorSize);
		cmd->mCommandList->SetGraphicsRootDescriptorTable(2, texHandle);

		cmd->mCommandList->IASetVertexBuffers(0, 1, &vbv);
		cmd->mCommandList->IASetIndexBuffer(&ibv);
		cmd->mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		cmd->mCommandList->DrawIndexedInstanced(meshGeo->IndexCount, 1, 0, 0, 0);
	}
}

void FS_Renderer::Update3DObjects()
{
	for (int i = 0; i < m_3DObjects.size(); i++)
	{
		FS_3DObject* object = m_3DObjects[i];
		if (object->IsActive() == false)
			continue;
		if (object->IsDirty() == false)
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
		MaterialManager* matMgr = FS_Device::Get()->m_FSMaterial;
		auto matIndex = object->GetMaterialIndex();
		if (matIndex >= 0 && matIndex < (int)matMgr->m_MaterialLibrary.size())
		{
			FS_Material* mat = matMgr->m_MaterialLibrary[matIndex];
			matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
			matConstants.Metallic = mat->Metallic;
			matConstants.Roughness = mat->Roughness;
		}
		mb->CopyData(0, matConstants);

		//----Update par objet>
	}
}
void FS_Renderer::Update3DAlphaObjects()
{
	FS_3DCamera* cam = FS_Device::Get()->Camera();
	XMFLOAT3 camPos = cam->GetPosition();
	XMVECTOR cameraPosition = XMLoadFloat3(&camPos);
	XMFLOAT3 camForward = cam->GetTransform().forward;
	XMVECTOR cameraForward = XMLoadFloat3(&camForward);

	bool isAllObjectsStatic = true;

	for (int i = 0; i < m_3DAlphaObjects.size(); i++)
	{
		FS_3DObject* object = m_3DAlphaObjects[i];
		if (object->IsActive() == false)
			continue;
			
		//Si l'objet est immobile et la camera aussi, 
		//pas besoin de recalculer la distance à la caméra ni de mettre à jour les constants buffers
		if (object->IsDirty() == false && cam->IsUpdatedThisFrame() == false)
			continue;
		object->SetDirty(false);

		isAllObjectsStatic = false;
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
		MaterialManager* matMgr = FS_Device::Get()->m_FSMaterial;
		auto matIndex = object->GetMaterialIndex();
		if (matIndex >= 0 && matIndex < (int)matMgr->m_MaterialLibrary.size())
		{
			FS_Material* mat = matMgr->m_MaterialLibrary[matIndex];
			matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
			matConstants.Metallic = mat->Metallic;
			matConstants.Roughness = mat->Roughness;
		}
		mb->CopyData(0, matConstants);

		XMFLOAT3 pos = { world.m[3][0], world.m[3][1], world.m[3][2] };
		XMVECTOR objPos = XMLoadFloat3(&pos);
		float depth = XMVectorGetX(XMVector3Dot(objPos - cameraPosition, cameraForward));
		object->SetDistanceToCamera(depth);
		//----Update par objet>
	}

	if(isAllObjectsStatic && cam->IsUpdatedThisFrame() == false)
		return;

	std::cout << "Objects are dirty or camera updated, refreshing data." << std::endl;

	std::cout << "Camera position: (" << camPos.x << ", " << camPos.y << ", " << camPos.z << ")" << std::endl;

	//Trier les objets par distance à la caméra (depth)
	std::sort(m_3DAlphaObjects.begin(), m_3DAlphaObjects.end(),
		[](FS_3DObject* a, FS_3DObject* b) {
			return a->GetDistanceToCamera() > b->GetDistanceToCamera(); // du plus loin au plus proche
		});
	//TODO Trier les CBs et MBs en même temps que les objets

	for(int i = 0; i < m_3DAlphaObjects.size(); i++)
	{
		auto world = m_3DAlphaObjects[i]->GetWorld();
		XMFLOAT3 pos = { world.m[3][0], world.m[3][1], world.m[3][2] };
		std::cout << "Object " << i << " position: (" << pos.x << ", " << pos.y << ", " << pos.z << "), depth: " << m_3DAlphaObjects[i]->GetDistanceToCamera() << std::endl;
	}
}
void FS_Renderer::UpdateSprites()
{
	if(m_spriteRenderEnabled == false)
		return;

	for (int i = 0; i < m_Sprites.size(); i++)
	{
		FS_Sprite* sprite = m_Sprites[i];
		if (sprite->IsActive() == false)
			continue;

		auto& cb = m_CB2Ds[i];
		auto& mb = m_MB2Ds[i];

		//<Update par objet----
		XMFLOAT4X4 world = sprite->GetWorld();
		XMMATRIX worldObject = XMLoadFloat4x4(&world);

		ObjectConstants2D objConstants;
		XMStoreFloat4x4(&objConstants.m_World, XMMatrixTranspose(worldObject));
		objConstants.m_Mask = sprite->GetMask();
		cb->CopyData(0, objConstants);

		MaterialConstants matConstants;
		matConstants.m_Color = { 1, 1, 1, 1 };
		// Fill material data from material manager so HLSL cbPerMaterial fields match
		MaterialManager* matMgr = FS_Device::Get()->m_FSMaterial;
		int matIndex = sprite->GetMaterialIndex();
		if (matIndex >= 0 && matIndex < matMgr->m_MaterialLibrary.size())
		{
			FS_Material* mat = matMgr->m_MaterialLibrary[matIndex];
			matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
			matConstants.Metallic = mat->Metallic;
			matConstants.Roughness = mat->Roughness;
		}
		mb->CopyData(0, matConstants);

		//----Update par objet>
	}
}

void FS_Renderer::UpdateMainPassCB()
{
	auto fsDevice = FS_Device::Get();
	auto fsCamera = fsDevice->Camera();

	// Zero-initialize to ensure light arrays are not left with garbage
	PassConstants mMainPassCB = {};
	XMFLOAT3 pos = fsCamera->GetPosition();

	mMainPassCB.EyePosW = { pos.x, pos.y, pos.z, 1.0f };
	mMainPassCB.AmbientLight = m_AmbientLight;

	for(int i = 0; i < MaxLights; i++)
	{
		if(i < m_dirLights.size())
			mMainPassCB.dirLights[i] = *m_dirLights[i];
		else
			mMainPassCB.dirLights[i] = DirectionalLight();

		if (i < m_spotLights.size())
			mMainPassCB.spotLights[i] = *m_spotLights[i];
		else
			mMainPassCB.spotLights[i] = DirectionalLight();

		if (i < m_pointLights.size())
			mMainPassCB.pointLights[i] = *m_pointLights[i];
		else
			mMainPassCB.pointLights[i] = DirectionalLight();
	}

	m_PassCB->CopyData(0, mMainPassCB);
}

void FS_Renderer::AfterUpdateObjects()
{
	AfterUpdateCameras();
}

void FS_Renderer::AfterUpdateCameras()
{
	FS_3DCamera* cam = FS_Device::Get()->m_FSCamera;
	cam->AfterUpdate();
}

void FS_Renderer::Draw()
{
	FS_Device* device = FS_Device::Get();
	FS_Command* cmd = device->m_FSCmd;
	FS_Window* win = device->m_FSWindow;
	FS_SwapChain* swap = device->m_FSSwapChain;

	cmd->FlushCommandQueue();

	ThrowIfFailed(cmd->mDirectCmdListAlloc->Reset());
	ThrowIfFailed(cmd->mCommandList->Reset(cmd->mDirectCmdListAlloc.Get(), nullptr));

	D3D12_VIEWPORT viewport = win->GetViewPort();
	D3D12_RECT scissorRect = win->GetScissorRect();

	cmd->mCommandList->RSSetViewports(1, &viewport);
	cmd->mCommandList->RSSetScissorRects(1, &scissorRect);

	// Transition Present -> RenderTarget
	auto presentToTarget = CD3DX12_RESOURCE_BARRIER::Transition(
		swap->CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	cmd->mCommandList->ResourceBarrier(1, &presentToTarget);

	cmd->mCommandList->ClearRenderTargetView(
		swap->CurrentBackBufferView(),
		Colors::LightSteelBlue,
		0, nullptr);

	cmd->mCommandList->ClearDepthStencilView(
		swap->DepthStencilView(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0,
		0, nullptr);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swap->CurrentBackBufferView();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = swap->DepthStencilView();

	cmd->mCommandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);

	// Root signature
	cmd->mCommandList->SetGraphicsRootSignature(m_RootSignature.Get());

	// Descriptor heap
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_SrvDescriptorHeap.Get() };
	cmd->mCommandList->SetDescriptorHeaps(1, descriptorHeaps);

	// Bind Light CB (b3)
	auto passCB = m_PassCB->Resource();
	cmd->mCommandList->SetGraphicsRootConstantBufferView(4, passCB->GetGPUVirtualAddress());

	// Bind camera3D CB (b1)
	cmd->mCommandList->SetGraphicsRootConstantBufferView(1,m_CameraCB3D->Resource()->GetGPUVirtualAddress());

	// Bind camera2D CB (b4)
	cmd->mCommandList->SetGraphicsRootConstantBufferView(5,m_CameraCB2D->Resource()->GetGPUVirtualAddress());

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

	cmd->mCommandList->ResourceBarrier(1, &targetToPresent);

	ThrowIfFailed(cmd->mCommandList->Close());

	ID3D12CommandList* cmdsLists[] = { cmd->mCommandList.Get() };
	cmd->mCommandQueue->ExecuteCommandLists(1, cmdsLists);

	ThrowIfFailed(swap->m_SwapChain->Present(0, 0));

	swap->m_CurrBackBuffer = (swap->m_CurrBackBuffer + 1) % SwapChainBufferCount;

	cmd->FlushCommandQueue();
}