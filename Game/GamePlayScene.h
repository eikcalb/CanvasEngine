#pragma once
#include "Scene.h"
#include "CollisionSystem.h"

// Game classes
#include "Cube.h"
#include "Voxel.h"

const int WIDTH = 512;
const int HEIGHT = 512;

class GamePlayScene :
	public Scene
{
	// Constants
public:

	// Data
protected:
	CollisionSystem						_collisionSystem;

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

