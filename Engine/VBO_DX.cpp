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
	bd.Usage = D3D11_USAGE_DEFAULT;                // write access access by CPU and GPU
	bd.ByteWidth = sizeof(Vertex) * numVertices;   // size is the VERTEX struct * num vertices
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = 0;    // allow CPU to write in buffer

	D3D11_SUBRESOURCE_DATA vbd;
	ZeroMemory(&vbd, sizeof(D3D10_SUBRESOURCE_DATA));
	vbd.pSysMem = vertices;
	rendererDX->GetDevice()->CreateBuffer(&bd, &vbd, &_vbo);        // create the buffer

	D3D11_BUFFER_DESC id;
	ZeroMemory(&id, sizeof(id));

	id.Usage = D3D11_USAGE_DEFAULT;
	id.ByteWidth = sizeof(unsigned int) * numIndices;
	id.BindFlags = D3D11_BIND_INDEX_BUFFER;
	id.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA iData;
	ZeroMemory(&iData, sizeof(D3D11_SUBRESOURCE_DATA));
	iData.pSysMem = indices;
	iData.SysMemPitch = 0;

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

	if (_numIndices > 0) {
		rendererDX->GetContext()->IASetIndexBuffer(_idx, DXGI_FORMAT_R16_UINT, offset);
		rendererDX->GetContext()->DrawIndexed(_numIndices, 0, 0);
	}
	else {
		rendererDX->GetContext()->Draw(_numVertices, 0);
	}
}

/******************************************************************************************************************/

#endif