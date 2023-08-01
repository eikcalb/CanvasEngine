#include "PhysicsComponent.h"
#include "GameObject.h"
#include "Message.h"
#include "ThrustMessage.h"

/******************************************************************************************************************/
// Structors
/******************************************************************************************************************/

PhysicsComponent::PhysicsComponent(GameObject* gob)
	: GameObjectComponent("physics", gob),
	_velocity(0.0f, 0.0f, 0.0f, 0.0f)
{

}

/******************************************************************************************************************/

PhysicsComponent::~PhysicsComponent()
{

}

/******************************************************************************************************************/
// Functions
/******************************************************************************************************************/

// Setup function -- called when owner object is initialised (using its own Start method)
void PhysicsComponent::Start()
{
	_owner->RegisterListener("thrust", this);
}

/******************************************************************************************************************/

// Main update function (called every frame)
void PhysicsComponent::Update(double deltaTime)
{


}

/******************************************************************************************************************/

// Message handler (called when message occurs)
void PhysicsComponent::OnMessage(Message* msg)
{
	if (msg->GetMessageType() == "thrust")
	{
		ThrustMessage* tm = (ThrustMessage*)msg;
		_velocity += tm->GetThrust();
	}
}

/******************************************************************************************************************/

// Shutdown function -- called when owner object is destroyed
void PhysicsComponent::End()
{
	_owner->UnregisterListener("thrust", this);
}

/******************************************************************************************************************/

void PhysicsComponent::LimitToMaximumSpeed(float max)
{
	_velocity.limitTo(max);
}

/******************************************************************************************************************/

Vector4 PhysicsComponent::GetObjectPosition()
{
	return _owner->GetPosition();
}

/******************************************************************************************************************/

void PhysicsComponent::SetObjectPosition(Vector4 v)
{
	_owner->SetPosition(v);
}

/******************************************************************************************************************/
