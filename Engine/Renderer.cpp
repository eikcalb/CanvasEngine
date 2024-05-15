#include "Renderer.h"

#include "GameObject.h"
#include "Mesh.h"

/******************************************************************************************************************/

Renderer::Renderer(): 
	_clearColour(Colour::Black()),
	 _cameraPos(glm::mat2x3(glm::vec3(0.0f, 0.0f,-5.0f), glm::vec3(0.0f, 0.0f, 0.0f)))
	//_cameraPos(glm::mat2x3(glm::vec3(256.0f, 256.0f,-700.0f ), glm::vec3(0.0f, 0.0f, 0.0f)))
{
	_initialCameraPos = _cameraPos;
}

/******************************************************************************************************************/

Renderer::~Renderer()
{
}

/******************************************************************************************************************/

void Renderer::Draw(std::shared_ptr<GameObject> rc)
{
	if (rc->ShouldDraw())
	{
		if (rc->GetMesh())
		{
			Draw(rc, rc->GetColour());
		}
	}
}

/******************************************************************************************************************/
