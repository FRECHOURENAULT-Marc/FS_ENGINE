#pragma once

namespace FSR
{
	class ShaderPSO
	{
	public:
		ShaderPSO() = default;
		~ShaderPSO();

		bool IsUI() { return m_IsUI; }
		bool IsTransparent() { return m_isTransparent; }
		ComPtr<ID3D12PipelineState> GetPSO() { return m_PSO; }

		void BuildShadersAndInputLayout(std::wstring _path, std::vector<D3D12_INPUT_ELEMENT_DESC> _Layout, bool isUI = false);
		void BuildPSO(ID3D12RootSignature* rootSignature, DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat, bool isTransparent = false, bool Msaa4xState = false, UINT m4xMsaaQuality = 0);

	protected:
		ComPtr<ID3D12PipelineState> m_PSO = nullptr;
		ComPtr<ID3DBlob> m_VSByteCode = nullptr;
		ComPtr<ID3DBlob> m_PSByteCode = nullptr;
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

		bool m_IsUI = false;
		bool m_isTransparent = false;
	};
}