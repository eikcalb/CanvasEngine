#pragma once
#include <memory>
#include <vector>

#include "Message.h"
#include "types.h"

struct NetworkMessageInfo: public BaseMessageType {
	std::string			peerID;
	std::vector<byte>	message;
};

class NetworkMessage : public Message
{
protected:
public:
	NetworkMessage(std::shared_ptr<NetworkMessageInfo> details, std::string type)
		: Message(details, type)
	{

	}
	virtual ~NetworkMessage();

	const NetworkMessageInfo* GetMessage() { return reinterpret_cast<NetworkMessageInfo*>(_data.get()); }
};
