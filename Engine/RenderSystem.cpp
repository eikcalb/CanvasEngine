#include "RenderSystem.h"
#include "GameObject.h"

/******************************************************************************************************************/
// Structors
/******************************************************************************************************************/

RenderSystem::RenderSystem()
	: _renderer()
{
}

/******************************************************************************************************************/

RenderSystem::~RenderSystem()
{
	_renderer.reset();
}

/******************************************************************************************************************/
// Functions
/******************************************************************************************************************/

void RenderSystem::Process(std::vector<std::shared_ptr<GameObject>>& list)
{
	if (!_renderer) return;

	for (auto obj : list)
	{
		if (obj->IsAlive() && obj->ShouldDraw())
		{
				_renderer->Draw(obj);
		}
	}

}

/******************************************************************************************************************/
