#pragma once
#include <vector>

class GameObject;
class RenderSystem;
class SceneController;
class Message;

/// Represents an abstract scene, i.e. mode of the game
class Scene
{
	// Constants
public:

	// Data
protected:
	SceneController*					_sceneManager;
	std::vector<GameObject*>		_gameObjects;		// The list of game objects

	// Structors
public:
	Scene();
	virtual ~Scene();


	// Gets/Sets
public:

	// Game objects
	std::vector<GameObject*>& GetGameObjects()			{ return _gameObjects; }
	void AddGameObject(GameObject* obj)					{ _gameObjects.push_back(obj); }

	void SetSceneManager(SceneController* s)				{ _sceneManager = s; }
	SceneController* GetSceneManager()				const	{ return _sceneManager; }


	// Functions
public:

	virtual void Initialise()							= 0;

	/// Respond to input
	virtual void OnKeyboard(int key, bool down)			= 0;

	// Respond to messages
	virtual void OnMessage(Message* msg)				;

	/// Update current scene
	virtual void Update(double deltaTime)				;

	/// Render current scene
	virtual void Render(RenderSystem* renderer)			= 0;


};

