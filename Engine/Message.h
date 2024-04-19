#pragma once
#include <memory>
#include <string>

struct BaseMessageType {};

class Message
{
	// Data
protected:
	std::string							_type = NULL;
	std::shared_ptr<BaseMessageType>	_data = nullptr;

public:
	unsigned long long timestamp;

public:
	Message() = default;
	Message(std::string type);
	Message(std::shared_ptr<BaseMessageType> data, std::string type);
	virtual ~Message() = default;

	std::shared_ptr<BaseMessageType> getData() { return _data; }

	// Gets/Sets
public:
	std::string GetMessageType() const { return _type; }
};
