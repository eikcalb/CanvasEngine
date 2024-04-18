#pragma once
#include <any>

#include "Colour.h"
#include "Game.h"
#include "GameObject.h"
#include "Message.h"


class Mesh;

class Cube :
	public GameObject
{
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
	virtual void OnMessage(Message<std::any>* msg);
	virtual void Reset();

	void SetWeight(u_short weight);
	void SetColor(Colour colour);
};

