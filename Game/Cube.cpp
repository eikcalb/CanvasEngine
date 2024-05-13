#include "Cube.h"

#include "MathsHelper.h"

/******************************************************************************************************************/

Cube::Cube(std::shared_ptr<Mesh> mesh, unsigned long count)
	: GameObject("Cube")
{
	_instanceCount = count;

	SetColour(Colour::White());
	SetMesh(mesh);
	SetShouldDraw(true);
}

/******************************************************************************************************************/

Cube::~Cube()
{
}

/******************************************************************************************************************/

void Cube::Update(double deltaTime)
{
	if (!ShouldUpdate()) {
		return;
	}

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
	SetShouldDraw(true);
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
	SetShouldDraw(true);
}

void Cube::SetCanRotate(bool rotate) {
	canRotate = rotate;
}

/******************************************************************************************************************/
