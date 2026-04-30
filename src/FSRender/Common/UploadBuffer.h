#pragma once

template<typename T>
class UploadBuffer
{
public:
    UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer);

    UploadBuffer(const UploadBuffer& rhs) = delete;
    UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
    ~UploadBuffer();

    ID3D12Resource* Resource()const;

    void CopyData(int elementIndex, const T& data);

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> m_UploadBuffer;
    BYTE* m_MappedData = nullptr;

    UINT m_ElementByteSize = 0;
    bool m_IsConstantBuffer = false;
};

#include "UploadBuffer.inl"