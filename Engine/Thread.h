#pragma once
#include <thread>
#include <iostream>
#include <chrono>
#include <sstream>
#include <windows.h>

#include "Task.h"

class Thread
{
private:
	std::thread mThread;
	/// <summary>
	///  Represents the currently running task.
	/// </summary>
	std::shared_ptr<Task> mTask;
	std::atomic<bool> mNeedsTask;
	std::atomic<bool> isAlive = false;

	std::condition_variable condition;

	void SetThreadAffinity(const TaskType& type);
public:
	Thread()
		: mNeedsTask(true)
	{
		isAlive = true;
		mTask = nullptr;
		mThread = std::thread([](Thread* thread) { thread->Run(); }, this);

		std::stringstream s;
		s << "Created new thread: " << mThread.get_id() << std::endl;
		auto ss = s.str();
		OutputDebugString(std::wstring(ss.begin(), ss.end()).c_str());
	}

	~Thread()
	{
		isAlive = false;
		mNeedsTask = false;
		mTask = nullptr;
		mThread.detach();
		condition.notify_one();
	}

	void SetTask(Task* pTask);
	void Run();
	bool NeedsTask();
};
