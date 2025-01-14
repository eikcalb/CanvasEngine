/***************************************************/
// DX code based on http://www.directxtutorial.com //
/***************************************************/

#pragma once
#if BUILD_DIRECTX
// include Windows and Direct3D headers
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <DirectXMath.h>

#include "Window.h"
#include "Game.h"

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

// DirectX window
class Window_DX :
	public Window
{
	// Data
private:

	HWND					_hWnd;

	// Structors
public:
	// Constructor
	Window_DX(int width, int height, HINSTANCE hInstance, int nCmdShow);

	// Destructor
	virtual ~Window_DX();


	// Functions
public:

	// Windows Message loop
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


	// Initialise window
	virtual void Initialise() override;
};


#endif