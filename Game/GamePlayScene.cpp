#include "GamePlayScene.h"

#define WIN32_LEAN_AND_MEAN // Required to prevent winsock/WinSock2 redifinition

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

GamePlayScene::GamePlayScene() : _lastMousePos()
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
			const auto& r = game->GetRendererSystem()->GetRenderer();
			const auto ray = r->CalculateMouseRay(mousePos.x, mousePos.y, game->GetWindowWidth(), game->GetWindowHeight());
			const auto rayOrigin = glm::vec3(ray[0]);
			const auto rayDirection = glm::vec3(ray[1]);
			const auto normal = glm::vec3(0.0f, 0.0f, 1.0f);

			SetMouseRay(rayDirection);
			// Get the cube position.
			auto pos = _cube->GetPosition();
			auto gridCenter = glm::vec3(pos.x() + (VOXEL_WIDTH - 1) * _cube->size, pos.y() + (VOXEL_HEIGHT - 1) * _cube->size, 0);
			auto gridSize = VOXEL_WIDTH * _cube->size;

			float planeDistance = -glm::dot(normal, gridCenter);
			// Check intersection with the larger square
			float t = -(glm::dot(rayOrigin, normal) + planeDistance) / glm::dot(rayDirection, normal);
			glm::vec3 intersectionPoint = rayOrigin + rayDirection * t;

			float halfSize = gridSize / 2.0f;
			glm::vec3 squareMin = gridCenter - glm::vec3(halfSize, halfSize, 0.0f);
			glm::vec3 squareMax = gridCenter + glm::vec3(halfSize, halfSize, 0.0f);

			// Step 3: Check if the intersection point lies within the bounds of the square
			auto intersects = intersectionPoint.x >= squareMin.x && intersectionPoint.x <= squareMax.x &&
				intersectionPoint.y >= squareMin.y && intersectionPoint.y <= squareMax.y;

			// r->IntersectMouseRay(orig, dir, pos, _cube->size)
			if (intersects) {
				_cube->SetColor(Colour::Green());
			}
			else {
				_cube->SetColor(Colour::Blue());
			}
			return;
			//auto rayOrigin = r->GetCameraPosition()[0];
			//float intersectionX = rayOrigin.x + t * ray.x;
			//float intersectionY = rayOrigin.y + t * ray.y;

			//auto isHit = (intersectionX >= minX && intersectionX <= maxX &&
			//	intersectionY >= minY && intersectionY <= maxY);
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
	const std::string mouseRayLabel = "Mouse Last Ray: X:" + std::to_string(_lastMouseRay.x) + ", Y:" + std::to_string(_lastMouseRay.y);
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
