#pragma once
#include "Scene.h"

#include "GL\GLM\GLM.hpp"
#include "GL\GLM\GTC\matrix_transform.hpp"

constexpr int WIDTH = 512;
constexpr int HEIGHT = 512;

class GamePlayScene :
	public Scene
{
	// Constants
public:

	// Data
protected:

	// Structors
public:
	GamePlayScene();
	virtual ~GamePlayScene();


	// Gets/Sets
public:

	// Functions
public:

	// Setup
	virtual void Initialise() override;

	virtual void Start() override;

	/// Respond to input
	virtual void OnKeyboard(int key, bool down) override;
	
	/// Update current scene
	virtual void Update(double deltaTime) override;

	/// Render current scene
	virtual void Render(RenderSystem* renderer) override;

	virtual void OnMessage(Message* msg) override;

	// Reset the game
	void Reset();
};
