#include "Game.h"
#include "Window.h"
#include "GameObject.h"
#include "KeyPressMessage.h"
#include "MouseInputMessage.h"
//#include "NetworkController.h"

Game* Game::TheGame = NULL;

/******************************************************************************************************************/

Game::Game()
	: _quitFlag(false),
	_sceneManager(this)
{
	_currentTime = clock();

	TheGame = this;

	// Setup controllers.
	// These controllers provide specific functionality to the application.
	// Each controller exposes functions that solve a specific set of problems.
	_inputController = InputController::Instance();
	//_networkController = NetworkController::Instance();
	//_resourceController = ResourceController::Instance();
	_threadController = ThreadController::Instance();
}

/******************************************************************************************************************/

Game::~Game()
{
	for (MeshMapIterator i = _meshes.begin();
		i != _meshes.end();
		++i)
	{
		i->second.reset();
	}
	_meshes.clear();
	delete _renderer;
	delete _window;
}

/******************************************************************************************************************/

void Game::Initialise(Window* w)
{
	_window = w;
	_renderer = w->GetRenderer();
	_renderSystem->SetRenderer(std::shared_ptr<Renderer>(_renderer));
}

inline std::shared_ptr<Mesh> Game::GetMesh(std::string name)
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

	//BroadcastMessage(&msg);
	_inputController->Notify(&msg);
}

void Game::OnMouse(LPARAM lParam, bool down)
{
	int xPos = GET_X_LPARAM(lParam);
	int yPos = GET_Y_LPARAM(lParam);
	// Create keypress message and send it to all objects
	auto data = std::make_shared< MouseMessageInfo>();
	data->pos = glm::vec2(xPos, yPos);
	data->clicked = down;
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
	_currentTime = temp_time;
	// fps check will be `if (_deltaTime >= 1/fps)`
}
