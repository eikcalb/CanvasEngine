#include "VoxGame.h"

#include "GamePlayScene.h"
#include <Utils.h>

/******************************************************************************************************************/

VoxGame::VoxGame() : Game("Voxel Game")
{
	_voxel = new VoxelCanvas();
}

/******************************************************************************************************************/

VoxGame::~VoxGame()
{
	_sceneController.reset();
	delete _voxel;
}

/******************************************************************************************************************/

void VoxGame::Initialise()
{
	// Initialise parent
	Game::Initialise();

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

	//_sceneController->PushScene(std::make_shared<GameMenuScene>());
	_sceneController->PushScene(std::make_shared<GamePlayScene>());

	_currentTime = Utils::GetTime();
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

	double temp_time = Utils::GetTime();
	_deltaTime = temp_time - _currentTime;

	if (_deltaTime >= 1.0 / GetFPS()) {
		// Update scenes
		_sceneController->Update(_deltaTime);

		// Check for exit
		if (_sceneController->GetCurrentScene() == NULL)
		{
			OutputDebugString(L"Cannot run the application when no scene is set!\r\n");
			return;
			/*SetQuitFlag(true);
			return;*/
		}

		// Draw
		Render();
		SetActualFPS(1.0 / (temp_time - _currentTime));
		_currentTime = temp_time;
	}
}

/******************************************************************************************************************/
