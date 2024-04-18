#pragma once
#include <any>
#include <memory>
#include <string>

#include "Message.h"
#include "Observer.h"


class GameObject;

/// <summary>
/// Represents a behavior. Behaviors contain logic that act on
/// a `GameObject`. These objects can update properties of the
/// objects they are attached to in a loop or via observers.
/// </summary>
class Behavior: public Observer
{
private:
	std::string _tag;
	std::shared_ptr<GameObject> _owner;
public:
	Behavior(std::string type, std::shared_ptr<GameObject> owner)
		: _owner(owner),
		_tag(type)
	{}

	std::string GetTag() { return _tag; }

	virtual void OnMessage(Message<std::any> msg) = 0;

	// Main update function (called every frame)
	virtual void Update(double deltaTime) = 0;
};

