#include "RenderSystem.h"
#include "GameObject.h"
#include "RenderComponent.h"

/******************************************************************************************************************/
// Structors
/******************************************************************************************************************/

RenderSystem::RenderSystem()
	: _renderer(NULL)
{
}

/******************************************************************************************************************/

RenderSystem::~RenderSystem()
{
}

/******************************************************************************************************************/
// Functions
/******************************************************************************************************************/

void RenderSystem::Process(std::vector<GameObject*>& list, double _)
{
	if (_renderer == NULL) return;
	for (GameObject* obj : list)
	{
		if (obj->IsAlive())
		{
			if (RenderComponent* rc = reinterpret_cast<RenderComponent*>(obj->GetComponent("render")))
			{
				_renderer->Draw(rc, _MVM);
			}
		}
	}

}

/******************************************************************************************************************/
