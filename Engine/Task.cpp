#include "Task.h"

Task::~Task()
{
}

void Task::Run()
{
	if (IsDone()) {
		return;
	}

	mFunction();
	mIsDone = true;
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