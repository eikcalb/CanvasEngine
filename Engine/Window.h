#pragma once
#include <memory>
#include <stdlib.h>
#include <time.h>

#include "Renderer.h"

class Game;

// Generic window class
// Handles window setup and input
class Window
{
	// Constants and statics
public:
	// Singleton (sort of)
	static std::shared_ptr<Window>	TheWindow;

	// Data
public:
	std::shared_ptr<Renderer>	_renderer;
	int							_width, _height;


	// Structors
public:
	Window(int width, int height);
	virtual ~Window();


	// Gets/sets
public:
	std::shared_ptr<Renderer>& GetRenderer()	{ return _renderer; }


	// Functions
public:
	virtual void Initialise() = 0;
};

