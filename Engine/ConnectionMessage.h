#pragma once
#include "Message.h"

#include "Connection.h"

class ConnectionMessage : public Message<std::shared_ptr<Connection>>
{
protected:
public:
	ConnectionMessage(std::shared_ptr<Connection> connection, std::string type)
		: Message<std::shared_ptr<Connection>>(connection, type)
	{

	}


};

