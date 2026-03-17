#include "pch.h"

FS_ShaderPSO::~FS_ShaderPSO()
{
	mPSO.Reset();
	mvsByteCode.Reset();
	mpsByteCode.Reset();
	mInputLayout.clear();
}

void FS_ShaderPSO::BuildShadersAndInputLayout(std::wstring _path, std::vector<D3D12_INPUT_ELEMENT_DESC> _Layout, bool isUI)
{
	mvsByteCode = d3dUtil::CompileShader(_path, nullptr, "VS", "vs_5_0");
	mpsByteCode = d3dUtil::CompileShader(_path, nullptr, "PS", "ps_5_0");

	mInputLayout = _Layout;
	mIsUI = isUI;
}

void FS_ShaderPSO::BuildPSO(ID3D12RootSignature* rootSignature, DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat, bool isTransparent, bool Msaa4xState, UINT msaaQuality)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	psoDesc.pRootSignature = rootSignature;
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()),
		mvsByteCode->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()),
		mpsByteCode->GetBufferSize()
	};
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	m_isTransparent = isTransparent;
	if(m_isTransparent)
	{
		// Blend standard alpha (src alpha over dest)
		D3D12_RENDER_TARGET_BLEND_DESC rtBlend = {};
		rtBlend.BlendEnable = TRUE;
		rtBlend.LogicOpEnable = FALSE;

		rtBlend.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		rtBlend.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		rtBlend.BlendOp = D3D12_BLEND_OP_ADD;

		rtBlend.SrcBlendAlpha = D3D12_BLEND_ONE;
		rtBlend.DestBlendAlpha = D3D12_BLEND_ZERO;
		rtBlend.BlendOpAlpha = D3D12_BLEND_OP_ADD;

		rtBlend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		blendDesc.RenderTarget[0] = rtBlend;
		psoDesc.BlendState = blendDesc;

		D3D12_DEPTH_STENCIL_DESC dsDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		psoDesc.DepthStencilState = dsDesc;
	}

	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = backBufferFormat;

	psoDesc.SampleDesc.Count = Msaa4xState ? 4 : 1;
	psoDesc.SampleDesc.Quality = Msaa4xState ? (msaaQuality - 1) : 0;

	psoDesc.DSVFormat = depthStencilFormat;
	ThrowIfFailed(FS_Device::Get()->m_d3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));
}