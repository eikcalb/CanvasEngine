#pragma once
#include <thread>
#include <iostream>
#include <chrono>
#include <windows.h>

#include "Task.h"

class Thread
{
private:
	std::thread mThread;
	/// <summary>
	///  Represents the currently running task.
	/// </summary>
	Task* mTask;
	std::atomic<bool> mNeedsTask;
	std::atomic<bool> isAlive = false;

	std::condition_variable condition;

	void SetThreadAffinity(const TaskType& type);
public:
	Thread()
		:mTask(nullptr), mNeedsTask(true)
	{
		isAlive = true;
		mThread = std::thread([](Thread* thread) { thread->Run(); }, this);
		std::cout << "Created new thread: " << mThread.get_id();
	}

	~Thread()
	{
		isAlive = false;
		mNeedsTask = false;
		mTask = nullptr;
		mThread.detach();
	}

	void SetTask(Task* pTask);
	void Run();
	bool NeedsTask();
};
