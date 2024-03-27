#pragma once
#include <thread>
#include <iostream>
#include <chrono>
#include "windows.h"
#include "Task.h"

class Thread
{
private:
	std::thread mThread;
	Task* mTask;
	bool mNeedsTask;
	bool isAlive = false;

	void SetThreadAffinity(const TaskType& type);
public:
	Thread();
	~Thread();

	void Start();
	void SetTask(Task* pTask);
	void Run();
	bool NeedsTask();
};
