#if BUILD_DIRECTX
#include "Window_DX.h"
#include "Renderer_DX.h"
#include "Game.h"


/******************************************************************************************************************/

Window_DX::Window_DX(Game* game, int width, int height, HINSTANCE hInstance, int nCmdShow)
	: Window(game, width, height)
{

	// Reset RNG
	srand(time(NULL));

	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = Window_DX::WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"WindowClass";

	RegisterClassEx(&wc);

	RECT wr = { 0, 0, _width, _height };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	_hWnd = CreateWindowEx(NULL,
		L"WindowClass",
		L"Vox DX",
		WS_OVERLAPPEDWINDOW,
		300,
		300,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(_hWnd, nCmdShow);
}

/******************************************************************************************************************/

Window_DX::~Window_DX()
{
}

/******************************************************************************************************************/

// This is the main message handler for the program
LRESULT CALLBACK Window_DX::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ACTIVATE:
	case WM_MOUSEACTIVATE:
		return MA_ACTIVATEANDEAT;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
	{
		TheWindow->GetGame()->OnMouse(lParam, true);
		break;
	}
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
	case WM_INPUT:
	case WM_MOUSEMOVE:
	{
		TheWindow->GetGame()->OnMouse(lParam, false);
		break;
	}
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: {
		TheWindow->GetGame()->OnKeyboard(wParam, true);
		break;
	}
	case WM_KEYUP:
	case WM_SYSKEYUP: {
		TheWindow->GetGame()->OnKeyboard(wParam, false);
		break;
	}
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

/******************************************************************************************************************/

void Window_DX::Initialise()
{
	// Initialise DirectX
	_renderer = new Renderer_DX(_hWnd);
	_renderer->Initialise(_width, _height);

	// Setup Game
	_game->Initialise(this);

	MSG msg;
	while (!_game->GetQuitFlag())
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}

		_game->Run();
	}

	// Clean up DirectX
	_renderer->Destroy();
}

/******************************************************************************************************************/

#endif