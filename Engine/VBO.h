#pragma once
#include <memory>

#include "Vertex.h"

class Renderer;

struct VBOInstanceData {
	bool	shouldUpdate;
	void* data;
};

// Abstract VBO class
class VBO
{
	// Data
protected:
	int _numVertices;
	int _numIndices;

	// Constructors
public:
	VBO();
	virtual ~VBO();

	// Functions
public:
	virtual void Create(Renderer* renderer, Vertex vertices[], int numVertices, unsigned short indices[], int numIndices, unsigned long instanceSize = 0) = 0;
	virtual void Draw(Renderer* renderer, const std::shared_ptr<VBOInstanceData> instanceData = nullptr, unsigned long count) = 0;
};

