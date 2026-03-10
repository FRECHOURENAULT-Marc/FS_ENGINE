#pragma once

class FS_ShaderPSO
{
public:
	FS_ShaderPSO() = default;
	~FS_ShaderPSO();

	void BuildShadersAndInputLayout(std::wstring _path, std::vector<D3D12_INPUT_ELEMENT_DESC> _Layout, bool isUI = false);

	void BuildPSO(ID3D12RootSignature* rootSignature, DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat, bool isTransparent = false, bool Msaa4xState = false, UINT m4xMsaaQuality = 0);

//protected:
	
	//<PSO----
	ComPtr<ID3D12PipelineState> mPSO = nullptr; // créé dans BuildPSO() ### utilisé dans Draw()
	ComPtr<ID3DBlob> mvsByteCode = nullptr; // créé dans BuildShadersAndInputLayout() ### utilisé dans BuildPSO()
	ComPtr<ID3DBlob> mpsByteCode = nullptr; // créé dans BuildShadersAndInputLayout() ### utilisé dans BuildPSO()
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout; // créé dans BuildShadersAndInputLayout() ### utilisé dans BuildPSO()

	bool mIsUI = false;
	//----PSO>
};