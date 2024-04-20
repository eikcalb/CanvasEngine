#pragma once
#include "Scene.h"

#include "GL\GLM\GLM.hpp"
#include "GL\GLM\GTC\matrix_transform.hpp"

class GameMenuScene :
    public Scene
{

	// Data
protected:
	// Matrices
	glm::mat4							_IM;  // Identity matrix
	glm::mat4							_MVM; // ModelView matrix

	// Structors
public:
	GameMenuScene();
	virtual ~GameMenuScene();


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

