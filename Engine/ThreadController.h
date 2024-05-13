#pragma once
#include <stdexcept>
#include <vector>
#include <memory>
#include <queue>
#include <mutex>

#include "Thread.h"

/// <summary>
/// `ThreadController` is used to create a thread pool and utilized in running tasks.
/// The processor afinity is set to only run on 2 cores. This is calculated based on
/// the principle that the bitmask represents logical processors (CPUs) and a bitmask
/// of 4 will capture all logical processors in the 2 cores (0, 1, 2, 3). Thread
/// affinity can be used to specify what particular processor we want a thread to
/// work on.
/// 
/// An alternative would be to use IO completion ports:
/// https://learn.microsoft.com/en-us/windows/win32/fileio/i-o-completion-ports,
///  but that defeats the requirement for this software to control the frequency and
/// affinity of the threads.
/// 
/// [EDIT]: Upon further research, it appears that using IOCP might work better while
/// fulfilling the requirements of the application, but that implementation can be
/// done in a separate class or a subclass. For this version of the application, we
/// will not be using IOCP.
/// </summary>
class ThreadController
{
private:
	std::atomic<bool> should_stop = false;
	std::vector<Thread*> threads;

	std::mutex tasksMutex;
	std::condition_variable condition;

	ThreadController()
	{
		// Here we get the number of processors in the computer.
		int numProcessors = static_cast<int>(std::thread::hardware_concurrency());
		if (numProcessors < 4) {
			throw std::exception("Minimum processor count allowed is 4");
		}

		// Set the allowed processors to run on. Processor 1 and processor 2.
		// Sourced from:
		// - https://stackoverflow.com/questions/12803585/example-usage-of-setprocessaffinitymask-in-c
		// - https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setprocessaffinitymask
		DWORD_PTR processorAffinity = (1 << 0) | (1 << 1);
		if (!SetProcessAffinityMask(GetCurrentProcess(), processorAffinity)) {
			OutputDebugString(L"SetProcessAffinityMask failed, GLE=" + GetLastError());
			throw std::runtime_error("Requirement for processor affinity failed");
		}

		// We will be creating a threadpool in order to alleviate overhead when creating
		// these threads on demand. We will double the threads used in the application.
		// So if we have 8 processors, we will have 16 threads.
		// 
		// Ideally, the system would be intelligent and manage the thread pool by creating
		// new threads when work increases, and killing unused threads when idle, sort of
		// how Kubernetes manages container pods.
		for (int i = 0; i < (numProcessors * 2); i++)
		{
			threads.emplace_back(new Thread());
		}
		OutputDebugString(L"Created thread count: " + threads.size());
	}

public:
	static std::shared_ptr<ThreadController > Instance();

	~ThreadController();

	ThreadController(const ThreadController& ThreadController) = delete;
	ThreadController& operator=(ThreadController const&) = delete;

	Task* const AddTask(std::function<void()> pFunction, const TaskType pType, const std::string tag);

	void Stop() {
		should_stop = true;
	}
};
 