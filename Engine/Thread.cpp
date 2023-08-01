#include "Thread.h"

void threadFunction(Thread* pThread)
{
	pThread->Run();
};


Thread::Thread()
	:mTask(nullptr), mNeedsTask(true)
{
	Start();
}


Thread::~Thread()
{
	mThread.detach();
}

void Thread::Start()
{
	mThread = std::thread(threadFunction, this);
}

void Thread::SetTask(Task* pTask)
{
	mTask = pTask;
	mNeedsTask = false;
}

void Thread::Run()
{
	using namespace std::chrono_literals;

	while (true)
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
		mask = 1 << 0;
		break;
	case TaskType::NETWORK:
		mask = 1 << 1;
		break;
	}

	if (!SetThreadAffinityMask(handle, mask))
	{
		OutputDebugString(L"SetThreadAffinityMask failed, GLE=" + GetLastError());
	}
}
