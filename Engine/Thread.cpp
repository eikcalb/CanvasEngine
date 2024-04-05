#include "Thread.h"

void Thread::SetTask(Task* pTask)
{
	mTask = pTask;
	mNeedsTask = false;
}

void Thread::Run()
{
	std::mutex mutex;

	while (isAlive)
	{
		std::unique_lock<std::mutex> lock;
		condition.wait(lock, [this] {
			// If we have a signal to exit, we should stop waiting
			if (!isAlive) {
				return true;
			}

			// When there is a task, we will exit the wait.
			return mTask != nullptr;
			});

		lock.unlock();

		// Since we have a task, we will process it.
		if (mTask)
		{
			SetThreadAffinity(mTask->TaskType());
			mTask->Run();

			mTask = nullptr;
			mNeedsTask = true;
		}
	}
}

bool Thread::NeedsTask()
{
	return mNeedsTask;
}

void Thread::SetThreadAffinity(const TaskType& type)
{
	const auto handle = mThread.native_handle();
	DWORD_PTR mask = 0;

	switch (type) {
	case TaskType::GRAPHICS:
		mask = 1 << (int)TaskType::GRAPHICS;
		break;
	case TaskType::NETWORK:
		mask = 1 << (int)TaskType::NETWORK;
		break;
	}

	if (!SetThreadAffinityMask(handle, mask))
	{
		OutputDebugString(L"SetThreadAffinityMask failed, GLE=" + GetLastError());
	}
}
