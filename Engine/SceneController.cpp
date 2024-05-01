#include "SceneController.h"

#include "Game.h"

/******************************************************************************************************************/
// Structors
/******************************************************************************************************************/

SceneController::SceneController()
{
}

/******************************************************************************************************************/

SceneController::~SceneController()
{
}

/******************************************************************************************************************/
// Functions
/******************************************************************************************************************/

std::shared_ptr<Game> SceneController::GetGame() const { return Game::TheGame; }

/******************************************************************************************************************/

void SceneController::AddGameObject(std::shared_ptr<GameObject> obj)
{
	auto currentScene = GetCurrentScene();
	if (currentScene)
	{
		currentScene->AddGameObject(obj);
	}
}

/******************************************************************************************************************/

std::vector<std::shared_ptr<GameObject>>& SceneController::GetGameObjects()
{
	auto currentScene = GetCurrentScene();
	if (currentScene)
	{
		return currentScene->GetGameObjects();
	}
	auto empty = std::vector<std::shared_ptr<GameObject>>();
	return empty;
}

/******************************************************************************************************************/

void SceneController::OnKeyboard(int key, bool down)
{
	auto currentScene = GetCurrentScene();
	if (currentScene)
	{
		currentScene->OnKeyboard(key, down);
	}
}

/******************************************************************************************************************/

void SceneController::OnMessage(Message* msg)
{
	auto currentScene = GetCurrentScene();
	if (currentScene)
	{
		currentScene->OnMessage(msg);
	}
}

/******************************************************************************************************************/

/// Update current scene
void SceneController::Update(double deltaTime)
{
	std::lock_guard<std::mutex> lk(_sceneMutex);

	auto currentScene = GetCurrentScene();
	if (currentScene)
	{
		currentScene->Update(deltaTime);
	}
}

/******************************************************************************************************************/

/// Render current scene
void SceneController::Render(RenderSystem* renderer)
{
	auto currentScene = GetCurrentScene();
	if (currentScene)
	{
		currentScene->Render(renderer);
	}
}

/******************************************************************************************************************/

void SceneController::PushScene(std::shared_ptr<Scene> s)
{
	std::lock_guard<std::mutex> lk(_sceneMutex);

	s->End();
	_scenes.push(s);
	Game::TheGame->ResetCameraPosition();
	s->Initialise();
}

/******************************************************************************************************************/

std::shared_ptr<SceneController> SceneController::Instance()
{
	static std::shared_ptr<SceneController> instance{ new SceneController() };
	return instance;
}
