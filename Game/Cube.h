#pragma once
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
	bool canRotate = false;

public:
	const int size = 2;

	// Structors
public:
	Cube(std::shared_ptr<Mesh> mesh, unsigned long count);
	virtual ~Cube();

	// Functions
public:
	virtual void Update(double deltaTime) override;
	virtual void OnMessage(Message* msg) override;
	virtual void Reset() override;

	unsigned short GetWeight(u_short weight) { return weight; };
	void SetWeight(u_short weight);
	void SetColor(Colour colour);
	void SetCanRotate(bool rotate);
};

