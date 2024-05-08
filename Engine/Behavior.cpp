#include "Behavior.h"
#include "GameObject.h"

Behavior::Behavior(std::string type, std::shared_ptr<GameObject> owner)
	:
	_owner(owner),
	_tag(type)
{}