#include "Window.h"
#include <stdlib.h>
#include <time.h>

std::shared_ptr<Window>	Window::TheWindow = NULL;

/******************************************************************************************************************/

Window::Window(Game* game, int width, int height)
	:	_renderer(0),
		_game(game),
		_width(width),
		_height(height)
{
	// Set static singleton
	TheWindow = std::shared_ptr<Window>(this);

	// Reset RNG
	srand(time(NULL));

}

/******************************************************************************************************************/

Window::~Window()
{
	delete _game;
	delete _renderer;
}

/******************************************************************************************************************/
