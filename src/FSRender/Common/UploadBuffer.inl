#include "pch.h"

template <typename T>
UploadBuffer<T>::UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer) : m_IsConstantBuffer(isConstantBuffer)
{
    m_ElementByteSize = sizeof(T);

    if (isConstantBuffer)
        m_ElementByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(T));

    CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(m_ElementByteSize * elementCount);

    ThrowIfFailed(device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_UploadBuffer)));

    ThrowIfFailed(m_UploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedData)));

}

template<typename T>
UploadBuffer<T>::~UploadBuffer()
{
    if (m_UploadBuffer != nullptr)
        m_UploadBuffer->Unmap(0, nullptr);

    m_MappedData = nullptr;
}

template<typename T>
ID3D12Resource* UploadBuffer<T>::Resource()const
{
    return m_UploadBuffer.Get();
}

template<typename T>
void UploadBuffer<T>::CopyData(int elementIndex, const T& data)
{
    memcpy(&m_MappedData[elementIndex * m_ElementByteSize], &data, sizeof(T));
}