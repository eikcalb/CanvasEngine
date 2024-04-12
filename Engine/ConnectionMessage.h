#pragma once
#include "Message.h"
#include "Connection.h"

class ConnectionMessage : public MessageBase<std::shared_ptr<Connection>>
{
protected:
	// This is the raw network data
	std::string raw;
public:
	ConnectionMessage(std::shared_ptr<Connection> connection, std::string type)
		: MessageBase(connection, type)
	{

	}


};

