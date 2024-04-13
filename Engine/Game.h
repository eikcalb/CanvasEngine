#pragma once
#include <time.h>
#include <map>
#include <vector>
#include <memory>

#include "RenderSystem.h"

#include "SceneManager.h"
#include <windows.h>
#include <windowsx.h>
#include "ThreadController.h"
#include "InputController.h"
#include "ResourceController.h"

// Forward declarations
class GameObject;
class Mesh;
class Message;
class Renderer;
class Window;

// Typedefs
typedef std::map<std::string, Mesh*> MeshMap;
typedef std::map<std::string, Mesh*>::iterator MeshMapIterator;


class Game
{
public:
	static Game* TheGame;

protected:
	double								_currentTime;
	double								_deltaTime;
	bool								_quitFlag;
	Renderer* _renderer;
	Window* _window;
	std::shared_ptr<InputController>	_inputController;
	std::shared_ptr<ResourceController>	_resourceController;
	std::shared_ptr<ThreadController>	_threadController;

	// Meshes
	MeshMap							_meshes;			// The map of meshes

	// Systems
	RenderSystem					_renderSystem;		// To handle rendering

	// Scene Manager
	SceneController					_sceneManager;

	// Structors
public:
	Game();
	virtual ~Game();


	// Gets/sets
public:
	// Meshes
	Mesh* GetMesh(std::string name);
	void AddMesh(std::string name, Mesh* mesh) { _meshes[name] = mesh; }

	// Quit flag
	bool GetQuitFlag()						const { return _quitFlag; }
	void SetQuitFlag(bool v) { _quitFlag = v; }

	// Renderer
	Renderer* GetRenderer()					const { return _renderer; }
	InputController* GetInputController()					const { return _inputController.get(); }

	// Functions
public:
	void AddGameObject(GameObject* obj) {
		_sceneManager.AddGameObject(obj);
		_inputController->AddObserver(reinterpret_cast<Observer*>(obj));
	}
	std::vector<GameObject*>& GetGameObjects() { return _sceneManager.GetGameObjects(); }

	// Initialise game
	virtual void Initialise(Window* w);

	// Keyboard input
	virtual void OnKeyboard(int key, bool down);
	virtual void OnMouse(LPARAM wParam, bool down);

	// Draw everything
	virtual void Render() = 0;

	// Main game loop (update)
	virtual void Run();

	// Message system
	void BroadcastMessage(Message* msg);

	// The game can respond to messages too
	virtual void ListenToMessage(Message* msg) {}
};


inline Mesh* Game::GetMesh(std::string name)
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