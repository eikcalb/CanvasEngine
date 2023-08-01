#include "GameObjectComponent.h"
#include "GameObject.h"
#include "Game.h"

/******************************************************************************************************************/
// Structors
/******************************************************************************************************************/

GameObjectComponent::GameObjectComponent(std::string type, GameObject* owner)
	: _owner(owner),
	  _componentType(type)
{
	// Add us to the owner component
	owner->AddComponent(this);
}

/******************************************************************************************************************/

GameObjectComponent::~GameObjectComponent()
{

}

/******************************************************************************************************************/
// Functions
/******************************************************************************************************************/

void GameObjectComponent::BroadcastMessage(Message* msg)
{
	Game::TheGame->BroadcastMessage(msg);
}

/******************************************************************************************************************/
