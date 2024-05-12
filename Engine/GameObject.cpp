#include "GameObject.h"

#include "Mesh.h"
#include "VBO.h"

/******************************************************************************************************************/
// Structors
/******************************************************************************************************************/

GameObject::GameObject(std::string type)
	: _angle(0),
	_scale(1),
	_position(0, 0),
	_alive(true),
	_type(type),
	_deleteFlag(false),
	_shouldDraw(false),
	_semPosition(1)
{
	_generatorData = std::make_shared<VBOInstanceData>();
}

/******************************************************************************************************************/

GameObject::~GameObject()
{
	End();
}

/******************************************************************************************************************/
// Component Functions
/******************************************************************************************************************/

bool GameObject::AddBehavior(std::shared_ptr<Behavior> b)
{
	if (_behaviors.find(b->GetTag()) != _behaviors.end())
	{
		// Error - already have this component!
		return false;
	}
	_behaviors[b->GetTag()] = b;

	return true;
}

/******************************************************************************************************************/

bool GameObject::RemoveBehavior(std::shared_ptr<Behavior> b)
{
	return RemoveBehavior(b->GetTag());
}

/******************************************************************************************************************/

bool GameObject::RemoveBehavior(std::string tag)
{
	auto i = _behaviors.find(tag);
	if (i != _behaviors.end())
	{
		// Remove it
		_behaviors.erase(i);

		// Delete it
		i->second.reset();

		return true;
	}

	// Couldn't find it
	return false;
}

/******************************************************************************************************************/

std::shared_ptr<Behavior> GameObject::GetBehavior(std::string tag)
{
	auto i = _behaviors.find(tag);
	if (i != _behaviors.end())
	{
		// Return it
		return i->second;
	}

	// Couldn't find it
	return NULL;
}

/******************************************************************************************************************/
// General Functions
/******************************************************************************************************************/

// Setup function -- called to initialise object and its components
void GameObject::Start()
{
	// Initialise all objects
	//for (auto& i : _behaviors)
	//{}
}

/******************************************************************************************************************/

// Main update function (called every frame)
void GameObject::Update(double deltaTime)
{
	if (!ShouldUpdate()) {
		return;
	}

	// Update all objects
	for (auto& i : _behaviors)
	{
		i.second->Update(deltaTime);
	}
}

/******************************************************************************************************************/

// Message handler (called when message occurs)
void GameObject::OnMessage(Message* msg)
{
	// Mock implementation of a message listener.
	// Subclasses should implement specific listener logic.
}

/******************************************************************************************************************/

// Shutdown function -- called when object is destroyed (i.e., from destructor)
void GameObject::End()
{
	// Clear behavior list
	_behaviors.clear();
}

/******************************************************************************************************************/

// Resets the game object to the start state (similar to Start(), but may be called more than once)
void GameObject::Reset()
{
	for (auto& i : _behaviors)
	{
		i.second->Reset();
	}
}

/******************************************************************************************************************/
