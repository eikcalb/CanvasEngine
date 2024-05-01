#include "VoxGame.h"

/******************************************************************************************************************/

VoxGame::VoxGame() : Game("Voxel Game")
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
	auto lineMesh = std::make_shared<Mesh>();
	lineMesh->LoadFromFile("CubeMesh.txt");
	AddMesh("line", lineMesh);

	auto cubeMesh = std::make_shared<Mesh>();
	cubeMesh->LoadFromFile("VoxMesh.txt");
	AddMesh("cube", cubeMesh);

	// Create VBOs
	for (MeshMapIterator i = _meshes.begin();
		i != _meshes.end();
		++i)
	{
		i->second->CreateVBO(_renderer.get());
	}

	_sceneController->PushScene(std::make_shared<GameMenuScene>());
}

/******************************************************************************************************************/

void VoxGame::OnKeyboard(int key, bool down)
{
	Game::OnKeyboard(key, down);

	_sceneController->OnKeyboard(key, down);
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
	//MVM *= glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	auto& cam = GetCameraPosition();
	MVM *= glm::lookAt(cam[0], cam[1], cam[2]);
	_renderSystem->SetMVM(MVM);

	_sceneController->Render(_renderSystem.get());
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
	// Get delta time:
// https://en.cppreference.com/w/cpp/chrono/c/clock
	double temp_time = clock();
	_deltaTime = (temp_time - _currentTime) / CLOCKS_PER_SEC;

	if (_deltaTime >= 1 / GetFPS()) {
		// Update scenes
		_sceneController->Update(_deltaTime);

		// Check for exit
		if (_sceneController->GetCurrentScene() == NULL)
		{
			OutputDebugString(L"Cannot run the application when no scene is set!");
			SetQuitFlag(true);
		}

		// Draw
		Render();
		_currentTime = temp_time;
	}
}

/******************************************************************************************************************/
