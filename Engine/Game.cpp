#include "Game.h"
#include "Window.h"
#include "GameObject.h"
#include "KeyPressMessage.h"
#include "MouseInputMessage.h"

Game* Game::TheGame = NULL;

/******************************************************************************************************************/

Game::Game()
	: _quitFlag(false),
	_sceneManager(this)
{
	_currentTime = clock();

	TheGame = this;
}

/******************************************************************************************************************/

Game::~Game()
{
	for (MeshMapIterator i = _meshes.begin();
		i != _meshes.end();
		++i)
	{
		delete i->second;
	}
	_meshes.clear();
}

/******************************************************************************************************************/

void Game::Initialise(Window* w)
{
	_window = w;
	_renderer = w->GetRenderer();
	_renderSystem.SetRenderer(_renderer);

	_inputController = InputController::Instance();
	_threadController = ThreadController::Instance();
}

/******************************************************************************************************************/

void Game::OnKeyboard(int key, bool down)
{
	// Create keypress message and send it to all objects
	KeyPressMessage msg(key, down);

	BroadcastMessage(&msg);
	_inputController->BroadcastMessage(&msg);
}

void Game::OnMouse(LPARAM lParam, bool down)
{
	int xPos = GET_X_LPARAM(lParam);
	int yPos = GET_Y_LPARAM(lParam);
	// Create keypress message and send it to all objects
	MouseInputMessage msg(glm::vec2(xPos, yPos), down);

	_inputController->BroadcastMessage(&msg);
}

/******************************************************************************************************************/

void Game::Run()
{
	// Get delta time
	double temp_time = clock();
	_deltaTime = (temp_time - _currentTime) / CLOCKS_PER_SEC;
	_currentTime = temp_time;
}

/******************************************************************************************************************/

void Game::BroadcastMessage(Message* msg)
{
//	ListenToMessage(msg);
}

/******************************************************************************************************************/
