#include "ThreadController.h"

ThreadController::~ThreadController()
{
	Stop();

	for (auto& thread : threads)
	{
		delete thread;
	}
}

/// <summary>
/// Adds a task to be run on a thread. All threads are running and we will add a task to the
/// next available thread. This approach is simple and an alternative would be to use a round
/// robin implementation that threats each thread like a call center agent and moves to the
/// next thread. With this current implementation, there might be situations of low thread use,
/// wherein the lower index threads will be more utilized than the others when tasks finish
/// quickly.
/// 
/// The Caveat here is that if tasks block each thread, then we will run out of threads.
/// </summary>
/// <param name="pFunction"></param>
/// <param name="pType"></param>
/// <param name="tag"></param>
/// <returns></returns>
Task* const ThreadController::AddTask(std::function<void()> pFunction, const TaskType pType, const std::string tag)
{
	Task* task = new Task(pFunction, pType, tag);
	{
		std::lock_guard lock(tasksMutex);

		for (auto& thread : threads) {
			if (thread->NeedsTask()) {
				thread->SetTask(task);
				break;
			}
		}
	}

	return task;
}

std::shared_ptr<ThreadController> ThreadController::Instance()
{
	static std::shared_ptr<ThreadController> instance{};
	return instance;
}
