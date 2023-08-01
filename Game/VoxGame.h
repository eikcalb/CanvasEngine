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
#include "MultiMesh.h"

#include "Components.h"
#include "Message.h"
#include "MultiMesh.h"

#include "GamePlayScene.h"

// Simple game state enum for the Asteroids game
enum GameState
{
	MainMenu,
	Playing,
	Paused,
	Dead,
	Resetting,
};

class VoxGame :
	public Game
{
	// Structors
public:
	VoxGame();
	virtual ~VoxGame();

	// Functions
public:

	// Initialise the game
	virtual void Initialise(Window* w);

	// Keyboard input
	virtual void OnKeyboard(int key, bool down);

	// Render everything
	virtual void Render();

	// Main game loop
	virtual void Run();

};

