#if BUILD_DIRECTX
#include "Window_DX.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "Renderer_DX.h"
#include "Game.h"


/******************************************************************************************************************/

Window_DX::Window_DX(int width, int height, HINSTANCE hInstance, int nCmdShow)
	: Window(width, height)
{

	// Reset RNG
	srand(time(NULL));

	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;// | CS_NOCLOSE;
	wc.lpfnWndProc = Window_DX::WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"WindowClass";

	RegisterClassEx(&wc);

	RECT wr = { 0, 0, _width, _height };
	AdjustWindowRectEx(&wr, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME, FALSE, 0);

	auto appName = Game::TheGame->GetName();

	_hWnd = CreateWindowEx(0,//WS_EX_TOPMOST,
		L"WindowClass",
		std::wstring(appName.begin(), appName.end()).c_str(),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE & ~WS_THICKFRAME,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wr.right,
		wr.bottom,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(_hWnd, nCmdShow);

	// Get the shown window dimension.
	RECT clientRect;
	GetClientRect(_hWnd, &clientRect);

	_width = clientRect.right - clientRect.left;
	_height = clientRect.bottom - clientRect.top;
}

/******************************************************************************************************************/

Window_DX::~Window_DX()
{
}

/******************************************************************************************************************/

//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// This is the main message handler for the program
LRESULT CALLBACK Window_DX::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) {
		return true;
	}

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
	//case WM_RBUTTONDOWN:
	//case WM_MBUTTONDOWN:
	//case WM_XBUTTONDOWN:
	case WM_LBUTTONUP:
	//case WM_RBUTTONUP:
	//case WM_MBUTTONUP:
	//case WM_MOUSEWHEEL:
	//case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
	//case WM_INPUT:
	//case WM_MOUSELEAVE: // Should we handle mouse leave?
	case WM_MOUSEMOVE:
	{
		Game::TheGame->OnMouse(wParam, lParam);
		break;
	}
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: {
		Game::TheGame->OnKeyboard(wParam, true);
		break;
	}
	case WM_KEYUP:
	case WM_SYSKEYUP: {
		Game::TheGame->OnKeyboard(wParam, false);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

/******************************************************************************************************************/

void Window_DX::Initialise()
{
	// Initialise DirectX
	_renderer = std::make_shared<Renderer_DX>(_hWnd);
	_renderer->Initialise(_width, _height);

	// Clean up DirectX
	//_renderer->Destroy();
}

/******************************************************************************************************************/

#endif