#include "VoxGame.h"

/******************************************************************************************************************/

VoxGame::VoxGame(): Game("Voxel Game")
{
}

/******************************************************************************************************************/

VoxGame::~VoxGame()
{
}

/******************************************************************************************************************/

void VoxGame::Initialise(std::shared_ptr<Window> w)
{
	// Initialise parent
	Game::Initialise(w);

	//////////////
	// Setup Meshes
	auto cube= std::make_shared<Mesh>();
	cube->LoadFromFile("CubeMesh.txt");
	AddMesh("cube", cube);

	// Create VBOs
	for (MeshMapIterator i = _meshes.begin();
		i != _meshes.end();
		++i)
	{
		i->second->CreateVBO(_renderer.get());
	}

	_sceneManager.PushScene(std::make_shared<GamePlayScene>());
}

/******************************************************************************************************************/

void VoxGame::OnKeyboard(int key, bool down)
{
	Game::OnKeyboard(key, down);

	_sceneManager.OnKeyboard(key, down);
}

/******************************************************************************************************************/

void VoxGame::Render()
{
	// Clear screen
	_renderer->ClearScreen();

	_renderer->GetHud()->Start();

	//// Add this code for 3D (need to change ScoreDisplay to fit inside 3D window)
	glm::mat4 MVM;
	MVM = glm::perspectiveFov(45.0f, (float)_window->_width, (float)_window->_height, 0.1f, 100.0f);
	MVM *= glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	_renderSystem->SetMVM(MVM);

	_sceneManager.Render(_renderSystem.get());
	_renderer->GetHud()->Render();

	_renderer->GetHud()->End();

	// Swap buffers
	_renderer->SwapBuffers();
}

/******************************************************************************************************************/

void VoxGame::Run()
{
	// Run parent method to get delta time etc
	Game::Run();

	// Update scenes
	_sceneManager.Update(_deltaTime);

	// Check for exit
	if (_sceneManager.GetCurrentScene() == NULL)
	{
		OutputDebugString(L"Cannot run the application when no scene is set!");
		SetQuitFlag(true);
	}

	// Draw
	Render();
}

/******************************************************************************************************************/
