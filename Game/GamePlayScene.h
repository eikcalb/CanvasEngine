#pragma once
#include "Scene.h"

// Game classes
#include "Voxel.h"

constexpr int WIDTH = 512;
constexpr int HEIGHT = 512;

class GamePlayScene :
	public Scene
{
	// Constants
public:

	// Data
protected:
	// Matrices
	glm::mat4							_IM;  // Identity matrix
	glm::mat4							_MVM; // ModelView matrix

	VoxelCanvas*						_voxel;

	// Structors
public:
	GamePlayScene();
	virtual ~GamePlayScene();


	// Gets/Sets
public:

	// Functions
public:

	// Setup
	virtual void Initialise();

	/// Respond to input
	virtual void OnKeyboard(int key, bool down);
	
	/// Update current scene
	virtual void Update(double deltaTime);

	/// Render current scene
	virtual void Render(RenderSystem* renderer);


	// Reset the game
	void Reset();
};

