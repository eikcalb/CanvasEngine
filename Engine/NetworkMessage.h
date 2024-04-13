#pragma once
#include <memory>
#include "Message.h"

struct NetworkMessageInfo {
	std::string			peerID;
	std::vector<byte>	message;
};


class NetworkMessage : public Message<std::shared_ptr<NetworkMessageInfo>>
{
protected:
public:
	NetworkMessage(std::shared_ptr<NetworkMessageInfo> details, std::string type)
		: Message(details, type)
	{

	}
	virtual ~NetworkMessage();
};
