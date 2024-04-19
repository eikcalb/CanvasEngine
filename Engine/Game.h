﻿#pragma once
//#pragma comment(lib, "Ws2_32.lib")
#define WIN32_LEAN_AND_MEAN 

#include <time.h>
#include <map>
#include <vector>
#include <memory>
#include <windows.h>
#include <windowsx.h>

#include "InputController.h"
#include "Mesh.h"
#include "ResourceController.h"
#include "RenderSystem.h"
#include "SceneController.h"
#include "ThreadController.h"

// Forward declarations
class GameObject;
class Mesh;
class Renderer;
class Window;
class NetworkController;

// Typedefs
typedef std::map<std::string, std::shared_ptr<Mesh>> MeshMap;
typedef std::map<std::string, std::shared_ptr<Mesh>>::iterator MeshMapIterator;


class Game : ObserverSubject
{
public:
	static std::shared_ptr<Game> TheGame;

protected:
	std::string							_name;

	double								_currentTime;
	double								_deltaTime;
	bool								_quitFlag;

	std::shared_ptr<InputController>	_inputController;
	std::shared_ptr<NetworkController>	_networkController; // ✅
	std::shared_ptr<ResourceController>	_resourceController;
	std::shared_ptr<ThreadController>	_threadController; // ✅

	MeshMap							_meshes;// The map of meshes
	std::shared_ptr<Renderer>		_renderer;
	std::shared_ptr<Window>			_window;
	// Systems
	std::shared_ptr<RenderSystem>	_renderSystem;// To handle rendering
	// Scene Manager
	SceneController	_sceneManager;

	// TODO: introduce audio manager?
public:
	Game();
	Game(const std::string name): Game() { _name = name; }
	virtual ~Game();


	// Gets/sets
public:
	const std::string GetName() const { return _name; }
	void SetName(const std::string name) { _name = name; }

	// Meshes
	std::shared_ptr<Mesh> GetMesh(std::string name);
	void AddMesh(std::string name, std::shared_ptr<Mesh> mesh) { _meshes[name] = mesh; }

	// Quit flag
	bool GetQuitFlag() const { return _quitFlag; }
	void SetQuitFlag(bool v) { _quitFlag = v; }

	// Renderer
	std::shared_ptr<Renderer> GetRenderer() const { return _renderer; }
	std::shared_ptr<InputController> GetInputController()	const { return _inputController; }

	// Functions
public:
	void AddGameObject(GameObject* obj) {
		_sceneManager.AddGameObject(std::shared_ptr<GameObject>(obj));
		_inputController->Observe(InputController::EVENT_KEY_INPUT, std::shared_ptr<GameObject>(obj));
	}
	std::vector<std::shared_ptr<GameObject>>& GetGameObjects() { return _sceneManager.GetGameObjects(); }

	// Initialise game
	virtual void Initialise(std::shared_ptr<Window> w);

	// Keyboard input
	virtual void OnKeyboard(int key, bool down);
	virtual void OnMouse(LPARAM wParam, bool down);

	// Draw everything
	virtual void Render() = 0;

	// Main game loop (update)
	virtual void Run();

	// The game can respond to messages too
	virtual void ListenToMessage(Message* msg) {}
};
