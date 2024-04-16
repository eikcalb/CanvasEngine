#pragma once
#include "Message.h"
#include <GL/GLM/glm.hpp>

struct MouseMessageInfo {
	glm::vec2			pos;
	bool				clicked;
};

class MouseInputMessage: public Message<std::shared_ptr<MouseMessageInfo>>
{
	// Data
protected:
	// Structors
public:
	MouseInputMessage(std::shared_ptr<MouseMessageInfo> data)
		: Message(data, "mouseinput")
	{

	}

	// Gets/Sets
public:
	glm::vec2	GetPosition()	const { return _data->pos; }
	bool		GetClicked()		const { return _data->clicked; }
};

