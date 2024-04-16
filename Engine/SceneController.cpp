#include "SceneController.h"
#include "Scene.h"

/******************************************************************************************************************/
// Structors
/******************************************************************************************************************/

SceneController::SceneController(Game* game)
	: _game(game)
{
}

/******************************************************************************************************************/

SceneController::~SceneController()
{
	delete _game;
}

/******************************************************************************************************************/
// Functions
/******************************************************************************************************************/

void SceneController::AddGameObject(std::shared_ptr<GameObject> obj)
{
	Scene* currentScene = GetCurrentScene();
	if (currentScene)
	{
		currentScene->AddGameObject(obj);
	}
}

/******************************************************************************************************************/

std::vector<std::shared_ptr<GameObject>>& SceneController::GetGameObjects()
{
	Scene* currentScene = GetCurrentScene();
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
	Scene* currentScene = GetCurrentScene();
	if (currentScene)
	{
		currentScene->OnKeyboard(key, down);
	}
}

/******************************************************************************************************************/

void SceneController::OnMessage(Message<std::any>* msg)
{
	Scene* currentScene = GetCurrentScene();
	if (currentScene)
	{
		currentScene->OnMessage(msg);
	}
}

/******************************************************************************************************************/

/// Update current scene
void SceneController::Update(double deltaTime)
{
	Scene* currentScene = GetCurrentScene();
	if (currentScene)
	{
		currentScene->Update(deltaTime);
	}
}

/******************************************************************************************************************/

/// Render current scene
void SceneController::Render(RenderSystem* renderer)
{
	Scene* currentScene = GetCurrentScene();
	if (currentScene)
	{
		currentScene->Render(renderer);
	}
}

/******************************************************************************************************************/

void SceneController::PushScene(std::shared_ptr<Scene> s)
{
	s->End();
	_scenes.push(s);
	s->SetSceneManager(std::shared_ptr<SceneController>(this));
	s->Initialise();
}

/******************************************************************************************************************/
