#pragma once
#include "Message.h"

#include "Connection.h"

struct ConnectionMessageInfo: public BaseMessageType {
	std::shared_ptr<Connection> conn;
};

class ConnectionMessage : public Message
{
public:
	ConnectionMessage(std::shared_ptr<ConnectionMessageInfo> connInfo, std::string type)
		: Message(connInfo, type)
	{

	}

	std::shared_ptr<Connection> GetConnection() { return reinterpret_cast<ConnectionMessageInfo*>(_data.get())->conn; }
};

