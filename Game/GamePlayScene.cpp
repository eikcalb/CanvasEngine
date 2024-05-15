#include "GamePlayScene.h"

#define WIN32_LEAN_AND_MEAN // Required to prevent winsock/WinSock2 redifinition
#include <D3Dcommon.h>

#include "Cube.h"
#include "CameraBehavior.h"
#include "Game.h"
#include "MouseInputMessage.h"
#include "RenderSystem.h"
#include "Message.h"
#include "VoxGame.h"

/******************************************************************************************************************/
// Structors
/******************************************************************************************************************/

GamePlayScene::GamePlayScene(): _lastMousePos()
{

}

/******************************************************************************************************************/

GamePlayScene::~GamePlayScene()
{
}

/******************************************************************************************************************/
// Functions
/******************************************************************************************************************/

void GamePlayScene::Start() {

}

void GamePlayScene::Initialise()
{
	VoxGame* game = (VoxGame*)Game::TheGame.get();
	auto voxel = game->GetVoxelCanvas();
	auto thisShared = std::shared_ptr<GamePlayScene>(this);
	
	// Setup Camera controls.
	std::shared_ptr<CameraBehavior> camBehavior = std::make_shared<CameraBehavior>(thisShared);
	AddBehavior(camBehavior);

	// Setup Listener.
	game->GetInputController()->Observe(InputController::EVENT_MOUSE_INPUT, thisShared);

	// Create the cube that will be rendered.
	auto mesh = Game::TheGame->GetMesh("cube");
	_cube = std::make_shared<Cube>(mesh, VOXEL_AREA);
	_cube->SetCanRotate(false);

	// Set the cube to be a generator.
	GeneratorBufferData base;
	auto& userColour = game->GetResourceController()->GetConfig()->colour;

	base.Colour = glm::vec4(userColour.r(), userColour.g(), userColour.b(), userColour.a());
	base.IsTransparent = false;
	base.IsInstanced = TRUE;
	voxel->Fill(base);
	//cube->SetColor(userColour);
	_cube->SetGeneratorData(voxel->GetVoxelData());
	_cube->SetShouldUpdateGenerator(true);
	AddGameObject(_cube);

	game->SetGameState(GameState::Playing);
}

/******************************************************************************************************************/

void GamePlayScene::OnKeyboard(int key, bool down)
{

	if (down) return; // Ignore key down events

	// Switch key presses
	switch (static_cast<KEYS>(key))
	{
	case KEYS::P:
		// Pausing the game state will prevent game objects from receiving updates.
		Game::TheGame->SetGameState(GameState::Paused);
		break;
	case KEYS::R:
		// Pausing the game state will prevent game objects from receiving updates.
		Reset();
		break;
	case KEYS::Space:
		Game::TheGame->SetGameState(GameState::Playing);
		break;
	case KEYS::Escape: // Escape
		SceneController::Instance()->PopScene();
		break;
	case KEYS::Up: // Up arrow-key
	case KEYS::Down: // Down arrow-key
		// Handled by camera behavior now.
		break;
	}
}

/******************************************************************************************************************/

void GamePlayScene::OnMessage(Message* msg)
{
	if (auto mouse = reinterpret_cast<MouseInputMessage*>(msg)) {
		auto& game = Game::TheGame;
		auto mousePos = mouse->GetPosition();
		SetMousePos(mousePos);

		// Check mouse hit.
		auto isLeftClicked = game->GetInputController()->GetMousePressed(MOUSE_BUTTON::LEFT);
		if (isLeftClicked) {
			auto r = game->GetRendererSystem()->GetRenderer();
			auto ray = r->CalculateMouseRay(mousePos.x, mousePos.y, game->GetWindowWidth(), game->GetWindowHeight());
			SetMouseRay(ray);
			// Get the cube position.
			auto pos = _cube->GetPosition();

			return;
			float minX = pos.x() - _cube->size / 2.0f;
			float maxX = pos.x() + _cube->size / 2.0f;
			float minY = pos.y() - _cube->size / 2.0f;
			float maxY = pos.y() + _cube->size / 2.0f;
			float minZ = pos.z() - _cube->size / 2.0f;
			float maxZ = pos.z() + _cube->size / 2.0f;

			// Check if the ray intersects with the plane of the front face of the cube
			float t = (maxZ - pos.z()) / ray.z;
			if (t < 0) {
				return; // Ray is pointing away from the front face
			}

			auto rayOrigin = r->GetCameraPosition()[0];
			float intersectionX = rayOrigin.x + t * ray.x;
			float intersectionY = rayOrigin.y + t * ray.y;

			auto isHit = (intersectionX >= minX && intersectionX <= maxX &&
				intersectionY >= minY && intersectionY <= maxY);
		}
	}
}

/******************************************************************************************************************/

/// Update current scene
void GamePlayScene::Update(double deltaTime)
{
	auto& game = Game::TheGame;
	const GameState gameState = game->GetGameState();
	if (gameState == GameState::Paused) {
		return;
	}

	Scene::Update(deltaTime);
}

/******************************************************************************************************************/

/// Render current scene
void GamePlayScene::Render(RenderSystem* renderer)
{ 
	const GameState gameState = Game::TheGame->GetGameState();

	const auto& r = renderer->GetRenderer()->GetHud();
	renderer->GetRenderer()->SetTopology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	renderer->Process(_gameObjects);

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

	const std::string mouseLabel = "Mouse Pos: X:" + std::to_string(_lastMousePos.x) + ", Y:" + std::to_string(_lastMousePos.y);
	r->Label(mouseLabel.c_str());
	const std::string mouseRayLabel = "Mouse Pos: X:" + std::to_string(_lastMouseRay.x) + ", Y:" + std::to_string(_lastMouseRay.y);
	r->Label(mouseRayLabel.c_str());
}

/******************************************************************************************************************/

void GamePlayScene::Reset()
{
	__super::Reset();

	for (int i = 0; i < (int)_gameObjects.size(); i++)
	{
		_gameObjects[i]->Reset();
	}

	auto _voxel = ((VoxGame*)Game::TheGame.get())->GetVoxelCanvas();
	if (_voxel) {
		_voxel->Reset();
	}
}

/******************************************************************************************************************/
