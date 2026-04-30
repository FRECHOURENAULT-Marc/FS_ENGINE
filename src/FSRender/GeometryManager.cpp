#include "pch.h"

namespace FSR
{

	MeshGeometry* GeometryManager::BuildGeometry(GeometryData* goeData)
	{
		std::vector<Vertex> vertex = goeData->m_Vertices;
		UINT vByteCount = goeData->m_Vertices.size() * sizeof(Vertex);
		std::vector<uint16_t> indices = goeData->m_Indices;
		UINT iByteCount = goeData->m_Indices.size() * sizeof(uint16_t);

		MeshGeometry* pGeo = new MeshGeometry();
		ThrowIfFailed(D3DCreateBlob(vByteCount, &pGeo->m_VertexBufferCPU));
		CopyMemory(pGeo->m_VertexBufferCPU->GetBufferPointer(), vertex.data(), vByteCount);

		ThrowIfFailed(D3DCreateBlob(iByteCount, &pGeo->m_IndexBufferCPU));
		CopyMemory(pGeo->m_IndexBufferCPU->GetBufferPointer(), indices.data(), iByteCount);

		ID3D12Device* device = Device::GetD3DDevice().Get();
		ID3D12GraphicsCommandList* cmdList = Device::GetCommand()->m_CommandList.Get();

		pGeo->m_VertexBufferGPU = d3dUtil::CreateDefaultBuffer(device,
			cmdList, vertex.data(), vByteCount, pGeo->m_VertexBufferUploader);

		pGeo->m_IndexBufferGPU = d3dUtil::CreateDefaultBuffer(device,
			cmdList, indices.data(), iByteCount, pGeo->m_IndexBufferUploader);

		pGeo->m_VertexByteStride = sizeof(Vertex);
		pGeo->m_VertexBufferByteSize = vByteCount;
		pGeo->m_IndexFormat = DXGI_FORMAT_R16_UINT;
		pGeo->m_IndexBufferByteSize = iByteCount;
		pGeo->m_IndexCount = iByteCount / sizeof(uint16_t);

		return pGeo;
	}

	GeometryManager::GeometryManager()
	{
		CommandList* cmdList = Device::GetCommand();

		ThrowIfFailed(cmdList->m_CommandList->Reset(cmdList->m_DirectCmdListAlloc.Get(), nullptr));

		m_geometries.push_back(BuildGeometry(GeometryGenerator::CreateBox()));

		m_geometries.push_back(BuildGeometry(GeometryGenerator::CreateBox_UVMapped()));

		m_geometries.push_back(BuildGeometry(GeometryGenerator::CreatePyramid()));
		m_geometries.push_back(BuildGeometry(GeometryGenerator::CreateSphere(0.5f, 10, 10)));
		m_geometries.push_back(BuildGeometry(GeometryGenerator::CreatePlane(1.0f, 1.0f)));
		m_geometries.push_back(BuildGeometry(GeometryGenerator::CreatePlaneScreenOriented(1.0f, 1.0f)));
		m_geometries.push_back(BuildGeometry(GeometryGenerator::CreateCylinder(0.5f, 0.5f, 1.0f, 10, 10)));
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
}