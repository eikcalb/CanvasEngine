#pragma once
#include <string>
#include <any>
#include <chrono>

template <typename T>
class Message
{
	// Data
protected:
	std::string _type;
	T			_data;

public:
	unsigned long long timestamp;

public:
	Message() = default;
	Message(std::string type);
	Message(T data, std::string type);
	virtual ~Message() = default;

	T getData() { return _data };

	// Gets/Sets
public:
	std::string GetMessageType() const { return _type; }
};


class BareMessage : public Message<std::any> {
public:
	BareMessage(std::string type) : Message(type) {};
};