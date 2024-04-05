#include "Task.h"

Task::~Task()
{
}

void Task::Run()
{
	if (IsDone()) {
		return;
	}

	std::cout << "Started running task: " << mTag;
	mFunction();
	mIsDone = true;
	std::cout << "Finished running task: " << mTag;
}

const TaskType& Task::TaskType()
{
	return mType;
}

bool Task::IsDone()
{
	return mIsDone;
}

void Task::CleanUpTask()
{
	delete this;
}