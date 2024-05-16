#include "VoxGame.h"

static const int		SCREEN_WIDTH = 1200;
static const int		SCREEN_HEIGHT = 800;

// the entry point for any Windows program
int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow
)
{
	// Create the Game object
	std::shared_ptr<VoxGame> game = std::make_shared<VoxGame>();
	game->GetThreadController()->AddTask(
		[&] {
			// Create a Window object
			game->StartUp(SCREEN_WIDTH, SCREEN_HEIGHT, hInstance, nCmdShow);
		},
		TaskType::GRAPHICS,
		"Main Graphics Thread"
	);

	// Wait for application to run
	while (!game->GetQuitFlag()) {
		// NOOP - This loop will continue running until the game has been quit.
	}

	// Shutdown controllers.
	//game->GetThreadController().reset();
	//game->GetInputController().reset();
	//game->GetResourceController().reset();
}