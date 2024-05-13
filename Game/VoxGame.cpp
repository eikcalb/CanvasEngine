#include "VoxGame.h"

#include "GamePlayScene.h"

/******************************************************************************************************************/

VoxGame::VoxGame() : Game("Voxel Game")
{
	_voxel = new VoxelCanvas();
}

/******************************************************************************************************************/

VoxGame::~VoxGame()
{
	delete _voxel;
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
	cubeMesh->LoadFromFile("VoxMesh.txt", sizeof(GeneratorBufferData), _voxel->GetSize());
	AddMesh("cube", cubeMesh);

	// Create VBOs
	for (MeshMapIterator i = _meshes.begin();
		i != _meshes.end();
		++i)
	{
		i->second->CreateVBO(_renderer.get());
	}

	GetThreadController()->AddTask(
		[&] {
			//_sceneController->PushScene(std::make_shared<GameMenuScene>());
			_sceneController->PushScene(std::make_shared<GamePlayScene>());
		},
		TaskType::GRAPHICS,
		"Setup Scenes"
	);
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

	_renderer->UpdateCamera();
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
