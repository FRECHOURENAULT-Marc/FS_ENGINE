#pragma once
namespace FSC
{
	class Thread;
	struct ThreadData
	{
		Thread* thread;
		std::function<void(void*)> function;
		void* data;
	};
	class Thread
	{
		ThreadData* m_ThreadData = nullptr;
		bool m_IsRunning = false;
		bool m_IsStopped = true;
		bool m_IsInfiniteLoop = false;
		bool m_IsFinished = false;

		static DWORD WINAPI ThreadEntry(LPVOID lpParameter);

		Thread(std::function<void(void*)> threadFunction, void* data = nullptr);
		virtual ~Thread();

	public:

		// Check if the thread is currently calling the function set in constructor
		virtual bool IsRunning() const { return m_IsRunning; };
		// Check if the thread is currently stopped (not running and not finished)
		virtual bool IsStopped() const { return m_IsStopped; };
		// Check if the thread is set to run in an infinite loop (execution until the thread is stopped)
		virtual bool IsInfiniteLoop() const { return m_IsInfiniteLoop; };
		// Check if the thread is finished (= is finished to call function set in constructor)
		virtual bool IsFinished() const { return m_IsFinished; };

		// Try to start the thread (if the thread is already running or not stopped, it won't start)
		virtual void TryStart();
		// Stop calling function set in constructor (if the thread is already stopped, it won't stop)
		virtual void Stop() { m_IsStopped = true; };
		// Try to close (= delete) the thread (if the thread is already stopped or still running, it won't close)
		virtual void TryClose();
		//Same as TryClose
		virtual void TryDestroy() { TryClose(); };

		// Wait until the current calling is finished (if IsInfiniteLoop == true, it never pass)
		virtual void WaitForCompletion();
		// Set the thread to run in an infinite loop (execution until the thread is stopped)
		virtual void DefineAsInfiniteLoop() { m_IsInfiniteLoop = true; };

		// Set (replace) the data to send to the function (if the thread is currently running, this call will be ignored)
		virtual void SetData(void* data);

		friend class ThreadManager;
	};

	class ThreadManager final
	{
		static inline ThreadManager* m_Instance = nullptr;

		std::vector<Thread*> m_Threads;

		static ThreadManager* Get();

		static void RemoveFinishedThreads();

	public:

		/// <summary>
		/// Create a thread (not started)
		/// </summary>
		/// <param name="threadFunction"> : The function to call in the thread, must return void</param>
		/// <param name="data"> : Parameter send to function (cast it in void*)</param>
		/// <returns>Thread class created, use to start/stop/close the thread</returns>
		static Thread* CreateThread(std::function<void(void*)> threadFunction, void* data = nullptr);

		friend class Thread;
	};

}
