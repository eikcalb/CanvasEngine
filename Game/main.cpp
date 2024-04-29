#include "VoxGame.h"

static const int		SCREEN_WIDTH = 1080;
static const int		SCREEN_HEIGHT = 712;


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

//#ifndef WIN_SOCK_NON_BLOCK
//	// This fixes the error of importing winsock2 multiple times.
//	#define WIN_SOCK_NON_BLOCK
//	#include <WinSock2.h>
//	#include <WS2tcpip.h>
//	#pragma comment(lib, "Ws2_32.lib")
//#endif // !WIN_SOCK_NON_BLOCK

#include "Window_DX.h"

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

// a struct to define a single vertex
struct VERTEX { FLOAT X, Y, Z; D3DXCOLOR Color; };

// the entry point for any Windows program
int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow
)
{
	// Create the Game object
	VoxGame game;
	game.GetThreadController()->AddTask(
		[&] {
			// Create a Window object
			Window_DX application(SCREEN_WIDTH, SCREEN_HEIGHT, hInstance, nCmdShow);
			application.Initialise();
		},
		TaskType::GRAPHICS,
		"Main Graphics Thread"
	);

	// Wait for application to run
	while (!game.GetQuitFlag()) {
		// NOOP - This loop will continue running until the game has been quit.
	}

	// Shutdown controllers.
	game.GetThreadController().reset();
}

#else

#include "GL\GXBase\GXBase.h"
#include "Window_GL.h"
using namespace gxbase;

// Simple application class (all it does is contain our main window)
class VoxApp
	: public App
{
private:
	Window_GL win;
	VoxGame* game;

public:
	VoxApp()
		: win(dynamic_cast<Game*>(new VoxApp()), SCREEN_WIDTH, SCREEN_HEIGHT)
	{
		game = dynamic_cast<VoxGame*>(win.GetGame());
	}

	~VoxApp()
	{
		delete game;
	}
};

// This is the single instance of our application
static VoxApp app;

#endif