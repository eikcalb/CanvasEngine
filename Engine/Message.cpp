#include "Message.h"
#include <chrono>

/******************************************************************************************************************/
// Structors
/******************************************************************************************************************/
Message::Message(std::string type)
	: _type(type), timestamp(-1)
{}

Message::Message(std::shared_ptr<BaseMessageType> data, std::string type)
	: _data(data), _type(type)
{
	const auto clock = std::chrono::system_clock::now();
	timestamp = std::chrono::duration_cast<std::chrono::seconds>(clock.time_since_epoch()).count();
}

/******************************************************************************************************************/

/******************************************************************************************************************/
// Functions
/******************************************************************************************************************/
