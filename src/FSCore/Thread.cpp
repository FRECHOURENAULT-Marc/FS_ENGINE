#include "pch.h"

namespace FSC
{
	DWORD __stdcall Thread::ThreadEntry(LPVOID lpParameter)
	{
		ThreadData* threadData = static_cast<ThreadData*>(lpParameter);
		Thread* thread = threadData->thread;
		thread->m_IsStopped = false;
		while(thread->m_IsStopped == false)
		{
			if (thread->IsRunning() == false && thread->IsInfiniteLoop() == false)
				break;

			threadData->thread->m_IsRunning = true;
			threadData->function(threadData->data);
			threadData->thread->m_IsRunning = false;
		}
		thread->m_IsStopped = true;
		return 0;
	}
	Thread::Thread(std::function<void(void*)> threadFunction, void* data)
	{
		m_ThreadData = new ThreadData();
		m_ThreadData->function = threadFunction;
		m_ThreadData->data = data;
		m_ThreadData->thread = this;
	}

	Thread::~Thread()
	{
		delete(m_ThreadData);
	}

	void Thread::TryStart()
	{
		if (m_IsStopped == false || m_IsRunning)
			return;
		m_IsRunning = true;
		CreateThread(NULL, 0, ThreadEntry, m_ThreadData, 0, 0);
	}
	void Thread::TryClose()
	{
		if(m_IsStopped == false || m_IsRunning)
			return;
		m_IsFinished = true;
		ThreadManager::RemoveFinishedThreads();
	}

	void Thread::WaitForCompletion()
	{
		while (IsRunning()) {};
	}

	void Thread::SetData(void* data)
	{
		if (IsRunning())
			return;
		m_ThreadData->data = data;
	}


	ThreadManager* ThreadManager::Get()
	{
		if (m_Instance == nullptr)
			m_Instance = new ThreadManager();
		return m_Instance;
	}

	void ThreadManager::RemoveFinishedThreads()
	{
		auto& threads = Get()->m_Threads;
		int lastIndex = threads.size() - 1;
		for(int i = lastIndex; i >= 0; i--)
		{
			Thread* t = threads[i];
			if (t->IsFinished() == false)
				continue;
			threads.erase(threads.begin() + i);
			delete(t);
		}
	}

	Thread* ThreadManager::CreateThread(std::function<void(void*)> threadFunction, void* data)
	{
		Thread* thread = new Thread(threadFunction, data);
		Get()->m_Threads.push_back(thread);
		return thread;
	}

}