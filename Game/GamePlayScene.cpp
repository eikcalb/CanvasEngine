#include "GamePlayScene.h"

#define WIN32_LEAN_AND_MEAN // Required to prevent winsock/WinSock2 redifinition

#include "Colour.h"
#include "Cube.h"
#include "CameraBehavior.h"
#include "Game.h"
#include "MouseInputMessage.h"
#include "InputController.h"
#include "RenderSystem.h"
#include "Message.h"
#include "VoxGame.h"
#include <Utils.h>

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
void GamePlayScene::End() {
	//if (Game::TheGame) {
	//	//auto thisShared = std::shared_ptr<GamePlayScene>(this);
	//	//VoxGame* game = (VoxGame*)Game::TheGame.get();
	//	//game->GetInputController()->UnObserve(InputController::EVENT_MOUSE_INPUT, thisShared);
	//}
}

void GamePlayScene::Initialise()
{
	auto game = reinterpret_cast<VoxGame*>(Game::TheGame);
	auto voxel = game->GetVoxelCanvas();
	auto thisShared = std::shared_ptr<GamePlayScene>(this);

	// Setup Camera controls.
	std::shared_ptr<CameraBehavior> camBehavior = std::make_shared<CameraBehavior>(thisShared);
	AddBehavior(camBehavior);

	// Setup Listener.
	game->GetInputController()->Observe(InputController::EVENT_MOUSE_INPUT, thisShared);
	game->GetNetworkController()->Observe(NetworkController::EVENT_TYPE_NEW_MESSAGE, thisShared);
	game->GetNetworkController()->GetConnectionStrategy()->Observe(ConnectionStrategy::EVENT_TYPE_NEW_CONNECTION, thisShared);

	// Create the cube that will be rendered.
	auto mesh = Game::TheGame->GetMesh("cube");
	_cube = std::make_shared<Cube>(mesh, VOXEL_AREA);
	_cube->SetCanRotate(false);

	// Set the cube to be a generator.
	GeneratorBufferData base;
	auto& userColour = game->GetResourceController()->GetConfig()->colour;

	base.Colour = glm::vec4(userColour.r(), userColour.g(), userColour.b(), userColour.a());
	base.IsTransparent = FALSE;
	base.IsIntegrityCheck = FALSE;
	voxel->Fill(base);
	_cube->SetColor(userColour);
	_cube->SetGeneratorData(voxel->GetVoxelData());
	_cube->SetShouldUpdateGenerator(true);
	AddGameObject(_cube);

	game->SetGameState(GameState::Playing);
}

void GamePlayScene::UpdatePeers() {
	// Sends current grid status to peers.
}

void GamePlayScene::HandleMessage(std::string peerID, const NetworkMessageContent& msg) {
	if (msg.type == MessageType::INIT) {
		peerDataMap[peerID] = {
			Colour(msg.content),
			0, // initial mass is zero as the mass will be calculated when we do an integrity test.
			msg.sequence
		};
	}
	else if (msg.type == MessageType::DRAW) {
		// New update from peer. Handlw the draw on canvas.
	}
	else if (msg.type == MessageType::INTEGRITY) {
		// Send my mass to the requestor.
	}
}

void GamePlayScene::SendInit(std::shared_ptr<Connection> conn) {
	const auto& colour = Game::TheGame->GetResourceController()->GetConfig()->colour;
	std::string text = "INIT;1;"
		+ std::to_string(colour.r()) + ","
		+ std::to_string(colour.g()) + ","
		+ std::to_string(colour.b())
		+ std::to_string(colour.a());
	conn->Send(Utils::stringToBytes(text));
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
	case KEYS::M:
		// Integrity check.
		if (Game::TheGame->GetGameState() == GameState::Validating) {
			Game::TheGame->SetGameState(GameState::Playing);
		}
		else if (Game::TheGame->GetGameState() == GameState::Playing) {
			Game::TheGame->SetGameState(GameState::Validating);
		}
		break;
	case KEYS::U:
	{
		const auto fps = Game::TheGame->GetFPS();
		Game::TheGame->SetFPS(fps + 1);
		break;
	}
	case KEYS::J:
	{
		const auto fps = Game::TheGame->GetFPS();
		Game::TheGame->SetFPS(std::max(1, int(fps) - 1));
		break;
	}
	case KEYS::Y:
	{
		const auto fps = Game::TheGame->GetNetworkController()->GetFPS();
		Game::TheGame->GetNetworkController()->SetFPS(fps + 1);
		break;
	}
	case KEYS::H:
	{
		const auto fps = Game::TheGame->GetNetworkController()->GetFPS();
		Game::TheGame->GetNetworkController()->SetFPS(std::max(1, int(fps) - 1));
		break;
	}
	case KEYS::Up: // Up arrow-key
	case KEYS::Down: // Down arrow-key
		// Handled by camera behavior now.
		break;
	}
}

/******************************************************************************************************************/

void GamePlayScene::OnMessage(Message* msg)
{
	const auto& type = msg->GetMessageType();
	if (type == InputController::EVENT_MOUSE_INPUT)
	{
#pragma region keyboard
		auto mouse = reinterpret_cast<MouseInputMessage*>(msg);
		const auto game = Game::TheGame;
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
			// If there is a hit, we will update the peers and reset update counter.
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
#pragma endregion keyboard
	}
	else if (type == ConnectionStrategy::EVENT_TYPE_NEW_CONNECTION) {
#pragma region new connection
		auto networkMessage = reinterpret_cast<ConnectionMessage*>(msg);
		const auto conn = networkMessage->GetConnection();
		conn->Observe(Connection::EVENT_TYPE_CLOSED_CONNECTION, std::shared_ptr<GamePlayScene>(this));
		SendInit(conn);
#pragma endregion new connection
	}
	else if (type == Connection::EVENT_TYPE_CLOSED_CONNECTION) {
		const auto& nMsg = reinterpret_cast<NetworkMessage*>(msg);
		peerDataMap.erase(nMsg->GetMessage()->peerID);
	}
	else if (type == NetworkController::EVENT_TYPE_NEW_MESSAGE) {
#pragma region new message
		auto networkMessage = reinterpret_cast<NetworkMessage*>(msg);
		const auto m = networkMessage->ParseNetworkMessage();
		HandleMessage(networkMessage->GetMessage()->peerID, m);
#pragma endregion new message
	}
}

/******************************************************************************************************************/

/// Update current scene
void GamePlayScene::Update(double deltaTime)
{
	const auto game = Game::TheGame;
	const GameState gameState = game->GetGameState();
	if (gameState == GameState::Paused) {
		return;
	}

	Scene::Update(deltaTime);

	// Update network with my data. Here we will have a counter that limits how frequently
	// we update the network about the voxel.
	if (_messageSendFreq > 1 / 2000) {
		// Time has passed. We can send status messages
		UpdatePeers();
		_messageSendFreq = 0;
	}
	_messageSendFreq += deltaTime;
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
	r->LabelText("Integrity Check", gameState == GameState::Validating ? "TRUE" : "FALSE");

	const std::string mouseLabel = "Mouse Pos: X:" + std::to_string(_lastMousePos.x) + ", Y:" + std::to_string(_lastMousePos.y);
	r->Label(mouseLabel.c_str());
	//const std::string mouseRayLabel = "Mouse Last Ray: X:" + std::to_string(_lastMouseRay.x) + ", Y:" + std::to_string(_lastMouseRay.y);
	//r->Label(mouseRayLabel.c_str());

	r->Space();
	r->Space();
	const std::string mouseRayLabel = "Player Count: " + std::to_string(Game::TheGame->GetNetworkController()->PeerCount() + 1);
	r->Label(mouseRayLabel.c_str());
	//The total mass of voxels on the current PC
	//The actual mass of voxels across all PCs
	//The starting mass of voxels across all PCs
	const std::string fpsTargetLabel = "Graphics Target FPS: " + std::to_string(Game::TheGame->GetFPS());
	r->Label(fpsTargetLabel.c_str());
	const std::string fpsActualLabel = "Graphics Actual FPS: " + std::to_string(Game::TheGame->GetActualFPS());
	r->Label(fpsActualLabel.c_str());
	const std::string fpsNetworkTargetLabel = "Network Target FPS: " + std::to_string(Game::TheGame->GetNetworkController()->GetFPS());
	r->Label(fpsNetworkTargetLabel.c_str());
	const std::string fpsNetworkActualLabel = "Network Actual FPS: " + std::to_string(Game::TheGame->GetNetworkController()->GetActualFPS());
	r->Label(fpsNetworkActualLabel.c_str());
}

/******************************************************************************************************************/

void GamePlayScene::Reset()
{
	__super::Reset();

	for (int i = 0; i < (int)_gameObjects.size(); i++)
	{
		_gameObjects[i]->Reset();
	}

	auto _voxel = (reinterpret_cast<VoxGame*>(Game::TheGame))->GetVoxelCanvas();
	if (_voxel) {
		_voxel->Reset();
	}
}

/******************************************************************************************************************/
