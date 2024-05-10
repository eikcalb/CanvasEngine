#pragma once
#include <vector>
#include "GameObject.h"

class RenderSystem;
class SceneController;

/// Represents an abstract scene, i.e. the view presented to users.
/// With this class, the application will display content to users.
/// In an MVC architecture, this will be the presentation (view).
/// Scenes can be nested in order to produce a group of game objects.
/// layer.
class Scene : public GameObject
{
	// Constants
public:

	// Data
protected:
	std::vector<std::shared_ptr<GameObject>> _gameObjects; // The list of game objects

	// Structors
public:
	Scene();
	virtual ~Scene();


	// Gets/Sets
public:

	// Game objects
	std::vector<std::shared_ptr<GameObject>>& GetGameObjects() { return _gameObjects; }
	void AddGameObject(std::shared_ptr<GameObject> obj) { _gameObjects.push_back(obj); }

	// Functions
public:

	virtual void Initialise() = 0;

	/// Respond to input
	virtual void OnKeyboard(int key, bool down) = 0;

	/// Update current scene
	virtual void Update(double deltaTime);

	/// Render current scene
	virtual void Render(RenderSystem* renderer) = 0;

	// Respond to messages
	virtual void OnMessage(Message* msg) override;
};

