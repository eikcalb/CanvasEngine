#include "GameMenuScene.h"

#include "Game.h"
#include "RenderSystem.h"
#include "Message.h"

#include "Cube.h"

/******************************************************************************************************************/
// Structors
/******************************************************************************************************************/

GameMenuScene::GameMenuScene()
{

}

/******************************************************************************************************************/

GameMenuScene::~GameMenuScene()
{
}

/******************************************************************************************************************/
// Functions
/******************************************************************************************************************/

void GameMenuScene::Initialise()
{

	std::shared_ptr<Cube> cube = nullptr;
	//for (int y = 0; y < VOXEL_HEIGHT; ++y) {
	//	for (int x = 0; x < VOXEL_WIDTH; ++x) {
	//		cube = std::make_shared<Cube>(_sceneManager->GetGame()->GetMesh("cube"));
	//		cube->Reset();
	//		_voxel->SetVoxel(x, y, cube.get());
	//		_gameObjects.push_back(cube);
	//	}
	//}

	// Start all objects to set them up
	for (int i = 0; i < (int)_gameObjects.size(); i++)
	{
		_gameObjects[i]->Start();
	}
}

/******************************************************************************************************************/

void GameMenuScene::OnKeyboard(int key, bool down)
{

	if (down) return; // Ignore key down events

	// Switch key presses
	switch (key)
	{
	case 80: // P = pause
		// Put code here to add a Pause screen
		break;

	case 27: // Escape
		_sceneManager->PopScene();
		break;
	}
}

/******************************************************************************************************************/

/// Update current scene
void GameMenuScene::Update(double deltaTime)
{
	Scene::Update(deltaTime);

	for (int i = 0; i < (int)_gameObjects.size(); i++)
	{
		if (_gameObjects[i]->IsAlive())
		{
			_gameObjects[i]->Update(deltaTime);
		}
	}
}

/******************************************************************************************************************/

/// Render current scene
void GameMenuScene::Render(RenderSystem* renderer)
{
	glm::mat4 MVM;

	renderer->Process(_gameObjects, 0);
}

/******************************************************************************************************************/

void GameMenuScene::Reset()
{
	for (int i = 0; i < (int)_gameObjects.size(); i++)
	{
		_gameObjects[i]->Reset();
	}
}

/******************************************************************************************************************/