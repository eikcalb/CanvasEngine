#pragma once
#include <string>
#include <any>

template <typename T>
class Message
{
	// Data
protected:
	std::string _type;
	T			_data;

	// Structors
public:
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