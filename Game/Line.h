#pragma once
#include "Colour.h"
#include "Game.h"
#include "GameObject.h"
#include "Message.h"


class Mesh;

class Line :
	public GameObject
{
	// Data
protected:
	bool canRotate = false;
	Colour colour = Colour();

	// Structors
public:
	Line(std::shared_ptr<Mesh> mesh);
	virtual ~Line();

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


