#pragma once
#include <functional>
#include <vector>

enum class TaskType {
	GRAPHICS = 0,
	NETWORK = 1
};

class Task
{
private:
	std::function<void()> mFunction = nullptr;
	TaskType mType;
	bool mIsDone;

public:

	Task(std::function<void()> pFunction, const TaskType pType) : mFunction(pFunction), mIsDone(false), mType(pType) {};

	~Task();

	void Run();
	const TaskType& TaskType();
	bool IsDone();
	void CleanUpTask();
};