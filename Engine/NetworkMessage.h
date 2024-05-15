#pragma once
#include <memory>
#include <vector>
#include <sstream>

#include "Message.h"
#include "types.h"

struct NetworkMessageInfo: public BaseMessageType {
	std::string			peerID;
	std::vector<byte>	message;
};

enum class MessageType {
	INIT = 0b0001,
	DRAW = 0b0010,
	INTEGRITY = 0b0100,
};

struct NetworkMessageContent {
	MessageType type;
	long sequence;
	std::string content;
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

    NetworkMessageContent ParseNetworkMessage() {
        const auto& raw = GetMessage()->message;
        const auto messageString = std::string(raw.begin(), raw.end());
        NetworkMessageContent messageContent;

        // Create a string stream to tokenize the input string
        std::istringstream iss(messageString);
        std::string token;

        // Parse each token separated by semicolons
        int tokenIndex = 0;
        while (std::getline(iss, token, ';')) {
            switch (tokenIndex) {
            case 0:
                // Parse MessageType
                if (token == "INIT")
                    messageContent.type = MessageType::INIT;
                else if (token == "DRAW")
                    messageContent.type = MessageType::DRAW;
                else if (token == "INTEGRITY")
                    messageContent.type = MessageType::INTEGRITY;
                else
                    throw std::invalid_argument("Invalid message type");
                break;
            case 1:
                // Parse sequence
                messageContent.sequence = std::stol(token);
                break;
            case 2:
                // Parse content
                messageContent.content = token;
                break;
            default:
                throw std::invalid_argument("Too many tokens in the message");
            }
            tokenIndex++;
        }

        // Validate that all required fields are present
        if (tokenIndex != 3) {
            throw std::invalid_argument("Insufficient tokens in the message");
        }

        return messageContent;
    }

};
