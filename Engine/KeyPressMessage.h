#pragma once
#include <string>

#include "Message.h"

struct KeyPressMessageInfo : public BaseMessageType {
	int					key;
	bool				down;
};

class KeyPressMessage
	: public Message
{
public:
	KeyPressMessage(std::shared_ptr<KeyPressMessageInfo> data, std::string type)
		: Message(data, type)
	{}


	// Gets/Sets
public:
	int GetKey()				const { return reinterpret_cast<KeyPressMessageInfo*>(_data.get())->key; }
	bool GetDown()				const { return reinterpret_cast<KeyPressMessageInfo*>(_data.get())->down; }
};