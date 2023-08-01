#pragma once
#include "Message.h"
#include <GL/GLM/glm.hpp>

class MouseInputMessage: public Message
{
	// Data
protected:
	glm::vec2	_pos;
	bool		_clicked;

	// Structors
public:
	MouseInputMessage(glm::vec2 pos, bool clicked);
	virtual ~MouseInputMessage();


	// Gets/Sets
public:
	glm::vec2	GetPosition()	const { return _pos; }
	bool		GetClicked()		const { return _clicked; }
};

