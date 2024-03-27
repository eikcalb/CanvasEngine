#include "NetworkMessage.h"

NetworkMessage::NetworkMessage(std::string raw)
	: Message("network"), raw(raw)
{

}

NetworkMessage::~NetworkMessage()
{

}