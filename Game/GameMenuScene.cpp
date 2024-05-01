#include "GameMenuScene.h"

#define WIN32_LEAN_AND_MEAN // Required to prevent winsock/WinSock2 redifinition
#include <D3Dcommon.h>

#include "Game.h"
#include "GamePlayScene.h"
#include "Line.h"
#include "Message.h"
#include "RenderSystem.h"

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
	auto lineMesh = Game::TheGame->GetMesh("line");
	std::shared_ptr<Line> line = std::make_shared<Line>(lineMesh);
	line->SetCanRotate(true);
	AddGameObject(line);

	// Start all objects to set them up
	for (int i = 0; i < (int)_gameObjects.size(); i++)
	{
		_gameObjects[i]->Start();
	}

	Game::TheGame->SetGameState(GameState::MainMenu);
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
	const auto& r = renderer->GetRenderer()->GetHud();
	renderer->GetRenderer()->SetTopology(D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	r->Label("Application Started!");
	r->Space();
	r->Space();
	r->Label("Press the spacebar to continue", Colour::Green());
	r->Label("Or press \"esc\" to quit", Colour::Red());

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
