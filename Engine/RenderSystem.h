#pragma once
#include <vector>
#include "System.h"
#include "Renderer.h"

class GameObject;

// Moves objects around based on PhysicsComponents
class RenderSystem :
	public System
{
	// Data
protected:
	std::shared_ptr<Renderer> _renderer;

	// Structors
public:
	RenderSystem();
	virtual ~RenderSystem();

	// Get / sets
public:
	std::shared_ptr<Renderer> GetRenderer() const { return _renderer; }
	void SetRenderer(std::shared_ptr<Renderer> r) { _renderer = r; }

	// Functions
public:
	virtual void Process(std::vector<std::shared_ptr<GameObject>>& list) override;
};

