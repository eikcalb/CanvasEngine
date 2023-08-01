#include "MouseInputMessage.h"

MouseInputMessage::MouseInputMessage(glm::vec2 pos, bool clicked)
	: Message("mouseinput"),
	_pos(pos),
	_clicked(clicked)
{

}

/******************************************************************************************************************/

MouseInputMessage::~MouseInputMessage()
{

}
