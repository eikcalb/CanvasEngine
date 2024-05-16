#pragma once
#include "Game.h"

// STL
#include <vector>
#include <sstream>

// GLM
#include "GL\GLM\GLM.hpp"
#include "GL\GLM\GTC\matrix_transform.hpp"

// Engine classes
#include "Renderer.h"
#include "Window.h"

#if BUILD_DIRECTX
/**********************************/
// http://www.directxtutorial.com //
/**********************************/

// include the basic windows header files and the Direct3D header files
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#include "Window_DX.h"

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")
#include "Window_DX.h"
#endif

#include "MultiMesh.h"

#include "Message.h"
#include "MultiMesh.h"

#include "GameMenuScene.h"
#include "Voxel.h"


class VoxGame :
	public Game
{
private:
	VoxelCanvas* _voxel;

	// Structors
public:
	VoxGame();
	virtual ~VoxGame();

	// Functions
public:
	VoxelCanvas* GetVoxelCanvas() { return _voxel; }

	// Initialise the game
	virtual void Initialise();

	// Keyboard input
	virtual void OnKeyboard(int key, bool down);

	// Render everything
	virtual void Render() override;

	// Main game loop
	virtual void Run();

public:
	void StartUp(int width, int height, HINSTANCE hInstance, int nCmdShow) {
#if BUILD_DIRECTX

		_window = std::make_shared<Window_DX>(width, height, hInstance, nCmdShow);
		_window->Initialise();

		Initialise();

		MSG msg;
		while (!Game::TheGame->GetQuitFlag())
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT) {
					Game::TheGame->SetQuitFlag(true);
					break;
				}
			}
			else {
				try {
					Game::TheGame->Run();
				}
				catch (std::exception& ex) {
					std::cout << ex.what() << std::endl;
					exit(1);
				}
			}
		}
#else

#endif
	}

};

