#pragma once
#include <memory>

#include "Message.h"
#include "types.h"

struct NetworkMessageInfo {
	std::string			peerID;
	std::vector<byte>	message;
};

class NetworkMessage : public Message<std::shared_ptr<NetworkMessageInfo>>
{
protected:
public:
	NetworkMessage(std::shared_ptr<NetworkMessageInfo> details, std::string type)
		: Message<std::shared_ptr<NetworkMessageInfo>>(details, type)
	{

	}
	virtual ~NetworkMessage();
};
