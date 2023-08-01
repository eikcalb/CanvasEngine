#include "ThreadController.h"

ThreadController::ThreadController()
{
	int numCores = static_cast<int>(std::thread::hardware_concurrency());
	if (numCores < 2) {
		throw std::exception("Minimum processor count allowed is 2");
	}

	// Set the allowed processors to run on
	DWORD_PTR processorAffinity = (1 << 0) | (1 << 1); // Processor 1 and processor 2
	if (!SetProcessAffinityMask(GetCurrentProcess(), processorAffinity)) {
		OutputDebugString(L"SetProcessAffinityMask failed, GLE=" + GetLastError());
	}

	for (int i = 0; i < (numCores * 2); i++)
	{
		mThreads.push_back(new Thread());
	}
}

ThreadController::~ThreadController()
{
	for (auto& thread : mThreads)
	{
		delete thread;
	}
}

Task* const ThreadController::AddTask(std::function<void()> pFunction, const TaskType pType)
{
	Task* task = new Task(pFunction, pType);

	mTasks.push(task);

	return task;
}

void ThreadController::ProcessTasks()
{
	for (auto thread : mThreads)
	{
		if (mTasks.size() > 0)
		{
			if (thread->NeedsTask())
			{
				thread->SetTask(mTasks.front());
				mTasks.pop();
			}
		}
	}
}

std::shared_ptr<ThreadController> ThreadController::Instance()
{
	static std::shared_ptr<ThreadController> instance{ new ThreadController() };
	return instance;
}
