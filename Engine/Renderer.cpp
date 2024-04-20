#include "Renderer.h"

#include "GameObject.h"
#include "Mesh.h"

/******************************************************************************************************************/

Renderer::Renderer()
	: _clearColour(Colour::Black())
{
}

/******************************************************************************************************************/

Renderer::~Renderer()
{
}

/******************************************************************************************************************/

void Renderer::Draw(std::shared_ptr<GameObject> rc, glm::mat4 MVM)
{
	if (rc->ShouldDraw())
	{
		MVM = glm::translate(MVM, glm::vec3(rc->GetPosition().x(), rc->GetPosition().y(), 0));
		MVM = glm::rotate(MVM, -rc->GetAngle(), glm::vec3(0, 0, 1)); // Rotate on z-axis
		MVM = glm::scale(MVM, glm::vec3(rc->GetScale(), rc->GetScale(), 1));

		if (rc->GetMesh())
		{
			Draw(rc->GetMesh(), MVM, rc->GetColour());
		}
	}
}

/******************************************************************************************************************/
