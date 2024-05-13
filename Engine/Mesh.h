#pragma once
#include <atomic>
#include <vector>
#include <string>

#include "Vertex.h"
class VBO;
class Renderer;


class Mesh
{
	// Constructors etc
public:
	Mesh();
	~Mesh();

	// Data
protected:
	std::atomic_bool _locked = false;	// True once we've made the VBO; can no longer add vertices etc unless reset
	VBO* _vbo = nullptr;
	std::vector<Vertex> _vertices;
	std::vector<unsigned short> _indices;

	unsigned long	_instanceCount;
	unsigned int	_instanceStride;

	// Gets/sets
public:
	VBO* GetVBO()			const { return _vbo; }

	int NumIndices()		const { return (int)_indices.size(); }
	const unsigned short GeIndex(int i)	const { return _indices[i]; }
	const unsigned short& GetIndexRef(int i) const { return _indices[i]; }

	int NumVertices()		const { return (int)_vertices.size(); }
	const Vertex GetVertex(int i)	const { return _vertices[i]; }
	const Vertex& GetVertexRef(int i) const { return _vertices[i]; }

private:
	unsigned long GetInstanceCount() { return _instanceCount; }
	unsigned long GetInstanceStride() { return _instanceStride; }

	// Functions
public:
	VBO* CreateVBO(Renderer* r);
	bool AddVertex(Vertex v);
	bool AddIndex(unsigned short i);
	bool Clear();
	bool DeleteVertex(int i);
	bool DeleteIndex(int i);

	// Loads the Mesh from a file (returns true if loaded OK)
	bool LoadFromFile(std::string filename);
	bool LoadFromFile(std::string filename, unsigned int instanceStride, unsigned long instanceCount);

	// Loads the Mesh from an existing stream (returns true if OK)
	bool LoadFromStream(std::ifstream& in);

	// Unlocks the mesh but also deletes the VBO
	void Reset();

	// Calculates max size of mesh in any direction
	float CalculateMaxSize();
};

