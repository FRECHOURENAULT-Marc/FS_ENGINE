#pragma once

extern const int gNumFrameResources;

inline std::wstring AnsiToWString(const std::string& str)
{
    WCHAR buffer[512];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
    return std::wstring(buffer);
}

class d3dUtil
{ 
public:

    static UINT CalcConstantBufferByteSize(UINT byteSize)
    {
        return (byteSize + 255) & ~255;
    }

    static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const void* initData, UINT64 byteSize, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

	static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target);
};

class DxException
{
public:
    DxException() = default;
    DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

    std::wstring ToString()const;

    HRESULT ErrorCode = S_OK;
    std::wstring FunctionName;
    std::wstring Filename;
    int LineNumber = -1;
};

namespace FSC 
{
	struct MeshGeometry
	{
		~MeshGeometry();

		Microsoft::WRL::ComPtr<ID3DBlob> m_VertexBufferCPU = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> m_IndexBufferCPU = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBufferGPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBufferGPU = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBufferUploader = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBufferUploader = nullptr;

		UINT m_VertexByteStride = 0;
		UINT m_VertexBufferByteSize = 0;
		DXGI_FORMAT m_IndexFormat = DXGI_FORMAT_R16_UINT;
		UINT m_IndexBufferByteSize = 0;
		UINT m_IndexCount;

		D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
		{
			D3D12_VERTEX_BUFFER_VIEW vbv;
			vbv.BufferLocation = m_VertexBufferGPU->GetGPUVirtualAddress();
			vbv.StrideInBytes = m_VertexByteStride;
			vbv.SizeInBytes = m_VertexBufferByteSize;

			return vbv;
		}

		D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
		{
			D3D12_INDEX_BUFFER_VIEW ibv;
			ibv.BufferLocation = m_IndexBufferGPU->GetGPUVirtualAddress();
			ibv.Format = m_IndexFormat;
			ibv.SizeInBytes = m_IndexBufferByteSize;

			return ibv;
		}

		void DisposeUploaders()
		{
			m_VertexBufferUploader = nullptr;
			m_IndexBufferUploader = nullptr;
		}
	};
}



#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif