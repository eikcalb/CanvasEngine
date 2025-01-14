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
	auto game = reinterpret_cast<VoxGame*>(Game::TheGame);
	auto thisShared = std::shared_ptr<GamePlayScene>(this);

	// Setup Listener.
	game->GetInputController()->Observe(InputController::EVENT_MOUSE_INPUT, thisShared);
	game->GetNetworkController()->Observe(NetworkController::EVENT_TYPE_NEW_MESSAGE, thisShared);
	game->GetNetworkController()->GetConnectionStrategy()->Observe(ConnectionStrategy::EVENT_TYPE_NEW_CONNECTION, thisShared);

	game->GetNetworkController()->Start();
}

/******************************************************************************************************************/

GamePlayScene::~GamePlayScene()
{
	//if (Game::TheGame) {
	//	auto thisShared = std::shared_ptr<GamePlayScene>(this);
	//	VoxGame* game = reinterpret_cast<VoxGame*>(Game::TheGame);

	//	game->GetInputController()->UnObserve(InputController::EVENT_MOUSE_INPUT, thisShared);
	//	//game->GetNetworkController()->UnObserve(NetworkController::EVENT_TYPE_NEW_MESSAGE, thisShared);
	//	//game->GetNetworkController()->GetConnectionStrategy()->UnObserve(ConnectionStrategy::EVENT_TYPE_NEW_CONNECTION, thisShared);
	//}
}

/******************************************************************************************************************/
// Functions
/******************************************************************************************************************/

void GamePlayScene::Start() {

}
void GamePlayScene::End() {

	SetAlive(false);
}

void GamePlayScene::Initialise()
{
	auto game = reinterpret_cast<VoxGame*>(Game::TheGame);
	auto voxel = game->GetVoxelCanvas();
	auto thisShared = std::shared_ptr<GamePlayScene>(this);

	// Setup Camera controls.
	std::shared_ptr<CameraBehavior> camBehavior = std::make_shared<CameraBehavior>(thisShared);
	AddBehavior(camBehavior);

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
	//_cube->SetColor(userColour);
	_cube->SetGeneratorData(voxel->GetVoxelData());
	_cube->SetShouldUpdateGenerator(true);
	AddGameObject(_cube);

	game->SetGameState(GameState::Playing);

	SetAlive(true);
}

std::string GamePlayScene::UpdatePeers() {
	// Calculates cube mass;
	auto game = reinterpret_cast<VoxGame*>(Game::TheGame);
	auto voxel = game->GetVoxelCanvas();
	auto current = voxel->GetVoxelData();
	std::string result;

	for (auto i = 0; i < VOXEL_AREA; i++) {
		result += std::to_string(current[i]._pad0);
	}

	return result;
}

void GamePlayScene::GetPaint(unsigned int x, unsigned int y) {

}

