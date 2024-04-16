#pragma once
#include <vector>
#include "System.h"
#include "Renderer.h"

// Moves objects around based on PhysicsComponents
class RenderSystem :
	public System
{
	// Data
protected:
	std::shared_ptr<Renderer> _renderer;
	glm::mat4 _MVM;

	// Structors
public:
	RenderSystem();
	virtual ~RenderSystem();

	// Get / sets
public:
	std::shared_ptr<Renderer> GetRenderer()		const	{ return _renderer; }
	void SetRenderer(std::shared_ptr<Renderer> r)		{ _renderer = r; }
	glm::mat4 GetMVM()			const	{ return _MVM; }
	void SetMVM(glm::mat4 m)			{ _MVM = m; }

	// Functions
public:
	virtual void Process(std::vector<std::shared_ptr<GameObject>>& list, double deltaTime);
};

