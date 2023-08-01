#pragma once
#include "CollisionMatrix.h"
#include "CollisionMessage.h"
#include "Components.h"
#include "DeadObjectMessage.h"
#include "Game.h"
#include "GameObject.h"
#include "Colour.h"
#include "CubeControllerComponent.h"

class Mesh;
class Message;

class Cube :
	public GameObject
{
	// Constants
public:
	static const float MAX_SPEED;
	static const float ACCELERATION;

	// Data
protected:
	unsigned short weight = 1;
	Colour colour = Colour();

	// Structors
public:
	Cube(Mesh* mesh);
	virtual ~Cube();

	// Functions
public:
	virtual void Update(double deltaTime);
	virtual void OnMessage(Message* msg);
	virtual void Reset();

	void SetWeight(u_short weight);
	void SetColor(Colour colour);
};

