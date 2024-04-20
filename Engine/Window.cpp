#include "Window.h"

#include "Game.h"

std::shared_ptr<Window>	Window::TheWindow = NULL;

/******************************************************************************************************************/

Window::Window(int width, int height)
	:	_renderer(0),
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
}

/******************************************************************************************************************/
