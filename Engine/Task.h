#pragma once
#include <iostream>
#include <functional>
#include <vector>

// forward declare the Thread class.
class Thread;

enum class TaskType {
	GRAPHICS = 0,
	NETWORK = 1
};

class Task
{
private:
	std::function<void()> mFunction = nullptr;
	std::string mTag;
	TaskType mType;
	bool mIsDone;

protected:
	void Run();
public:

	Task(std::function<void()> pFunction, const TaskType pType, const std::string mTag) : mFunction(pFunction), mIsDone(false), mType(pType), mTag(mTag) {};

	~Task();

	const TaskType& TaskType();
	bool IsDone();
	void CleanUpTask();

	inline std::string GetTag() { return mTag; }

	friend Thread;
};