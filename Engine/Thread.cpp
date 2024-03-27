#include "Thread.h"

Thread::Thread()
	:mTask(nullptr), mNeedsTask(true)
{
	isAlive = true;
	Start();
}


Thread::~Thread()
{
	isAlive = false;
	mThread.detach();
}

void Thread::Start()
{
	mThread = std::thread([](Thread* thread) { thread->Run(); }, this);
}

void Thread::SetTask(Task* pTask)
{
	mTask = pTask;
	mNeedsTask = false;
}

void Thread::Run()
{
	using namespace std::chrono_literals;

	while (isAlive)
	{
		if (mTask)
		{
			SetThreadAffinity(mTask->TaskType());
			mTask->Run();

			mTask = nullptr;
			mNeedsTask = true;
		}
		std::this_thread::sleep_for(1s);
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
