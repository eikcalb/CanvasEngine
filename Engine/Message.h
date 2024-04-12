#pragma once
#include <string>
#include <any>

template <typename T>
class MessageBase
{
	// Data
protected:
	std::string _type;
	T			_data;

	// Structors
public:
	MessageBase(std::string type);
	MessageBase(T data, std::string type);
	virtual ~MessageBase() = default;

	T getData() { return _data };

	// Gets/Sets
public:
	std::string GetMessageType()		const { return _type; }
};

class Message: MessageBase<std::any>
{
};