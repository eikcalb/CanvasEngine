#include "Cube.h"

#include "MathsHelper.h"

/******************************************************************************************************************/

Cube::Cube(std::shared_ptr<Mesh> mesh)
	: GameObject("Cube")
{
	SetColour(Colour::Yellow());
	SetMesh(mesh);
	ShouldDraw(true);
}

/******************************************************************************************************************/

Cube::~Cube()
{
}

/******************************************************************************************************************/

void Cube::Update(double deltaTime)
{
	GameObject::Update(deltaTime);

	if (canRotate) {
		SetAngle(GetAngle() + 90 * deltaTime);
	}
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

void Cube::SetCanRotate(bool rotate) {
	canRotate = rotate;
}

/******************************************************************************************************************/
