#pragma once

struct FS_Command
{

public:

	FS_Command();

	void CreateCommandObjects(ID3D12Device* md3dDevice);
	void ExecuteCommands();

	void FlushCommandQueue();
	bool CloseCommandList();

	Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence; // créé dans InitDirect3D() ### utilisée dans FlushCommandQueue()
	UINT64 mCurrentFence = 0;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue; // créé dans CreateCommandObjects() ### utilisé dans trop de fonctions... (faire un get ?)
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
};
