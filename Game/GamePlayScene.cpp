#include "GamePlayScene.h"

#define WIN32_LEAN_AND_MEAN // Required to prevent winsock/WinSock2 redifinition
#include <D3Dcommon.h>

#include "Game.h"
#include "RenderSystem.h"
#include "Message.h"

/******************************************************************************************************************/
// Structors
/******************************************************************************************************************/

GamePlayScene::GamePlayScene() : _voxel(nullptr)
{

}

/******************************************************************************************************************/

GamePlayScene::~GamePlayScene()
{
}

/******************************************************************************************************************/
// Functions
/******************************************************************************************************************/

void GamePlayScene::Initialise()
{
	//_voxel = new VoxelCanvas;
	auto mesh = Game::TheGame->GetMesh("cube");
	std::shared_ptr<Cube> cube = std::make_shared<Cube>(mesh);
	cube->SetCanRotate(false);
	AddGameObject(cube);

	//std::shared_ptr<Cube> cube = nullptr;
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

	Game::TheGame->SetGameState(GameState::Playing);
}

/******************************************************************************************************************/

void GamePlayScene::OnKeyboard(int key, bool down)
{

	if (down) return; // Ignore key down events



	// Switch key presses
	switch (static_cast<KEYS>(key))
	{
	case KEYS::P: // P = pause
		// Pausing the game state will prevent game objects from receiving updates.
		Game::TheGame->SetGameState(GameState::Paused);
		break;
	case KEYS::Space: // P = pause
		Game::TheGame->SetGameState(GameState::Playing);
		break;
	case KEYS::Escape: // Escape
		SceneController::Instance()->PopScene();
		break;
	case KEYS::Up: // Up arrow-key
	case KEYS::Down: // Down arrow-key
		// Handled in update loop
		break;
	}
}

/******************************************************************************************************************/

/// Update current scene
void GamePlayScene::Update(double deltaTime)
{
	auto& game = Game::TheGame;
	const GameState gameState = game->GetGameState();

	const auto& renderer = game->GetRendererSystem()->GetRenderer();
	glm::vec3 camPos = renderer->GetCameraPosition();
	if (game->GetInputController()->IsKeyPressed(KEYS::OemComma)) {
		camPos += glm::vec3(0, 10 * deltaTime, 0);
	}
	else if (game->GetInputController()->IsKeyPressed(KEYS::OemPeriod)) {
		camPos += glm::vec3(0, 10 * -deltaTime, 0);
	}

	if (game->GetInputController()->IsKeyPressed(KEYS::Up)) {
		camPos += glm::vec3(0, 0, 10 * deltaTime);
	}
	else if (game->GetInputController()->IsKeyPressed(KEYS::Down)) {
		camPos += glm::vec3(0, 0, 10 * -deltaTime);
	}

	if (game->GetInputController()->IsKeyPressed(KEYS::Left)) {
		camPos += glm::vec3(10 * deltaTime, 0, 0);
	}
	else if (game->GetInputController()->IsKeyPressed(KEYS::Right)) {
		camPos += glm::vec3(10 * -deltaTime, 0, 0);
	}
	renderer->SetCameraPosition(camPos);

	if (gameState == GameState::Paused) {
		return;
	}

	Scene::Update(deltaTime);

	for (int i = 0; i < (int)_gameObjects.size(); i++)
	{
		if (_gameObjects[i]->IsAlive())
		{
			_gameObjects[i]->Update(deltaTime / 10);
		}
	}
}

/******************************************************************************************************************/

/// Render current scene
void GamePlayScene::Render(RenderSystem* renderer)
{
	const GameState gameState = Game::TheGame->GetGameState();

	const auto& r = renderer->GetRenderer()->GetHud();
	renderer->GetRenderer()->SetTopology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (gameState == GameState::Paused) {
		r->Label("Paused...Press the spacebar to continue", Colour::Green());
	}
	else if (gameState == GameState::Playing) {
		r->Label("Press \"p\" to pause", Colour::Blue());
	}
	r->Label("Press \"esc\" to go back", Colour::Red());

	r->Space();
	r->Space();
	r->LabelText("User ID", ResourceController::Instance()->GetConfig()->id.c_str());

	renderer->Process(_gameObjects);
}

/******************************************************************************************************************/

void GamePlayScene::Reset()
{
	for (int i = 0; i < (int)_gameObjects.size(); i++)
	{
		_gameObjects[i]->Reset();
	}
}

/******************************************************************************************************************/
