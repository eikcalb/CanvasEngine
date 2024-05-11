#include "Line.h"

#include "MathsHelper.h"

/******************************************************************************************************************/

Line::Line(std::shared_ptr<Mesh> mesh)
	: GameObject("Line")
{
	SetColour(Colour::White());
	SetMesh(mesh);
	SetShouldDraw(true);
}

/******************************************************************************************************************/

Line::~Line()
{
}

/******************************************************************************************************************/

void Line::Update(double deltaTime)
{
	GameObject::Update(deltaTime);

	if (canRotate) {
		SetAngle(GetAngle() + 90 * deltaTime);
	}
}

/******************************************************************************************************************/

void Line::OnMessage(Message* msg)
{
}

/******************************************************************************************************************/

void Line::Reset()
{
	SetShouldDraw(true);
	for (auto& behavior : _behaviors) {
		behavior.second->Reset();
	}

	_position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
}

void Line::SetWeight(u_short weight) {
	weight = weight;
}

void Line::SetColor(Colour colour) {

	colour = colour;
	SetShouldDraw(true);
}

void Line::SetCanRotate(bool rotate) {
	canRotate = rotate;
}

/******************************************************************************************************************/
