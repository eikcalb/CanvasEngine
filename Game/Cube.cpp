#include "Cube.h"


/******************************************************************************************************************/

Cube::Cube(Mesh* mesh)
	: GameObject("cube")
{
	new CubeControllerComponent(this);

	CollisionComponent* cc = new CollisionComponent(this);
	cc->SetCollisionRadius(mesh->CalculateMaxSize() * 0.9f);
	cc->SetCollisionID(CollisionID::Cube_ID);
	cc->SetCollisionMatrixFlag(CollisionID::Canvas_ID);

	RenderComponent* rc = new RenderComponent(this);
	rc->SetColour(Colour(0.5f, 0.5f, 1.0f, 1.0f));
	rc->SetMesh(mesh);
	rc->ShouldDraw(true);
}

/******************************************************************************************************************/

Cube::~Cube()
{
}

/******************************************************************************************************************/

void Cube::Update(double deltaTime)
{
	GameObject::Update(deltaTime);
}

/******************************************************************************************************************/

void Cube::OnMessage(Message* msg)
{
	if (msg->GetMessageType() == "collision")
	{
		CollisionMessage* col = (CollisionMessage*)msg;
		if (col->GetCollidee() == this ||
			col->GetCollider() == this)
		{
			RenderComponent* rc = (RenderComponent*)GetComponent("render");

			// Change game state
			Message msg("state=dead");
			Game::TheGame->ListenToMessage(&msg);
		}
	}

	GameObject::OnMessage(msg);
}

/******************************************************************************************************************/

void Cube::Reset()
{
	RenderComponent* rc = (RenderComponent*)GetComponent("render");
	rc->ShouldDraw(true);

	CubeControllerComponent* sc = (CubeControllerComponent*)GetComponent("input");
	sc->Reset();

	_position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
}

void  Cube::SetWeight(u_short weight) {
	weight = weight;
}

void  Cube::SetColor(Colour colour) {
	RenderComponent* rc = (RenderComponent*)GetComponent("render");
	colour = colour;
	rc->ShouldDraw(true);
}

/******************************************************************************************************************/
