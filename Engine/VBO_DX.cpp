#if BUILD_DIRECTX
#include "VBO_DX.h"

#include "Renderer_DX.h"

/******************************************************************************************************************/

VBO_DX::VBO_DX()
	: _vbo(NULL)
{
}

/******************************************************************************************************************/

VBO_DX::~VBO_DX()
{
	delete _idx;
	delete _vbo;
}

/******************************************************************************************************************/

void VBO_DX::Create(Renderer* renderer, Vertex vertices[], int numVertices, unsigned int indices[], int numIndices)
{
	Renderer_DX* rendererDX = (Renderer_DX*)renderer;

	_numVertices = numVertices;
	_numIndices = numIndices;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	bd.ByteWidth = sizeof(Vertex) * numVertices;   // size is the VERTEX struct * num vertices
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

	rendererDX->GetDevice()->CreateBuffer(&bd, NULL, &_vbo);        // create the buffer

	// copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	rendererDX->GetContext()->Map(_vbo, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);		// map the buffer
	memcpy(ms.pData, vertices, sizeof(Vertex) * numVertices);			// copy the data
	rendererDX->GetContext()->Unmap(_vbo, NULL);											// unmap the buffer


	D3D11_BUFFER_DESC id;
	ZeroMemory(&id, sizeof(id));

	id.Usage = D3D11_USAGE_DYNAMIC;
	id.ByteWidth = sizeof(unsigned int) * numIndices;
	id.BindFlags = D3D11_BIND_INDEX_BUFFER;
	id.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA iData;
	ZeroMemory(&iData, sizeof(D3D11_SUBRESOURCE_DATA));
	iData.pSysMem = indices;
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;

	rendererDX->GetDevice()->CreateBuffer(&id, &iData, &_idx);
}

/******************************************************************************************************************/

void VBO_DX::Draw(Renderer* renderer)
{
	Renderer_DX* rendererDX = (Renderer_DX*)renderer;

	// select which vertex buffer to display
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	rendererDX->GetContext()->IASetVertexBuffers(0, 1, &_vbo, &stride, &offset);
	rendererDX->GetContext()->IASetIndexBuffer(_idx, DXGI_FORMAT_R16_UINT, 0);

	if (_numIndices > 0) {
		rendererDX->GetContext()->DrawIndexed(_numIndices, 0, 0);
	}
	else {
		rendererDX->GetContext()->Draw(_numVertices, 0);
	}
}

/******************************************************************************************************************/

#endif