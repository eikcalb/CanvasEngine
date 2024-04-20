#pragma once
#include <stack>
#include <vector>

#include "Scene.h"

class Game;

/// Handles the Scenes for a game
class SceneController : public Observer
{
	// Constants
public:

	// Data
protected:
	std::stack<std::shared_ptr<Scene>>	_scenes;

	// Structors
	SceneController();
public:
	~SceneController();


	// Gets/Sets
public:
	SceneController(const SceneController& SceneController) = delete;
	SceneController& operator=(SceneController const&) = delete;

	std::shared_ptr<Scene> GetCurrentScene()	const { if (_scenes.size() > 0) return _scenes.top(); else return NULL; }
	std::shared_ptr<Game> GetGame() const;

	// Functions
public:
	// Game object handling
	void AddGameObject(std::shared_ptr<GameObject> obj);
	std::vector<std::shared_ptr<GameObject>>& GetGameObjects();

	/// Respond to input
	void OnKeyboard(int key, bool down);

	/// Respond to messages
	void OnMessage(Message* msg) override;

	/// Update current scene
	void Update(double deltaTime);

	/// Render current scene
	void Render(RenderSystem* renderer);

	/// Pop the top scene. If no scenes remain, we should quit.
	void PopScene() { _scenes.pop(); }

	/// Push a new scene
	void PushScene(std::shared_ptr<Scene> s);

	static std::shared_ptr< SceneController > Instance();
};

