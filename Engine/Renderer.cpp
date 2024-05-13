#include "Renderer.h"

#include "GameObject.h"
#include "Mesh.h"

/******************************************************************************************************************/

Renderer::Renderer(): 
	_clearColour(Colour::Green()),
	_cameraPos(glm::vec3(0.0f, 0.0f,-5.0f))
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
