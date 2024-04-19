#pragma once
#include <GL/GLM/glm.hpp>
#include "Message.h"

struct MouseMessageInfo : public BaseMessageType {
	glm::vec2			pos;
	bool				clicked;
};

class MouseInputMessage : public Message
{
	// Data
protected:
	// Structors
public:
	MouseInputMessage(std::shared_ptr<MouseMessageInfo> data, std::string type)
		: Message(data, type)
	{

	}

	// Gets/Sets
public:
	glm::vec2	GetPosition()	const { return reinterpret_cast<MouseMessageInfo*>(_data.get())->pos; }
	bool		GetClicked()	const { return reinterpret_cast<MouseMessageInfo*>(_data.get())->clicked; }
};

