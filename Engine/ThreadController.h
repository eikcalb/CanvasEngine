#pragma once
#include <stdexcept>
#include <vector>
#include <memory>
#include <queue>
#include <mutex>
#include "Thread.h"

class ThreadController
{
private:
	std::vector<Thread*> mThreads;
	std::queue<Task*> mTasks;

	ThreadController();

public:
	static std::shared_ptr<ThreadController > Instance();

	~ThreadController();

	ThreadController(const ThreadController& ThreadController) = delete;
	ThreadController& operator=(ThreadController const&) = delete;

	Task* const AddTask(std::function<void()> pFunction, const TaskType pType);
	void ProcessTasks();
};

