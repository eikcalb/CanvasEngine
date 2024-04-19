#include "Cube.h"


/******************************************************************************************************************/

Cube::Cube(Mesh* mesh)
	: GameObject("Cube")
{
	//new CubeControllerComponent(this);

	//RenderComponent* rc = new RenderComponent(this);
	//rc->SetColour(Colour(0.5f, 0.5f, 1.0f, 1.0f));
	//rc->SetMesh(mesh);
	//rc->ShouldDraw(true);
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
}

/******************************************************************************************************************/

void Cube::Reset()
{
	ShouldDraw(true);
	for (auto& behavior : _behaviors) {
		behavior.second->Reset();
	}

	_position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
}

void  Cube::SetWeight(u_short weight) {
	weight = weight;
}

void  Cube::SetColor(Colour colour) {
	
	colour = colour;
	ShouldDraw(true);
}

/******************************************************************************************************************/
