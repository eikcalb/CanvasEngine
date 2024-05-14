#pragma once

#define WIN32_LEAN_AND_MEAN // Required to prevent winsock/WinSock2 redifinition
#include <D3Dcommon.h>

// GLM
#include "GL\GLM\GLM.hpp"
#include "GL\GLM\GTC\matrix_transform.hpp"
#include "GL\GLM\GTC\type_ptr.hpp"

#include "Colour.h"
#include "Hud.h"

class GameObject;
class Mesh;

// Platform independent renderer base class
// Basically represents a graphics context and its active shaders.
// Ideally, we should have an array of shaders and each shader can
// be specified during the draw call.
class Renderer
{
	// Data
protected:
	Colour					_clearColour;	// Screen clear colour
	std::shared_ptr<Hud>	_hud;
	glm::mat2x3				_initialCameraPos;
	glm::mat2x3				_cameraPos;

public:
	D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST;

	// Structors
public:
	Renderer();
	virtual ~Renderer();

	// Gets/sets
public:
	Colour GetClearColour()				const { return _clearColour; }
	void SetClearColour(Colour c) { _clearColour = c; }
	void SetTopology(D3D_PRIMITIVE_TOPOLOGY tgy) { topology = tgy; }
	std::shared_ptr<Hud> GetHud() { return _hud; }

	glm::mat2x3& GetCameraPosition() { return _cameraPos; }
	void SetCameraPosition(glm::mat2x3 position) {
		_cameraPos = position;
		UpdateCamera();
	}
	void ResetCameraPosition() { _cameraPos = _initialCameraPos; }

	virtual void UpdateCamera() = 0;

	virtual glm::vec3 CalculateMouseRay(const int x, const int y, unsigned int width, unsigned int height) = 0;

	// Functions
public:
	virtual void ClearScreen() = 0;

	virtual void Draw(const std::shared_ptr<GameObject> go, const Colour& colour) = 0;
	virtual void Draw(std::shared_ptr<GameObject> gob);

	virtual void Initialise(int width, int height) = 0;

	virtual void Destroy() = 0;

	virtual void SwapBuffers() = 0;
};

