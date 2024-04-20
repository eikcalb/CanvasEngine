#include "GameMenuScene.h"

#include "Game.h"
#include "RenderSystem.h"
#include "Message.h"
#include "Cube.h"
#include "GamePlayScene.h"

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
	auto cubeMesh = Game::TheGame->GetMesh("cube");
	std::shared_ptr<Cube> cube = std::make_shared<Cube>(cubeMesh);
	cube->SetCanRotate(true);
	AddGameObject(cube);
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
	switch (static_cast<KEYS>(key))
	{
	case KEYS::Space:
		SceneController::Instance()->PushScene(std::make_shared<GamePlayScene>());
		break;

	case KEYS::Escape: // Escape
		SceneController::Instance()->PopScene();
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

	const auto& r = renderer->GetRenderer()->GetHud();
	r->Label("Application Started!");
	r->Space();
	r->Space();
	r->Label("Press the spacebar to continue");

	renderer->Process(_gameObjects);
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
