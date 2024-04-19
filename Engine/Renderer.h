#pragma once

// GLM
#include "GL\GLM\GLM.hpp"
#include "GL\GLM\GTC\matrix_transform.hpp"
#include "GL\GLM\GTC\type_ptr.hpp"

#include "Colour.h"
#include "Hud.h"

class GameObject;
class Mesh;

// Platform independent renderer base class
// Basically represents a graphics context and its active shaders
class Renderer
{
	// Data
protected:
	Colour					_clearColour;	// Screen clear colour
	std::shared_ptr<Hud>	_hud;

	// Structors
public:
	Renderer();
	virtual ~Renderer();

	// Gets/sets
public:
	Colour GetClearColour()				const { return _clearColour; }
	void SetClearColour(Colour c) { _clearColour = c; }
	std::shared_ptr<Hud> GetHud() { return _hud; }

	// Functions
public:
	virtual void ClearScreen() = 0;

	virtual void Draw(const std::shared_ptr<Mesh> mesh, glm::mat4 MVM, const Colour& colour) = 0;
	virtual void Draw(std::shared_ptr<GameObject> gob, glm::mat4 MVM);

	virtual void Initialise(int width, int height) = 0;

	virtual void Destroy() = 0;

	virtual void SwapBuffers() = 0;
};

