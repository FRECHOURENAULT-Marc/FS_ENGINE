#include "pch.h"

namespace FSR
{
	CommandList::CommandList()
	{
	}

	void CommandList::CreateCommandObjects(ID3D12Device* md3dDevice)
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		ThrowIfFailed(md3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)));

		ThrowIfFailed(md3dDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(m_DirectCmdListAlloc.GetAddressOf())));

		ThrowIfFailed(md3dDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_DirectCmdListAlloc.Get(),
			nullptr,
			IID_PPV_ARGS(m_CommandList.GetAddressOf())));

		m_CommandList->Close();
	}
	void CommandList::ExecuteCommands()
	{
		ThrowIfFailed(m_CommandList->Close());
		ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	}
	void CommandList::FlushCommandQueue()
	{
		mCurrentFence++;

		ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), mCurrentFence));

		if (m_Fence->GetCompletedValue() < mCurrentFence)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

			ThrowIfFailed(m_Fence->SetEventOnCompletion(mCurrentFence, eventHandle));

			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	bool CommandList::CloseCommandList()
	{
		return m_CommandList->Close();
	}
}
