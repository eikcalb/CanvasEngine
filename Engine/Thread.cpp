#include "Thread.h"

void Thread::SetTask(Task* pTask)
{
	mTask = std::shared_ptr<Task>(pTask);
	mNeedsTask = false;
}

void Thread::Run()
{
	std::mutex mutex;

	while (isAlive)
	{
		std::unique_lock<std::mutex> lock(mutex);
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
		if (isAlive && mTask)
		{
			SetThreadAffinity(mTask->TaskType());
			mTask->Run();

			mTask.reset();
			mTask = nullptr;
			mNeedsTask = true;
		}
	}

	// This will trigger all pending threads waiting when the loop
	// has been triggered to stop.
	condition.notify_all();
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
