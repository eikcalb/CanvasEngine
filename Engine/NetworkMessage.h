#pragma once
#include <memory>
#include "Message.h"

class NetworkMessage : public Message
{
protected:
	// This is the raw network data
	std::string raw;
public:
	NetworkMessage(std::string raw);
	virtual ~NetworkMessage();
};

