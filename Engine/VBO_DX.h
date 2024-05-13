#pragma once
#if BUILD_DIRECTX
#include <memory>
#include <d3dx11.h>
#include <d3dx10.h>

#include "VBO.h"
#include "Vertex.h"

class Renderer_DX;

// DirectX specific VBO class
class VBO_DX :
	public VBO
{
	// Data
protected:
	ID3D11Buffer* _idx;
	ID3D11Buffer* _vbo;
	ID3D11Buffer* _ins;

	ID3D11ShaderResourceView* _srv;

	// Structors
public:
	VBO_DX();
	virtual ~VBO_DX();

	// Functions
public:
	virtual void Create(
		Renderer* renderer,
		Vertex vertices[],
		int numVertices,
		unsigned short indices[],
		int numIndices,
		unsigned int stride = 0,
		unsigned long size = 0
	) override;
	virtual void Draw(Renderer* renderer, const std::shared_ptr<VBOInstanceData> instanceData = nullptr, unsigned long count = 1) override;
};

#endif