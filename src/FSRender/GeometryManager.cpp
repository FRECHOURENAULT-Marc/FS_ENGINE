#include "pch.h"

MeshGeometry* GeometryManager::BuildGeometry(GeometryData* goeData)
{
	std::vector<Vertex> vertex = goeData->vertices;
	UINT vByteCount = goeData->vertices.size() * sizeof(Vertex);
	std::vector<uint16_t> indices = goeData->indices;
	UINT iByteCount = goeData->indices.size() * sizeof(uint16_t);

	MeshGeometry* pGeo = new MeshGeometry();
	ThrowIfFailed(D3DCreateBlob(vByteCount, &pGeo->VertexBufferCPU));
	CopyMemory(pGeo->VertexBufferCPU->GetBufferPointer(), vertex.data(), vByteCount);
	
	ThrowIfFailed(D3DCreateBlob(iByteCount, &pGeo->IndexBufferCPU));
	CopyMemory(pGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), iByteCount);
	
	ID3D12Device* device = FS_Device::Get()->m_d3dDevice.Get();
	ID3D12GraphicsCommandList* cmdList = FS_Device::Get()->m_FSCmd->mCommandList.Get();

	pGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device,
		cmdList, vertex.data(), vByteCount, pGeo->VertexBufferUploader);
	
	pGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(device,
		cmdList, indices.data(), iByteCount, pGeo->IndexBufferUploader);
	
	pGeo->VertexByteStride = sizeof(Vertex);
	pGeo->VertexBufferByteSize = vByteCount;
	pGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	pGeo->IndexBufferByteSize = iByteCount;
	pGeo->IndexCount = iByteCount / sizeof(uint16_t);

	return pGeo;
}

GeometryManager::GeometryManager()
{
	FS_Command* cmdList = FS_Device::Get()->CommandList();

	ThrowIfFailed(cmdList->mCommandList->Reset(cmdList->mDirectCmdListAlloc.Get(), nullptr));

	m_geometries.push_back(BuildGeometry(GeometryGenerator::CreateBox()));
	m_geometries.push_back(BuildGeometry(GeometryGenerator::CreatePyramid()));
	m_geometries.push_back(BuildGeometry(GeometryGenerator::CreateSphere(1.0f, 20, 20)));
	m_geometries.push_back(BuildGeometry(GeometryGenerator::CreatePlane(1.0f, 1.0f)));
	m_geometries.push_back(BuildGeometry(GeometryGenerator::CreateCylinder(1.0f, 1.0f, 3.0f, 50, 50)));
	m_geometries.push_back(BuildGeometry(GeometryGenerator::CreateCapsule(1.0f, 2.0f, 20, 20)));

	m_instance = this;

	cmdList->ExecuteCommands();

	cmdList->FlushCommandQueue();
}

GeometryManager* GeometryManager::Get()
{
	if (m_instance == nullptr)
		m_instance = new GeometryManager();

	return m_instance;
}