#include "Mesh.h"

#include "VBO.h"
#include "Renderer.h"

#if BUILD_DIRECTX
#include "VBO_DX.h"
#endif
#if BUILD_OPENGL
#include "VBO_GL.h"
#endif
#include "Vector4.h"
#include <iostream>
#include <fstream>
using namespace std;

/******************************************************************************************************************/

Mesh::Mesh()
{}

/******************************************************************************************************************/

Mesh::~Mesh()
{
	delete _vbo;
	_vbo = NULL;
}

/******************************************************************************************************************/

bool Mesh::AddVertex(Vertex v)
{
	if (!_locked)
	{
		_vertices.push_back(v);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************************************************/

bool Mesh::AddIndex(unsigned short i)
{
	if (!_locked)
	{
		_indices.push_back(i);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************************************************/

bool Mesh::DeleteVertex(int i)
{
	if (!_locked)
	{
		_vertices.erase(_vertices.begin() + i);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************************************************/

bool Mesh::DeleteIndex(int i)
{
	if (!_locked)
	{
		_indices.erase(_indices.begin() + i);
		return true;
	}
	else
	{
		return false;
	}
}
/******************************************************************************************************************/

bool Mesh::Clear()
{
	if (!_locked)
	{
		_indices.clear();
		_vertices.clear();
		return true;
	}
	return false;
}

/******************************************************************************************************************/

void Mesh::Reset()
{
	delete _vbo;
	_vbo = NULL;
	_locked = false;
}

/******************************************************************************************************************/

VBO* Mesh::CreateVBO(Renderer* renderer)
{
	_locked = true;

#if BUILD_DIRECTX
	_vbo = new VBO_DX();
#else
	_vbo = new VBO_GL();
#endif

	_vbo->Create(renderer, _vertices.data(), NumVertices(), _indices.data(), NumIndices());

	return _vbo;
}

/******************************************************************************************************************/

float Mesh::CalculateMaxSize()
{
	float max = 0;
	for (int i = 0; i < NumVertices(); i++)
	{
		Vertex& v = _vertices[i];
		Vector4 vec(v.position.x, v.position.y, v.position.z, 1);
		float dist = vec.lengthSquared();
		if (dist > max)
		{
			max = dist;
		}
	}
	return sqrt(max);
}

/******************************************************************************************************************/

bool Mesh::LoadFromFile(std::string filename)
{
	ifstream in(filename);
	if (in)
	{
		return LoadFromStream(in);
	}
	else
	{
		// Could not open file
		return false;
	}
}

/******************************************************************************************************************/

bool Mesh::LoadFromStream(std::ifstream& in)
{
	int numVertices;
	in >> numVertices;
	for (int i = 0; i < numVertices; i++)
	{
		Vertex v;
		in >> v.position.x;
		in >> v.position.y;
		in >> v.position.z;
		in >> v.color.r;
		in >> v.color.g;
		in >> v.color.b;
		in >> v.color.a;
		AddVertex(v);
	}

	int numIndices;
	in >> numIndices;
	for (int i = 0; i < numIndices; i++)
	{
		unsigned int index;
		in >> index;
		AddIndex(index);
	}
	return true;
}

/******************************************************************************************************************/
