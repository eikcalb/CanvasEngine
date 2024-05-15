#include "Game.h"

#include "Window.h"
#include "GameObject.h"
#include "KeyPressMessage.h"
#include "MouseInputMessage.h"
#include "NetworkController.h"
#include "TCPConnection.h"

std::shared_ptr<Game> Game::TheGame = nullptr;

/******************************************************************************************************************/

Game::Game()
	: _quitFlag(false)
{
	_currentTime = clock();

	SetGameState(GameState::MainMenu);

	TheGame = std::shared_ptr<Game>(this);

	_renderSystem = std::make_shared<RenderSystem>();

	// Setup controllers.
	// These controllers provide specific functionality to the application.
	// Each controller exposes functions that solve a specific set of problems.
	_inputController = InputController::Instance();
	_networkController = NetworkController::Instance();
	_resourceController = ResourceController::Instance();
	_sceneController = SceneController::Instance();
	_threadController = ThreadController::Instance();
}

/******************************************************************************************************************/

Game::~Game()
{
	// TODO: Need to fix this memory leak preventing smooth cleanup on shutdown

	//for (MeshMapIterator i = _meshes.begin();
	//	i != _meshes.end();
	//	++i)
	//{
	//	i->second.reset();
	//}
}

/******************************************************************************************************************/

void Game::Initialise(std::shared_ptr<Window> w)
{
	_window = w;
	_renderer = w->GetRenderer();
	_renderSystem->SetRenderer(_renderer);

	// Initialize network.
	CommunicationConfig cc{};
	cc.promiscuous = true; // Allow listening to a specific port for connections.
	auto cs = new TCPConnection(_resourceController->GetConfig());
	cs->SetConnectionValidator([&](PeerDetails* peerAddressInfo) {
		auto& peerMap = _networkController->GetPeerMap();
		// We will check if the peer has already been connected to.
		if (peerMap.count(peerAddressInfo->GetIDString()) <= 0) {
			// Only if the peer identity does not already exist will we approve a connection.
			return true;
		}

		return false;
		});

	_networkController->SetCommunicationConfig(&cc);
	_networkController->SetConnectionStrategy(cs);
	_networkController->Start();
	cs->Observe(ConnectionStrategy::EVENT_TYPE_NEW_CONNECTION, _networkController);
	_inputController->Observe(InputController::EVENT_KEY_INPUT, _networkController);
}

int Game::GetWindowHeight() { return _window->_height; }

int Game::GetWindowWidth() { return _window->_width; }

std::shared_ptr<Mesh> Game::GetMesh(std::string name)
{
	// Found
	MeshMapIterator i = _meshes.find(name);
	if (i != _meshes.end())
	{
		return i->second;
	}
	// Not found
	return NULL;
}

/******************************************************************************************************************/

void Game::OnKeyboard(int key, bool down)
{
	// Create keypress message and send it to all objects
	auto data = std::make_shared< KeyPressMessageInfo>();
	data->key = key;
	data->down = down;
	KeyPressMessage msg(data, InputController::EVENT_KEY_INPUT);

	_inputController->SetKeyPressed(static_cast<KEYS>(key), down);
	_inputController->Notify(&msg);
}

void Game::OnMouse(WPARAM buttonState, LPARAM lParam)
{
	int xPos = GET_X_LPARAM(lParam);
	int yPos = GET_Y_LPARAM(lParam);

	// Get button states
	_inputController->SetMousePressed(MOUSE_BUTTON::LEFT, buttonState & static_cast<int>(MOUSE_BUTTON::LEFT));
	_inputController->SetMousePressed(MOUSE_BUTTON::RIGHT, buttonState & static_cast<int>(MOUSE_BUTTON::RIGHT));
	_inputController->SetMousePressed(MOUSE_BUTTON::MIDDLE, buttonState & static_cast<int>(MOUSE_BUTTON::MIDDLE));
	_inputController->SetMousePressed(MOUSE_BUTTON::FORWARD, buttonState & static_cast<int>(MOUSE_BUTTON::FORWARD));
	_inputController->SetMousePressed(MOUSE_BUTTON::BACKWARD, buttonState & static_cast<int>(MOUSE_BUTTON::BACKWARD));

	// Create keypress message and send it to all objects
	auto data = std::make_shared<MouseMessageInfo>();
	data->pos = glm::vec2(xPos, yPos);
	MouseInputMessage msg(std::shared_ptr<MouseMessageInfo>(data), InputController::EVENT_MOUSE_INPUT);

	_inputController->Notify(&msg);
}

/******************************************************************************************************************/

void Game::Run()
{
	// Get delta time:
	// https://en.cppreference.com/w/cpp/chrono/c/clock
	double temp_time = clock();
	_deltaTime = (temp_time - _currentTime) / CLOCKS_PER_SEC;
	//_currentTime = temp_time;
	// fps check will be `if (_deltaTime >= 1/fps)`
}
