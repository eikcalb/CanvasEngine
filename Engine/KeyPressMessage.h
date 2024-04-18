#pragma once
#include <string>

#include "Message.h"

struct KeyPressMessageInfo {
	int					key;
	bool				down;
};

class KeyPressMessage
	: public Message<std::shared_ptr<KeyPressMessageInfo>>
{
public:
	KeyPressMessage(std::shared_ptr<KeyPressMessageInfo> data)
		: Message<std::shared_ptr<KeyPressMessageInfo>>(data, "keypress")
	{}


	// Gets/Sets
public:
	int GetKey()				const { return _data->key; }
	bool GetDown()				const { return _data->down; }
};