void GamePlayScene::HandleMessage(std::string peerID, const NetworkMessageContent& msg) {
	auto game = reinterpret_cast<VoxGame*>(Game::TheGame);
	auto voxel = game->GetVoxelCanvas();

	if (msg.type == MessageType::INIT) {
		PeerData pd{};
		pd.colour = Colour(msg.content);
		pd.lastSequenceID = msg.sequence;

		peerDataMap[peerID] = pd;
	}
	else if (msg.type == MessageType::DRAW) {
		// New update from peer. Handlw the draw on canvas.
		std::vector<std::string> tokens = Utils::splitString(msg.content, ',');
		// We've been robbed??
		auto voxel = game->GetVoxelCanvas();
		auto& target = voxel->GetAt(std::atoi(tokens[0].c_str()), std::atoi(tokens[1].c_str()));

		auto p = peerDataMap[peerID];
		target.Colour.r = p.colour.r();
		target.Colour.g = p.colour.g();
		target.Colour.b = p.colour.b();
		target.IsTransparent = TRUE;
		voxel->UpdateAt(std::atoi(tokens[0].c_str()), std::atoi(tokens[1].c_str()), target);
	}
	else if (msg.type == MessageType::INTEGRITY) {
		// Send my mass to the requestor.
		auto current = voxel->GetVoxelData();

		const auto state = std::atoi(msg.content.c_str());
		if (msg.sequence == 1) {
			// This is a response.
			auto p = peerDataMap[peerID];
			p.integrityPending = false;
			VoxelCanvas::ParseMassString(msg.content, p.mass);

			// Check if all peers have sent info.
			_pendingIntegrityCount--;
			if (_pendingIntegrityCount <= 0) {
				// Mix and match the coloring. Probably best to refactor these
				// into a separate function.
				for (auto i = 0; i < VOXEL_AREA; i++) {
					// Compute mass for each point.
					long mass = current[i]._pad0;
					for (const auto& p1 : peerDataMap) {
						mass += p1.second.mass[i];
					}

					current[i].IsIntegrityCheck = TRUE;
					if (mass > peerDataMap.size()) {
						current[i].IsIntegrityCheck = FALSE;
						current[i].Colour = glm::vec4(1, 0, 0, 0);

					}
					else if (mass < peerDataMap.size()) {
						current[i].IsIntegrityCheck = FALSE;
						current[i].Colour = glm::vec4(0, 0, 1, 0);
					}
				}
			}
		}
		else if (state == 1) {
			// Integrity request. Respond with mass.
			auto response = UpdatePeers();
			Game::TheGame->SetGameState(GameState::Validating);
			SendIntegrity(peerID, response);
		}
		else if (state == 0) {
			// Integrity request complete.
			Game::TheGame->SetGameState(GameState::Playing);
		}
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

void GamePlayScene::SendDraw(unsigned int x, unsigned int y) {
	auto game = reinterpret_cast<VoxGame*>(Game::TheGame);
	const auto& rootMap = game->GetNetworkController()->GetPeerMap();

	std::string text = "DRAW;0;"
		+ std::to_string(x) + ","
		+ std::to_string(y);

	// Send to all.
	for (auto& p : peerDataMap) {
		std::shared_ptr<Connection> peer = rootMap.at(p.first);
		peer->Send(Utils::stringToBytes(text));
	}
}

void GamePlayScene::SendIntegrity(unsigned int on) {
	// Integrity check.
	auto game = reinterpret_cast<VoxGame*>(Game::TheGame);
	const auto& rootMap = game->GetNetworkController()->GetPeerMap();

	std::string text = "INTEGRITY;0;" + std::to_string(on);
	// Send to all.
	for (auto& p : peerDataMap) {
		std::shared_ptr<Connection> peer = rootMap.at(p.first);
		peer->Send(Utils::stringToBytes(text));
		p.second.integrityPending = true;
	}
}

void GamePlayScene::SendIntegrity(std::string peerID, std::string response) {
	// Integrity check response.
	auto game = reinterpret_cast<VoxGame*>(Game::TheGame);
	const auto& rootMap = game->GetNetworkController()->GetPeerMap();

	std::string text = "INTEGRITY;1;" + response;
	rootMap.at(peerID)->Send(Utils::stringToBytes(text));
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
	case KEYS::M: {
		// Integrity check.
		auto game = reinterpret_cast<VoxGame*>(Game::TheGame);
		auto voxel = game->GetVoxelCanvas();
		auto current = voxel->GetVoxelData();

		if (Game::TheGame->GetGameState() == GameState::Validating) {
			for (auto i = 0; i < VOXEL_AREA; i++) {
				current[i].IsIntegrityCheck = FALSE;
			}
			Game::TheGame->SetGameState(GameState::Playing);
			SendIntegrity(0);
			_pendingIntegrityCount = 0;
		}
		else if (Game::TheGame->GetGameState() == GameState::Playing) {
			for (auto i = 0; i < VOXEL_AREA; i++) {
				current[i].IsIntegrityCheck = TRUE;
			}
			Game::TheGame->SetGameState(GameState::Validating);
			SendIntegrity(1);
			_pendingIntegrityCount = peerDataMap.size();
		}
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
#pragma region mouse
		if (Game::TheGame->GetGameState() == GameState::Validating || Game::TheGame->GetGameState() == GameState::Paused) {
			return;
		}

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
			auto gridCenter = glm::vec3(-pos.x() + ((VOXEL_WIDTH / 2) - 1) * _cube->size, -pos.y() + ((VOXEL_HEIGHT / 2) - 1) * _cube->size, 0);
			auto gridSize = VOXEL_WIDTH * _cube->size;

			// Calculate the position of the mouse relative to the center of the square
			int relativeX = mousePos.x - gridCenter.x;
			int relativeY = mousePos.y - gridCenter.y;

			// Convert the relative mouse position to grid coordinates
			int gridX = ((relativeX + gridSize / 2) / 2);
			int gridY = ((relativeY + gridSize / 2) / 2);

			// Calculate the subsquare indices
			int subSquareX = gridX % gridSize;
			int subSquareY = gridY % gridSize;
			auto game = reinterpret_cast<VoxGame*>(Game::TheGame);
			auto voxel = game->GetVoxelCanvas();
			auto& target = voxel->GetAt(subSquareX, subSquareY);

			target.IsTransparent = TRUE;
			voxel->UpdateAt(subSquareX, subSquareY, target);
			SendDraw(subSquareX, subSquareY);
			return;

			//float planeDistance = -glm::dot(normal, gridCenter);
			//// Check intersection with the larger square
			//float t = -(glm::dot(rayOrigin, normal) + planeDistance) / glm::dot(rayDirection, normal);
			//glm::vec3 intersectionPoint = rayOrigin + rayDirection * t;

			//float halfSize = gridSize / 2.0f;
			//glm::vec3 squareMin = gridCenter - glm::vec3(halfSize, halfSize, 0.0f);
			//glm::vec3 squareMax = gridCenter + glm::vec3(halfSize, halfSize, 0.0f);

			//// Step 3: Check if the intersection point lies within the bounds of the square
			//auto intersects = intersectionPoint.x >= squareMin.x && intersectionPoint.x <= squareMax.x &&
			//	intersectionPoint.y >= squareMin.y && intersectionPoint.y <= squareMax.y;

			// r->IntersectMouseRay(orig, dir, pos, _cube->size)
			// If there is a hit, we will update the peers and reset update counter.
			//if (intersects) {
			//	_cube->SetColor(Colour::Green());
			//}
			//else {
			//	_cube->SetColor(Colour::Blue());
			//}
			//return;
		//auto rayOrigin = r->GetCameraPosition()[0];
		//float intersectionX = rayOrigin.x + t * ray.x;
		//float intersectionY = rayOrigin.y + t * ray.y;

		//auto isHit = (intersectionX >= minX && intersectionX <= maxX &&
		//	intersectionY >= minY && intersectionY <= maxY);
		}
#pragma endregion mouse
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
		if (_pendingIntegrityCount > 0) {
			_pendingIntegrityCount--;
		}
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
	if (gameState == GameState::Paused || gameState == GameState::Validating) {
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
	const std::string mouseRayLabel = "Mouse Last Ray: X:" + std::to_string(_lastMouseRay.x) + ", Y:" + std::to_string(_lastMouseRay.y);
	r->Label(mouseRayLabel.c_str());

	r->Space();
	r->Space();
	const std::string playerCountLabel = "Player Count: " + std::to_string(Game::TheGame->GetNetworkController()->PeerCount() + 1);
	r->Label(playerCountLabel.c_str());
	//The total mass of voxels on the current PC
	const std::string voxMassLabel = "Total Mass: " + std::to_string(VOXEL_AREA);
	r->Label(voxMassLabel.c_str());
	//The actual mass of voxels across all PCs
	const std::string voxMassAllLabel = "Total Mass All: " + std::to_string(VOXEL_AREA * (Game::TheGame->GetNetworkController()->PeerCount() + 1));
	r->Label(voxMassAllLabel.c_str());
	//The starting mass of voxels across all PCs
	const std::string voxMassAllStartLabel = "Total Mass All Start: " + std::to_string(VOXEL_AREA * (Game::TheGame->GetNetworkController()->PeerCount() + 1));
	r->Label(voxMassAllStartLabel.c_str());
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
	auto game = reinterpret_cast<VoxGame*>(Game::TheGame);
	auto voxel = game->GetVoxelCanvas();
	auto current = voxel->GetVoxelData();
	auto& userColour = game->GetResourceController()->GetConfig()->colour;
	auto glUC = glm::vec4(userColour.r(), userColour.g(), userColour.b(), userColour.a());

	for (auto i = 0; i < VOXEL_AREA; i++) {
		current[i].Colour = glUC;
		current[i].IsIntegrityCheck = FALSE;
	}

	if (Game::TheGame->GetGameState() == GameState::Validating) {
		Game::TheGame->SetGameState(GameState::Playing);
	}

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
