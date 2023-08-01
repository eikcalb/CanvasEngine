#include "CubeControllerComponent.h"
#include "GameObject.h"
#include "Game.h"
#include "KeyPressMessage.h"
#include "Cube.h"
#include <WinUser.h>

const float CubeControllerComponent::ACCELERATION = 1.0f;

/******************************************************************************************************************/
// Structors
/******************************************************************************************************************/

CubeControllerComponent::CubeControllerComponent(GameObject* gob)
	: UserInputComponent(gob)
{
}

/******************************************************************************************************************/

CubeControllerComponent::~CubeControllerComponent()
{
}

/******************************************************************************************************************/
// Functions
/******************************************************************************************************************/

// Setup function -- called when owner object is initialised (using its own Start method)
void CubeControllerComponent::Start()
{
	// Register as a listener
	_owner->RegisterListener("keypress", this);
	_owner->RegisterListener("mouseinput", this);
}

/******************************************************************************************************************/

// Main update function (called every frame)
void CubeControllerComponent::Update(double deltaTime)
{
	
}

/******************************************************************************************************************/

// Message handler (called when message occurs)
void CubeControllerComponent::OnMessage(Message* msg)
{

}

/******************************************************************************************************************/

// Shutdown function -- called when owner object is destroyed
void CubeControllerComponent::End()
{
	_owner->UnregisterListener("keypress", this);
	_owner->UnregisterListener("mouseinput", this);
}

/******************************************************************************************************************/

void CubeControllerComponent::Reset()
{
}

/******************************************************************************************************************/
